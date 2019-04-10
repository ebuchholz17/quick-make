#include <dsound.h>

#include "directsound_audio.h"

void initDirectSound (HWND window, win_sound_output *soundOutput) {
    HMODULE directSoundLibrary = LoadLibraryA("dsound.dll");
    if (directSoundLibrary) {
        direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(directSoundLibrary, "DirectSoundCreate");

        LPDIRECTSOUND directSound;
        if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &directSound, 0))) {
            WAVEFORMATEX waveFormat = {};
            waveFormat.wFormatTag = WAVE_FORMAT_PCM;
            waveFormat.nChannels = 1;
            waveFormat.nSamplesPerSec = soundOutput->samplesPerSecond;
            waveFormat.wBitsPerSample = 16;
            waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            waveFormat.cbSize = 0;

            if (SUCCEEDED(directSound->SetCooperativeLevel(window, DSSCL_PRIORITY))) {
                DSBUFFERDESC bufferDescription = {};
                bufferDescription.dwSize = sizeof(bufferDescription);
                bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

                LPDIRECTSOUNDBUFFER primaryBuffer;
                if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &primaryBuffer, 0))) {
                    HRESULT error = primaryBuffer->SetFormat(&waveFormat);
                    if (!SUCCEEDED(error)) {
                        assert(false);
                    }
                }
                else {
                    assert(false);
                }
            }
            else {
                assert(false);
            }

            DSBUFFERDESC bufferDescription = {};
            bufferDescription.dwSize = sizeof(bufferDescription);
            bufferDescription.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
            bufferDescription.dwBufferBytes = soundOutput->secondaryBufferSize;
            bufferDescription.lpwfxFormat = &waveFormat;
            HRESULT error = directSound->CreateSoundBuffer(&bufferDescription, &soundOutput->secondaryBuffer, 0);
            if (!SUCCEEDED(error)) {
                assert(false);
            }
        }
        else {
            // TODO(ebuchholz): just an error messages? and play game without sound
            assert(false);
        }
    }
    else {
        assert(false);
    }
}

void clearSecondaryBuffer (win_sound_output *soundOutput) {
    void *region1;
    DWORD region1Size;
    void *region2;
    DWORD region2Size;
    if (SUCCEEDED(soundOutput->secondaryBuffer->Lock(0, soundOutput->secondaryBufferSize, 
                                                     &region1, &region1Size,
                                                     &region2, &region2Size,
                                                     0)))
    {
        unsigned char *destSample = (unsigned char *)region1;
        for (DWORD byteIndex = 0; byteIndex < region1Size; ++byteIndex) {
            *destSample++ = 0;
        }
        destSample = (unsigned char *)region2;
        for (DWORD byteIndex = 0; byteIndex < region2Size; ++byteIndex) {
            *destSample++ = 0;
        }

    }
    soundOutput->secondaryBuffer->Unlock(region1, region1Size, region2, region2Size);
}

void getNumSoundSamples(win_sound_output *soundOutput, int *numSoundSampleBytes, DWORD *byteToLock,
                        float timeFromFrameBeginning) 
{
    DWORD playCursor, writeCursor;
    *numSoundSampleBytes = 0;
    if (soundOutput->secondaryBuffer->GetCurrentPosition(&playCursor, &writeCursor) == DS_OK) {
        if (!soundOutput->soundValid) {
            soundOutput->dataCursor = writeCursor / soundOutput->bytesPerSample + 800;
            soundOutput->soundValid = true;
        }
        int safetyBytes = (int)((((float)soundOutput->samplesPerSecond * (float)soundOutput->bytesPerSample) / 60.0f) / 3.0f);

        //char message[1000];
        //sprintf_s(message, "Play cursor: %d\n", playCursor);
        //OutputDebugString(message);
        //sprintf_s(message, "Write cursor: %d\n", writeCursor);
        //OutputDebugString(message);
        //sprintf_s(message, "data cursor: %d\n", soundOutput->dataCursor*2);
        //OutputDebugString(message);

        // TODO(ebuchholz): make frame time not hardcoded
        // NOTE(ebuchholz): using handmade hero method of determining how many bytes to write
        *byteToLock = (soundOutput->dataCursor * soundOutput->bytesPerSample) % soundOutput->secondaryBufferSize;

        DWORD expectedBytesPerFrame = (int)((float)(soundOutput->samplesPerSecond * soundOutput->bytesPerSample) / 60.0f);

        float secondsPerFrame = (1.0f/60.0f);
        float secondsLeftInFrame = secondsPerFrame - timeFromFrameBeginning;
        DWORD bytesUntilNextFrame = (DWORD)((secondsLeftInFrame / secondsPerFrame)*(float)expectedBytesPerFrame);

        DWORD expectedFrameBoundaryByte = playCursor + bytesUntilNextFrame;

        DWORD safeWriteCursor = writeCursor;
        if (safeWriteCursor < playCursor) {
            safeWriteCursor +=  soundOutput->secondaryBufferSize;
        }
        assert(safeWriteCursor >= playCursor);
        safeWriteCursor += safetyBytes;

        bool lowLatency = safeWriteCursor < expectedFrameBoundaryByte;
        DWORD targetCursor;
        //if (lowLatency) {
        //    targetCursor = expectedFrameBoundaryByte + expectedBytesPerFrame;
        //}
        //else {
            // try writing two frames ahead
            // TODO(ebuchholz): use same method as web build?
            targetCursor = writeCursor + (2*expectedBytesPerFrame) + safetyBytes;
        //}
        targetCursor = targetCursor % soundOutput->secondaryBufferSize;
        if (*byteToLock > targetCursor) {
            *numSoundSampleBytes = (soundOutput->secondaryBufferSize - *byteToLock) + targetCursor;
        }
        else {
            *numSoundSampleBytes = targetCursor - *byteToLock;
        }
    }
    else {
        soundOutput->soundValid = false;
    }
}

void fillDirectSoundBuffer(win_sound_output *soundOutput, DWORD byteToLock, DWORD bytesToWrite, 
                           game_sound_output *gameSoundOutput) 
{
    void *region1;
    DWORD region1Size;
    void *region2;
    DWORD region2Size;
    if (SUCCEEDED(soundOutput->secondaryBuffer->Lock(byteToLock, bytesToWrite,
                                                     &region1, &region1Size,
                                                     &region2, &region2Size,
                                                     0)))
    {
        DWORD region1SampleCount = region1Size / soundOutput->bytesPerSample;
        short *destSample = (short *)region1;
        sound_sample *sourceSample = gameSoundOutput->samples;
        for (DWORD sampleIndex = 0; sampleIndex < region1SampleCount; ++sampleIndex) {
            *destSample++ = (short)(sourceSample->value * 32767);
            sourceSample++;
            soundOutput->dataCursor++;
        }
        DWORD region2SampleCount = region2Size / soundOutput->bytesPerSample;
        destSample = (short *)region2;
        for (DWORD sampleIndex = 0; sampleIndex < region2SampleCount; ++sampleIndex) {
            *destSample++ = (short)(sourceSample->value * 32767);
            sourceSample++;
            soundOutput->dataCursor++;
        }

    }
    soundOutput->secondaryBuffer->Unlock(region1, region1Size, region2, region2Size);
}
