#import "ios_platform.h"

#include "../../../game/quickmake_game.cpp"

static int gameWidth = 768;
static int gameHeight = 432;
static float targetMSPerFrame = 1000.0f / 60.0f;

@interface IOSPlatform () {
    game_memory gameMemory; 
    platform_options options;
    render_command_list renderCommands;
    game_input input;
}
@end

@implementation IOSPlatform

- (void)setup {
    // set up update loop
    self.timer = [CADisplayLink displayLinkWithTarget:self selector:@selector(update)];
    [self.timer addToRunLoop:NSRunLoop.mainRunLoop forMode:NSDefaultRunLoopMode];

    // init renderer

    // init game memory
    // TODO(ebuchholz): determine memory limitations for ios
    gameMemory = {};
    gameMemory.memoryCapacity = 50 * 1024 * 1024; // 50MB arbitrarily decided
    gameMemory.memory = malloc(gameMemory.memoryCapacity);
    gameMemory.tempMemoryCapacity = 10 * 1024 * 1024; // another 10MB arbitrarily decided
    gameMemory.tempMemory = malloc(gameMemory.tempMemoryCapacity);

    for (unsigned int i = 0; i < gameMemory.memoryCapacity; ++i) {
        *((char *)gameMemory.memory + i) = 0;
    }

    // init sounds

    // set up platform options
    options = {};
    //options.audioSampleRate = soundOutput.samplesPerSecond;

    // set up and load assets
    asset_list assetList = {};
    assetList.numAssetsToLoad = 0;
    assetList.maxAssetsToLoad = 100;
    assetList.assetsToLoad = 
        (asset_to_load *)malloc(assetList.maxAssetsToLoad * sizeof(asset_to_load));

    getGameAssetList(&assetList);

    memory_arena workingAssetMemory = {};
    workingAssetMemory.capacity = 30 * 1024 * 1024; // 30MB limit for working with asset files?
    workingAssetMemory.base = malloc(workingAssetMemory.capacity);

    // TODO(ebuchholz): determine how files/resources are loaded on ios
    NSBundle *mainResourceBundle = [NSBundle mainBundle];
    for (int i = 0; i < assetList.numAssetsToLoad; ++i) {
        asset_to_load *assetToLoad = assetList.assetsToLoad + i;

        NSString *fullPath = [NSString stringWithUTF8String:assetToLoad->path];
        NSString *fileNameWithoutExt = [fullPath stringByDeletingPathExtension];
        NSString *fileName = [[fullPath lastPathComponent] stringByDeletingPathExtension];
        NSString *extension = [fullPath pathExtension];
        NSString *resourcePath = [mainResourceBundle pathForResource:fileNameWithoutExt ofType:extension];

        workingAssetMemory.size = 0;
        switch (assetToLoad->type){
            // catch-all for text files? no work to be done on platform except reading the file
            case ASSET_TYPE_ANIMATION_DATA: {
                // TODO(ebuchholz): define real skeleton+animations file format define real skeleton+animations file format define real skeleton+animations file format define real skeleton+animations file format define real skeleton+animations file format
                char *fileData = 0;
                //char *fileData = readEntireTextFile(assetToLoad->path);
                parseGameAsset(fileData, 0, assetToLoad->type, assetToLoad->key, assetToLoad->secondKey, 
                               &gameMemory, &workingAssetMemory, &options, 0);
                //free(fileData);
            } break;
            case ASSET_TYPE_OBJ: {
                NSString *fileString = [NSString stringWithContentsOfFile:resourcePath 
                                        encoding:NSUTF8StringEncoding error: NULL];
                char *fileData = (char *)[fileString UTF8String];
                parseGameAsset(fileData, 0, ASSET_TYPE_OBJ, assetToLoad->key, assetToLoad->secondKey, 
                               &gameMemory, &workingAssetMemory, &options, 0);

                //loadRendererMesh(&rendererMemory, (loaded_mesh_asset *)workingAssetMemory.base);
            } break;
            case ASSET_TYPE_QMM: {
                char *fileData = 0;
                //char *fileData = readEntireTextFile(assetToLoad->path);
                parseGameAsset(fileData, 0, ASSET_TYPE_QMM, assetToLoad->key, assetToLoad->secondKey, 
                               &gameMemory, &workingAssetMemory, &options, 0);
                //free(fileData);

                //loadRendererAnimatedMesh(&rendererMemory, (loaded_animated_mesh_asset *)workingAssetMemory.base);
            } break;
            case ASSET_TYPE_BMP: {
                NSData *fileNSData = [NSData dataWithContentsOfFile:resourcePath];
                char *fileData = (char *)[fileNSData bytes];

                parseGameAsset(fileData, 0, ASSET_TYPE_BMP, assetToLoad->key, assetToLoad->secondKey, 
                               &gameMemory, &workingAssetMemory, &options, 0);

                // load texture onto gpu
                //loadRendererTexture(&rendererMemory, (loaded_texture_asset *)workingAssetMemory.base);
            } break;
            case ASSET_TYPE_WAV: {
                NSData *fileNSData = [NSData dataWithContentsOfFile:resourcePath];
                char *fileData = (char *)[fileNSData bytes];

                parseGameAsset(fileData, 0, ASSET_TYPE_WAV, assetToLoad->key, assetToLoad->secondKey, 
                               &gameMemory, &workingAssetMemory, &options, 0);
            } break;
            case ASSET_TYPE_MIDI: {
                NSData *fileNSData = [NSData dataWithContentsOfFile:resourcePath];
                char *fileData = (char *)[fileNSData bytes];

                parseGameAsset(fileData, 0, ASSET_TYPE_MIDI, assetToLoad->key, assetToLoad->secondKey, 
                               &gameMemory, &workingAssetMemory, &options, 0);
            } break;
            case ASSET_TYPE_DATA: {
                NSString *fileString = [NSString stringWithContentsOfFile:resourcePath 
                                        encoding:NSUTF8StringEncoding error: NULL];
                unsigned int size = (unsigned int)[fileString lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
                char *fileData = (char *)[fileString UTF8String];
                parseGameAsset(fileData, 0, ASSET_TYPE_DATA, assetToLoad->key, assetToLoad->secondKey, 
                               &gameMemory, &workingAssetMemory, &options, size);
            } break;
            case ASSET_TYPE_ATLAS: {
                NSString *fileString = [NSString stringWithContentsOfFile:resourcePath 
                                        encoding:NSUTF8StringEncoding error: NULL];
                char *atlasData = (char *)[fileString UTF8String];

                NSString *bitmapResourcePath = [mainResourceBundle pathForResource:fileNameWithoutExt ofType:@"bmp"];
                NSData *fileNSData = [NSData dataWithContentsOfFile:bitmapResourcePath];
                char *bitmapData = (char *)[fileNSData bytes];

                parseGameAsset(atlasData, bitmapData, ASSET_TYPE_ATLAS, assetToLoad->key, assetToLoad->secondKey, 
                               &gameMemory, &workingAssetMemory, &options, 0);

                //loadRendererTexture(&rendererMemory, (loaded_texture_asset *)workingAssetMemory.base);
            } break;
        }
    }

    free(workingAssetMemory.base);

    // render command memory
    renderCommands = {};
    int memoryCapacity = 1 * 1024 * 1024;
    renderCommands.windowWidth = gameWidth;
    renderCommands.windowHeight = gameHeight;
    renderCommands.memory.base = malloc(memoryCapacity);
    renderCommands.memory.size = 0;
    renderCommands.memory.capacity = memoryCapacity;

    // init input
    input = {};

    // remember time
}

- (void)update {
    // get input
    input.pointerJustDown = false;
    // NOTE(ebuchholz): no keys or controller buttons can be pressed unless we support keyboard/controller on ios

    // TODO(ebuchholz): read touches

    // set up render commands
    renderCommands.windowWidth = gameWidth;
    renderCommands.windowHeight = gameHeight;
    renderCommands.memory.size = 0;
    memset(renderCommands.memory.base, 0, renderCommands.memory.capacity);
    memset(gameMemory.tempMemory, 0, gameMemory.tempMemoryCapacity);

    // reset platform triggers
    platform_triggers platformTriggers = {};

    // update game
    updateGame(&input, &gameMemory, &renderCommands, &options, &platformTriggers);

    // update audio

    // render frame

    // reset input

    // remember time
}

@end
