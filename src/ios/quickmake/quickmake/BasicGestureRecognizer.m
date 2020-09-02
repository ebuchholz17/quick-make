#import <UIKit/UIGestureRecognizerSubclass.h>
#import "BasicGestureRecognizer.h"

// see: https://stackoverflow.com/a/20866354
@interface BasicGestureRecognizer ()

@property (nonatomic, assign) id target;

@end

@implementation BasicGestureRecognizer : UIGestureRecognizer
 
- (id)initWithTarget:(id)target {
    if (self =  [super initWithTarget:target  action:Nil]) {
        self.target = target;
    }
    return self;
}

- (void)reset {
    [super reset];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesBegan:touches withEvent:event];

    [self.target touchesBegan:touches withEvent:event];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesMoved:touches withEvent:event];

    [self.target touchesMoved:touches withEvent:event];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesEnded:touches withEvent: event];

    [self.target touchesEnded:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesCancelled:touches withEvent: event];

    [self.target touchesCancelled:touches withEvent:event];
}

@end
