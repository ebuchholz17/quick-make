//
//  ViewController.m
//  quickmake
//
//  Created by Erik Buchholz on 7/18/20.
//  Copyright © 2020 Erik Buchholz. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.

    self.iosPlatform = [IOSPlatform new];
    [self.iosPlatform setup];

}

@end
