#include "block_game.h"

void initBlockGame (memory_arena *memory, block_game* blockGame) {
    grid_block *blocks = blockGame->blocks;

    grid_block *block = &blocks[blockGame->numBlocks];
    blockGame->numBlocks++;
    *block = {};
    block->row = 3;
    block->col = 2;

    block = &blocks[blockGame->numBlocks];
    blockGame->numBlocks++;
    *block = {};
    block->row = 3;
    block->col = 3;

    block = &blocks[blockGame->numBlocks];
    blockGame->numBlocks++;
    *block = {};
    block->row = 3;
    block->col = 4;

    block = &blocks[blockGame->numBlocks];
    blockGame->numBlocks++;
    *block = {};
    block->row = 3;
    block->col = 5;

    //for (int i = 0; i < NUM_GRID_ROWS; ++i) {
    //    for (int j = 0; j < NUM_GRID_COLS; ++j) {
    //        grid_block *block = &blocks[i * NUM_GRID_COLS + j];
    //        block->row = i;
    //        block->col = j;
    //        blockGame->numBlocks++;
    //    }
    //}

    blockGame->sheep = {};
    blockGame->sheep.row = 6;
    blockGame->sheep.col = 6;
}

void updateSheep (block_game *blockGame, game_input *input) {
    float gridRowStart = -((float)(NUM_GRID_ROWS - 1) * GRID_BLOCK_WIDTH) * 0.5f;
    float gridColStart = -((float)(NUM_GRID_COLS - 1) * GRID_BLOCK_HEIGHT) * 0.5f;

    block_sheep *sheep = &blockGame->sheep;
    if (!sheep->moving) {
        sheep->prevRow = sheep->row;
        sheep->prevCol = sheep->col;

        bool startMoving = false;
        if (blockGame->nextMoveDirection == -1) {
            if (input->turnUpButton) {
                blockGame->nextMoveDirection = 0;
            }
            else if (input->turnDownButton) {
                blockGame->nextMoveDirection = 1;
            }
            else if (input->turnLeftButton) {
                blockGame->nextMoveDirection = 2;
            }
            else if (input->turnRightButton) {
                blockGame->nextMoveDirection = 3;
            }
        }
        if (blockGame->nextMoveDirection != -1) {
            if (blockGame->nextMoveDirection == 0) {
                sheep->row--;
                startMoving = true;
            }
            else if (blockGame->nextMoveDirection == 1) {
                sheep->row++;
                startMoving = true;
            }
            else if (blockGame->nextMoveDirection == 2) {
                sheep->col--;
                startMoving = true;
            }
            else if (blockGame->nextMoveDirection == 3) {
                sheep->col++;
                startMoving = true;
            }
            blockGame->nextMoveDirection = -1;

            if (startMoving) {
                sheep->moving = true;
                sheep->moveTime = 0.0f;
            }
        }

        sheep->x = gridColStart + GRID_BLOCK_WIDTH * sheep->col;
        sheep->y = gridRowStart + GRID_BLOCK_HEIGHT * sheep->row;
    }

    if (sheep->moving) {
        sheep->moveTime += DELTA_TIME;
        if (sheep->moveTime > SHEEP_MOVE_SPEED) { sheep->moveTime = SHEEP_MOVE_SPEED; }

        float prevX = gridColStart + GRID_BLOCK_WIDTH * sheep->prevCol;
        float prevY = gridRowStart + GRID_BLOCK_WIDTH * sheep->prevRow;

        float nextX = gridColStart + GRID_BLOCK_WIDTH * sheep->col;
        float nextY = gridRowStart + GRID_BLOCK_WIDTH * sheep->row;

        float t = sheep->moveTime / SHEEP_MOVE_SPEED;
        sheep->x = prevX + t * (nextX - prevX);
        sheep->y = prevY + t * (nextY - prevY);

        if (sheep->moveTime == SHEEP_MOVE_SPEED) {
            sheep->moving = false;
        }
    }
}

void updateBlockGame (memory_arena *memory, memory_arena *tempMemory, game_assets *assets, game_input *input, 
                      block_game *blockGame, sprite_list *spriteList) 
{
    if (input->turnUpButtonJustPressed) {
        blockGame->nextMoveDirection = 0;
    }
    else if (input->turnDownButtonJustPressed) {
        blockGame->nextMoveDirection = 1;
    }
    else if (input->turnLeftButtonJustPressed) {
        blockGame->nextMoveDirection = 2;
    }
    else if (input->turnRightButtonJustPressed) {
        blockGame->nextMoveDirection = 3;
    }

    updateSheep(blockGame, input);

    pushSpriteTransform(spriteList, Vector2(GAME_WIDTH/2.0f, GAME_HEIGHT/2.0f));

    float gridRowStart = -((float)(NUM_GRID_ROWS - 1) * GRID_BLOCK_WIDTH) * 0.5f;
    float gridColStart = -((float)(NUM_GRID_COLS - 1) * GRID_BLOCK_HEIGHT) * 0.5f;
    for (int i = 0; i < NUM_GRID_ROWS; ++i) {
        for (int j = 0; j < NUM_GRID_COLS; ++j) {
            addSprite(gridColStart + GRID_BLOCK_WIDTH * j, gridRowStart + GRID_BLOCK_HEIGHT * i, assets, ATLAS_KEY_GAME, "tile_backing", spriteList, 0.5f, 0.5f);
        }
    }

    grid_block *blocks = blockGame->blocks;

    for (int i = 0; i < blockGame->numBlocks; ++i) {
        grid_block *block = &blocks[i];
        addSprite(gridColStart + GRID_BLOCK_WIDTH * block->col, gridRowStart + GRID_BLOCK_HEIGHT * block->row, assets, ATLAS_KEY_GAME, "blue_tile", spriteList, 0.5f, 0.5f);
    }

    block_sheep *sheep = &blockGame->sheep;
    addSprite(sheep->x, sheep->y, assets, ATLAS_KEY_GAME, "sheep", spriteList, 0.5f, 0.5f);

    popSpriteMatrix(spriteList);
}
