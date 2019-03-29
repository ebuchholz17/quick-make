#ifndef QUICKMAKE_SPRITES_H
#define QUICKMAKE_SPRITES_H

// TODO(ebuchholz): make stretchy? these are arbitrary numbers
#define MAX_SPRITES_PER_FRAME 2000

struct sprite {
    matrix3x3 parentTransform;
    vector2 pos;
    vector2 anchor;
    vector2 frameCorners[4];
    float width;
    float height;
    float scale;
    float rotation;
    float alpha;
    unsigned int tint;
    texture_key textureKey;
};

#define MATRIX_STACK_CAPACITY 20

struct sprite_list {
    matrix3x3 matrixStack[MATRIX_STACK_CAPACITY];
    sprite *sprites;

    int numSprites;
    unsigned int matrixStackIndex;
};

#endif
