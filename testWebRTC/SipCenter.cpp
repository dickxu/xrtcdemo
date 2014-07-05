//
//  SipCenter.cpp
//  testWebRTC
//
//  Created by Yongze Xu on 6/10/14.
//  Copyright (c) 2014 sunrui. All rights reserved.
//

#include "SipCenter.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

// "<sip:to@antisip.com>"
#define DEFAULT_SIP_DOMAIN "sip.uskee.org"
#define DEFAULT_SIP_USER "2011"
#define DEFAULT_SIP_PASSWD "2011"
#define DEFAULT_SIP_SCHEME "digest"
#define DEFAULT_SIP_EXPIRES 3600
#define DEFAULT_SIP_PORT 5062       /* Listening SIP port */

#define safe_quit(ctx)          if (ctx) {eXosip_quit(ctx); ctx=NULL;}
#define assert_return(p, v)     if (!(p)) {return (v);}


extern void ShowMessage(std::string msg);

CSipCenter *g_sip = new CSipCenter();


CSipCenter::CSipCenter()
{
    m_ctx = NULL;
    m_rid = -1;
    m_cid = -1;
    
    m_port = DEFAULT_SIP_PORT;
    m_expires = DEFAULT_SIP_EXPIRES;
    
    m_uname = DEFAULT_SIP_USER;
    m_passwd = DEFAULT_SIP_PASSWD;
    m_domain = DEFAULT_SIP_DOMAIN;
    m_proxy = "sip:" + m_domain;
    m_from = "sip:" + m_uname + "@" + m_domain;
    
    //m_sdp = "";
    m_register = false;
    m_started = false;
}

CSipCenter::~CSipCenter()
{
    safe_quit(m_ctx);
}

bool CSipCenter::Init()
{
    TRACE_INITIALIZE (TRACE_LEVEL7, (FILE *)stdout);
    
    m_ctx = eXosip_malloc();
    assert_return(m_ctx != NULL, false);
    
    int iret = eXosip_init(m_ctx);
    assert_return(iret == OSIP_SUCCESS, false);
    
    iret = eXosip_listen_addr (m_ctx, IPPROTO_UDP, NULL, m_port, AF_INET, 0);
    if (iret != 0) {
        safe_quit(m_ctx);
        fprintf (stderr, "could not initialize transport layer\n");
        return false;
    }
    
    char localip[32];
    eXosip_masquerade_contact (m_ctx, localip, m_port);
    
#if 0
    int val=17000;
    eXosip_set_option (m_ctx, EXOSIP_OPT_UDP_KEEP_ALIVE, (void*)&val);
    val=2;
    eXosip_set_option (m_ctx, EXOSIP_OPT_DNS_CAPABILITIES, (void*)&val);
    val=1;
    eXosip_set_option (m_ctx, EXOSIP_OPT_USE_RPORT, (void*)&val);
    val=26;
    eXosip_set_option (m_ctx, EXOSIP_OPT_SET_DSCP, (void*)&val);
    eXosip_set_option (m_ctx, EXOSIP_OPT_SET_IPV4_FOR_GATEWAY, DEFAULT_SIP_DOMAIN);
    eXosip_masquerade_contact (m_ctx, "91.121.81.212", 10456);
#endif
    
    eXosip_set_user_agent(m_ctx, (const char *)"xrtcsip/1.0.0");
    iret = eXosip_add_authentication_info (m_ctx, m_uname.c_str(), m_uname.c_str(), m_passwd.c_str(), NULL, NULL);
    assert_return(iret == OSIP_SUCCESS, false);
    
    return true;
}

void * CSipCenter::ThreadStart(void * param)
{
    CSipCenter * ptr = (CSipCenter *)param;
    if (ptr) {
        ptr->Run();
    }
    return NULL;
}

bool CSipCenter::Start()
{
    int iret = 0;
    if (!m_started)
    {
        m_quit = false;
        iret = pthread_create(&m_pid, NULL, ThreadStart, (void *)this);
        m_started = true;
    }
    return (iret == 0);
}

bool CSipCenter::Stop()
{
    m_quit = true;
    pthread_join(m_pid, NULL);
    m_started = false;
    return true;
}

bool CSipCenter::SendInvite(std::string to, std::string subject, std::string sdp)
{
    osip_message_t *invite = NULL;
    int iret = eXosip_call_build_initial_invite (m_ctx, &invite, to.c_str(), m_from.c_str(), "", subject.c_str());
    assert_return(iret == OSIP_SUCCESS, false);
    //osip_message_set_supported (invite, "100rel");
    
    //char localip[128];
    //eXosip_guess_localip (m_osip, AF_INET, localip, 128);
    //iret = osip_message_set_body (invite, sdp.c_str(), sdp.size());
    assert_return(iret == OSIP_SUCCESS, false);
    
    iret = osip_message_set_content_type (invite, "application/sdp");
    assert_return(iret == OSIP_SUCCESS, false);
    
    eXosip_lock (m_ctx);
    m_cid = eXosip_call_send_initial_invite (m_ctx, invite);
    if (m_cid > 0)
    {
        eXosip_call_set_reference (m_ctx, m_cid, (void *)this);
    }
    eXosip_unlock (m_ctx);
    
    return true;
}

bool CSipCenter::SendAnswer(eXosip_event_t *evt, std::string sdp)
{
    osip_message_t *answer = NULL;
    int tid = evt->tid;
    int did = evt->did;
    
    eXosip_lock (m_ctx);
    int iret = eXosip_call_build_answer (m_ctx, tid, 200, &answer);
    if (iret != 0)
    {
        eXosip_call_send_answer (m_ctx, tid, 180, NULL);
    }
    else
    {
        //iret = sdp_complete_200ok (did, answer);
        osip_message_set_body (answer, sdp.c_str(), sdp.size());
        osip_message_set_content_type (answer, "application/sdp");
        eXosip_call_send_answer (m_ctx, tid, 200, answer);
    }
    eXosip_unlock (m_ctx);
    
    return true;
}

bool CSipCenter::SendInfo(eXosip_event_t *evt, std::string message)
{
    int iret = 0;
    int did = evt->did;
    osip_message_t *info;
    
    eXosip_lock (m_ctx);
    iret = eXosip_call_build_info (m_ctx, did, &info);
    if (iret == 0)
    {
        osip_message_set_content_type (info, "application/text");
        osip_message_set_body (info, message.c_str(), message.size());
        iret = eXosip_call_send_request (m_ctx, did, info);
    }
    eXosip_unlock (m_ctx);
    
    return true;
}

bool CSipCenter::SendRegister()
{
    assert_return(!m_register, true);
    
    if (m_rid >= 1 && m_register)
    {
        return UpdateRegister(m_expires);
    }
 
    int iret = 0;
    osip_message_t *reg = NULL;
    std::string contact = "";
    
    eXosip_lock (m_ctx);
    m_rid = eXosip_register_build_initial_register (m_ctx, m_from.c_str(), m_proxy.c_str(),
                                                    contact.c_str(), m_expires * 2, &reg);
    if (m_rid < 1) {
        eXosip_unlock (m_ctx);
        return false;
    }
    
    osip_message_set_supported (reg, "100rel");
    osip_message_set_supported(reg, "path");
    
    iret = eXosip_register_send_register (m_ctx, m_rid, reg);
    eXosip_unlock (m_ctx);
    assert_return(iret == OSIP_SUCCESS, false);
    
    return true;
}

bool CSipCenter::UpdateRegister(int expires)
{
    if (m_rid < 1)
    {
        return false;
    }
 
    int iret = 0;
    osip_message_t *reg = NULL;
    
    eXosip_lock (m_ctx);
    iret = eXosip_register_build_register (m_ctx, m_rid, expires, &reg);
    if (iret == OSIP_SUCCESS)
    {
        iret = eXosip_register_send_register (m_ctx, m_rid, reg);
    }
    eXosip_unlock (m_ctx);
    assert_return(iret == OSIP_SUCCESS, false);
    
    return true;
}

bool CSipCenter::Teriminate()
{
    eXosip_lock (m_ctx);
    int iret = eXosip_call_terminate (m_ctx, m_cid, m_did);
    eXosip_unlock (m_ctx);
    assert_return(iret == OSIP_SUCCESS, false);
    
    return true;
}

void CSipCenter::Run()
{
    osip_usleep(100*1000);
    
    int iret = 0;
    for (; !m_quit;)
    {
        eXosip_event_t *event;
        if (!(event = eXosip_event_wait (m_ctx, 0, 1))) {
            eXosip_execute (m_ctx);
            eXosip_automatic_action (m_ctx);
            osip_usleep (10000);
            continue;
        }
        
        eXosip_execute (m_ctx);
        eXosip_automatic_action (m_ctx);
        
        switch (event->type) {
            case EXOSIP_REGISTRATION_SUCCESS:
                m_register = true;
                break;
            case EXOSIP_REGISTRATION_FAILURE:
                break;
            case EXOSIP_CALL_INVITE:
            case EXOSIP_CALL_REINVITE:
                break;
            case EXOSIP_CALL_NOANSWER:
                break;
            case EXOSIP_CALL_RINGING:
            case EXOSIP_CALL_ANSWERED:
            case EXOSIP_CALL_REQUESTFAILURE:
                eXosip_default_action(m_ctx, event);
                break;
            case EXOSIP_CALL_SERVERFAILURE:
            case EXOSIP_CALL_GLOBALFAILURE:
            case EXOSIP_CALL_ACK:
                break;
            case EXOSIP_CALL_CANCELLED:
            case EXOSIP_CALL_CLOSED:
            case EXOSIP_CALL_RELEASED:
            default:
                printf("[osip] recv message=%d\n", event->type);
                break;
        }
        eXosip_event_free(event);
    }
    iret = 1;
}


