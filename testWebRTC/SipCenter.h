//
//  SipCenter.h
//  testWebRTC
//
//  Created by Yongze Xu on 6/10/14.
//  Copyright (c) 2014 sunrui. All rights reserved.
//

#ifndef __testWebRTC__SipCenter__
#define __testWebRTC__SipCenter__

#include <iostream>

#include "eXosip2/eXosip.h"

class CSipCenter {
public:
    CSipCenter();
    ~CSipCenter();
    
public:
    bool Init();

private:
    struct eXosip_t *m_osip;
    unsigned short m_port;
};

extern CSipCenter *g_sip;

#endif /* defined(__testWebRTC__SipCenter__) */
