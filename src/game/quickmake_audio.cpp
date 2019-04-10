#include "quickmake_audio.h"

float sineWave (float hz, float t) {
    return sinf((PI*2.0f*(float)hz) * t);
}

float squareWave (float hz, float t) {
    float sine = sineWave(hz, t);
    return sine >= 0.0f ? 1.0f : -1.0f;
}

float triangleWave (float hz, float t) {
    float sine = sineWave(hz, t);
    return (2.0f/PI)*asinf(sine);
}

// TODO(ebuchholz): add sawtooth

float noiseWave () {
    return randomFloat() * 2.0f - 1.0f;
}

// TODO(ebuchholz): specify sound type
int playSound (game_sounds *gameSounds, instrument_type instrumentType, float hz) {
    synth_sound *sound = 0;
    int soundIndex = -1;
    for (int i = 0; i < MAX_SOUNDS; ++i) {
        synth_sound *currentSound = gameSounds->sounds + i;
        if (!currentSound->active) {
            sound = currentSound;
            soundIndex = i;
        }
    }
    if (sound) {
        *sound = {};
        sound->instrumentType = instrumentType;
        sound->active = true;
        sound->pressed = true;
        sound->hz = hz;
    }
    return soundIndex;
}

float updateSound (synth_sound *sound, sound_instrument *instrument, float dt) {
    sound->t += dt;
    float volume = 0.0f;
    sound_envelope *envelope = &instrument->envelope;
    switch (sound->state) {
        case SOUND_STATE_ATTACK: {
            volume = (sound->t / envelope->attackTime) * envelope->attackVolume;
            if (sound->t > envelope->attackTime){
                sound->state = SOUND_STATE_DECAY;
            }
        } break;
        case SOUND_STATE_DECAY: {
            float decayDuration = sound->t - envelope->attackTime;
            volume = envelope->sustainVolume + 
                     ((envelope->decayTime - decayDuration) / envelope->decayTime) * 
                     (envelope->attackVolume - envelope->sustainVolume);
            if (decayDuration > envelope->decayTime) {
                sound->state = SOUND_STATE_SUSTAIN;
            }
        } break;
        case SOUND_STATE_SUSTAIN: {
            volume = envelope->sustainVolume;
            if (!sound->pressed) {
                sound->state = SOUND_STATE_RELEASE;
                sound->releaseTime = sound->t;
            }
        } break;
        case SOUND_STATE_RELEASE: {
            float relaseDuration = sound->t - sound->releaseTime;
            volume = ((envelope->releaseTime - relaseDuration) / envelope->releaseTime) * 
                     envelope->sustainVolume;
            if (relaseDuration > envelope->releaseTime) {
                sound->active = false;
            }
        } break;
    }
    volume *= 0.2f; // QQQ

    float tone = 0.0f;
    float pitch = sound->hz;
    for (int i = 0; i < instrument->numWaveForms; ++i) {
        sound_waveform *waveform = &instrument->waveforms[i];
        switch (waveform->waveType) {
            case OSCILLATOR_TYPE_SINE: {
               tone += waveform->volume * sineWave(pitch * waveform->muliplier, sound->t);
            } break;
            case OSCILLATOR_TYPE_TRIANGLE: {
               tone += waveform->volume * triangleWave(pitch * waveform->muliplier, sound->t);
            } break;
            case OSCILLATOR_TYPE_SQUARE: {
               tone += waveform->volume * squareWave(pitch * waveform->muliplier, sound->t);
            } break;
            case OSCILLATOR_TYPE_SAWTOOTH: {

            } break;
            case OSCILLATOR_TYPE_NOISE: {
               tone += waveform->volume * noiseWave();
            } break;
        }
    }
    return volume * tone;
    //return volume * sineWave(880, sound->t);
}
