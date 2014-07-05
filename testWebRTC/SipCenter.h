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
    bool Start();
    bool Stop();
    bool IsRegister() { return m_register; }
    
    bool SendInvite(std::string to, std::string subject, std::string sdp);
    bool SendAnswer(eXosip_event_t *evt, std::string sdp);
    bool SendInfo(eXosip_event_t *evt, std::string message);
    
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
    
    //std::string m_sdp;
    bool m_register;
    
    bool m_started;
    bool m_quit;
    pthread_t m_pid;
};

extern CSipCenter *g_sip;

#endif /* defined(__testWebRTC__SipCenter__) */
