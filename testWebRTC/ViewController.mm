//
//  ViewController.m
//  testWebRTC
//
//  Created by sunrui on 14-6-7.
//  Copyright (c) 2014年 sunrui. All rights reserved.
//

#import "ViewController.h"
#include "XmppCenter.h"

@interface ViewController ()

@end

@implementation ViewController
@synthesize rtc_center = _rtc_center;

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    // For rtc init
    _rtc_center = [[RtcCenter alloc]init];
    if([_rtc_center Init]) {
        [_rtc_center SetLocalStream];
    }
    
    if (!g_xmpp) {
        g_xmpp = new XmppCenter();
        g_xmpp->Init();
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (IBAction)registerSip:(id)sender
{
    printf("registerSip...\n");
}

- (IBAction)hangUp:(id)sender
{
    printf("hangUp...\n");
}

- (IBAction)answerCall:(id)sender
{
    printf("answerCall..\n");
}

- (IBAction)call:(id)sender
{
    //NSLog(@"%@",self.input.text);
    NSLog(@"%s",self.input.text.UTF8String);
    printf("call ...\n");
    [self.input resignFirstResponder]; //收起键盘

    std::string touser = "sip:2013@sip.uskee.org";
    [_rtc_center xrtc]->SetupCall();
}

@end
