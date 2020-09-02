#import <UIKit/UIKit.h>
#import <Metal/Metal.h>

@interface MetalRenderer : NSObject {
    float vertexData[9];
};

struct render_command_list;
struct loaded_texture_asset;

- (void)initRendererWithLayer:(CALayer *)layer;
- (void)onResizedToBounds:(CGRect)bounds scale:(CGFloat)scale;
- (void)loadTexture:(struct loaded_texture_asset *)loadedTexture;
- (void)renderFrameWithRenderCommandList:(struct render_command_list *)renderCommands;

@property (strong, nonatomic) id<MTLDevice> device;
@property (strong, nonatomic) CAMetalLayer *metalLayer;
@property (strong, nonatomic) id<MTLTexture> depthBuffer;
@property (strong, nonatomic) id<MTLRenderPipelineState> spritePipelineState;
@property (strong, nonatomic) id<MTLDepthStencilState> depthStencilState;
@property (strong, nonatomic) id<MTLCommandQueue> commandQueue;
@property (strong, nonatomic) CADisplayLink *timer;

@property (strong, nonatomic) id<MTLSamplerState> samplerNearestClamp;

@property (strong, nonatomic) id<MTLBuffer> spritePositionBuffer;
@property (strong, nonatomic) id<MTLBuffer> spriteTextureBuffer;
@property (strong, nonatomic) id<MTLBuffer> spriteColorBuffer;
@property (strong, nonatomic) id<MTLBuffer> spriteIndexBuffer;
@property (strong, nonatomic) id<MTLBuffer> spriteUniformBuffer;

@end
