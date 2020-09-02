//
//  ViewController.m
//  quickmake
//
//  Created by Erik Buchholz on 7/18/20.
//  Copyright © 2020 Erik Buchholz. All rights reserved.
//

#import "ViewController.h"
#import "BasicGestureRecognizer.h":

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    //self.view.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    self.iosPlatform = [IOSPlatform new];
    [self.iosPlatform setupWithLayer:self.view.layer];
    
    BasicGestureRecognizer *basicGestureRecognizer = [[BasicGestureRecognizer alloc] initWithTarget:self.iosPlatform];
    [self.view addGestureRecognizer:basicGestureRecognizer];
}

- (void)viewDidAppear:(BOOL)animated {
}

- (void)viewDidLayoutSubviews {
    CGFloat scale = [UIScreen mainScreen].scale;
    
    if (self.view.window) {
        scale = self.view.window.screen.scale;
    }
    
    [self.iosPlatform onResizedToBounds:self.view.bounds scale:scale];
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

@end
