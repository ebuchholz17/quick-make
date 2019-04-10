#include "piano_game.h"

void initPianoGame (piano_game *pianoGame) {
    pianoGame->playingSoundIndex = -1;
}

void updatePianoGame (game_sounds *gameSounds, game_input *input, piano_game *pianoGame) {
    if (pianoGame->playingSoundIndex == -1) {
        if (input->pointerJustDown) {
            int soundIndex = playSound(gameSounds, INSTRUMENT_TYPE_PIANO);
            if (soundIndex != -1) {
                pianoGame->playingSoundIndex = soundIndex;
            }
        }
    }
    else {
        synth_sound *sound = gameSounds->sounds + pianoGame->playingSoundIndex;
        if (!sound->active) {
            pianoGame->playingSoundIndex = -1;
        }
        else {
            if (input->pointerJustDown) {
                sound->pressed = false;
                int soundIndex = playSound(gameSounds, INSTRUMENT_TYPE_PIANO);
                if (soundIndex != -1) {
                    pianoGame->playingSoundIndex = soundIndex;
                }
            }
            if (!input->pointerDown) {
                sound->pressed = false;
            }
        }
    }
}
