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
    g_sip->SendMessage(str);
}

- (void) OnIceCandidate:(const std::string &)candidate sdpMid:(const std::string &)mid sdpMLineIndex:(int)index
{
    g_sip->SendMessage(candidate);
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
