#ifndef QUICKMAKE_AUDIO_H
#define QUICKMAKE_AUDIO_H

#define MAX_INSTRUMENT_SOUNDS 64
#define MAX_PLAYING_SOUNDS 16
#define MAX_WAVEFORMS 10

#define NUM_PITCHES 128
#define NUM_MIDI_CHANNELS 16

static float midiNoteFrequencies[] = {
    8.175798916f,
    8.661957218f,
    9.177023997f,
    9.722718241f,
    10.30086115f,
    10.91338223f,
    11.56232571f,
    12.24985737f,
    12.9782718f,
    13.75f,
    14.56761755f,
    15.43385316f,
    16.35159783f,
    17.32391444f,
    18.35404799f,
    19.44543648f,
    20.60172231f,
    21.82676446f,
    23.12465142f,
    24.49971475f,
    25.9565436f,
    27.5f,
    29.13523509f,
    30.86770633f,
    32.70319566f,
    34.64782887f,
    36.70809599f,
    38.89087297f,
    41.20344461f,
    43.65352893f,
    46.24930284f,
    48.9994295f,
    51.9130872f,
    55.0f,
    58.27047019f,
    61.73541266f,
    65.40639133f,
    69.29565774f,
    73.41619198f,
    77.78174593f,
    82.40688923f,
    87.30705786f,
    92.49860568f,
    97.998859f,
    103.8261744f,
    110.0f,
    116.5409404f,
    123.4708253f,
    130.8127827f,
    138.5913155f,
    146.832384f,
    155.5634919f,
    164.8137785f,
    174.6141157f,
    184.9972114f,
    195.997718f,
    207.6523488f,
    220.0f,
    233.0818808f,
    246.9416506f,
    261.6255653f,
    277.182631f,
    293.6647679f,
    311.1269837f,
    329.6275569f,
    349.2282314f,
    369.9944227f,
    391.995436f,
    415.3046976f,
    440.0f,
    466.1637615f,
    493.8833013f,
    523.2511306f,
    554.365262f,
    587.3295358f,
    622.2539674f,
    659.2551138f,
    698.4564629f,
    739.9888454f,
    783.990872f,
    830.6093952f,
    880.0f,
    932.327523f,
    987.7666025f,
    1046.502261f,
    1108.730524f,
    1174.659072f,
    1244.507935f,
    1318.510228f,
    1396.912926f,
    1479.977691f,
    1567.981744f,
    1661.21879f,
    1760.0f,
    1864.655046f,
    1975.533205f,
    2093.004522f,
    2217.461048f,
    2349.318143f,
    2489.01587f,
    2637.020455f,
    2793.825851f,
    2959.955382f,
    3135.963488f,
    3322.437581f,
    3520.0f,
    3729.310092f,
    3951.06641f,
    4186.009045f,
    4434.922096f,
    4698.636287f,
    4978.03174f,
    5274.040911f,
    5587.651703f,
    5919.910763f,
    6271.926976f,
    6644.875161f,
    7040.0f,
    7458.620184f,
    7902.13282f,
    8372.01809f,
    8869.844191f,
    9397.272573f,
    9956.063479f,
    10548.08182f,
    11175.30341f,
    11839.82153f,
    12543.85395f,
};

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

enum sound_filter_type {
    SOUND_FILTER_TYPE_NONE,
    SOUND_FILTER_TYPE_LOW_PASS,
    SOUND_FILTER_TYPE_HIGH_PASS,
    SOUND_FILTER_TYPE_BAND_PASS
};

struct sound_filter {
    sound_filter_type type;
    float cutoff;
    float resonance;
    float feedback;
};

enum instrument_type {
    INSTRUMENT_TYPE_PIANO,
    INSTRUMENT_TYPE_TRUMPET,
    INSTRUMENT_TYPE_GUITAR,
    INSTRUMENT_TYPE_VIOLIN,
    INSTRUMENT_TYPE_FLUTE,
    INSTRUMENT_TYPE_SNARE_DRUM,
    INSTRUMENT_TYPE_COUNT
};

struct sound_instrument {
    instrument_type type;
    sound_waveform waveforms[MAX_WAVEFORMS];
    int numWaveForms;
    sound_envelope envelope;
    sound_filter filter;
};

struct synth_sound {
    instrument_type instrumentType;
    sound_state state;
    float t;
    float cycleT;
    float releaseTime;
    float hz;
    float period;
    unsigned char midiNote;

    bool pressed;
    bool active;

    float bufferedVal0;
    float bufferedVal1;
};

struct playing_sound {
    char *key;
    int currentSample;
};

enum midi_event_code {
    MIDI_EVENT_CODE_NOTE_OFF_EVENT = 0x08,
    MIDI_EVENT_CODE_NOTE_ON_EVENT = 0x09,
    MIDI_EVENT_CODE_POLYPHONIC_KEY_PRESSURE = 0x0a,
    MIDI_EVENT_CODE_CONTROL_CHANGE = 0x0b,
    MIDI_EVENT_CODE_PROGRAM_CHANGE = 0x0c,
    MIDI_EVENT_CODE_CHANNEL_PRESSURE = 0x0d,
    MIDI_EVENT_CODE_PITCH_WHEEL_CHANGE = 0x0e,
    MIDI_EVENT_CODE_SYSTEM_EXCLUSIVE = 0xf0,
    MIDI_EVENT_CODE_SONG_POSITION_POINTER = 0xf2,
    MIDI_EVENT_CODE_SONG_SELECT = 0xf3,
    MIDI_EVENT_CODE_TUNE_REQUEST = 0xf6,
    MIDI_EVENT_CODE_END_OF_EXCLUSIVE = 0xf7,
    MIDI_EVENT_CODE_TIMING_CLOCK = 0xf8,
    MIDI_EVENT_CODE_START = 0xfa,
    MIDI_EVENT_CODE_CONTINUE = 0xfb,
    MIDI_EVENT_CODE_STOP = 0xfc,
    MIDI_EVENT_CODE_ACTIVE_SENSING = 0xfe,
    //MIDI_EVENT_CODE_RESET = 0x00,
    MIDI_EVENT_CODE_META_EVENT = 0xff,

    MIDI_EVENT_CODE_SEQUENCE_NUMBER = 0x00,
    MIDI_EVENT_CODE_TEXT_EVENT = 0x01,
    MIDI_EVENT_CODE_COPYRIGHT_NOTICE = 0x02,
    MIDI_EVENT_CODE_SEQUENCE_TRACK_NAME = 0x03,
    MIDI_EVENT_CODE_INSTRUMENT_NAME = 0x04,
    MIDI_EVENT_CODE_LYRIC = 0x05,
    MIDI_EVENT_CODE_MARKER = 0x06,
    MIDI_EVENT_CODE_CUE_POINT = 0x07,
    MIDI_EVENT_CODE_MIDI_CHANNEL_PREFIX = 0x20,
    MIDI_EVENT_CODE_END_OF_TRACK = 0x2f,
    MIDI_EVENT_CODE_SET_TEMPO = 0x51,
    MIDI_EVENT_CODE_SMPTE_OFFSET = 0x54,
    MIDI_EVENT_CODE_TIME_SIGNATURE = 0x58,
    MIDI_EVENT_CODE_KEY_SIGNATURE = 0x59,
    MIDI_EVENT_CODE_SEQUENCER_SPECIFIC_META_EVENT = 0x7f
};

struct midi_track_playback {
    char *cursor;
    unsigned int ticksSinceLastEvent;
    unsigned int nextEventDeltaTime;
    double time;
    midi_event_code lastCode;
    bool complete;
};

struct midi_channel {
    instrument_type instrument;
    synth_sound instrumentSounds[NUM_PITCHES];
    int numPlayingSounds;
};

struct midi_playback {
    bool playing;
    int ticksPerQuarterNote;
    double tickDuration;
    int numTracks;
    midi_track_playback tracks[MAX_MIDI_TRACKS];
    midi_channel channels[NUM_MIDI_CHANNELS];
};

struct game_sounds {
    sound_instrument instruments[INSTRUMENT_TYPE_COUNT];
    synth_sound instrumentSounds[MAX_INSTRUMENT_SOUNDS];
    playing_sound playingSounds[MAX_PLAYING_SOUNDS];
    int numPlayingSounds;

    midi_playback bgmState;
};

#endif
