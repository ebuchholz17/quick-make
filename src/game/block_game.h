#ifndef BLOCK_GAME_H
#define BLOCK_GAME_H

#define NUM_GRID_ROWS 10
#define NUM_GRID_COLS 10
#define GRID_BLOCK_WIDTH 16.0f
#define GRID_BLOCK_HEIGHT 16.0f

#define SHEEP_MOVE_SPEED 0.15f

struct block_sheep {
    int row;
    int col;

    float x;
    float y;

    bool moving;
    int prevCol;
    int prevRow;
    float moveTime;
};

struct grid_block {
    int row;
    int col;
};

struct block_game {
    grid_block blocks[NUM_GRID_ROWS * NUM_GRID_COLS];
    int numBlocks = 0;

    block_sheep sheep;
    int nextMoveDirection;
};


#endif
