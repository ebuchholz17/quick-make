#import <UIKit/UIKit.h>

@interface IOSPlatform : NSObject

- (void)setup;

@property (strong, nonatomic) CADisplayLink *timer;

@end

