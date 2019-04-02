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
#include "block_game.cpp"

struct game_state {
    memory_arena memory;
    memory_arena tempMemory; // cleared every frame
    bool assetsInitialized;
    game_assets assets;
    bool gameInitialized;

    float visualizationT;
    block_game blockGame;
};

#endif
