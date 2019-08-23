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

void addSprite (float x, float y, game_assets *assets, texture_key textureKey, sprite_list *spriteList, 
                float anchorX = 0.0f, float anchorY = 0.0f, float scale=1.0f, float rotation = 0.0f, 
                float alpha = 1.0f, unsigned int tint = 0xffffff) 
{
    sprite *nextSprite = createSpriteAndSetProperties(x, y, assets, spriteList, anchorX, anchorY, scale, rotation, alpha, tint);

    // TODO(ebuchholz): maybe pointer to texture info instead of copying?
    texture_asset *texAsset = assets->textures[textureKey];
    nextSprite->textureKey = textureKey;
    nextSprite->width = (float)texAsset->width;
    nextSprite->height = (float)texAsset->height;

    nextSprite->frameCorners[0] = Vector2(0.0f, 1.0f);
    nextSprite->frameCorners[1] = Vector2(1.0f, 1.0f);
    nextSprite->frameCorners[2] = Vector2(0.0f, 0.0f);
    nextSprite->frameCorners[3] = Vector2(1.0f, 0.0f);
}

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

// TODO(ebuchholz): maybe add support for bitmap fonts defined with .fnt files
struct bitmap_font_letter_coord {
    float x;
    float y;
};
static bitmap_font_letter_coord fontLetterCoords[255];

void initLetterCoords () {
    bitmap_font_letter_coord coord;

    // initialize everything to space
    coord.x = 121.0f;
    coord.y = 41.0f;
    for (int i = 0; i < 255; ++i) {
        fontLetterCoords[i] = coord;
    }

    coord.x = 1.0f;
    coord.y = 1.0f;
    fontLetterCoords['a'] = coord;
    fontLetterCoords['A'] = coord;

    coord.x = 11.0f;
    coord.y = 1.0f;
    fontLetterCoords['b'] = coord;
    fontLetterCoords['B'] = coord;

    coord.x = 21.0f;
    coord.y = 1.0f;
    fontLetterCoords['c'] = coord;
    fontLetterCoords['C'] = coord;

    coord.x = 31.0f;
    coord.y = 1.0f;
    fontLetterCoords['d'] = coord;
    fontLetterCoords['D'] = coord;

    coord.x = 41.0f;
    coord.y = 1.0f;
    fontLetterCoords['e'] = coord;
    fontLetterCoords['E'] = coord;

    coord.x = 51.0f;
    coord.y = 1.0f;
    fontLetterCoords['f'] = coord;
    fontLetterCoords['F'] = coord;

    coord.x = 61.0f;
    coord.y = 1.0f;
    fontLetterCoords['g'] = coord;
    fontLetterCoords['G'] = coord;

    coord.x = 71.0f;
    coord.y = 1.0f;
    fontLetterCoords['h'] = coord;
    fontLetterCoords['H'] = coord;

    coord.x = 81.0f;
    coord.y = 1.0f;
    fontLetterCoords['i'] = coord;
    fontLetterCoords['I'] = coord;

    coord.x = 91.0f;
    coord.y = 1.0f;
    fontLetterCoords['j'] = coord;
    fontLetterCoords['J'] = coord;

    coord.x = 101.0f;
    coord.y = 1.0f;
    fontLetterCoords['k'] = coord;
    fontLetterCoords['K'] = coord;

    coord.x = 111.0f;
    coord.y = 1.0f;
    fontLetterCoords['l'] = coord;
    fontLetterCoords['L'] = coord;

    coord.x = 121.0f;
    coord.y = 1.0f;
    fontLetterCoords['m'] = coord;
    fontLetterCoords['M'] = coord;

    coord.x = 131.0f;
    coord.y = 1.0f;
    fontLetterCoords['n'] = coord;
    fontLetterCoords['N'] = coord;

    coord.x = 1.0f;
    coord.y = 11.0f;
    fontLetterCoords['o'] = coord;
    fontLetterCoords['O'] = coord;

    coord.x = 11.0f;
    coord.y = 11.0f;
    fontLetterCoords['p'] = coord;
    fontLetterCoords['P'] = coord;

    coord.x = 21.0f;
    coord.y = 11.0f;
    fontLetterCoords['q'] = coord;
    fontLetterCoords['Q'] = coord;

    coord.x = 31.0f;
    coord.y = 11.0f;
    fontLetterCoords['r'] = coord;
    fontLetterCoords['R'] = coord;

    coord.x = 41.0f;
    coord.y = 11.0f;
    fontLetterCoords['s'] = coord;
    fontLetterCoords['S'] = coord;

    coord.x = 51.0f;
    coord.y = 11.0f;
    fontLetterCoords['t'] = coord;
    fontLetterCoords['T'] = coord;

    coord.x = 61.0f;
    coord.y = 11.0f;
    fontLetterCoords['u'] = coord;
    fontLetterCoords['U'] = coord;

    coord.x = 71.0f;
    coord.y = 11.0f;
    fontLetterCoords['v'] = coord;
    fontLetterCoords['V'] = coord;

    coord.x = 81.0f;
    coord.y = 11.0f;
    fontLetterCoords['w'] = coord;
    fontLetterCoords['W'] = coord;

    coord.x = 91.0f;
    coord.y = 11.0f;
    fontLetterCoords['x'] = coord;
    fontLetterCoords['X'] = coord;

    coord.x = 101.0f;
    coord.y = 11.0f;
    fontLetterCoords['y'] = coord;
    fontLetterCoords['Y'] = coord;

    coord.x = 111.0f;
    coord.y = 11.0f;
    fontLetterCoords['z'] = coord;
    fontLetterCoords['Z'] = coord;

    coord.x = 121.0f;
    coord.y = 11.0f;
    fontLetterCoords['0'] = coord;

    coord.x = 131.0f;
    coord.y = 11.0f;
    fontLetterCoords['1'] = coord;

    coord.x = 1.0f;
    coord.y = 21.0f;
    fontLetterCoords['2'] = coord;

    coord.x = 11.0f;
    coord.y = 21.0f;
    fontLetterCoords['3'] = coord;

    coord.x = 21.0f;
    coord.y = 21.0f;
    fontLetterCoords['4'] = coord;

    coord.x = 31.0f;
    coord.y = 21.0f;
    fontLetterCoords['5'] = coord;

    coord.x = 41.0f;
    coord.y = 21.0f;
    fontLetterCoords['6'] = coord;

    coord.x = 51.0f;
    coord.y = 21.0f;
    fontLetterCoords['7'] = coord;

    coord.x = 61.0f;
    coord.y = 21.0f;
    fontLetterCoords['8'] = coord;

    coord.x = 71.0f;
    coord.y = 21.0f;
    fontLetterCoords['9'] = coord;

    coord.x = 81.0f;
    coord.y = 21.0f;
    fontLetterCoords['`'] = coord;

    coord.x = 91.0f;
    coord.y = 21.0f;
    fontLetterCoords['~'] = coord;

    coord.x = 101.0f;
    coord.y = 21.0f;
    fontLetterCoords['!'] = coord;

    coord.x = 111.0f;
    coord.y = 21.0f;
    fontLetterCoords['@'] = coord;

    coord.x = 121.0f;
    coord.y = 21.0f;
    fontLetterCoords['#'] = coord;

    coord.x = 131.0f;
    coord.y = 21.0f;
    fontLetterCoords['$'] = coord;

    coord.x = 1.0f;
    coord.y = 31.0f;
    fontLetterCoords['%'] = coord;

    coord.x = 11.0f;
    coord.y = 31.0f;
    fontLetterCoords['^'] = coord;

    coord.x = 21.0f;
    coord.y = 31.0f;
    fontLetterCoords['&'] = coord;

    coord.x = 31.0f;
    coord.y = 31.0f;
    fontLetterCoords['*'] = coord;

    coord.x = 41.0f;
    coord.y = 31.0f;
    fontLetterCoords['('] = coord;

    coord.x = 51.0f;
    coord.y = 31.0f;
    fontLetterCoords[')'] = coord;

    coord.x = 61.0f;
    coord.y = 31.0f;
    fontLetterCoords['-'] = coord;

    coord.x = 71.0f;
    coord.y = 31.0f;
    fontLetterCoords['_'] = coord;

    coord.x = 81.0f;
    coord.y = 31.0f;
    fontLetterCoords['='] = coord;

    coord.x = 91.0f;
    coord.y = 31.0f;
    fontLetterCoords['+'] = coord;

    coord.x = 101.0f;
    coord.y = 31.0f;
    fontLetterCoords['['] = coord;

    coord.x = 111.0f;
    coord.y = 31.0f;
    fontLetterCoords['{'] = coord;

    coord.x = 121.0f;
    coord.y = 31.0f;
    fontLetterCoords[']'] = coord;

    coord.x = 131.0f;
    coord.y = 31.0f;
    fontLetterCoords['}'] = coord;

    coord.x = 1.0f;
    coord.y = 41.0f;
    fontLetterCoords[';'] = coord;

    coord.x = 11.0f;
    coord.y = 41.0f;
    fontLetterCoords[':'] = coord;

    coord.x = 21.0f;
    coord.y = 41.0f;
    fontLetterCoords['\''] = coord;

    coord.x = 31.0f;
    coord.y = 41.0f;
    fontLetterCoords['"'] = coord;

    coord.x = 41.0f;
    coord.y = 41.0f;
    fontLetterCoords[','] = coord;

    coord.x = 51.0f;
    coord.y = 41.0f;
    fontLetterCoords['<'] = coord;

    coord.x = 61.0f;
    coord.y = 41.0f;
    fontLetterCoords['.'] = coord;

    coord.x = 71.0f;
    coord.y = 41.0f;
    fontLetterCoords['>'] = coord;

    coord.x = 81.0f;
    coord.y = 41.0f;
    fontLetterCoords['/'] = coord;

    coord.x = 91.0f;
    coord.y = 41.0f;
    fontLetterCoords['?'] = coord;

    coord.x = 101.0f;
    coord.y = 41.0f;
    fontLetterCoords['\\'] = coord;

    coord.x = 111.0f;
    coord.y = 41.0f;
    fontLetterCoords['|'] = coord;

    coord.x = 121.0f;
    coord.y = 41.0f;
    fontLetterCoords[' '] = coord;
}

void addText (float x, float y, char *text, game_assets *assets, texture_key fontTextureKey, sprite_list *spriteList) {
    char *currentLetter = text;
    float letterTexWidth = 8.0f / 140.0f;
    float letterTexHeight = 8.0f / 50.0f;
    float xOffset = 0.0f;
    float yOffset = 0.0f;
    while (*currentLetter != 0) {
        char letter = *currentLetter;

        if (letter == '\n') {
            xOffset = 0.0f;
            yOffset += 8.0f;
            ++currentLetter;
        }
        else {
            sprite *nextSprite = createSpriteAndSetProperties(x + xOffset, y + yOffset, assets, spriteList);

            // TODO(ebuchholz): maybe pointer to texture info instead of copying?
            texture_asset *texAsset = assets->textures[fontTextureKey];
            nextSprite->textureKey = fontTextureKey;
            nextSprite->width = 8.0f;//(float)texAsset->width;
            nextSprite->height = 8.0f;//(float)texAsset->height;

            bitmap_font_letter_coord *coord = fontLetterCoords + letter;
            float coordX = coord->x / 140.0f;
            float coordY = ((50.0f - coord->y) / 50.0f);
            
            nextSprite->frameCorners[0] = Vector2(coordX, coordY);
            nextSprite->frameCorners[1] = Vector2(coordX + letterTexWidth, coordY);
            nextSprite->frameCorners[2] = Vector2(coordX, coordY - letterTexHeight);
            nextSprite->frameCorners[3] = Vector2(coordX + letterTexWidth, coordY - letterTexHeight);

            ++currentLetter;
            xOffset += 8.0f;
        }
    }
}
