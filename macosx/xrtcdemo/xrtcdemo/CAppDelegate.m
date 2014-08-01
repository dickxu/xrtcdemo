//
//  CAppDelegate.m
//  xrtcdemo
//
//  Created by Yongze Xu on 8/1/14.
//  Copyright (c) 2014 uskee. All rights reserved.
//

#import "CAppDelegate.h"

@implementation CAppDelegate

@synthesize window = window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

- (IBAction)btnLogin:(id)sender
{
    NSString *loginUri = [loginField stringValue];
    loginUri = NULL;
}

- (IBAction)btnCall:(id)sender
{
    NSString *callUri = [callField stringValue];
    callUri = NULL;
}

@end
