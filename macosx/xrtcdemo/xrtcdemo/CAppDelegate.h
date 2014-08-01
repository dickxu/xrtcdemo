//
//  CAppDelegate.h
//  xrtcdemo
//
//  Created by Yongze Xu on 8/1/14.
//  Copyright (c) 2014 uskee. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface CAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
    
    IBOutlet NSTextField *loginField;
    IBOutlet NSTextField *callField;
}

@property (assign) IBOutlet NSWindow *window;

@end
