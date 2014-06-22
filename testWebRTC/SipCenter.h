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
#include "pjsua.h"

class CSipCenter {
public:
    CSipCenter();
    ~CSipCenter();
    
    int Init();
    int Uninit();
    
    int SetAccount(std::string user, std::string password, std::string realm);
    int AddTransport();
    int Start();
    int Register();
    int Call(std::string szuri);
    
protected:
    int Looping();
    
public:
    void OnIncomingCall(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata);
    void OnCallState(pjsua_call_id call_id, pjsip_event *e);
    void OnCallMediaState(pjsua_call_id call_id);
    
private:
    pjsua_acc_id m_acc_id;
    std::string m_user;
    std::string m_passwd;
    std::string m_realm;
    std::string m_scheme;
    
    pj_uint16_t m_port;
    std::string m_sdp;
};

extern CSipCenter *g_sip;

#endif /* defined(__testWebRTC__SipCenter__) */
