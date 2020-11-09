#include "piano_game.h"

void initPianoGame (piano_game *pianoGame) {
    for (int i = 0; i < SOUND_NOTE_COUNT; ++i) {
        *(pianoGame->notes + i) = {};
    }
}

bool keyJustPressedForNote (game_input *input, sound_note soundNote) {
    switch (soundNote) {
    // NOTE(ebuchholz): special case for testing touch
    case SOUND_NOTE_C:
        return input->aKey.justPressed;
    case SOUND_NOTE_C_SHARP:
        return input->wKey.justPressed;
    case SOUND_NOTE_D:
        return input->sKey.justPressed;
    case SOUND_NOTE_D_SHARP:
        return input->eKey.justPressed;
    case SOUND_NOTE_E:
        return input->dKey.justPressed;
    case SOUND_NOTE_F:
        return input->fKey.justPressed;
    case SOUND_NOTE_F_SHARP:
        return input->tKey.justPressed;
    case SOUND_NOTE_G:
        return input->gKey.justPressed;
    case SOUND_NOTE_G_SHARP:
        return input->yKey.justPressed;
    case SOUND_NOTE_A:
        return input->hKey.justPressed;
    case SOUND_NOTE_A_SHARP:
        return input->uKey.justPressed;
    case SOUND_NOTE_B:
        return input->jKey.justPressed;
    case SOUND_NOTE_C_2:
        return input->kKey.justPressed;
    }
    return false;
}

bool keyPressedForNote (game_input *input, sound_note soundNote) {
    switch (soundNote) {
    // NOTE(ebuchholz): special case for testing touch
    case SOUND_NOTE_C:
        return input->aKey.down;
    case SOUND_NOTE_C_SHARP:
        return input->wKey.down;
    case SOUND_NOTE_D:
        return input->sKey.down;
    case SOUND_NOTE_D_SHARP:
        return input->eKey.down;
    case SOUND_NOTE_E:
        return input->dKey.down;
    case SOUND_NOTE_F:
        return input->fKey.down;
    case SOUND_NOTE_F_SHARP:
        return input->tKey.down;
    case SOUND_NOTE_G:
        return input->gKey.down;
    case SOUND_NOTE_G_SHARP:
        return input->yKey.down;
    case SOUND_NOTE_A:
        return input->hKey.down;
    case SOUND_NOTE_A_SHARP:
        return input->uKey.down;
    case SOUND_NOTE_B:
        return input->jKey.down;
    case SOUND_NOTE_C_2:
        return input->kKey.down;
    }
    return false;
}

float noteHz (sound_note soundNote) {
    switch (soundNote) {
    case SOUND_NOTE_C:
        return 261.6256f;
    case SOUND_NOTE_C_SHARP:
        return 277.1826f;
    case SOUND_NOTE_D:
        return 293.6648f;
    case SOUND_NOTE_D_SHARP:
        return 311.1270f;
    case SOUND_NOTE_E:
        return 329.6276f;
    case SOUND_NOTE_F:
        return 349.2282f;
    case SOUND_NOTE_F_SHARP:
        return 369.9944f;
    case SOUND_NOTE_G:
        return 391.9954f;
    case SOUND_NOTE_G_SHARP:
        return 415.3047f;
    case SOUND_NOTE_A:
        return 440.0f;
    case SOUND_NOTE_A_SHARP:
        return 466.1638f;
    case SOUND_NOTE_B:
        return 493.8833f;
    case SOUND_NOTE_C_2:
        return 523.2511f;
    }
    return 0;
}

int noteTappedByPointer (float pointerX, float pointerY) {
    // black keys
    sound_note blackKeys[] = {
        SOUND_NOTE_C_SHARP,
        SOUND_NOTE_D_SHARP,
        SOUND_NOTE_F_SHARP,
        SOUND_NOTE_G_SHARP,
        SOUND_NOTE_A_SHARP
    };
    for (int i = 0; i < 5; ++i) {
        int offset = 36;
        if (i > 1) { offset += 48; }
        float startX = i * 48.0f + offset;

        if (pointerX >= startX && pointerX < startX + 24.0f && pointerY >= 0.0f && pointerY < 129.0f) {
            return blackKeys[i];
        }
    }

    sound_note whiteKeys[] = {
        SOUND_NOTE_C,
        SOUND_NOTE_D,
        SOUND_NOTE_E,
        SOUND_NOTE_F,
        SOUND_NOTE_G,
        SOUND_NOTE_A,
        SOUND_NOTE_B,
        SOUND_NOTE_C_2
    };
    for (int i = 0; i < 8; ++i) {
        float startX = i * 48.0f;
        if (pointerX >= startX && pointerX < startX + 48.0f && pointerY >= 0.0f && pointerY < 216.0f) {
            return whiteKeys[i];
        }
    }
    return -1;
}

void processKey (piano_game_note *pianoNote, sound_note noteIndex, game_sounds *gameSounds, bool keyPressed) {
    if (!pianoNote->playing) {
        if (keyPressed) {
            int soundIndex = playInstrument(gameSounds, INSTRUMENT_TYPE_PIANO, noteHz(noteIndex));
            if (soundIndex != -1) {
                pianoNote->playing = true;
                pianoNote->playingSoundIndex = soundIndex;
            }
        }
    }
    else {
        synth_sound *sound = gameSounds->instrumentSounds + pianoNote->playingSoundIndex;
        if (!sound->active) {
            pianoNote->playing = false;
            pianoNote->playingSoundIndex = -1;
        }
        else {
            if (!sound->pressed && keyPressed) {
                sound->pressed = false;
                int soundIndex = playInstrument(gameSounds, INSTRUMENT_TYPE_PIANO, noteHz(noteIndex));
                if (soundIndex != -1) {
                    pianoNote->playing = true;
                    pianoNote->playingSoundIndex = soundIndex;
                }
            }
            if (!keyPressed) {
                sound->pressed = false;
            }
        }
    }
}

void updatePianoGame (game_sounds *gameSounds, game_assets *assets, game_input *input, piano_game *pianoGame, sprite_list *spriteList, memory_arena *tempMemory) {
    matrix3x3 gameTransform = peekSpriteMatrix(spriteList);
    vector3 localPointerPos = Vector3((float)input->pointerX, (float)input->pointerY, 1.0f);
    localPointerPos = inverse(gameTransform) * localPointerPos;

    // avoid processing keys twice
    bool keysTappedByPointer[SOUND_NOTE_COUNT] = {};
    if (input->pointerDown) {
        int pianoNoteIndex = noteTappedByPointer(localPointerPos.x, localPointerPos.y);
        if (pianoNoteIndex != -1) {
            if (input->pointerJustDown) {
                if (input->pointerJustDown) {
                    //playSound("menu_button", gameSounds);
                    playSound(numToString(pianoNoteIndex, tempMemory), gameSounds, assets);
                }
            }   
            else {

                //keysTappedByPointer[pianoNoteIndex] = true;
                //piano_game_note *pianoNote = pianoGame->notes + pianoNoteIndex;
                //processKey(pianoNote, (sound_note)pianoNoteIndex, gameSounds, true);
            }
        }
    }
    for (int i = 0; i < SOUND_NOTE_COUNT; ++i) {
        if (!keysTappedByPointer[i]) {
            piano_game_note *pianoNote = pianoGame->notes + i;
            bool keyPressed = keyPressedForNote(input, (sound_note)i);
            processKey(pianoNote, (sound_note)i, gameSounds, keyPressed);
        }
    }

    // draw keys
    sound_note whiteKeys[] = {
        SOUND_NOTE_C,
        SOUND_NOTE_D,
        SOUND_NOTE_E,
        SOUND_NOTE_F,
        SOUND_NOTE_G,
        SOUND_NOTE_A,
        SOUND_NOTE_B,
        SOUND_NOTE_C_2
    };
    for (int i = 0; i < 8; ++i) {
        piano_game_note *pianoNote = pianoGame->notes + whiteKeys[i];
        const char *frameName = 0;
        if (pianoNote->playing) {
            synth_sound *sound = gameSounds->instrumentSounds + pianoNote->playingSoundIndex;
            frameName = sound->pressed ? "white_key_pressed" : "white_key";
        }
        else {
            frameName = "white_key";
        }
        addSprite(i * 48.0f, 0.0f, assets, "atlas", (char *)frameName, spriteList);
    }

    sound_note blackKeys[] = {
        SOUND_NOTE_C_SHARP,
        SOUND_NOTE_D_SHARP,
        SOUND_NOTE_F_SHARP,
        SOUND_NOTE_G_SHARP,
        SOUND_NOTE_A_SHARP
    };
    for (int i = 0; i < 5; ++i) {
        piano_game_note *pianoNote = pianoGame->notes + blackKeys[i];
        const char *frameName = 0;
        if (pianoNote->playing) {
            synth_sound *sound = gameSounds->instrumentSounds + pianoNote->playingSoundIndex;
            frameName = sound->pressed ? "black_key_pressed" : "black_key";
        }
        else {
            frameName = "black_key";
        }
        int offset = 36;
        if (i > 1) { offset += 48; }
        addSprite(i * 48.0f + offset, 0.0f, assets, "atlas", (char *)frameName, spriteList);
    }
    addSprite(localPointerPos.x, localPointerPos.y, assets, "atlas", "sheep", spriteList, 0.5f, 0.5f);
}
