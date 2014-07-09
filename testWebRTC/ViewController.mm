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

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    g_xmpp->Init();
    g_xmpp->PushTask("initlocalstream", "");
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (IBAction)login:(id)sender
{
    printf("login...\n");
    [self.input_from resignFirstResponder]; // recv keyboard
    std::string fromuser = self.input_from.text.UTF8String;
    g_xmpp->SetAccount(fromuser, fromuser);
    g_xmpp->Start();
}

- (IBAction)call:(id)sender
{
    printf("call ...\n");
    [self.input_to resignFirstResponder]; // recv keyboard
    std::string touser = self.input_to.text.UTF8String;
    
    usleep(2000*1000);
    g_xmpp->SetRemote(touser);
    g_xmpp->PushTask("setupcall", "");
}

@end
