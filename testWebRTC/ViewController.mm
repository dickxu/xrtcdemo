//
//  ViewController.m
//  testWebRTC
//
//  Created by sunrui on 14-6-7.
//  Copyright (c) 2014年 sunrui. All rights reserved.
//

#import "ViewController.h"
#include "SipCenter.h"


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
    
    // for sip init
    if (g_sip != NULL) {
        g_sip->Init();
        g_sip->SetRTC([_rtc_center xrtc]);
        g_sip->Start();
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
    g_sip->SendRegister();
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
    g_sip->SetToUser(touser);
    g_sip->SendInvite("Calling User", "");
    [_rtc_center xrtc]->SetupCall();
}

@end
