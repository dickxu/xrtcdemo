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
#define DEFAULT_SIP_USER "2015"
#define DEFAULT_SIP_PASSWD "2015"
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
    m_status = S_Unknown;
    
    m_rtc = NULL;
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
    
    iret = eXosip_listen_addr (m_ctx, IPPROTO_TCP, NULL, m_port, AF_INET, 0);
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
    
    m_status = S_Init;
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
    if (m_status & S_Start)
        return true;
    
    m_quit = false;
    int iret = pthread_create(&m_pid, NULL, ThreadStart, (void *)this);
    return (iret == 0);
}

bool CSipCenter::Stop()
{
    m_quit = true;
    pthread_join(m_pid, NULL);
    m_status = (m_status | S_Stop);
    return true;
}

bool CSipCenter::SendInvite(std::string subject, std::string sdp)
{
    if (!(m_status & S_Register) || m_to.empty())
    {
        return false;
    }
    
    osip_message_t *invite = NULL;
    int iret = eXosip_call_build_initial_invite (m_ctx, &invite, m_to.c_str(), m_from.c_str(), NULL, subject.c_str());
    assert_return(iret == OSIP_SUCCESS, false);
    //osip_message_set_supported (invite, "100rel");
    
    if (!sdp.empty())
    {
        iret = osip_message_set_body (invite, sdp.c_str(), sdp.size());
        assert_return(iret == OSIP_SUCCESS, false);
        iret = osip_message_set_content_type (invite, "application/sdp");
        assert_return(iret == OSIP_SUCCESS, false);
    }
    
    eXosip_lock (m_ctx);
    m_cid = eXosip_call_send_initial_invite (m_ctx, invite);
    if (m_cid > 0)
    {
        eXosip_call_set_reference (m_ctx, m_cid, (void *)this);
    }
    eXosip_unlock (m_ctx);
    
    return true;
}

bool CSipCenter::SendAnswer(std::string sdp)
{
    assert_return(m_tid > 0, false);
    
    osip_message_t *answer = NULL;
    
    eXosip_lock (m_ctx);
    int iret = eXosip_call_build_answer (m_ctx, m_tid, 200, &answer);
    if (iret != 0)
    {
        eXosip_call_send_answer (m_ctx, m_tid, 180, NULL);
    }
    else
    {
        osip_message_set_body (answer, sdp.c_str(), sdp.size());
        osip_message_set_content_type (answer, "application/sdp");
        eXosip_call_send_answer (m_ctx, m_tid, 200, answer);
    }
    eXosip_unlock (m_ctx);
    
    return true;
}

bool CSipCenter::SendInfo(std::string szinfo)
{
    assert_return(m_did > 0, false);
    
    int iret = 0;
    osip_message_t *info;
    
    eXosip_lock (m_ctx);
    iret = eXosip_call_build_info (m_ctx, m_did, &info);
    if (iret == OSIP_SUCCESS)
    {
        osip_message_set_content_type (info, "application/text");
        osip_message_set_body (info, szinfo.c_str(), szinfo.size());
        iret = eXosip_call_send_request (m_ctx, m_did, info);
    }
    eXosip_unlock (m_ctx);
    
    return (iret == OSIP_SUCCESS);
}

bool CSipCenter::SendMessage(std::string szmsg)
{
    int iret = 0;
    osip_message_t *msg = NULL;
    
    eXosip_lock (m_ctx);
    iret = eXosip_message_build_request(m_ctx, &msg, (const char *)"MESSAGE", m_to.c_str(), m_from.c_str(), NULL);
    if (iret == OSIP_SUCCESS)
    {
        osip_message_set_content_type (msg, "application/text");
        osip_message_set_body(msg, szmsg.c_str(), szmsg.size());
        m_tid = eXosip_message_send_request(m_ctx, msg);
    }
    else
    {
        m_tid = -1;
    }
    eXosip_unlock (m_ctx);
    
    return (m_tid > 0);
}

bool CSipCenter::SendRegister()
{
    if (m_status & S_Register)
    {
        return UpdateRegister(m_expires);
    }
 
    int iret = 0;
    osip_message_t *reg = NULL;
    
    eXosip_lock (m_ctx);
    m_rid = eXosip_register_build_initial_register (m_ctx, m_from.c_str(), m_proxy.c_str(),
                                                    NULL, m_expires, &reg);
    if (m_rid < 1) {
        eXosip_unlock (m_ctx);
        return false;
    }
    
    //osip_message_set_supported (reg, "100rel");
    //osip_message_set_supported(reg, "path");
    
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
    m_status = S_Start;
    osip_usleep(100*1000);
    
    for (; !m_quit;)
    {
        eXosip_event_t *event;
        if (!(event = eXosip_event_wait (m_ctx, 0, 1))) {
            eXosip_execute (m_ctx);
            eXosip_automatic_action (m_ctx);
            osip_usleep (5000);
            continue;
        }
        
        eXosip_execute (m_ctx);
        eXosip_automatic_action (m_ctx);
        
        switch (event->type) {
            case EXOSIP_REGISTRATION_SUCCESS:
                m_status = (m_status | S_Register);
                printf("register OK!\n");
                break;
            case EXOSIP_REGISTRATION_FAILURE:
                break;
            case EXOSIP_CALL_INVITE:
                m_status = (m_status | S_Init);
                m_cid = event->cid;
                m_tid = event->tid;
                m_did = event->did;
                printf("[====sipcenter] recv invite!, cid=%d, tid=%d, did=%d\n", m_cid, m_tid, m_did);
                break;
            case EXOSIP_CALL_RINGING:
                m_did = event->did;
                break;
            case EXOSIP_CALL_REINVITE:
            case EXOSIP_CALL_NOANSWER:
            case EXOSIP_CALL_ANSWERED:
            case EXOSIP_CALL_REQUESTFAILURE:
            case EXOSIP_CALL_SERVERFAILURE:
            case EXOSIP_CALL_GLOBALFAILURE:
            case EXOSIP_CALL_CANCELLED:
            case EXOSIP_CALL_CLOSED:
            case EXOSIP_CALL_RELEASED:
            default:
                printf("[====sipcenter] recieved eXosip event (type, did, cid) = (%d, %d, %d)\n",
                       event->type, event->did, event->cid);
                break;
        }
        eXosip_event_free(event);
    }
    m_status = S_Stop;
}


