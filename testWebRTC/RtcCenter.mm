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

@interface RtcSink()
@end

@implementation RtcSink


- (void) OnSessionDescription:(const std::string &)sdp
{
    XmppTask task("localsdp", sdp);
    g_xmpp->PushTask(task);
    g_xmpp->SendMessage("remotesdp", sdp);
}

- (void) OnIceCandidate:(const std::string &)candidate
{
    g_xmpp->SendMessage("candidate", candidate);
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

- (void) OnIceConnectionState:(int)state
{
    if (state == kIceDisconnected || state == kIceFailed)
    {
        g_xmpp->PushTask("closecall", "");
    }
}

- (void) OnFailureMesssage:(std::string)str
{
    g_xmpp->PushTask("closecall", "");
}


@end
