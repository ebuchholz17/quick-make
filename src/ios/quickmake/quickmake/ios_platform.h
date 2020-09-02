#import <UIKit/UIKit.h>
#import "metal_renderer.h"

@interface IOSPlatform : NSObject

- (void)setupWithLayer:(CALayer *)layer;
- (void)onResizedToBounds:(CGRect)bounds scale:(CGFloat)scale;

@property (strong, nonatomic) CADisplayLink *timer;
@property (strong, nonatomic) MetalRenderer *metalRenderer;

@end

