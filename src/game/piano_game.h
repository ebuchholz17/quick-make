#ifndef PIANO_GAME_H
#define PIANO_GAME_H

// TODO(ebuchholz): whole range
enum sound_note {
    SOUND_NOTE_C,
    SOUND_NOTE_C_SHARP,
    SOUND_NOTE_D,
    SOUND_NOTE_D_SHARP,
    SOUND_NOTE_E,
    SOUND_NOTE_F,
    SOUND_NOTE_F_SHARP,
    SOUND_NOTE_G,
    SOUND_NOTE_G_SHARP,
    SOUND_NOTE_A,
    SOUND_NOTE_A_SHARP,
    SOUND_NOTE_B,
    SOUND_NOTE_C_2,
    SOUND_NOTE_COUNT
};

struct piano_game_note {
    int playingSoundIndex;
    bool playing;
};

struct piano_game {
    piano_game_note notes[SOUND_NOTE_COUNT];
};

#endif
