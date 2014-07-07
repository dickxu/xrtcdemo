//
//  RtcCenter.m
//  testWebRTC
//
//  Created by Yongze Xu on 6/22/14.
//  Copyright (c) 2014 sunrui. All rights reserved.
//

#import "RtcCenter.h"
#include <string>
#include <sstream>

@interface RtcCenter()
@end

@implementation RtcCenter
@synthesize xrtc = _xrtc;

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

- (void) OnSessionDescription:(const std::string &)sdp
{
    _xrtc->SetLocalDescription(sdp);
    g_xmpp->SendMessage("sdp", sdp);
}

- (void) OnIceCandidate:(const std::string &)candidate
{
    g_xmpp->SendMessage("ice", candidate);
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

- (void) OnXmppSessionDescription:(const std::string &)sdp
{
    _xrtc->SetRemoteDescription(sdp);
    _xrtc->AnswerCall();
    
}

- (void) OnXmppIceCandidate:(const std::string &)candidate
{
    _xrtc->AddIceCandidate(candidate);
}

@end
