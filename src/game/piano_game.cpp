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
        return input->aKey.justPressed || input->pointerJustDown;
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
        return input->aKey.down || input->pointerDown;
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

void updatePianoGame (game_sounds *gameSounds, game_assets *assets, game_input *input, piano_game *pianoGame, sprite_list *spriteList) {
    for (int i = 0; i < SOUND_NOTE_COUNT; ++i) {
        piano_game_note *pianoNote = pianoGame->notes + i;

        if (!pianoNote->playing) {
            if (keyPressedForNote(input, (sound_note)i)) {
                int soundIndex = playSound(gameSounds, INSTRUMENT_TYPE_PIANO, noteHz((sound_note)i));
                if (soundIndex != -1) {
                    pianoNote->playing = true;
                    pianoNote->playingSoundIndex = soundIndex;
                }
            }
        }
        else {
            synth_sound *sound = gameSounds->sounds + pianoNote->playingSoundIndex;
            if (!sound->active) {
                pianoNote->playing = false;
                pianoNote->playingSoundIndex = -1;
            }
            else {
                if (keyJustPressedForNote(input, (sound_note)i)) {
                    sound->pressed = false;
                    int soundIndex = playSound(gameSounds, INSTRUMENT_TYPE_PIANO, noteHz((sound_note)i));
                    if (soundIndex != -1) {
                        pianoNote->playing = true;
                        pianoNote->playingSoundIndex = soundIndex;
                    }
                }
                if (!keyPressedForNote(input, (sound_note)i)) {
                    sound->pressed = false;
                }
            }
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
        char *frameName = 0;
        if (pianoNote->playing) {
            synth_sound *sound = gameSounds->sounds + pianoNote->playingSoundIndex;
            frameName = sound->pressed ? "white_key_pressed" : "white_key";
        }
        else {
            frameName = "white_key";
        }
        addSprite(i * 48.0f, 0.0f, assets, ATLAS_KEY_GAME, frameName, spriteList);
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
        char *frameName = 0;
        if (pianoNote->playing) {
            synth_sound *sound = gameSounds->sounds + pianoNote->playingSoundIndex;
            frameName = sound->pressed ? "black_key_pressed" : "black_key";
        }
        else {
            frameName = "black_key";
        }
        int offset = 36;
        if (i > 1) { offset += 48; }
        addSprite(i * 48.0f + offset, 0.0f, assets, ATLAS_KEY_GAME, frameName, spriteList);
    }
    //addSprite(GRID_COL_START + GRID_BLOCK_WIDTH * j, GRID_ROW_START + GRID_BLOCK_HEIGHT * i, assets, ATLAS_KEY_GAME, "tile_backing", spriteList, 0.5f, 0.5f);
    //if (pianoGame->playingSoundIndex == -1) {
    //    if (input->pointerJustDown) {
    //        int soundIndex = playSound(gameSounds, INSTRUMENT_TYPE_PIANO, 440);
    //        if (soundIndex != -1) {
    //            pianoGame->playingSoundIndex = soundIndex;
    //        }
    //    }
    //}
    //else {
    //    synth_sound *sound = gameSounds->sounds + pianoGame->playingSoundIndex;
    //    if (!sound->active) {
    //        pianoGame->playingSoundIndex = -1;
    //    }
    //    else {
    //        if (input->pointerJustDown) {
    //            sound->pressed = false;
    //            int soundIndex = playSound(gameSounds, INSTRUMENT_TYPE_PIANO, 440);
    //            if (soundIndex != -1) {
    //                pianoGame->playingSoundIndex = soundIndex;
    //            }
    //        }
    //        if (!input->pointerDown) {
    //            sound->pressed = false;
    //        }
    //    }
    //}
}
