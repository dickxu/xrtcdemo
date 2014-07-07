//
//  RTCCenter.h
//  testWebRTC
//
//  Created by Yongze Xu on 6/10/14.
//  Copyright (c) 2014 sunrui. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "xrtc_api.h"
#import "XmppCenter.h"

@interface RtcCenter : NSObject<IRtcSink, IXmppSink> {
}
@property (nonatomic) IRtcCenter *xrtc;

- (bool) Init;
- (bool) SetLocalStream;
@end
