#include "block_game.h"

void addNewBlockToGrid (block_game *blockGame, int row, int col) {
    int freeBlockIndex = -1;
    for (int i = 0; i < blockGame->numBlocks; ++i) {
        grid_block *block = blockGame->blocks + i;
        if (!block->active) {
            freeBlockIndex = i;
            break;
        }
    }

    grid_block *block;
    if (freeBlockIndex != -1) {
        block = blockGame->blocks + freeBlockIndex;
        *block = {};
        block->id = freeBlockIndex;
    }
    else {
        block = &blockGame->blocks[blockGame->numBlocks];
        *block = {};
        block->id = blockGame->numBlocks;
        blockGame->numBlocks++;
    }
    block->active = true;

    block->row = row;
    block->col = col;

    // TODO(ebuchholz): un-copy-paste gridrow/colstart
    float gridRowStart = -((float)(NUM_GRID_ROWS - 1) * GRID_BLOCK_WIDTH) * 0.5f;
    float gridColStart = -((float)(NUM_GRID_COLS - 1) * GRID_BLOCK_HEIGHT) * 0.5f;
    block->x = gridColStart + GRID_BLOCK_WIDTH * block->col;
    block->y = gridRowStart + GRID_BLOCK_HEIGHT * block->row;

    blockGame->grid[row * NUM_GRID_COLS + col] = block->id;
}

void addBlockIfUnoccupied (block_game *blockGame, int row, int col) {
    if (blockGame->grid[row * NUM_GRID_COLS + col] == -1) {
        addNewBlockToGrid(blockGame, row, col);
    }
}


direction tryMoveSheep (block_game *blockGame, game_input *input) {
    block_sheep *sheep = &blockGame->sheep;
    if (blockGame->nextMoveDirection == DIRECTION_NONE) {
        if (input->turnUpButton) {
            blockGame->nextMoveDirection = DIRECTION_UP;
        }
        else if (input->turnDownButton) {
            blockGame->nextMoveDirection = DIRECTION_DOWN;
        }
        else if (input->turnLeftButton) {
            blockGame->nextMoveDirection = DIRECTION_LEFT;
        }
        else if (input->turnRightButton) {
            blockGame->nextMoveDirection = DIRECTION_RIGHT;
        }
    }
    return blockGame->nextMoveDirection;
}

void moveSheep (block_game *blockGame, game_input *input) {
    float gridRowStart = -((float)(NUM_GRID_ROWS - 1) * GRID_BLOCK_WIDTH) * 0.5f;
    float gridColStart = -((float)(NUM_GRID_COLS - 1) * GRID_BLOCK_HEIGHT) * 0.5f;

    block_sheep *sheep = &blockGame->sheep;
    sheep->moveTime += DELTA_TIME;
    if (sheep->moveTime > BLOCK_MOVE_SPEED) { sheep->moveTime = BLOCK_MOVE_SPEED; }

    float prevX = gridColStart + GRID_BLOCK_WIDTH * sheep->prevCol;
    float prevY = gridRowStart + GRID_BLOCK_WIDTH * sheep->prevRow;

    float nextX = gridColStart + GRID_BLOCK_WIDTH * sheep->col;
    float nextY = gridRowStart + GRID_BLOCK_WIDTH * sheep->row;

    float t = sheep->moveTime / BLOCK_MOVE_SPEED;
    sheep->x = prevX + t * (nextX - prevX);
    sheep->y = prevY + t * (nextY - prevY);

    if (sheep->moveTime == BLOCK_MOVE_SPEED) {
        sheep->moving = false;
    }
}

void moveBlocks (block_game *blockGame, game_input *input) {
    float gridRowStart = -((float)(NUM_GRID_ROWS - 1) * GRID_BLOCK_WIDTH) * 0.5f;
    float gridColStart = -((float)(NUM_GRID_COLS - 1) * GRID_BLOCK_HEIGHT) * 0.5f;

    grid_block *blocks = blockGame->blocks;
    for (int i = 0; i < blockGame->numBlocks; ++i) {
        grid_block *block = blocks + i;

        if (block->active) {
            if (block->moving) {
                block->moveTime += DELTA_TIME;
                if (block->moveTime > BLOCK_MOVE_SPEED) { block->moveTime = BLOCK_MOVE_SPEED; }

                float prevX = gridColStart + GRID_BLOCK_WIDTH * block->prevCol;
                float prevY = gridRowStart + GRID_BLOCK_WIDTH * block->prevRow;

                float nextX = gridColStart + GRID_BLOCK_WIDTH * block->col;
                float nextY = gridRowStart + GRID_BLOCK_WIDTH * block->row;

                float t = block->moveTime / BLOCK_MOVE_SPEED;
                block->x = prevX + t * (nextX - prevX);
                block->y = prevY + t * (nextY - prevY);

                if (block->moveTime == BLOCK_MOVE_SPEED) {
                    block->moving = false;
                }
            }
            else {
                block->x = gridColStart + GRID_BLOCK_WIDTH * block->col;
                block->y = gridRowStart + GRID_BLOCK_HEIGHT * block->row;
            }
        } 
    }
}

void nextRowCol (int row, int col, direction dir, int *outRow, int *outCol) {
    *outRow = row;
    *outCol = col;
    if (dir == DIRECTION_UP) {
        *outRow = row - 1;
    }
    else if (dir == DIRECTION_DOWN) {
        *outRow = row + 1;
    }
    else if (dir == DIRECTION_LEFT) {
        *outCol = col - 1;
    }
    else if (dir == DIRECTION_RIGHT) {
        *outCol = col + 1;
    }
}

bool tryMoveBlockInDirection (block_game *blockGame, int row, int col, direction dir) {
    int nextRow, nextCol;
    nextRowCol(row, col, dir, &nextRow, &nextCol);
    if (nextRow < 0 || nextRow >= NUM_GRID_ROWS || 
        nextCol < 0 || nextCol >= NUM_GRID_COLS) 
    {
        return false;
    }
    int nextGridID = blockGame->grid[nextRow * NUM_GRID_ROWS + nextCol];

    bool canMoveBlock = false;
    if (nextGridID == -1) {
        canMoveBlock = true;
    }
    else {
        canMoveBlock = tryMoveBlockInDirection(blockGame, nextRow, nextCol, dir);
    }

    if (canMoveBlock) {
        int blockID = blockGame->grid[row * NUM_GRID_ROWS + col];
        grid_block *block = &blockGame->blocks[blockID];
        block->prevRow = block->row;
        block->prevCol = block->col;
        block->row = nextRow;
        block->col = nextCol;
        block->moving = true;
        block->moveTime = 0.0f;
        blockGame->grid[nextRow * NUM_GRID_ROWS + nextCol] = blockID;
        blockGame->grid[row * NUM_GRID_ROWS + col] = -1;
    }

    return canMoveBlock;
}

void blockBubbleSort (grid_block *blocks, block_game *blockGame) {
    for (int i = 0; i < blockGame->numBlocks; ++i) {
        for (int j = i; j > 0; --j) {
            grid_block firstBlock = blocks[j-1];
            grid_block secondBlock = blocks[j];
            if (secondBlock.row < firstBlock.row) {
                blocks[j-1] = secondBlock;
                blocks[j] = firstBlock;
            }
        }
    }
}

bool blockFitsOnGrid (block_piece *blockPiece, int row, int col) {
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            if (blockPiece->filledCells[(i+2) * 5 + (j+2)] == 1) {
                int gridRow = i + row;
                int gridCol = j + col;
                if (gridRow < 0 || gridRow >= NUM_GRID_ROWS) { return false; }
                if (gridCol < 0 || gridCol >= NUM_GRID_COLS) { return false; }
            }
        }
    }
    return true;
}

void chooseNextBlockPiece (block_piece *blockPiece) {
    unsigned int nextPieceType = randomUint() % 19;

    int *nextPiece;
    switch (nextPieceType) {
        default: {
            nextPiece = 0;
            assert(false);
        } break;
        case 0: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 1: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 1, 1, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 2: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 3: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 1, 0, 0,
                0, 1, 1, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 4: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 5: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 1, 1, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 6: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 1, 1, 0,
                0, 0, 1, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 7: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 1, 1, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 8: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 9: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 1, 1, 1, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 10: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 1, 1, 0,
                0, 0, 1, 1, 0,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 11: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 1, 1, 1, 0,
                0, 1, 1, 1, 0,
                0, 1, 1, 1, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 12: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 1, 1, 1, 0,
                0, 1, 0, 0, 0,
                0, 1, 0, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 13: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 1, 1, 1, 0,
                0, 0, 0, 1, 0,
                0, 0, 0, 1, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 14: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 1, 0, 0, 0,
                0, 1, 0, 0, 0,
                0, 1, 1, 1, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 15: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 0, 1, 0,
                0, 0, 0, 1, 0,
                0, 1, 1, 1, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 16: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 1, 1, 1, 1,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 17: {
            int blockType[] = {
                0, 0, 1, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 18: {
            int blockType[] = {
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                1, 1, 1, 1, 1,
                0, 0, 0, 0, 0,
                0, 0, 0, 0, 0
            };
            nextPiece = blockType;
        } break;
        case 19: {
            int blockType[] = {
                0, 0, 1, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 1, 0, 0
            };
            nextPiece = blockType;
        } break;
    }

    for (int i = 0; i < 5*5; ++i) {
        blockPiece->filledCells[i] = nextPiece[i];
    }

    unsigned int randomColor = randomUint() % 6;
    switch (randomColor) {
        case 0: {
            blockPiece->color = "red_tile";
        } break;
        case 1: {
            blockPiece->color = "green_tile";
        } break;
        case 2: {
            blockPiece->color = "blue_tile";
        } break;
        case 3: {
            blockPiece->color = "cyan_tile";
        } break;
        case 4: {
            blockPiece->color = "magenta_tile";
        } break;
        case 5: {
            blockPiece->color = "yellow_tile";
        } break;
    }
}

void chooseNextBlockPieceLocation (block_game *blockGame) {
    block_piece *blockPiece = &blockGame->nextPiece;

    int row = 0;
    int col = 0;
    do {
        row = randomUint() % NUM_GRID_ROWS;
        col = randomUint() % NUM_GRID_COLS;
    } while (!blockFitsOnGrid(blockPiece, row, col));

    blockGame->nextBlockPieceRow = row;
    blockGame->nextBlockPieceCol = col;
}

void initBlockGame (memory_arena *memory, block_game* blockGame) {
    setRNGSeed(0); // TODO(ebuchholz): seed with time?

    *blockGame = {};
    grid_block *blocks = blockGame->blocks;

    for (int i = 0; i < NUM_GRID_ROWS; ++i) {
        for (int j = 0; j < NUM_GRID_COLS; ++j) {
            blockGame->grid[i * NUM_GRID_COLS + j] = -1;
        }
    }

    //addNewBlockToGrid(blockGame, 3, 1);
    //addNewBlockToGrid(blockGame, 3, 2);
    //addNewBlockToGrid(blockGame, 3, 3);
    //addNewBlockToGrid(blockGame, 3, 4);
    //addNewBlockToGrid(blockGame, 3, 5);
    //addNewBlockToGrid(blockGame, 3, 6);
    //addNewBlockToGrid(blockGame, 3, 7);
    //addNewBlockToGrid(blockGame, 3, 8);

    //addNewBlockToGrid(blockGame, 7, 1);
    //addNewBlockToGrid(blockGame, 7, 2);
    //addNewBlockToGrid(blockGame, 7, 3);
    //addNewBlockToGrid(blockGame, 7, 4);
    //addNewBlockToGrid(blockGame, 7, 5);
    //addNewBlockToGrid(blockGame, 7, 6);
    //addNewBlockToGrid(blockGame, 7, 7);
    //addNewBlockToGrid(blockGame, 7, 8);

    //addNewBlockToGrid(blockGame, 8, 4);
    //addNewBlockToGrid(blockGame, 8, 5);
    //addNewBlockToGrid(blockGame, 8, 6);
    //addNewBlockToGrid(blockGame, 8, 7);

    blockGame->sheep = {};
    blockGame->sheep.row = 6;
    blockGame->sheep.col = 6;

    blockGame->timeToSpawnNextPiece = 1.0f;
    blockGame->nextPieceTimer = 0.0f;

    chooseNextBlockPiece(&blockGame->nextPiece);
    chooseNextBlockPieceLocation(blockGame);
}

void updateBlockGame (memory_arena *memory, memory_arena *tempMemory, game_assets *assets, game_input *input, 
                      block_game *blockGame, sprite_list *spriteList) 
{
    if (input->turnUpButtonJustPressed) {
        blockGame->nextMoveDirection = DIRECTION_UP;
    }
    else if (input->turnDownButtonJustPressed) {
        blockGame->nextMoveDirection = DIRECTION_DOWN;
    }
    else if (input->turnLeftButtonJustPressed) {
        blockGame->nextMoveDirection = DIRECTION_LEFT;
    }
    else if (input->turnRightButtonJustPressed) {
        blockGame->nextMoveDirection = DIRECTION_RIGHT;
    }

    float gridRowStart = -((float)(NUM_GRID_ROWS - 1) * GRID_BLOCK_WIDTH) * 0.5f;
    float gridColStart = -((float)(NUM_GRID_COLS - 1) * GRID_BLOCK_HEIGHT) * 0.5f;

    block_sheep *sheep = &blockGame->sheep;
    if (!sheep->moving) {
        sheep->prevRow = sheep->row;
        sheep->prevCol = sheep->col;

        sheep->x = gridColStart + GRID_BLOCK_WIDTH * sheep->col;
        sheep->y = gridRowStart + GRID_BLOCK_HEIGHT * sheep->row;

        direction sheepMoveDir = tryMoveSheep(blockGame, input);
        if (sheepMoveDir != DIRECTION_NONE) {
            blockGame->nextMoveDirection = DIRECTION_NONE;

            int nextRow = sheep->row;
            int nextCol = sheep->col;

            bool validMove = true;
            nextRowCol(nextRow, nextCol, sheepMoveDir, &nextRow, &nextCol);

            if (nextRow < 0 || nextRow >= NUM_GRID_ROWS || 
                nextCol < 0 || nextCol >= NUM_GRID_COLS) 
            {
                validMove = false;
            }
            else if (blockGame->grid[nextRow * NUM_GRID_COLS + nextCol] != -1) {
                validMove = tryMoveBlockInDirection(blockGame, nextRow, nextCol, sheepMoveDir);
            }
            
            if (validMove) {
                sheep->row = nextRow;
                sheep->col = nextCol;
                sheep->moving = true;
                sheep->moveTime = 0.0f;
            }
        }
    }
    if (sheep->moving) {
        moveSheep(blockGame, input);
    }
    moveBlocks(blockGame, input);

    // check for lines
    bool rowsToClear[NUM_GRID_ROWS];
    for (int i = 0; i < NUM_GRID_ROWS; ++i) {
        bool foundFullRow = true;
        for (int j = 0; j < NUM_GRID_COLS; ++j) {
            int blockID = blockGame->grid[i * NUM_GRID_ROWS + j];
            if (blockID == -1) {
                foundFullRow = false;
                break;
            }
            else {
                grid_block *block = blockGame->blocks + blockID;
                if (block->moving) {
                    foundFullRow = false;
                    break;
                }
            }
        }
        if (foundFullRow) {
            rowsToClear[i] = true;
        }
        else {
            rowsToClear[i] = false;
        }
    }

    bool colsToClear[NUM_GRID_COLS];
    for (int i = 0; i < NUM_GRID_COLS; ++i) {
        bool foundFullCol = true;
        for (int j = 0; j < NUM_GRID_ROWS; ++j) {
            int blockID = blockGame->grid[j * NUM_GRID_ROWS + i];
            if (blockID == -1) {
                foundFullCol = false;
                break;
            }
            else {
                grid_block *block = blockGame->blocks + blockID;
                if (block->moving) {
                    foundFullCol = false;
                    break;
                }
            }
        }
        if (foundFullCol) {
            colsToClear[i] = true;
        }
        else {
            colsToClear[i] = false;
        }
    }

    // TODO(ebuchholz): animate, pause until done
    for (int i = 0; i < NUM_GRID_ROWS; ++i) {
        if (rowsToClear[i]) {
            for (int j = 0; j < NUM_GRID_COLS; ++j) {
                int blockID = blockGame->grid[i * NUM_GRID_ROWS + j];
                grid_block *block = blockGame->blocks + blockID;

                blockGame->grid[i * NUM_GRID_ROWS + j] = -1;
                block->active = false;
            }
        }
    }
    for (int i = 0; i < NUM_GRID_COLS; ++i) {
        if (colsToClear[i]) {
            for (int j = 0; j < NUM_GRID_ROWS; ++j) {
                int blockID = blockGame->grid[j * NUM_GRID_ROWS + i];
                // may have been cleared in a row
                if (blockID != -1) {
                    grid_block *block = blockGame->blocks + blockID;

                    blockGame->grid[j * NUM_GRID_ROWS + i] = -1;
                    block->active = false;
                }
            }
        }
    }

    blockGame->nextPieceTimer += DELTA_TIME;
    if (blockGame->nextPieceTimer >= blockGame->timeToSpawnNextPiece) {
        blockGame->nextPieceTimer -= blockGame->timeToSpawnNextPiece;

        block_piece *nextPiece = &blockGame->nextPiece;
        addBlockIfUnoccupied(blockGame, randomUint() % NUM_GRID_ROWS, randomUint() % NUM_GRID_COLS);
        //for (int i = -2; i <= 2; ++i) {
        //    for (int j = -2; j <= 2; ++j) {
        //        if (nextPiece->filledCells[(i+2) * 5 + (j+2)]) {
        //            int gridRow = i + blockGame->nextBlockPieceRow;
        //            int gridCol = j + blockGame->nextBlockPieceCol;
        //            addBlockIfUnoccupied(blockGame, gridRow, gridCol);
        //        }
        //    }
        //}
        chooseNextBlockPiece(&blockGame->nextPiece);
        chooseNextBlockPieceLocation(blockGame);
    }

    pushSpriteTransform(spriteList, Vector2(GAME_WIDTH/2.0f, GAME_HEIGHT/2.0f));

    for (int i = 0; i < NUM_GRID_ROWS; ++i) {
        for (int j = 0; j < NUM_GRID_COLS; ++j) {
            addSprite(gridColStart + GRID_BLOCK_WIDTH * j, gridRowStart + GRID_BLOCK_HEIGHT * i, assets, ATLAS_KEY_GAME, "tile_backing", spriteList, 0.5f, 0.5f);
        }
    }

    grid_block sortedBlocks[NUM_GRID_ROWS * NUM_GRID_COLS];
    // TODO(ebuchholz): memory copy
    for (int i = 0; i < blockGame->numBlocks; ++i) {
        sortedBlocks[i] = blockGame->blocks[i];
    }
    blockBubbleSort(sortedBlocks, blockGame);

    for (int i = 0; i < blockGame->numBlocks; ++i) {
        grid_block *block = &sortedBlocks[i];
        if (block->active) {
            addSprite(block->x, block->y, assets, ATLAS_KEY_GAME, "blue_tile", spriteList, 0.5f, 0.5f);
        }
    }

    addSprite(sheep->x, sheep->y, assets, ATLAS_KEY_GAME, "sheep", spriteList, 0.5f, 0.5f);

    popSpriteMatrix(spriteList);
}
