#ifndef BLOCK_GAME_H
#define BLOCK_GAME_H

#define NUM_GRID_ROWS 10
#define NUM_GRID_COLS 10
#define GRID_BLOCK_WIDTH 16.0f
#define GRID_BLOCK_HEIGHT 16.0f

#define BLOCK_MOVE_SPEED 0.15f

enum direction {
    DIRECTION_NONE,
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
};

// TODO(ebuchholz): group together "movable block" stuff
struct block_sheep {
    bool moving;
    float moveTime;

    int row;
    int col;
    int prevCol;
    int prevRow;
    float x;
    float y;
};

struct grid_block {
    char *color;
    int id;
    bool active;

    bool moving;
    float moveTime;

    int row;
    int col;
    int prevCol;
    int prevRow;
    float x;
    float y;
};

struct block_piece {
    int filledCells[5*5];
    char *color;
};

enum block_game_state {
    BLOCK_GAME_STATE_MOVING_BLOCKS,
    BLOCK_GAME_STATE_CLEARING_LINES
};

struct block_game {
    grid_block blocks[NUM_GRID_ROWS * NUM_GRID_COLS];
    int numBlocks = 0;

    int grid[NUM_GRID_ROWS * NUM_GRID_COLS];

    block_sheep sheep;
    direction nextMoveDirection;

    int score;

    float timeToSpawnNextPiece;
    float nextPieceTimer;

    block_piece nextPiece;
    int nextBlockPieceRow;
    int nextBlockPieceCol;

    float blockIndicatorTimer;
    bool fadingInBlockIndicator;

    block_game_state gameState;

    // TODO(ebuchholz): bit flags?
    bool rowsBeingCleared[NUM_GRID_ROWS];
    bool colsBeingCleared[NUM_GRID_COLS];
    bool clearedBlocksVisible;
    float clearingBlocksTimer;

};


#endif
