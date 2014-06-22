//
//  SipCenter.cpp
//  testWebRTC
//
//  Created by Yongze Xu on 6/10/14.
//  Copyright (c) 2014 sunrui. All rights reserved.
//

#include "SipCenter.h"

#define THIS_FILE "SipCenter"

#define SIP_DOMAIN "sip.uskee.org"
#define SIP_USER "2011"
#define SIP_PASSWD "20112"
#define SIP_SCHEME "digest"
#define SIP_PORT 5060       /* Listening SIP port */

#define AF pj_AF_INET()
#define current_acc pjsua_acc_get_default()

#define assert_return(p, v) if (!(p)) {return (v);}
#define pj_status_assert(t, s) \
    if ((s) != PJ_SUCCESS) { \
        pjsua_perror(THIS_FILE, (t), (s)); \
        return (s); \
    }


extern "C" void srtp_deinit ()
{
}



//
//==================================
//

CSipCenter *g_sip = new CSipCenter();

static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata) {
    if (g_sip) g_sip->OnIncomingCall(acc_id, call_id, rdata);
}

static void on_call_state(pjsua_call_id call_id, pjsip_event *e) {
    if (g_sip) g_sip->OnCallState(call_id, e);
}

static void on_call_media_state(pjsua_call_id call_id) {
    if (g_sip) g_sip->OnCallMediaState(call_id);
}

CSipCenter::CSipCenter()
{
    m_acc_id = -1;
    m_user = SIP_USER;
    m_passwd = SIP_PASSWD;
    m_realm = SIP_DOMAIN;
    m_scheme = SIP_SCHEME;
    m_port = SIP_PORT;
    m_sdp = "";
}


CSipCenter::~CSipCenter()
{}


int CSipCenter::Init()
{
    pj_status_t status;
    
    /* Create pjsua first! */
    status = pjsua_create();
    pj_status_assert("Error in pjsua_create()", status);

    /* Init pjsua */
    pjsua_config cfg;
    pjsua_logging_config log_cfg;
    
    pjsua_config_default(&cfg);
    cfg.cb.on_incoming_call = &on_incoming_call;
    cfg.cb.on_call_media_state = &on_call_media_state;
    cfg.cb.on_call_state = &on_call_state;
    cfg.use_srtp = PJMEDIA_SRTP_DISABLED;
    
    pjsua_logging_config_default(&log_cfg);
    log_cfg.console_level = 4;
    
    status = pjsua_init(&cfg, &log_cfg, NULL);
    pj_status_assert("Error in pjsua_init()", status);
    
    return status;
}


int CSipCenter::Uninit()
{
    pjsua_call_hangup_all();
    pjsua_destroy();
    return 0;
}

int CSipCenter::SetAccount(std::string user, std::string password, std::string realm)
{
    m_user = user;
    m_passwd = password;
    m_realm = realm;
    return 0;
}

int CSipCenter::AddTransport()
{
    /* Add UDP transport. */
    pjsua_transport_config cfg;
    pjsua_transport_config_default(&cfg);
    cfg.port = m_port;
    pj_status_t status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
    pj_status_assert("Error creating transport", status);
    return status;
}

int CSipCenter::Start()
{
    /* Initialization is done, now start pjsua */
    pj_status_t status = pjsua_start();
    pj_status_assert("Error starting pjsua", status);
    return status;
}

int CSipCenter::Register()
{
    /* Register to SIP server by creating SIP account. */
    pjsua_acc_config cfg;
    pjsua_acc_config_default(&cfg);
    
    std::string idstr = "sip:" + m_user + "@" + m_realm;
    std::string reg_uri_str = std::string("sip:") + m_realm;
    cfg.id = pj_str((char *)idstr.c_str());
    cfg.reg_uri = pj_str((char *)reg_uri_str.c_str());
    
    cfg.cred_count = 1;
    cfg.cred_info[0].realm = pj_str((char *)m_realm.c_str());
    cfg.cred_info[0].scheme = pj_str((char *)m_scheme.c_str());
    cfg.cred_info[0].username = pj_str((char *)m_user.c_str());
    cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    cfg.cred_info[0].data = pj_str((char *)m_passwd.c_str());
    
    pj_status_t status = pjsua_acc_add(&cfg, PJ_TRUE, &m_acc_id);
    pj_status_assert("Error adding account", status);
    return status;
}


/*
 * If URL is specified, then make call immediately.
 */
int CSipCenter::Call(std::string szuri)
{
    /* If URL is specified, make call to the URL. */
    pj_str_t uri = pj_str((char *)szuri.c_str());
    pj_status_t status = pjsua_call_make_call(m_acc_id, &uri, 0, NULL, NULL, NULL);
    pj_status_assert("Error making call", status);
    return status;
}

int CSipCenter::Looping()
{
    return 0;
}

/* Callback called by the library upon receiving incoming call */
void CSipCenter::OnIncomingCall(pjsua_acc_id acc_id, pjsua_call_id call_id, pjsip_rx_data *rdata)
{
    pjsua_call_info ci;
    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);
    pjsua_call_get_info(call_id, &ci);
    
    PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!", (int)ci.remote_info.slen, ci.remote_info.ptr));
    
    /* Automatically answer incoming calls with 200/OK */
    pjsua_call_answer(call_id, 200, NULL, NULL);
}

/* Callback called by the library when call's state has changed */
void CSipCenter::OnCallState(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;
    PJ_UNUSED_ARG(e);
    pjsua_call_get_info(call_id, &ci);
    
    PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id, (int)ci.state_text.slen, ci.state_text.ptr));
}

/* Callback called by the library when call's media state has changed */
void CSipCenter::OnCallMediaState(pjsua_call_id call_id)
{
    pjsua_call_info ci;
    pjsua_call_get_info(call_id, &ci);
    
    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        // When media is active, connect call to sound device.
        //pjsua_conf_connect(ci.conf_slot, 0);
        //pjsua_conf_connect(0, ci.conf_slot);
    }
}
