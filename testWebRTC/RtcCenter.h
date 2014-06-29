//
//  RTCCenter.h
//  testWebRTC
//
//  Created by Yongze Xu on 6/10/14.
//  Copyright (c) 2014 sunrui. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "xrtc_api.h"

@interface RtcCenter : NSObject<IRtcSink> {
}
@property (nonatomic) IRtcCenter *xrtc;
@property (nonatomic, readwrite) std::string touser;

- (bool) Init;
- (bool) SetLocalStream;
@end
