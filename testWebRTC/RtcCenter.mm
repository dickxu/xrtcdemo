//
//  RtcCenter.m
//  testWebRTC
//
//  Created by Yongze Xu on 6/22/14.
//  Copyright (c) 2014 sunrui. All rights reserved.
//

#import "RtcCenter.h"
#include "SipCenter.h"
#include <string>


void ShowMessage(std::string msg)
{
    NSString *message = [NSString stringWithCString:msg.c_str()
                                           encoding:[NSString defaultCStringEncoding]];
    UIAlertView *toast = [[UIAlertView alloc] initWithTitle:nil
                                                    message:message
                                                   delegate:nil
                                          cancelButtonTitle:nil
                                          otherButtonTitles:nil, nil];
    [toast show];
    
    int duration = 1; // duration in seconds
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, duration * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        [toast dismissWithClickedButtonIndex:0 animated:YES];
    });
}

@interface RtcCenter()
@end

@implementation RtcCenter
@synthesize xrtc = _xrtc;
@synthesize touser = _touser;

- (bool) Init
{
    bool bret = xrtc_init();
    if (!bret)
        return false;
    
    bret = xrtc_create(_xrtc);
    if (bret) {
        _xrtc->SetSink(self);
    }
    
    return bret;
}

- (bool) SetLocalStream
{
    long lret = _xrtc->GetUserMedia(true, false);
    lret = _xrtc->CreatePeerConnection();
    lret = _xrtc->AddLocalStream();
    return true;
}

- (void) OnSessionDescription:(const std::string &)type sdp:(const std::string &)str
{
    if (g_sip->IsRegister() && !_touser.empty()) {
        g_sip->SendInvite(_touser, type, str);
    }
}

- (void) OnIceCandidate:(const std::string &)candidate sdpMid:(const std::string &)mid sdpMLineIndex:(int)index
{
    if (g_sip->IsRegister() && !_touser.empty()) {
        g_sip->SendInvite(_touser, candidate, mid);
    }
}

- (void) OnRemoteStream:(int)action
{
    action = 1;
}

- (void) OnGetUserMedia:(int)error errstr:(std::string)str
{
    if (error == 0)
    {
        error = 0;
    }
}

- (void) OnFailureMesssage:(std::string)str
{
    str = "";
}

@end
