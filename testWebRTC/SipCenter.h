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
#include "xrtc_api.h"

enum {
    S_Unknown   = 0x0000,
    S_Stop      = 0x0001,
    S_Quit      = 0x0002,
    S_Middle    = 0x0004,
    S_Init      = 0x0008,
    S_Start     = 0x0010,
    S_Register  = 0x0020,
    S_Invite    = 0x0040,
    S_Call      = 0x0080,
};

class CSipCenter {
public:
    CSipCenter();
    ~CSipCenter();
    
public:
    bool Init();
    bool Start();
    bool Stop();
    
    int GetStatus() { return m_status; }
    void SetRTC(IRtcCenter *ptr) { m_rtc = ptr;}
    void SetToUser(std::string to) { m_to = to; }
    
    bool SendInvite(std::string subject, std::string sdp);
    bool SendAnswer(std::string sdp);
    bool SendInfo(std::string info);
    bool SendMessage(std::string msg);
    
    bool SendRegister();
    bool UpdateRegister(int expires);
    bool Teriminate();
    
protected:
    static void * ThreadStart(void * param);
    void Run();

private:
    struct eXosip_t *m_ctx;
    int m_rid; // registration identifier
    int m_cid; // call identifier, for INVITE and CANCEL
    int m_did; // dialog identifier
    int m_tid; // transaction identifier
    
    unsigned short m_port;
    unsigned int m_expires;
    
    std::string m_uname;
    std::string m_passwd;
    std::string m_domain;
    std::string m_from;
    std::string m_proxy;
    std::string m_to;
    
    int m_status;
    bool m_quit;
    pthread_t m_pid;
    IRtcCenter *m_rtc;
};

extern CSipCenter *g_sip;

#endif /* defined(__testWebRTC__SipCenter__) */
