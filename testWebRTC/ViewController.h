//
//  ViewController.h
//  testWebRTC
//
//  Created by sunrui on 14-6-7.
//  Copyright (c) 2014年 sunrui. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RtcCenter.h"

@interface ViewController : UIViewController
@property (weak, nonatomic) IBOutlet UITextField *input;
@property (retain) RtcCenter *rtc_center;
@end
