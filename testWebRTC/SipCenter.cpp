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

#define SIP_DOMAIN "sip.uskee.org"
#define SIP_USER "2011"
#define SIP_PASSWD "20112"
#define SIP_SCHEME "digest"
#define SIP_PORT 5060       /* Listening SIP port */
#define assert_return(p, v) if (!(p)) {return (v);}


CSipCenter *g_sip = new CSipCenter();

CSipCenter::CSipCenter()
{
    m_osip = eXosip_malloc();
}

CSipCenter::~CSipCenter()
{
    eXosip_quit(m_osip);
    m_osip = NULL;
}

bool CSipCenter::Init()
{
    TRACE_INITIALIZE (6, stdout);
    
    int iret = eXosip_init(m_osip);
    assert_return(iret != 0, false);
    
    iret = eXosip_listen_addr (m_osip, IPPROTO_UDP, NULL, m_port, AF_INET, 0);
    if (iret != 0) {
        eXosip_quit(NULL);
        fprintf (stderr, "could not initialize transport layer\n");
        return -1;
    }
    return true;
}