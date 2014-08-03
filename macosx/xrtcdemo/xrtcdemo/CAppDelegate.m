//
//  CAppDelegate.m
//  xrtcdemo
//
//  Created by Yongze Xu on 8/1/14.
//  Copyright (c) 2014 uskee. All rights reserved.
//

#import "CAppDelegate.h"

#include "XmppCenter.h"

@implementation CAppDelegate

@synthesize window = window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    
    g_xmpp->Init();
    g_xmpp->PushTask("initlocalstream", "");
}

- (IBAction)btnLogin:(id)sender
{
    std::string fromuser;
    NSString *tmp = [loginField stringValue];
    fromuser = [tmp cStringUsingEncoding:NSUTF8StringEncoding];
    
    g_xmpp->SetAccount(fromuser, fromuser);
    g_xmpp->Start();
}

- (IBAction)btnCall:(id)sender
{
    std::string touser;
    NSString *tmp = [callField stringValue];
    touser = [tmp cStringUsingEncoding:NSUTF8StringEncoding];
    
    usleep(2000*1000);
    g_xmpp->SetRemote(touser);
    g_xmpp->PushTask("setupcall", "");
}

@end
