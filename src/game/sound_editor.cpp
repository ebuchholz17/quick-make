#include "sound_editor.h"

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

void processKey (sound_editor_note *note, sound_note noteIndex, game_sounds *gameSounds, bool keyPressed) {
    if (!note->playing) {
        if (keyPressed) {
            int soundIndex = playInstrument(gameSounds, INSTRUMENT_TYPE_PIANO, noteHz(noteIndex));
            if (soundIndex != -1) {
                note->playing = true;
                note->playingSoundIndex = soundIndex;
            }
        }
    }
    else {
        synth_sound *sound = gameSounds->instrumentSounds + note->playingSoundIndex;
        if (!sound->active) {
            note->playing = false;
            note->playingSoundIndex = -1;
        }
        else {
            if (!sound->pressed && keyPressed) {
                sound->pressed = false;
                int soundIndex = playInstrument(gameSounds, INSTRUMENT_TYPE_PIANO, noteHz(noteIndex));
                if (soundIndex != -1) {
                    note->playing = true;
                    note->playingSoundIndex = soundIndex;
                }
            }
            if (!keyPressed) {
                sound->pressed = false;
            }
        }
    }
}

struct status_text_pos {
    float x;
    float y;
};

void showFloatValue (status_text_pos *currentPos, char *text, float value, float offset, game_assets *assets, sprite_list *spriteList, memory_arena *stringMemory) {
    addText(currentPos->x, currentPos->y, text, assets, TEXTURE_KEY_FONT, spriteList);

    bool negative = false;
    if (value < 0) {
        negative = true;
    }
    int intValue = (int)(value * 1000);
    if (intValue < 0) {
        intValue = -intValue;
    }
    int integerPart = intValue / 1000;
    char *intString = appendString(numToString(integerPart, stringMemory), ".", stringMemory);

    int fractionalPart = intValue % 1000;
    char *fracString = numToString(fractionalPart, stringMemory);

    if (fractionalPart == 0) {
        fracString = "000";
    }
    else {
        int temp = fractionalPart;
        while (temp < 100) {
            fracString = appendString("0", fracString, stringMemory);
            temp *= 10;
        }
    }

    char *number = appendString(intString, fracString, stringMemory);
    if (negative) {
        number = appendString("-", number, stringMemory);
    }
    addText(currentPos->x + offset, currentPos->y, number, assets, TEXTURE_KEY_FONT, spriteList);
    currentPos->y += 24;
}

void showIntValue (status_text_pos *currentPos, char *text, int value, float offset, game_assets *assets, sprite_list *spriteList, memory_arena *stringMemory) {
    addText(currentPos->x, currentPos->y, text, assets, TEXTURE_KEY_FONT, spriteList);

    char *number = numToString(value, stringMemory);
    addText(currentPos->x + offset, currentPos->y, number, assets, TEXTURE_KEY_FONT, spriteList);
    currentPos->y += 24;
}

void showWaveType (status_text_pos *currentPos, char *text, int value, float offset, game_assets *assets, sprite_list *spriteList, memory_arena *stringMemory) {
    addText(currentPos->x, currentPos->y, text, assets, TEXTURE_KEY_FONT, spriteList);

    char *waveText;
    switch (value) {
    default:
    case OSCILLATOR_TYPE_SINE:
        waveText = "sine";
        break;
    case OSCILLATOR_TYPE_TRIANGLE:
        waveText = "triangle";
        break;
    case OSCILLATOR_TYPE_SQUARE:
        waveText = "square";
        break;
    case OSCILLATOR_TYPE_SAWTOOTH:
        waveText = "sawtooth";
        break;
    case OSCILLATOR_TYPE_NOISE:
        waveText = "noise";
        break;
    }
    addText(currentPos->x + offset, currentPos->y, waveText, assets, TEXTURE_KEY_FONT, spriteList);
    currentPos->y += 24;
}

void showFilterType (status_text_pos *currentPos, char *text, int value, float offset, game_assets *assets, sprite_list *spriteList, memory_arena *stringMemory) {
    addText(currentPos->x, currentPos->y, text, assets, TEXTURE_KEY_FONT, spriteList);

    char *waveText;
    switch (value) {
    default:
    case SOUND_FILTER_TYPE_NONE:
        waveText = "none";
        break;
    case SOUND_FILTER_TYPE_LOW_PASS:
        waveText = "low pass";
        break;
    case SOUND_FILTER_TYPE_HIGH_PASS:
        waveText = "high pass";
        break;
    case SOUND_FILTER_TYPE_BAND_PASS:
        waveText = "band pass";
        break;
    }
    addText(currentPos->x + offset, currentPos->y, waveText, assets, TEXTURE_KEY_FONT, spriteList);
    currentPos->y += 24;
}

void showBoolValue (status_text_pos *currentPos, char *text, bool value, float offset, game_assets *assets, sprite_list *spriteList, memory_arena *stringMemory) {
    addText(currentPos->x, currentPos->y, text, assets, TEXTURE_KEY_FONT, spriteList);

    char *trueFalse;
    if (value) {
        trueFalse = "true";
    }
    else {
        trueFalse = "false";
    }
    addText(currentPos->x + offset, currentPos->y, trueFalse, assets, TEXTURE_KEY_FONT, spriteList);
    currentPos->y += 24;
}

bool doButton (float x, float y, float width, float height, bool pointerJustDown, float pointerX, float pointerY, 
               game_assets *assets, atlas_key atlasKey, char *frame, sprite_list *spriteList) 
{
    bool inBox = false;
    if (pointerX >= x && pointerX < x + width && pointerY >= y && pointerY < y + height) {
        inBox = true;
    }

    unsigned int tint = 0xffffff;
    bool clicked = false;
    if (inBox) {
        if (pointerJustDown) {
            clicked = true;
            tint = 0xbbbbbb;
        }
        else {
            tint = 0xdddddd;
        }
    }
    addSprite(x, y, assets, atlasKey, frame, spriteList, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, tint);

    return clicked;
}

void doEditArrows (status_text_pos *currentPos, float *value, float inc, bool pointerJustDown, float pointerX, float pointerY, game_assets *assets, sprite_list *spriteList) {
    if (doButton(currentPos->x, currentPos->y, 16, 16, pointerJustDown, pointerX, pointerY,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "arrow_left_button", spriteList)) 
    {
        *value = *value - inc;
    }
    currentPos->x += 24;
    if (doButton(currentPos->x, currentPos->y, 16, 16, pointerJustDown, pointerX, pointerY,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "arrow_right_button", spriteList)) 
    {
        *value = *value + inc;
    }
    currentPos->x += 20;
}

void doIntArrows (status_text_pos *currentPos, int *value, bool pointerJustDown, float pointerX, float pointerY, game_assets *assets, sprite_list *spriteList) {
    if (doButton(currentPos->x, currentPos->y, 16, 16, pointerJustDown, pointerX, pointerY,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "arrow_left_button", spriteList)) 
    {
        *value = *value - 1;
    }
    currentPos->x += 24;
    if (doButton(currentPos->x, currentPos->y, 16, 16, pointerJustDown, pointerX, pointerY,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "arrow_right_button", spriteList)) 
    {
        *value = *value + 1;
    }
    currentPos->x += 20;
}

void doBoolButton (status_text_pos *currentPos, bool *value, bool pointerJustDown, float pointerX, float pointerY, game_assets *assets, sprite_list *spriteList) {
    if (doButton(currentPos->x, currentPos->y, 16, 16, pointerJustDown, pointerX, pointerY,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "arrow_down_button", spriteList)) 
    {
        *value = !*value;
    }
    currentPos->x += 20;
}

void updateSoundEditor (memory_arena *memory, memory_arena *tempMemory, sound_editor *soundEditor,
                        game_assets *assets, game_input *input, sprite_list *spriteList, game_sounds *gameSounds, render_command_list *renderCommands)
{
    memory_arena stringMemory = {};
    stringMemory.capacity = 512 * 1024;
    stringMemory.base = allocateMemorySize(tempMemory, stringMemory.capacity);

    pushSpriteTransform(spriteList, Vector2(), 0.5f);

    matrix3x3 gameTransform = peekSpriteMatrix(spriteList);
    vector3 localPointerPos = Vector3((float)input->pointerX, (float)input->pointerY, 1.0f);
    localPointerPos = inverse(gameTransform) * localPointerPos;

    sound_instrument *instrument = &gameSounds->instruments[INSTRUMENT_TYPE_PIANO];

    status_text_pos currentPos = {};
    currentPos.x = 5;
    currentPos.y = 5;

    sound_envelope *envelope = &instrument->envelope;

    doEditArrows(&currentPos, &envelope->attackTime, 0.02f, input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
    showFloatValue(&currentPos, "attackTime", envelope->attackTime, 100.0f, assets, spriteList, &stringMemory);
    currentPos.x = 5;

    doEditArrows(&currentPos, &envelope->attackVolume, 0.05f, input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
    showFloatValue(&currentPos, "attackVolume", envelope->attackVolume, 100.0f, assets, spriteList, &stringMemory);
    currentPos.x = 5;

    doEditArrows(&currentPos, &envelope->decayTime, 0.02f, input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
    showFloatValue(&currentPos, "decayTime", envelope->decayTime, 100.0f, assets, spriteList, &stringMemory);
    currentPos.x = 5;

    doBoolButton(&currentPos, &envelope->sustain, input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
    showBoolValue(&currentPos, "sustain", envelope->sustain, 100.0f, assets, spriteList, &stringMemory);
    currentPos.x = 5;

    doEditArrows(&currentPos, &envelope->sustainVolume, 0.05f, input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
    showFloatValue(&currentPos, "sustainVol", envelope->sustainVolume, 100.0f, assets, spriteList, &stringMemory);
    currentPos.x = 5;

    doEditArrows(&currentPos, &envelope->releaseTime, 0.02f, input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
    showFloatValue(&currentPos, "releaseTime", envelope->releaseTime, 100.0f, assets, spriteList, &stringMemory);
    currentPos.x = 5;

    doIntArrows(&currentPos, &instrument->numWaveForms, input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
    showIntValue(&currentPos, "numWaveForms", instrument->numWaveForms, 100.0f, assets, spriteList, &stringMemory);
    currentPos.x = 5;

    sound_filter *filter = &instrument->filter;
    doIntArrows(&currentPos, (int *)(&filter->type), input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
    filter->type = (sound_filter_type)(filter->type % 4);
    showFilterType(&currentPos, "filter", filter->type, 40.0f, assets, spriteList, &stringMemory);
    currentPos.x = 5;

    float cutoff = filter->cutoff;
    doEditArrows(&currentPos, &filter->cutoff, 0.02f, input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
    showFloatValue(&currentPos, "cutoff", filter->cutoff, 100.0f, assets, spriteList, &stringMemory);
    currentPos.x = 5;
    if (cutoff != filter->cutoff) {
        calcFeedbackForFilter(filter);
    }

    float resonance = filter->resonance;
    doEditArrows(&currentPos, &filter->resonance, 0.02f, input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
    showFloatValue(&currentPos, "resonance", filter->resonance, 100.0f, assets, spriteList, &stringMemory);
    currentPos.x = 5;
    if (resonance != filter->resonance) {
        calcFeedbackForFilter(filter);
    }

    for (int waveIndex = 0; waveIndex < instrument->numWaveForms; ++waveIndex) {
        float xStart = 205.0f + 145.0f * (waveIndex % 4);
        currentPos.x = xStart;
        currentPos.y = 5.0f + 80 * (waveIndex / 4);
        sound_waveform *waveform = &instrument->waveforms[waveIndex];

        doIntArrows(&currentPos, (int *)(&waveform->waveType), input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
        waveform->waveType = (oscillator_type)(waveform->waveType % 5);
        showWaveType(&currentPos, "type", waveform->waveType, 40.0f, assets, spriteList, &stringMemory);

        currentPos.x = xStart;

        doEditArrows(&currentPos, &waveform->volume, 0.10f, input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
        showFloatValue(&currentPos, "vol", waveform->volume, 40.0f, assets, spriteList, &stringMemory);
        currentPos.x = xStart;

        doEditArrows(&currentPos, &waveform->muliplier, 0.2f, input->pointerJustDown, localPointerPos.x, localPointerPos.y, assets, spriteList);
        showFloatValue(&currentPos, "mul", waveform->muliplier, 40.0f, assets, spriteList, &stringMemory);
    }

    if (!soundEditor->initialized) {
        soundEditor->initialized = true;
        for (int i = 0; i < SOUND_NOTE_COUNT; ++i) {
            *(soundEditor->notes + i) = {};
        }
        playBGM(MIDI_KEY_TEST, gameSounds, assets);
    }
    for (int i = 0; i < SOUND_NOTE_COUNT; ++i) {
        sound_editor_note *note = soundEditor->notes + i;
        bool keyPressed = keyPressedForNote(input, (sound_note)i);
        processKey(note, (sound_note)i, gameSounds, keyPressed);
    }

    popSpriteMatrix(spriteList);

    float screenWidth = (float)renderCommands->windowWidth;
    float screenHeight = (float)renderCommands->windowHeight;
    //matrix4x4 projMatrix = createOrthgraphicMatrix(0.1f, 1000.0f, screenWidth, screenHeight);
    matrix4x4 projMatrix = createOrthgraphicMatrix(0.0f, 1000.0f, 0.0f, GAME_WIDTH * 2.0f, 0.0f, GAME_HEIGHT * 2.0f);

    quaternion camRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
    matrix4x4 viewMatrix = createViewMatrix(camRotation,
                                            0.0f, 
                                            0.0f, 
                                            0.0f); 

    render_command_set_camera *setCameraCommand = 
        (render_command_set_camera *)pushRenderCommand(renderCommands,
                                                       RENDER_COMMAND_SET_CAMERA,
                                                       sizeof(render_command_set_camera));
    setCameraCommand->viewMatrix = viewMatrix;
    setCameraCommand->projMatrix = projMatrix;

    render_command_lines *lineCommand = startLines(renderCommands);
    synth_sound dummySound = {};
    dummySound.instrumentType = INSTRUMENT_TYPE_PIANO;
    dummySound.active = true;
    dummySound.pressed = true;
    dummySound.hz = noteHz(SOUND_NOTE_A);
    float lastX = 0.0f;
    float lastY = 300.0f;
    float t = 0.0f;
    float dt = 1.0f / 10000.0f;

    while (t <= 0.05f) {
        t += dt;
        float value = updateInstrument(&dummySound, instrument, dt);
        float newX = (t / 0.05f) * (2.0f * (float)GAME_WIDTH);
        float newY = 300.0f + (value * 1000.0f);
        drawLine(lastX, lastY, 0.0f, newX, newY, 0.0f, renderCommands, lineCommand);
        lastX = newX;
        lastY = newY;
    }
    

    //drawLine(-300.0f, -300.0f, 0.0f, 300.0f, 300.0f, 0.0f, renderCommands, lineCommand);
    //drawLine(-300.0f, -300.0f, 0.0f, 300.0f, 300.0f, 0.0f, renderCommands, lineCommand);
    //drawLine(-300.0f, -300.0f, 0.0f, 300.0f, 300.0f, 0.0f, renderCommands, lineCommand);
    

}
