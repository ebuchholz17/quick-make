#include "quickmake_game.h"

static void initSounds (game_sounds *gameSounds) {
    instrument_type instrumentType;
    sound_instrument *instrument;
    sound_envelope *envelope;
    sound_waveform *waveform;
    sound_filter *filter;

    instrumentType = INSTRUMENT_TYPE_PIANO;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.01f;
    envelope->attackVolume = 1.0f;
    envelope->decayTime = 0.07f;
    envelope->sustain = true;
    envelope->sustainVolume = 0.8f;
    envelope->releaseTime = 0.6f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.6f;
    waveform->muliplier = 1.0f;
    waveform = &instrument->waveforms[1];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.3f;
    waveform->muliplier = 2.0f;
    waveform = &instrument->waveforms[2];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.1f;
    waveform->muliplier = 3.0f;
    instrument->numWaveForms = 3;

    gameSounds->numPlayingSounds = 0;

    instrumentType = INSTRUMENT_TYPE_TRUMPET;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.1f;
    envelope->attackVolume = 0.7f;
    envelope->decayTime = 0.07f;
    envelope->sustain = true;
    envelope->sustainVolume = 0.5f;
    envelope->releaseTime = 0.1f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_SAWTOOTH;
    waveform->volume = 1.0f;
    waveform->muliplier = 1.0f;
    filter = &instrument->filter;
    filter->cutoff = 0.5f;
    filter->resonance = 0.2f;
    calcFeedbackForFilter(filter);
    instrument->numWaveForms = 1;

    instrumentType = INSTRUMENT_TYPE_GUITAR;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.01f;
    envelope->attackVolume = 1.0f;
    envelope->decayTime = 0.03f;
    envelope->sustain = true;
    envelope->sustainVolume = 0.6f;
    envelope->releaseTime = 0.4f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_TRIANGLE;
    waveform->volume = 1.0f;
    waveform->muliplier = 1.0f;
    waveform = &instrument->waveforms[1];
    waveform->waveType = OSCILLATOR_TYPE_TRIANGLE;
    waveform->volume = 0.8f;
    waveform->muliplier = 2.0f;
    waveform = &instrument->waveforms[2];
    waveform->waveType = OSCILLATOR_TYPE_TRIANGLE;
    waveform->volume = 0.6f;
    waveform->muliplier = 3.0f;
    waveform = &instrument->waveforms[3];
    waveform->waveType = OSCILLATOR_TYPE_TRIANGLE;
    waveform->volume = 0.4f;
    waveform->muliplier = 4.0f;
    instrument->numWaveForms = 4;
    filter = &instrument->filter;
    filter->cutoff = 0.5f;
    filter->resonance = 0.2f;
    calcFeedbackForFilter(filter);

    instrumentType = INSTRUMENT_TYPE_VIOLIN;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.03f;
    envelope->attackVolume = 1.0f;
    envelope->decayTime = 0.15f;
    envelope->sustain = true;
    envelope->sustainVolume = 0.9f;
    envelope->releaseTime = 0.2f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_SAWTOOTH;
    waveform->volume = 1.0f;
    waveform->muliplier = 1.0f;
    waveform = &instrument->waveforms[1];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.7f;
    waveform->muliplier = 2.0f;
    waveform = &instrument->waveforms[2];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.3f;
    waveform->muliplier = 3.0f;
    waveform = &instrument->waveforms[3];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.2f;
    waveform->muliplier = 4.0f;
    instrument->numWaveForms = 4;
    filter = &instrument->filter;
    filter->cutoff = 0.4f;
    filter->resonance = 0.3f;
    calcFeedbackForFilter(filter);

    instrumentType = INSTRUMENT_TYPE_FLUTE;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.15f;
    envelope->attackVolume = 0.5f;
    envelope->decayTime = 0.07f;
    envelope->sustain = true;
    envelope->sustainVolume = 0.45f;
    envelope->releaseTime = 0.1f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 1.0f;
    waveform->muliplier = 1.0f;
    waveform = &instrument->waveforms[1];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.7f;
    waveform->muliplier = 2.0f;
    waveform = &instrument->waveforms[2];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.5f;
    waveform->muliplier = 3.0f;
    waveform = &instrument->waveforms[3];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.3f;
    waveform->muliplier = 4.0f;
    waveform = &instrument->waveforms[4];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.1f;
    waveform->muliplier = 5.0f;
    waveform = &instrument->waveforms[5];
    waveform->waveType = OSCILLATOR_TYPE_NOISE;
    waveform->volume = 0.03f;
    waveform->muliplier = 1.0f;
    instrument->numWaveForms = 6;
    filter = &instrument->filter;
    filter->cutoff = 0.4f;
    filter->resonance = 0.3f;
    calcFeedbackForFilter(filter);

    instrumentType = INSTRUMENT_TYPE_SNARE_DRUM;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.01f;
    envelope->attackVolume = 1.0f;
    envelope->decayTime = 0.01f;
    envelope->sustain = false;
    envelope->sustainVolume = 0.35f;
    envelope->releaseTime = 0.14f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_NOISE;
    waveform->volume = 1.0f;
    waveform->muliplier = 1.0f;
    instrument->numWaveForms = 1;
    filter = &instrument->filter;
    filter->cutoff = 0.84f;
    filter->resonance = 0.2f;
    calcFeedbackForFilter(filter);
}

//static void growTransformGroupEntryList () {
//
//}

//static transform_group *addTransformGroup (transform_group_list *groupList, memory_arena *tempMemory) {
//    assert(groupList->numGroups < MAX_TRANSFORM_GROUPS);
//
//    transform_group *group = &groupList->groups[groupList->numGroups];
//    *group = {};
//    ++groupList->numGroups;
//
//    group->children.length = 0;
//    group->children.capacity = 16;
//    group->children.entries = (transform_group_entry *)allocateMemorySize(tempMemory, group->children.capacity * sizeof(transform_group_entry)); 
//
//    return group;
//}

//static void addChild () {
//
//}


// TODO(ebuchholz): Maybe pack everything into a single file and load that?
extern "C" void getGameAssetList (asset_list *assetList) {
    pushAsset(assetList, "assets/textures/font.bmp", ASSET_TYPE_BMP, TEXTURE_KEY_FONT);
    pushAsset(assetList, "assets/textures/atlas.txt", ASSET_TYPE_ATLAS, ATLAS_KEY_GAME, TEXTURE_KEY_GAME_ATLAS);
    pushAsset(assetList, "assets/textures/hitbox_editor_atlas.txt", ASSET_TYPE_ATLAS, ATLAS_KEY_HITBOX_EDITOR, TEXTURE_KEY_HITBOX_EDITOR_ATLAS);

    //pushAsset(assetList, "assets/sounds/menu_button.wav", ASSET_TYPE_WAV, SOUND_KEY_MENU_BUTTON);

    //pushAsset(assetList, "assets/data/data.txt", ASSET_TYPE_DATA, DATA_KEY_HITBOX_DATA);

    pushAsset(assetList, "assets/midi/onestop.mid", ASSET_TYPE_MIDI, MIDI_KEY_TEST);
}

extern "C" void parseGameAsset (void *assetData, void *secondAssetData, asset_type type, int key, int secondKey,
                                game_memory *gameMemory, memory_arena *workingMemory, platform_options *options, unsigned int size) 
{
    game_state *gameState = (game_state *)gameMemory->memory;
    if (!gameState->assetsInitialized) {
        gameState->assetsInitialized = true;

        gameState->memory = {};
        gameState->memory.size = 0;
        gameState->memory.capacity = gameMemory->memoryCapacity - sizeof(game_state);
        gameState->memory.base = (char *)gameMemory->memory + sizeof(game_state);

        gameState->assets = {};
        game_assets *assets = &gameState->assets;
        assets->assetMemory = {};
        assets->assetMemory.size = 0;
        assets->assetMemory.capacity = 10 * 1024 * 1024; // 1MB of asset data???
        assets->assetMemory.base = allocateMemorySize(&gameState->memory, assets->assetMemory.capacity); 
        assets->numMeshes = 0;

        gameState->sineT = 0.0f;
    } 
    // parse data
    // create/copy stuff into game memory
    // place whatever the platform needs at the beginning of working memory
    switch (type) {
    default:
        assert(false); // must provide a valid type
        break;
    case ASSET_TYPE_ANIMATION_DATA:
        parseAnimationData(assetData, &gameState->assets, key, workingMemory);
        break;
    case ASSET_TYPE_OBJ:
        parseOBJ(assetData, &gameState->assets, key, workingMemory);
        break;
    case ASSET_TYPE_QMM:
        parseQMM(assetData, &gameState->assets, key, workingMemory);
        break;
    case ASSET_TYPE_BMP:
        parseBitmap(assetData, &gameState->assets, key, workingMemory);
        break;
    case ASSET_TYPE_WAV:
        parseWav(assetData, &gameState->assets, key, workingMemory, options);
        break;
    case ASSET_TYPE_MIDI:
        parseMidi(assetData, &gameState->assets, key, workingMemory, options);
        break;
    case ASSET_TYPE_ATLAS:
        parseBitmap(secondAssetData, &gameState->assets, secondKey, workingMemory);
        parseAtlas(assetData, &gameState->assets, key, secondKey, workingMemory);
        break;
    case ASSET_TYPE_DATA:
        loadDataFile(assetData, &gameState->assets, key, workingMemory, size);
        break;
    }
}

// TODO(ebuchholz); better interface for interactively loading files at runtime
extern "C" void loadFile (game_memory *gameMemory, void *fileData, unsigned int fileSize) {
    game_state *gameState = (game_state *)gameMemory->memory;
    // TODO(ebuchholz): copy file data?
    gameState->fileJustLoaded = true;
    gameState->loadedFileData = fileData;
    gameState->loadedFileSize = fileSize;

    // TODO(ebuchholz): do something with file?
}

extern "C" void updateGame (game_input *input, game_memory *gameMemory, render_command_list *renderCommands, 
                            platform_options *options, platform_triggers *triggers) { 
    game_state *gameState = (game_state *)gameMemory->memory;
    if (!gameState->gameInitialized) {
        gameState->gameInitialized = true;
        gameState->sounds = {};
        initSounds(&gameState->sounds);

        initLetterCoords();

        //initBlockGame(&gameState->memory, &gameState->blockGame);
        //initPianoGame(&gameState->pianoGame);
        //initSkeletalGame(&gameState->memory, &gameState->skeletalGame);
        //initHitboxEditor(&gameState->memory, &gameState->assets, &gameState->hitboxEditor);
    }
    // general purpose temporary storage
    gameState->tempMemory = {};
    gameState->tempMemory.size = 0;
    gameState->tempMemory.capacity = gameMemory->tempMemoryCapacity;
    gameState->tempMemory.base = (char *)gameMemory->tempMemory;

    // Zero memory here? since uint8array.fill is slow in firefix

    sprite_list spriteList = {};
    spriteList.sprites = (sprite *)allocateMemorySize(&gameState->tempMemory, sizeof(sprite) * MAX_SPRITES_PER_FRAME);
    spriteList.numSprites = 0;
    spriteList.matrixStack[0] = identityMatrix3x3();

    //transform_group_list groupList;
    //groupList.groups = (transform_group *)allocateMemorySize(&gameState->tempMemory, sizeof(transform_group) * MAX_TRANSFORM_GROUPS);
    //groupList.numGroups = 0;

    //transform_group *rootGroup = addTransformGroup(&groupList, &gameState->tempMemory);

    // TODO(ebuchholz): get screen dimensions from render commands? and use them
    float screenWidth = (float)renderCommands->windowWidth;
    float screenHeight = (float)renderCommands->windowHeight;

    float normalAspectRatio = GAME_WIDTH / GAME_HEIGHT;
    float actualAspectRatio = screenWidth / screenHeight;
    float gameScale;
    vector2 gameOrigin;
    if (actualAspectRatio < normalAspectRatio) {
        float widthRatio = screenWidth / GAME_WIDTH;
        gameScale = widthRatio;
        //gameScale *= 0.5f;
        gameOrigin.x = (screenWidth - (GAME_WIDTH * gameScale)) / 2.0f;
        gameOrigin.y = (screenHeight - (GAME_HEIGHT * gameScale)) / 2.0f;
    }
    else {
        float heightRatio = screenHeight / GAME_HEIGHT;
        gameScale = heightRatio;
        //gameScale *= 0.5f;
        gameOrigin.x = (screenWidth - (GAME_WIDTH * gameScale)) / 2.0f;
        gameOrigin.y = (screenHeight - (GAME_HEIGHT * gameScale)) / 2.0f;
    }

    //gameState->visualizationT += DELTA_TIME;
    //render_command_background_visualization *visualizationCommand = 
    //    (render_command_background_visualization *)pushRenderCommand(renderCommands, 
    //                                                                 RENDER_COMMAND_BACKGROUND_VISUALIZATION,
    //                                                                 sizeof(render_command_background_visualization));
    //visualizationCommand->t = gameState->visualizationT;

    pushSpriteTransform(&spriteList, gameOrigin, gameScale);

    if (gameState->fileJustLoaded) {
        //loadFileIntoHitboxEditor(&gameState->hitboxEditor, &gameState->assets, gameState->loadedFileData, gameState->loadedFileSize);
        gameState->fileJustLoaded = false;
    }

    //updateBlockGame(&gameState->memory, &gameState->tempMemory, &gameState->assets, input, &gameState->blockGame, &spriteList);
    //updatePianoGame(&gameState->sounds, &gameState->assets, input, &gameState->pianoGame, &spriteList);
    //updateSkeletalGame(&gameState->memory, &gameState->tempMemory, &gameState->assets, input, &gameState->skeletalGame, &spriteList, renderCommands);
    //updateControllerTestGame(&gameState->memory, &gameState->tempMemory, &gameState->assets, input, &spriteList);
    //updateHitboxEditor(&gameState->memory, &gameState->tempMemory, &gameState->assets, input, &spriteList, &gameState->hitboxEditor);
    //updateTestGame(&gameState->memory, &gameState->tempMemory, &gameState->assets, input, &spriteList, &gameState->sounds);
    updateSoundEditor(&gameState->memory, &gameState->tempMemory, &gameState->soundEditor, &gameState->assets, input, &spriteList, &gameState->sounds, renderCommands);
    //if (gameState->hitboxEditor.requestFileLoad) {
    //    triggers->triggerFileWindow = true;
    //}
    //if (gameState->hitboxEditor.requestFileSave) {
    //    triggers->triggerFileSave = true;
    //    triggers->fileToSaveData = gameState->hitboxEditor.fileToSaveData;
    //    triggers->fileToSaveSize = gameState->hitboxEditor.fileToSaveSize;
    //}

    popSpriteMatrix(&spriteList);

    render_command_sprite_list *spriteListCommand = 
        (render_command_sprite_list *)pushRenderCommand(renderCommands,
                                                 RENDER_COMMAND_SPRITE_LIST,
                                                 sizeof(render_command_sprite_list));
    spriteListCommand->numSprites = spriteList.numSprites;
    spriteListCommand->sprites = (render_sprite *)allocateMemorySize(&renderCommands->memory, 
                                                                     sizeof(render_sprite) * spriteListCommand->numSprites);

    matrix3x3 spriteTransform;
    for (int i = 0; i < spriteList.numSprites; ++i) {
        sprite *sprite = &spriteList.sprites[i];

        // TODO(ebuchholz): base case for untransformed sprites (common case?)
        render_sprite *renderSprite = &spriteListCommand->sprites[i];
        spriteTransform = identityMatrix3x3();
        spriteTransform.m[2] = -sprite->anchor.x;
        spriteTransform.m[5] = -sprite->anchor.y;

        float scaledWidth = sprite->scale * sprite->width;
        float scaledHeight = sprite->scale * sprite->height;
        matrix3x3 scaleMatrix = scaleMatrix3x3(scaledWidth, scaledHeight);
        spriteTransform = scaleMatrix * spriteTransform;
        spriteTransform = rotationMatrix3x3(sprite->rotation) * spriteTransform;
        spriteTransform = translationMatrix(sprite->pos.x, sprite->pos.y) * spriteTransform;

        spriteTransform = sprite->parentTransform * spriteTransform;

        renderSprite->pos[0] = spriteTransform * Vector2(0.0f, 0.0f);
        renderSprite->pos[1] = spriteTransform * Vector2(1.0f, 0.0f);
        renderSprite->pos[2] = spriteTransform * Vector2(0.0f, 1.0f);
        renderSprite->pos[3] = spriteTransform * Vector2(1.0f, 1.0f);

        // determine uvs for texture atlases
        renderSprite->texCoord[0] = sprite->frameCorners[0];
        renderSprite->texCoord[1] = sprite->frameCorners[1];
        renderSprite->texCoord[2] = sprite->frameCorners[2];
        renderSprite->texCoord[3] = sprite->frameCorners[3];

        float red = (float)((sprite->tint >> 16) & (0xff)) / 255.0f;
        float green = (float)((sprite->tint >> 8) & (0xff)) / 255.0f;
        float blue = (float)((sprite->tint) & (0xff)) / 255.0f;

        vector4 color;
        color.r = red;
        color.g = green;
        color.b = blue;
        color.a = sprite->alpha;
        renderSprite->color[0] = color;
        renderSprite->color[1] = color;
        renderSprite->color[2] = color;
        renderSprite->color[3] = color;

        renderSprite->textureKey = sprite->textureKey;
    }
}

extern "C" void getGameSoundSamples (game_memory *gameMemory, game_sound_output *soundOutput) { 
    game_state *gameState = (game_state *)gameMemory->memory;
    game_sounds *gameSounds = &gameState->sounds;
    game_assets *assets = &gameState->assets;

    float volume = 1.0f;
    float dt = 1.0f / (float)soundOutput->samplesPerSecond;
    
    sound_sample *sampleOut = soundOutput->samples;

    // update background music
    updateBGM(gameSounds, sampleOut, soundOutput->sampleCount, soundOutput->samplesPerSecond);

    sampleOut = soundOutput->samples;
    for (int i = 0; i < soundOutput->sampleCount; ++i) 
    {
        float sampleValue = 0.0f;
        
        for (int soundIndex = 0; soundIndex < MAX_INSTRUMENT_SOUNDS; ++soundIndex) {
            synth_sound *sound = gameSounds->instrumentSounds + soundIndex;
            sound_instrument *instrument = gameSounds->instruments + sound->instrumentType;
            if (sound->active) {
                sampleValue += updateInstrument(sound, instrument, dt);
            }
        }

        for (int soundIndex = gameSounds->numPlayingSounds - 1; soundIndex >= 0; --soundIndex) {
            playing_sound *sound = gameSounds->playingSounds + soundIndex;
            sound_asset *soundAsset = assets->sounds[sound->key];

            short soundValue = soundAsset->samples[sound->currentSample];
            float floatValue = (float)soundValue / 32767;
            sampleValue += floatValue;

            ++sound->currentSample;
            if (sound->currentSample >= soundAsset->numSamples) {
                playing_sound *lastSound = gameSounds->playingSounds + (gameSounds->numPlayingSounds - 1);
                *sound = *lastSound;
                --gameSounds->numPlayingSounds;
            }
        }

        // clamp between -1 and 1
        sampleValue = sampleValue > 1.0f ? 1.0f : sampleValue;
        sampleValue = sampleValue < -1.0f ? -1.0f : sampleValue;
        sampleOut->value = (0.5f * sampleOut->value) + (0.5f * (sampleValue * volume));
        assert(sampleOut->value >= -1.0f && sampleOut->value <= 1.0f);
        ++sampleOut;
    }
}
