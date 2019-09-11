#ifndef QUICKMAKE_GAME_H
#define QUICKMAKE_GAME_H

#define DELTA_TIME (1.0f / 60.0f)
#define GAME_WIDTH 1152.0f
#define GAME_HEIGHT 648.0f


#include "quickmake_platform.h"
#include "quickmake_math.cpp"
#include "quickmake_util.h"
#include "quickmake_assets.cpp"
#include "quickmake_sprites.cpp"
#include "quickmake_audio.cpp"
#include "quickmake_rendering.cpp"
//#include "block_game.cpp"
//#include "piano_game.cpp"
#include "skeletal_game.cpp"

struct game_state {
    memory_arena memory;
    memory_arena tempMemory; // cleared every frame
    bool assetsInitialized;
    game_assets assets;
    bool gameInitialized;

    float visualizationT;
    //piano_game pianoGame;
    //block_game blockGame;
    skeletal_game skeletalGame;

    game_sounds sounds;

    float sineT;
};

#endif
