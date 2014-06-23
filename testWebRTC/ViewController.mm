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
    long lret = 0;
    _rtc_center = [[RtcCenter alloc]init];
    if([_rtc_center Init]) {
        lret = [_rtc_center xrtc]->GetUserMedia(true, false);
        lret = [_rtc_center xrtc]->CreatePeerConnection();
        lret = [_rtc_center xrtc]->AddLocalStream();
    }
    
    // for sip init
    if (g_sip != NULL) {

    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)hangUp:(id)sender
{
#warning "Hangup"
    printf("hangUp...\n");
}

- (IBAction)answerCall:(id)sender
{
#warning "Answer"
    printf("answerCall..\n");
}

- (IBAction)call:(id)sender
{
#warning "Call"
    //NSLog(@"%@",self.input.text);
    NSLog(@"%s",self.input.text.UTF8String);
    printf("call ...\n");
    [self.input resignFirstResponder]; //收起键盘
    
    [_rtc_center xrtc]->SetupCall();
}

@end
