#ifndef QUICKMAKE_AUDIO_H
#define QUICKMAKE_AUDIO_H

#define MAX_SOUNDS 16
#define MAX_WAVEFORMS 10

struct sound_envelope {
    float attackTime;
    float attackVolume;
    float decayTime;
    bool sustain;
    float sustainVolume;
    float releaseTime;
};

enum sound_state {
    SOUND_STATE_ATTACK,
    SOUND_STATE_DECAY,
    SOUND_STATE_SUSTAIN,
    SOUND_STATE_RELEASE
};

enum oscillator_type {
    OSCILLATOR_TYPE_SINE,
    OSCILLATOR_TYPE_TRIANGLE,
    OSCILLATOR_TYPE_SQUARE,
    OSCILLATOR_TYPE_SAWTOOTH,
    OSCILLATOR_TYPE_NOISE
};

struct sound_waveform {
    oscillator_type waveType;
    float volume;
    float muliplier;
};

enum instrument_type {
    INSTRUMENT_TYPE_PIANO,
    //INSTRUMENT_TYPE_HARMONICA,
    //INSTRUMENT_TYPE_SNARE_DRUM,
    INSTRUMENT_TYPE_COUNT
};

struct sound_instrument {
    instrument_type type;
    sound_waveform waveforms[MAX_WAVEFORMS];
    int numWaveForms;
    sound_envelope envelope;
};

struct synth_sound {
    instrument_type instrumentType;
    sound_state state;
    float t;
    float releaseTime;
    float hz;

    bool pressed;
    bool active;
};

struct game_sounds {
    sound_instrument instruments[INSTRUMENT_TYPE_COUNT];
    synth_sound sounds[MAX_SOUNDS];
};

#endif
