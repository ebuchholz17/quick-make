#ifndef SOUND_EDITOR_H
#define SOUND_EDITOR_H

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

struct sound_editor_note {
    int playingSoundIndex;
    bool playing;
};

struct sound_editor {
    bool initialized;
    sound_editor_note notes[SOUND_NOTE_COUNT];
};

#endif
