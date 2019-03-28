#include "quickmake_sprites.h"

void pushSpriteMatrix (matrix3x3 transform, sprite_list *spriteList) {
    assert(spriteList->matrixStackIndex < MATRIX_STACK_CAPACITY);
    spriteList->matrixStackIndex++;
    spriteList->matrixStack[spriteList->matrixStackIndex] = 
        spriteList->matrixStack[spriteList->matrixStackIndex - 1] * transform;
}

matrix3x3 peekSpriteMatrix (sprite_list *spriteList) {
    return spriteList->matrixStack[spriteList->matrixStackIndex];
}

matrix3x3 popSpriteMatrix (sprite_list *spriteList) {
    assert(spriteList->matrixStackIndex > 0);
    matrix3x3 result = spriteList->matrixStack[spriteList->matrixStackIndex];
    spriteList->matrixStackIndex--;
    return result;
}

void pushSpriteTransform(sprite_list *spriteList, vector2 pos, float scale = 1.0f, float rotation = 0.0f) {
    matrix3x3 transform = scaleMatrix3x3(scale, scale);
    transform = rotationMatrix3x3(rotation) * transform;
    transform = translationMatrix(pos.x, pos.y) * transform;

    pushSpriteMatrix(transform, spriteList);
}

sprite *createSpriteAndSetProperties (float x, float y, game_assets *assets, sprite_list *spriteList, 
                                      float anchorX = 0.0f, float anchorY = 0.0f, float scale=1.0f, float rotation = 0.0f, 
                                      float alpha = 1.0f, unsigned int tint = 0xffffff) 
{
    assert(spriteList->numSprites < MAX_SPRITES_PER_FRAME);

    sprite *nextSprite = &spriteList->sprites[spriteList->numSprites];
    *nextSprite = {};
    ++spriteList->numSprites;
    nextSprite->pos.x = x;
    nextSprite->pos.y = y;
    nextSprite->anchor.x = anchorX;
    nextSprite->anchor.y = anchorY;
    nextSprite->scale = scale;
    nextSprite->rotation = rotation;
    nextSprite->alpha = alpha;
    nextSprite->tint = tint;

    nextSprite->parentTransform = peekSpriteMatrix(spriteList);

    return nextSprite;
}

//void addSprite (float x, float y, game_assets *assets, texture_key textureKey, sprite_list *spriteList, 
//                       float anchorX = 0.0f, float anchorY = 0.0f, float scale=1.0f, float rotation = 0.0f, 
//                       float alpha = 1.0f, unsigned int tint = 0xffffff) 
//{
//    sprite *nextSprite = createSpriteAndSetProperties(x, y, assets, spriteList, anchorX, anchorY, scale, rotation, alpha, tint);
//
//    // TODO(ebuchholz): maybe pointer to texture info instead of copying?
//    texture_asset *texAsset = assets->textures[textureKey];
//    nextSprite->textureKey = textureKey
//    nextSprite->width = (float)texAsset->width;
//    nextSprite->height = (float)texAsset->height;
//
//    nextSprite->frameCorners[0] = Vector2(0.0f, 1.0f);
//    nextSprite->frameCorners[1] = Vector2(1.0f, 1.0f);
//    nextSprite->frameCorners[2] = Vector2(0.0f, 0.0f);
//    nextSprite->frameCorners[3] = Vector2(1.0f, 0.0f);
//}

void addSprite (float x, float y, game_assets *assets, atlas_key atlasKey, char *frameName, sprite_list *spriteList, 
                float anchorX = 0.0f, float anchorY = 0.0f, float scale=1.0f, float rotation = 0.0f, 
                float alpha = 1.0f, unsigned int tint = 0xffffff) 
{
    sprite *nextSprite = createSpriteAndSetProperties(x, y, assets, spriteList, anchorX, anchorY, scale, rotation, alpha, tint);

    atlas_asset *atlas = assets->atlases[atlasKey];
    nextSprite->textureKey = atlas->textureKey;

    atlas_frame *frame = getAtlasFrame(assets, atlasKey, frameName);
    nextSprite->width = (float)frame->frameWidth;
    nextSprite->height = (float)frame->frameHeight;
    for (int i = 0; i < 4; ++i) {
        nextSprite->frameCorners[i] = frame->frameCorners[i];
    }
}
