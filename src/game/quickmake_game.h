#ifndef QUICKMAKE_GAME_H
#define QUICKMAKE_GAME_H

#define DELTA_TIME (1.0f / 60.0f)
#define GAME_WIDTH 384.0f
#define GAME_HEIGHT 216.0f

#include "quickmake_platform.h"
#include "quickmake_math.cpp"
#include "quickmake_util.h"
#include "quickmake_assets.cpp"
#include "quickmake_sprites.cpp"
#include "quickmake_audio.cpp"
#include "quickmake_rendering.cpp"

//#include "hitbox/hitbox.cpp"

#include "sound_editor.cpp"
//#include "test_game.cpp"
//#include "block_game.cpp"
//#include "piano_game.cpp"
//#include "skeletal_game.cpp"
//#include "controller_test_game.cpp"
//#include "hitbox_editor/hitbox_editor.cpp"


struct game_state {
    memory_arena memory;
    memory_arena tempMemory; // cleared every frame

    bool assetsInitialized;
    game_assets assets;
    bool gameInitialized;

    bool fileJustLoaded;
    void *loadedFileData;
    unsigned int loadedFileSize;

    float visualizationT;
    sound_editor soundEditor;
    //piano_game pianoGame;
    //block_game blockGame;
    //skeletal_game skeletalGame;
    //hitbox_editor hitboxEditor;

    game_sounds sounds;

    float sineT;
};

#endif
