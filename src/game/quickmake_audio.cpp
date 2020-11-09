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


float sawToothWave (float hz, float t) {
    float value = 2.0f * (t * hz - floorf(0.5f + (t * hz)));
    return value;
}

float noiseWave () {
    return randomFloat() * 2.0f - 1.0f;
}

// TODO(ebuchholz): specify sound type
int playInstrument (game_sounds *gameSounds, instrument_type instrumentType, float hz) {
    synth_sound *sound = 0;
    int soundIndex = -1;
    for (int i = 0; i < MAX_INSTRUMENT_SOUNDS; ++i) {
        synth_sound *currentSound = gameSounds->instrumentSounds + i;
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
        sound->period = 1.0f / hz;
    }
    return soundIndex;
}


void stopMIDInstrument (midi_channel *channel, unsigned char midiNote) {
    // TODO(ebuchholz): check for existing sound with same note
    synth_sound *sound = &channel->instrumentSounds[channel->numPlayingSounds];
    ++channel->numPlayingSounds;

    for (int i = 0; i < channel->numPlayingSounds; ++i) {
        synth_sound *currentSound = channel->instrumentSounds + i;
        if (currentSound->midiNote == midiNote) {
            currentSound->pressed = false;
        }
    }
}

void playMIDInstrument (midi_channel *channel, unsigned char midiNote, unsigned char velocity) {
    if (velocity == 0){
        stopMIDInstrument(channel, midiNote);
    }
    else {
        assert(channel->numPlayingSounds < NUM_PITCHES);
        // TODO(ebuchholz): check for existing sound with same note
        synth_sound *sound = 0;
        for (int i = 0; i < channel->numPlayingSounds; ++i) {
            synth_sound *playingSound = &channel->instrumentSounds[i];
            if (playingSound->midiNote == midiNote) {
                sound = playingSound;
                break;
            }
        }
        if (!sound) {
            sound = &channel->instrumentSounds[channel->numPlayingSounds];
            ++channel->numPlayingSounds;
        }

        *sound = {};
        sound->instrumentType = channel->instrument;
        sound->active = true;
        sound->pressed = true;
        sound->hz = midiNoteFrequencies[midiNote];
        sound->period = 1.0f / sound->hz;
        sound->midiNote = midiNote;
    }
}

void instrumentBendPitch (midi_channel *channel, unsigned int amount) {
    for (int i = 0; i < channel->numPlayingSounds; ++i) {
        synth_sound *playingSound = &channel->instrumentSounds[i];
        double newHz = 440.0 * 
                      pow(2.0, ((double)playingSound->midiNote - 69.0)/12.0 + ((double)amount - 8192.0)/(4096.0 * 12.0));
        playingSound->hz = (float)newHz;
        playingSound->period = 1.0f / playingSound->hz;
    }
}

float updateWaveform (synth_sound *sound, sound_instrument *instrument, float dt) {
    sound->t += dt;
    sound->cycleT += dt / sound->period;
    while (sound->cycleT >= 1.0f) {
        sound->cycleT -= 1.0f;
    }
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
            if (!sound->pressed || !envelope->sustain) {
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
    volume *= 0.1f; // QQQ

    float tone = 0.0f;
    float pitch = sound->hz;
    float waveTime = sound->cycleT * sound->period;
    for (int i = 0; i < instrument->numWaveForms; ++i) {
        sound_waveform *waveform = &instrument->waveforms[i];
        switch (waveform->waveType) {
            case OSCILLATOR_TYPE_SINE: {
               tone += waveform->volume * sineWave(pitch * waveform->muliplier, waveTime);
            } break;
            case OSCILLATOR_TYPE_TRIANGLE: {
               tone += waveform->volume * triangleWave(pitch * waveform->muliplier, waveTime);
            } break;
            case OSCILLATOR_TYPE_SQUARE: {
               tone += waveform->volume * squareWave(pitch * waveform->muliplier, waveTime);
            } break;
            case OSCILLATOR_TYPE_SAWTOOTH: {
               tone += waveform->volume * sawToothWave(pitch * waveform->muliplier, waveTime);
            } break;
            case OSCILLATOR_TYPE_NOISE: {
               tone += waveform->volume * noiseWave();
            } break;
        }
    }
    return volume * tone;
    //return volume * sineWave(880, sound->t);
}

void calcFeedbackForFilter (sound_filter *filter) {
    filter->feedback = filter->resonance + filter->resonance / (1.0f - filter->cutoff);
}

inline float updateInstrument (synth_sound *sound, sound_instrument *instrument, float dt) {
    float amplitude = updateWaveform(sound, instrument, dt);

    sound_filter *filter = &instrument->filter;
    sound->bufferedVal0 += filter->cutoff * (amplitude - sound->bufferedVal0 + filter->feedback * (sound->bufferedVal0 - sound->bufferedVal1));
    sound->bufferedVal1 += filter->cutoff * (sound->bufferedVal0 - sound->bufferedVal1);
    switch (filter->type) {
    default:
        return amplitude;
    case SOUND_FILTER_TYPE_LOW_PASS:
        return sound->bufferedVal1;
    case SOUND_FILTER_TYPE_HIGH_PASS:
        return amplitude - sound->bufferedVal1;
    case SOUND_FILTER_TYPE_BAND_PASS:
        return sound->bufferedVal0 - sound->bufferedVal1;
    }
}

void playSound (char *key, game_sounds *gameSounds, game_assets *assets) {
    // ignore sounds beyond the limit
    if(gameSounds->numPlayingSounds >= MAX_PLAYING_SOUNDS) { 
        return;
    }
    playing_sound *playingSound = &gameSounds->playingSounds[gameSounds->numPlayingSounds];
    ++gameSounds->numPlayingSounds;

    sound_asset *soundAsset = getSound(assets, key);
    playingSound->key = soundAsset->key; // use the pointer assigned to the sound asset, which is permanent
    playingSound->currentSample = 0;
}


unsigned int getVariableLengthValue (char *data, int *length) {
    unsigned int result = 0;
    unsigned char *cursor = (unsigned char *)data;
    unsigned char value = (*cursor);
    result |= (value & 0x7f);
    *length = 1;
    while ((value & 0x80) != 0) {
        result <<= 7;
        cursor++;
        value = *cursor;
        result |= (value & 0x7f);
        ++(*length);
    }
    result = bigEndianToLittleEndian(result);
    assert(*length <= 4);
    return result;
}

void playBGM (char *key, game_sounds *gameSounds, game_assets *assets) {
    midi_playback *bgmState = &gameSounds->bgmState;
    bgmState->playing = true;

    midi_asset *midiAsset = getMidi(assets, key);
    bgmState->ticksPerQuarterNote = midiAsset->ticksPerQuarterNote;
    bgmState->tickDuration = midiAsset->tickDuration;
    bgmState->numTracks = midiAsset->numTracks;

    for (int trackIndex = 0; trackIndex < bgmState->numTracks; ++trackIndex) {
        midi_track *trackData = &midiAsset->tracks[trackIndex];
        midi_track_playback *trackPlayback = &bgmState->tracks[trackIndex];
        trackPlayback->cursor = trackData->data;
        trackPlayback->ticksSinceLastEvent = 0;
        int length;
        trackPlayback->nextEventDeltaTime = bigEndianToLittleEndian(getVariableLengthValue(trackPlayback->cursor, &length));
        trackPlayback->time = 0.0f;
        trackPlayback->complete = false;
    }
}

void updateBGM (game_sounds *gameSounds, sound_sample *sampleOut, int sampleCount, int samplesPerSecond) {
    midi_playback *bgmState = &gameSounds->bgmState;
    bool allTracksComplete = true;
    for (int i = 0; i < sampleCount; ++i) {

        float sampleValue = 0.0f;
        double dt = 1.0 / (double)samplesPerSecond;
        double tickDuration = bgmState->tickDuration;

        if (bgmState->playing) {
            for (int trackIndex = 0; trackIndex < bgmState->numTracks; ++trackIndex) {
                midi_track_playback *trackPlayback = &bgmState->tracks[trackIndex];
                if (trackPlayback->complete) {
                    continue;
                }
                else {
                    allTracksComplete = false;
                }
                trackPlayback->time += dt;
                while (trackPlayback->time >= tickDuration) {
                    trackPlayback->time -= tickDuration;
                    trackPlayback->ticksSinceLastEvent++;
                }
                while (trackPlayback->ticksSinceLastEvent >= trackPlayback->nextEventDeltaTime) {
                    trackPlayback->ticksSinceLastEvent -= trackPlayback->nextEventDeltaTime;

                    // process event
                    int length;
                    unsigned int deltaTime = getVariableLengthValue(trackPlayback->cursor, &length);
                    deltaTime = bigEndianToLittleEndian(deltaTime);
                    trackPlayback->cursor += length;

                    unsigned char eventCode = *trackPlayback->cursor;
                    if (eventCode < MIDI_EVENT_CODE_SYSTEM_EXCLUSIVE) {
                        // channel commands
                        unsigned char data0;
                        unsigned char upperEventCode = (eventCode >> 4) & 0x0f;
                        if (upperEventCode < 0x08) {
                            // repeat last event
                            data0 = eventCode;
                            eventCode = (char)trackPlayback->lastCode;
                            upperEventCode = (eventCode >> 4) & 0x0f;
                        }
                        else {
                            trackPlayback->cursor++;
                            data0 = *trackPlayback->cursor;
                        }
                        unsigned char channel = eventCode & 0x0f;
                        switch (upperEventCode) {
                            case MIDI_EVENT_CODE_NOTE_OFF_EVENT: {
                                midi_channel *midiChannel = &bgmState->channels[channel];
                                stopMIDInstrument(midiChannel, data0);
                                ++trackPlayback->cursor;
                            } break;
                            case MIDI_EVENT_CODE_NOTE_ON_EVENT: {
                                ++trackPlayback->cursor;
                                char data1 = *trackPlayback->cursor;

                                midi_channel *midiChannel = &bgmState->channels[channel];
                                if (channel == 9) {
                                    midiChannel->instrument = INSTRUMENT_TYPE_SNARE_DRUM;
                                }
                                playMIDInstrument(midiChannel, data0, data1);
                            } break;
                            case MIDI_EVENT_CODE_POLYPHONIC_KEY_PRESSURE: {
                                // not supported, skip
                                ++trackPlayback->cursor;
                            } break;
                            case MIDI_EVENT_CODE_CONTROL_CHANGE: {
                                ++trackPlayback->cursor;
                            } break;
                            case MIDI_EVENT_CODE_PROGRAM_CHANGE: {
                                midi_channel *midiChannel = &bgmState->channels[channel];
                                // TODO(ebuchholz): support more instruments
                                // pianos, chromatic percussion, organs
                                if (data0 <= 24) {
                                    midiChannel->instrument = INSTRUMENT_TYPE_PIANO;
                                }
                                // guitar/bass
                                else if (data0 <= 40) {
                                    midiChannel->instrument = INSTRUMENT_TYPE_GUITAR;
                                }
                                // violin/ensemble
                                else if (data0 <= 56) {
                                    midiChannel->instrument = INSTRUMENT_TYPE_VIOLIN;
                                }
                                // brass
                                else if (data0 <= 64) {
                                    midiChannel->instrument = INSTRUMENT_TYPE_TRUMPET;
                                }
                                // wind
                                else if (data0 <= 80) {
                                    midiChannel->instrument = INSTRUMENT_TYPE_FLUTE;
                                }
                                // drums
                                else {
                                    midiChannel->instrument = INSTRUMENT_TYPE_PIANO;
                                }
                            } break;
                            case MIDI_EVENT_CODE_CHANNEL_PRESSURE: {
                                // not supported, skip
                            } break;
                            case MIDI_EVENT_CODE_PITCH_WHEEL_CHANGE: {
                                // not supported, skip
                                ++trackPlayback->cursor;
                                char data1 = *trackPlayback->cursor;
                                unsigned int pitchBendValue = 0;
                                pitchBendValue |= data0;
                                pitchBendValue |= (data1 << 7);
                                midi_channel *midiChannel = &bgmState->channels[channel];

                                instrumentBendPitch(midiChannel, pitchBendValue);
                            } break;
                        }
                    }
                    else if (eventCode == MIDI_EVENT_CODE_SYSTEM_EXCLUSIVE) {
                        // not supported, just listen for end event
                        // TODO(ebuchholz): support packet-based messages?
                        while (eventCode != MIDI_EVENT_CODE_END_OF_EXCLUSIVE) {
                            ++trackPlayback->cursor;
                            eventCode = *trackPlayback->cursor;
                        }
                    }
                    else if (eventCode == MIDI_EVENT_CODE_META_EVENT) {
                        ++trackPlayback->cursor;
                        unsigned char metaCode = *trackPlayback->cursor;
                        switch (metaCode) {
                            case MIDI_EVENT_CODE_SEQUENCE_NUMBER: {
                                ++trackPlayback->cursor;
                                unsigned char sequenceNum = *trackPlayback->cursor;
                            } break;
                            case MIDI_EVENT_CODE_TEXT_EVENT: {
                                ++trackPlayback->cursor;
                                
                                int varLength;
                                unsigned int textLength = getVariableLengthValue(trackPlayback->cursor, &varLength);
                                textLength = bigEndianToLittleEndian(textLength);
                                trackPlayback->cursor += varLength;
                                // ignore text for now
                                trackPlayback->cursor += textLength-1;
                            } break;
                            case MIDI_EVENT_CODE_COPYRIGHT_NOTICE: {
                                ++trackPlayback->cursor;
                                
                                int varLength;
                                unsigned int textLength = getVariableLengthValue(trackPlayback->cursor, &varLength);
                                textLength = bigEndianToLittleEndian(textLength);
                                trackPlayback->cursor += varLength;
                                // ignore text for now
                                trackPlayback->cursor += textLength-1;
                            } break;
                            case MIDI_EVENT_CODE_SEQUENCE_TRACK_NAME: {
                                ++trackPlayback->cursor;
                                
                                int varLength;
                                unsigned int textLength = getVariableLengthValue(trackPlayback->cursor, &varLength);
                                textLength = bigEndianToLittleEndian(textLength);
                                trackPlayback->cursor += varLength;
                                // ignore text for now
                                trackPlayback->cursor += textLength-1;
                            } break;
                            case MIDI_EVENT_CODE_INSTRUMENT_NAME: {
                                ++trackPlayback->cursor;
                                
                                int varLength;
                                unsigned int textLength = getVariableLengthValue(trackPlayback->cursor, &varLength);
                                textLength = bigEndianToLittleEndian(textLength);
                                trackPlayback->cursor += varLength;
                                // ignore text for now
                                trackPlayback->cursor += textLength-1;
                            } break;
                            case MIDI_EVENT_CODE_LYRIC: {
                                ++trackPlayback->cursor;
                                
                                int varLength;
                                unsigned int textLength = getVariableLengthValue(trackPlayback->cursor, &varLength);
                                textLength = bigEndianToLittleEndian(textLength);
                                trackPlayback->cursor += varLength;
                                // ignore text for now
                                trackPlayback->cursor += textLength-1;
                            } break;
                            case MIDI_EVENT_CODE_MARKER: {
                                ++trackPlayback->cursor;
                                
                                int varLength;
                                unsigned int textLength = getVariableLengthValue(trackPlayback->cursor, &varLength);
                                textLength = bigEndianToLittleEndian(textLength);
                                trackPlayback->cursor += varLength;
                                // ignore text for now
                                trackPlayback->cursor += textLength-1;
                            } break;
                            case MIDI_EVENT_CODE_CUE_POINT: {
                                ++trackPlayback->cursor;
                                
                                int varLength;
                                unsigned int textLength = getVariableLengthValue(trackPlayback->cursor, &varLength);
                                textLength = bigEndianToLittleEndian(textLength);
                                trackPlayback->cursor += varLength;
                                // ignore text for now
                                trackPlayback->cursor += textLength-1;
                            } break;
                            case MIDI_EVENT_CODE_MIDI_CHANNEL_PREFIX: {
                                ++trackPlayback->cursor;
                                unsigned char numberOne = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                unsigned char channelNum = *trackPlayback->cursor;
                                // ignore for now
                            } break;
                            case MIDI_EVENT_CODE_END_OF_TRACK: {
                                ++trackPlayback->cursor;
                                unsigned char numberZero = *trackPlayback->cursor;
                                trackPlayback->complete = true;
                            } break;
                            case MIDI_EVENT_CODE_SET_TEMPO: {
                                ++trackPlayback->cursor;
                                unsigned char numberThree = *trackPlayback->cursor;

                                ++trackPlayback->cursor;
                                unsigned char highByte = *trackPlayback->cursor;

                                ++trackPlayback->cursor;
                                unsigned char midByte = *trackPlayback->cursor;

                                ++trackPlayback->cursor;
                                unsigned char lowByte = *trackPlayback->cursor;

                                unsigned int tempo = 0;
                                tempo += (highByte << 16);
                                tempo += (midByte << 8);
                                tempo += (lowByte);

                                double beatsPerSecond = 1000000.0 / ((double)tempo);
                                double ticksPerSecond = (double)bgmState->ticksPerQuarterNote * beatsPerSecond;
                                bgmState->tickDuration = 1.0 / ticksPerSecond;
                            } break;
                            case MIDI_EVENT_CODE_SMPTE_OFFSET: {
                                ++trackPlayback->cursor;
                                unsigned char numberFive = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                unsigned char hours = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                unsigned char minutes = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                unsigned char seconds = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                unsigned char frame = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                unsigned char fracFrame = *trackPlayback->cursor;
                                // not used for now
                            } break;
                            case MIDI_EVENT_CODE_TIME_SIGNATURE: {
                                ++trackPlayback->cursor;
                                unsigned char numberFour = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                unsigned char numerator = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                unsigned char denominator = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                unsigned char clocksPerClick = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                unsigned char numNotes = *trackPlayback->cursor;
                                // not used for now
                            } break;
                            case MIDI_EVENT_CODE_KEY_SIGNATURE: {
                                ++trackPlayback->cursor;
                                unsigned char numberTwo = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                char sharpsFlats = *trackPlayback->cursor;
                                ++trackPlayback->cursor;
                                unsigned char majorMinor = *trackPlayback->cursor;
                                // not used for now
                            } break;
                            case MIDI_EVENT_CODE_SEQUENCER_SPECIFIC_META_EVENT: {
                                ++trackPlayback->cursor;
                                
                                int varLength;
                                unsigned int dataLength = getVariableLengthValue(trackPlayback->cursor, &varLength);
                                dataLength = bigEndianToLittleEndian(dataLength);
                                trackPlayback->cursor += varLength;
                                // ignore data for now
                                trackPlayback->cursor += dataLength-1;
                            } break;
                        }
                    }
                    else {
                        // ignore these for now
                        switch (eventCode) {
                            case MIDI_EVENT_CODE_SONG_POSITION_POINTER: {
                                // not supported, skip
                                ++trackPlayback->cursor;
                                ++trackPlayback->cursor;
                            } break;
                            case MIDI_EVENT_CODE_SONG_SELECT: {
                                // not supported, skip
                                ++trackPlayback->cursor;
                            } break;
                            case MIDI_EVENT_CODE_TUNE_REQUEST: {
                                // not supported, skip
                            } break;
                            case MIDI_EVENT_CODE_END_OF_EXCLUSIVE: {
                                // not supported, skip
                            } break;
                            case MIDI_EVENT_CODE_TIMING_CLOCK: {
                                // not supported, skip
                            } break;
                            case MIDI_EVENT_CODE_START: {
                                // not supported, skip
                            } break;
                            case MIDI_EVENT_CODE_CONTINUE: {
                                // not supported, skip
                            } break;
                            case MIDI_EVENT_CODE_STOP: {
                                // not supported, skip
                            } break;
                            case MIDI_EVENT_CODE_ACTIVE_SENSING: {
                                // not supported, skip
                            } break;
                        }
                    }

                    if (trackPlayback->complete) {
                        break;
                    }
                    else {
                        ++trackPlayback->cursor;
                        trackPlayback->lastCode = (midi_event_code)eventCode;
                        trackPlayback->nextEventDeltaTime = bigEndianToLittleEndian(getVariableLengthValue(trackPlayback->cursor, &length));
                    }
                }
            }
        }
        if (allTracksComplete) {
            bgmState->playing = false;
            for (int channelIndex = 0; channelIndex < NUM_MIDI_CHANNELS; ++channelIndex) {
                midi_channel *channel = &bgmState->channels[channelIndex];
                for (int soundIndex = 0; soundIndex < channel->numPlayingSounds; ++soundIndex) {
                    synth_sound *sound = channel->instrumentSounds + soundIndex;
                    sound->pressed = false;
                }
            }
        }
        for (int channelIndex = 0; channelIndex < NUM_MIDI_CHANNELS; ++channelIndex) {
            midi_channel *channel = &bgmState->channels[channelIndex];
            for (int soundIndex = 0; soundIndex < channel->numPlayingSounds; ++soundIndex) {
                synth_sound *sound = channel->instrumentSounds + soundIndex;
                sound_instrument *instrument = gameSounds->instruments + sound->instrumentType;
                if (sound->active) {
                    sampleValue += updateInstrument(sound, instrument, (float)dt);
                }
                else {
                    synth_sound lastSound = channel->instrumentSounds[channel->numPlayingSounds - 1];
                    channel->instrumentSounds[channel->numPlayingSounds - 1] = *sound;
                    channel->instrumentSounds[soundIndex] = lastSound;
                    --soundIndex;
                    --channel->numPlayingSounds;
                }
            }
        }
        float volume = 1.0f;
        sampleValue = sampleValue > 1.0f ? 1.0f : sampleValue;
        sampleValue = sampleValue < -1.0f ? -1.0f : sampleValue;
        sampleOut->value = sampleValue * volume;
        assert(sampleOut->value >= -1.0f && sampleOut->value <= 1.0f);
        ++sampleOut;
    }
}

