#import "metal_renderer.h"

#include "../../../game/quickmake_platform.h"

struct metal_texture {
    int key;
    id<MTLTexture> texture;
};
#define MAX_METAL_TEXTURES 100

#define MAX_SPRITES_PER_BATCH 100000
#define MAX_SPRITE_BATCHES 100000

struct shader_sprite_uniforms {
    float screenWidth;
    float screenHeight;
};

@interface MetalRenderer () {
    int numTextures;
    metal_texture textures[MAX_METAL_TEXTURES];
    float *spriteVertexPositions;
    float *spriteTextureCoords;
    float *spriteColors;
    unsigned int *spriteIndices;
    unsigned int spriteFloatParamsOffset;
    unsigned int spriteColorOffset;
    unsigned int spriteIndexOffset;
    unsigned int spriteUniformOffset;
}
@end

@implementation MetalRenderer

- (void)initRendererWithLayer:(CALayer *)layer {
    self.device = MTLCreateSystemDefaultDevice();

    self.metalLayer = [CAMetalLayer new];
    self.metalLayer.device = _device;
    self.metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    self.metalLayer.framebufferOnly = YES;
    self.metalLayer.frame = layer.frame;
    [layer addSublayer:self.metalLayer];

    id<MTLLibrary> defaultLibrary = [self.device newDefaultLibrary];
    id<MTLFunction> spriteVertexProgram = [defaultLibrary newFunctionWithName:@"spriteVertexShader"];
    id<MTLFunction> spriteFragmentProgram = [defaultLibrary newFunctionWithName:@"spriteFragmentShader"];

    MTLRenderPipelineDescriptor *spritePipelineDescriptor = [MTLRenderPipelineDescriptor new];
    spritePipelineDescriptor.vertexFunction = spriteVertexProgram;
    spritePipelineDescriptor.fragmentFunction = spriteFragmentProgram;
    spritePipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    spritePipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;

    // blend mode setup for sprites with alpha
    spritePipelineDescriptor.colorAttachments[0].blendingEnabled = YES;

    spritePipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
    spritePipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    spritePipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
    spritePipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

    MTLDepthStencilDescriptor *depthStencilDescriptor = [MTLDepthStencilDescriptor new];
    depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionLess;
    depthStencilDescriptor.depthWriteEnabled = NO;
    self.depthStencilState = [self.device newDepthStencilStateWithDescriptor:depthStencilDescriptor];

    NSError *error;
    self.spritePipelineState = [self.device newRenderPipelineStateWithDescriptor:spritePipelineDescriptor error:&error];

    unsigned int numPositions = 8 * MAX_SPRITES_PER_BATCH;
    self.spritePositionBuffer = [self.device newBufferWithLength:(sizeof(float) * numPositions)
                                 options:MTLResourceOptionCPUCacheModeDefault];

    unsigned int numTextureCoords = 8 * MAX_SPRITES_PER_BATCH;
    self.spriteTextureBuffer = [self.device newBufferWithLength:(sizeof(float) * numTextureCoords)
                                 options:MTLResourceOptionCPUCacheModeDefault];

    unsigned int numColors = 16 * MAX_SPRITES_PER_BATCH;
    self.spriteColorBuffer = [self.device newBufferWithLength:(sizeof(float) * numColors)
                                 options:MTLResourceOptionCPUCacheModeDefault];

    unsigned int numIndices = 6 * MAX_SPRITES_PER_BATCH;
    self.spriteIndexBuffer = [self.device newBufferWithLength:(sizeof(unsigned int) * numIndices)
                                 options:MTLResourceOptionCPUCacheModeDefault];

    self->spriteVertexPositions = (float *)malloc(sizeof(float) * numPositions);
    self->spriteTextureCoords = (float *)malloc(sizeof(float) * numTextureCoords);
    self->spriteColors = (float *)malloc(sizeof(float) * numColors);
    self->spriteIndices = (unsigned int *)malloc(sizeof(unsigned int) * numIndices);

    unsigned int indexIndex = 0;
    for (unsigned int i = 0; i < MAX_SPRITES_PER_BATCH; ++i) {
        unsigned int vertexNum = i * 4;
        self->spriteIndices[indexIndex++] = vertexNum;
        self->spriteIndices[indexIndex++] = vertexNum+3;
        self->spriteIndices[indexIndex++] = vertexNum+1;
        self->spriteIndices[indexIndex++] = vertexNum;
        self->spriteIndices[indexIndex++] = vertexNum+2;
        self->spriteIndices[indexIndex++] = vertexNum+3;
    }
    memcpy(self.spriteIndexBuffer.contents, self->spriteIndices, sizeof(unsigned int) * numIndices);

    self.spriteUniformBuffer = [self.device newBufferWithLength:sizeof(shader_sprite_uniforms) * MAX_SPRITE_BATCHES
                                            options:MTLResourceOptionCPUCacheModeDefault];

    self.commandQueue = [self.device newCommandQueue];

    // basic sampler
    MTLSamplerDescriptor *samplerDesc = [MTLSamplerDescriptor new];
    samplerDesc.sAddressMode = MTLSamplerAddressModeClampToEdge;
    samplerDesc.tAddressMode = MTLSamplerAddressModeClampToEdge;
    samplerDesc.minFilter = MTLSamplerMinMagFilterNearest;
    samplerDesc.magFilter = MTLSamplerMinMagFilterNearest;
    self.samplerNearestClamp = [self.device newSamplerStateWithDescriptor:samplerDesc];
}

- (void)onResizedToBounds:(CGRect)bounds scale:(CGFloat)scale {
   self.metalLayer.frame = bounds;

   CGSize drawableSize = bounds.size;
   drawableSize.width *= scale;
   drawableSize.height *= scale;
   self.metalLayer.drawableSize = drawableSize;

   if ([self.depthBuffer width] != drawableSize.width ||
       [self.depthBuffer height] != drawableSize.height)
    {
        MTLTextureDescriptor *depthBufferDesc = 
            [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float
                                  width:drawableSize.width
                                  height:drawableSize.height
                                  mipmapped:NO];
        depthBufferDesc.usage = MTLTextureUsageRenderTarget;
        depthBufferDesc.storageMode = MTLStorageModePrivate;
        self.depthBuffer = [self.device newTextureWithDescriptor:depthBufferDesc];
    }
}

- (void)loadTexture:(loaded_texture_asset *)loadedTexture {
    assert(self->numTextures < MAX_METAL_TEXTURES);
    assert(self->numTextures == loadedTexture->key);
    //TODO(ebuchholz): tag textures in some better way
    metal_texture *metalTexture = &self->textures[self->numTextures];
    ++self->numTextures;
    metalTexture->key = loadedTexture->key;

    MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor 
                                               texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                               width:loadedTexture->width 
                                               height:loadedTexture->height 
                                               mipmapped:NO];
    id<MTLTexture> texture = [self.device newTextureWithDescriptor:textureDescriptor];
    metalTexture->texture = texture;

    MTLRegion region = MTLRegionMake2D(0, 0, loadedTexture->width, loadedTexture->height);
    [texture replaceRegion:region mipmapLevel:0 withBytes:loadedTexture->pixels
     bytesPerRow:(4 * loadedTexture->width)];
}


- (void)flushSprites:(int)numSpritesBatched 
        renderEncoder:(id<MTLRenderCommandEncoder>)renderEncoder 
        windowWidth:(float)windowWidth windowHeight:(float)windowHeight
        textureKey:(int)textureKey 
{
    memcpy((char *)self.spritePositionBuffer.contents + self->spriteFloatParamsOffset, self->spriteVertexPositions, 
           numSpritesBatched * 8 * sizeof(float));
    [renderEncoder setVertexBuffer:self.spritePositionBuffer offset:0 atIndex:0];

    memcpy((char *)self.spriteTextureBuffer.contents + self->spriteFloatParamsOffset, self->spriteTextureCoords,
           numSpritesBatched * 8 * sizeof(float));
    [renderEncoder setVertexBuffer:self.spriteTextureBuffer offset:0 atIndex:1];

    self->spriteFloatParamsOffset += numSpritesBatched * 8 * sizeof(float);

    memcpy((char *)self.spriteColorBuffer.contents + self->spriteColorOffset, self->spriteColors,
           numSpritesBatched * 16 * sizeof(float));
    [renderEncoder setVertexBuffer:self.spriteColorBuffer offset:0 atIndex:2];

    self->spriteColorOffset += numSpritesBatched * 16 * sizeof(float);

    shader_sprite_uniforms uniforms;
    uniforms.screenWidth = windowWidth;
    uniforms.screenHeight = windowHeight;
    memcpy((char *)self.spriteUniformBuffer.contents + self->spriteUniformOffset, &uniforms, 16);//sizeof(shader_sprite_uniforms));
    [renderEncoder setVertexBuffer:self.spriteUniformBuffer offset:self->spriteUniformOffset atIndex:3];
    [renderEncoder setFragmentBuffer:self.spriteUniformBuffer offset:self->spriteUniformOffset atIndex:0];

    self->spriteUniformOffset += sizeof(shader_sprite_uniforms);

    metal_texture *texture = &self->textures[textureKey];
    [renderEncoder setFragmentTexture:texture->texture atIndex:0];
    [renderEncoder setFragmentSamplerState:self.samplerNearestClamp atIndex:0];

    [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                   indexCount:numSpritesBatched * 6
                   indexType:MTLIndexTypeUInt32
                   indexBuffer:self.spriteIndexBuffer
                   indexBufferOffset:self->spriteIndexOffset];

    self->spriteIndexOffset += numSpritesBatched * 6 * sizeof(unsigned int);
}

- (void)drawSpriteList:(render_command_sprite_list *)spriteListCommand 
        commandBuffer:(id<MTLCommandBuffer>)commandBuffer 
        renderPassDescriptor:(MTLRenderPassDescriptor *)renderPassDescriptor 
        windowWidth:(float)windowWidth windowHeight:(float)windowHeight
{

    if (spriteListCommand->numSprites > 0) {
        MTLViewport viewport;
        viewport.originX = 0;
        viewport.originY = 0;
        viewport.width = windowWidth;
        viewport.height = windowHeight;

        id<MTLRenderCommandEncoder> renderEncoder = 
            [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        [renderEncoder setViewport:viewport];
        [renderEncoder setRenderPipelineState:self.spritePipelineState];
        [renderEncoder setDepthStencilState:self.depthStencilState];
        [renderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
        [renderEncoder setCullMode:MTLCullModeBack];

        render_sprite *sprites = spriteListCommand->sprites;
        int currentTextureKey = sprites[0].textureKey;
        int numSpritesBatched = 0;
        int numSpriteBatches = 0;

        int numSprites = spriteListCommand->numSprites < MAX_SPRITES_PER_BATCH ? spriteListCommand->numSprites : MAX_SPRITES_PER_BATCH;

        for (int i = 0; i < numSprites; ++i) {
            render_sprite *sprite = &sprites[i];

            if (sprite->textureKey != currentTextureKey) {
                [self flushSprites:numSpritesBatched 
                      renderEncoder:renderEncoder 
                      windowWidth:windowWidth windowHeight:windowHeight 
                      textureKey:currentTextureKey];
                numSpritesBatched = 0;
                currentTextureKey = sprite->textureKey;

                ++numSpriteBatches;
                if (numSpriteBatches >= MAX_SPRITE_BATCHES) {
                    break;
                }
            }

            int numFloats = numSpritesBatched * 8;
            int numColors = numSpritesBatched * 16;

            self->spriteVertexPositions[numFloats]   = sprite->pos[0].x;
            self->spriteVertexPositions[numFloats+1] = sprite->pos[0].y;
            self->spriteVertexPositions[numFloats+2] = sprite->pos[1].x;
            self->spriteVertexPositions[numFloats+3] = sprite->pos[1].y;
            self->spriteVertexPositions[numFloats+4] = sprite->pos[2].x;
            self->spriteVertexPositions[numFloats+5] = sprite->pos[2].y;
            self->spriteVertexPositions[numFloats+6] = sprite->pos[3].x;
            self->spriteVertexPositions[numFloats+7] = sprite->pos[3].y;

            self->spriteTextureCoords[numFloats]   = sprite->texCoord[0].x;
            self->spriteTextureCoords[numFloats+1] = sprite->texCoord[0].y;
            self->spriteTextureCoords[numFloats+2] = sprite->texCoord[1].x;
            self->spriteTextureCoords[numFloats+3] = sprite->texCoord[1].y;
            self->spriteTextureCoords[numFloats+4] = sprite->texCoord[2].x;
            self->spriteTextureCoords[numFloats+5] = sprite->texCoord[2].y;
            self->spriteTextureCoords[numFloats+6] = sprite->texCoord[3].x;
            self->spriteTextureCoords[numFloats+7] = sprite->texCoord[3].y;

            self->spriteColors[numColors]   =  sprite->color[0].r;
            self->spriteColors[numColors+1] =  sprite->color[0].g;
            self->spriteColors[numColors+2] =  sprite->color[0].b;
            self->spriteColors[numColors+3] =  sprite->color[0].a;
            self->spriteColors[numColors+4] =  sprite->color[1].r;
            self->spriteColors[numColors+5] =  sprite->color[1].g;
            self->spriteColors[numColors+6] =  sprite->color[1].b;
            self->spriteColors[numColors+7] =  sprite->color[1].a;
            self->spriteColors[numColors+8] =  sprite->color[2].r;
            self->spriteColors[numColors+9] =  sprite->color[2].g;
            self->spriteColors[numColors+10] = sprite->color[2].b;
            self->spriteColors[numColors+11] = sprite->color[2].a;
            self->spriteColors[numColors+12] = sprite->color[3].r;
            self->spriteColors[numColors+13] = sprite->color[3].g;
            self->spriteColors[numColors+14] = sprite->color[3].b;
            self->spriteColors[numColors+15] = sprite->color[3].a;

            ++numSpritesBatched;
        }
        if (numSpritesBatched > 0) {
            [self flushSprites:numSpritesBatched 
                  renderEncoder:renderEncoder 
                  windowWidth:windowWidth windowHeight:windowHeight 
                  textureKey:currentTextureKey];
        }

        [renderEncoder endEncoding];
    }
}

- (void)renderFrameWithRenderCommandList:(render_command_list *)renderCommands {
    id<CAMetalDrawable> drawable = [self.metalLayer nextDrawable];
    if (drawable == nil) {
        return;
    }

    MTLRenderPassDescriptor *renderPassDescriptor = [MTLRenderPassDescriptor new];
    renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
    renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.7, 0.8, 1.0);
    renderPassDescriptor.depthAttachment.texture = self.depthBuffer;
    renderPassDescriptor.depthAttachment.clearDepth = 1.0;
    renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
    renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionDontCare;

    id<MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];

    // reset sprite buffer state
    self->spriteFloatParamsOffset = 0;
    self->spriteColorOffset = 0;
    self->spriteIndexOffset = 0;
    self->spriteUniformOffset = 0;

    unsigned int renderCommandOffset = 0;
    while (renderCommandOffset < renderCommands->memory.size) {
        render_command_header *header = 
            (render_command_header *)((char *)renderCommands->memory.base + 
                                     renderCommandOffset);
        renderCommandOffset += sizeof(render_command_header);
        switch(header->type) {
            default:
                // error
                assert(false);
                break;
            case RENDER_COMMAND_SPRITE: 
            {
                // TODO(ebuchholz): have a way to not have to set the program for command
                //    GLuint program = renderer->shaders[SHADER_TYPE_SPRITE].program;
                //    glUseProgram(program);

                //    render_command_sprite *spriteCommand = 
                //        (render_command_sprite *)((char *)renderCommands->memory.base + 
                //                                renderCommandOffset);
                //    drawSprite(renderer, program, spriteCommand, (float)renderCommands->windowWidth, (float)renderCommands->windowHeight);
                renderCommandOffset += sizeof(render_command_sprite);
            } break;
            case RENDER_COMMAND_SPRITE_LIST: 
            {
                render_command_sprite_list *spriteListCommand = 
                    (render_command_sprite_list *)((char *)renderCommands->memory.base + renderCommandOffset);
                [self drawSpriteList:spriteListCommand commandBuffer:commandBuffer 
                      renderPassDescriptor:renderPassDescriptor 
                      windowWidth:renderCommands->windowWidth 
                      windowHeight:renderCommands->windowHeight];
                renderCommandOffset += sizeof(render_command_sprite_list);
                renderCommandOffset += spriteListCommand->numSprites * sizeof(render_sprite);
            } break;
            case RENDER_COMMAND_MODEL: 
            {
                render_command_model *modelCommand = 
                    (render_command_model *)((char *)renderCommands->memory.base + 
                                            renderCommandOffset);
                //drawModel(renderer, program, modelCommand);
                renderCommandOffset += sizeof(render_command_model);
            } break;
            case RENDER_COMMAND_ANIMATED_MODEL: 
            {
                render_command_animated_model *animatedModelCommand = 
                    (render_command_animated_model *)((char *)renderCommands->memory.base + 
                                                      renderCommandOffset);
                //drawAnimatedModel(renderer, program, animatedModelCommand);
                renderCommandOffset += sizeof(render_command_animated_model);
                renderCommandOffset += sizeof(matrix4x4) * animatedModelCommand->numBones; // also account for the bone Data
            } break;
            case RENDER_COMMAND_LINES: 
            {
                render_command_lines *lineCommand = 
                    (render_command_lines *)((char *)renderCommands->memory.base + 
                                            renderCommandOffset);
                //drawLines(renderer, program, lineCommand);
                renderCommandOffset += sizeof(render_command_lines);
                renderCommandOffset += sizeof(line) * lineCommand->numLines; // also account for the line data
            } break;
            case RENDER_COMMAND_SET_CAMERA: 
            {
                render_command_set_camera *setCameraCommand = 
                    (render_command_set_camera *)((char *)renderCommands->memory.base + 
                                                  renderCommandOffset);
                //setCamera(renderer, setCameraCommand);
                renderCommandOffset += sizeof(render_command_set_camera);
            } break;
            case RENDER_COMMAND_BACKGROUND_VISUALIZATION:
            {
                render_command_background_visualization *visualizationCommand = 
                    (render_command_background_visualization *)((char *)renderCommands->memory.base + 
                                                                renderCommandOffset);
                //renderBackgroundVisualization(renderer, (float)renderCommands->windowWidth, (float)renderCommands->windowHeight, visualizationCommand);
                renderCommandOffset += sizeof(render_command_background_visualization);
            } break;
        }
    }

    [commandBuffer presentDrawable:drawable];
    [commandBuffer commit];
}

@end
