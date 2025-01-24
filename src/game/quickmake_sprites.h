#ifndef QUICKMAKE_SPRITES_H
#define QUICKMAKE_SPRITES_H

#include "quickmake_math.h"
#include "quickmake_assets.h"

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
    unsigned int textureID;
};

#define MATRIX_STACK_CAPACITY 20

struct sprite_list {
    matrix3x3 matrixStack[MATRIX_STACK_CAPACITY];
    sprite *sprites;

    int numSprites;
    unsigned int matrixStackIndex;
};

void pushSpriteTransform(sprite_list *spriteList, vector2 pos, float scale , float rotation ) ;
matrix3x3 popSpriteMatrix (sprite_list *spriteList) ;
void pushSpriteMatrix (matrix3x3 transform, sprite_list *spriteList) ;

matrix3x3 peekSpriteMatrix (sprite_list *spriteList) ;

void addSprite (float x, float y, game_assets *assets, char *textureKey, sprite_list *spriteList, 
                float anchorX , float anchorY , float scale, float rotation , 
                float alpha , unsigned int tint ) 
;

void addSprite (float x, float y, game_assets *assets, char *atlasKey, char *frameName, sprite_list *spriteList, 
                float anchorX , float anchorY , float scale, float rotation , 
                float alpha , unsigned int tint ) 
;
void addText (float x, float y, char *text, game_assets *assets, char *fontTextureKey, sprite_list *spriteList) ;

#endif
