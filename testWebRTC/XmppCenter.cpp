#include "XmppCenter.h"

#define SAFE_DEL(p) if (p) { delete p; p = NULL;}

XmppCenter *g_xmpp = new XmppCenter;


XmppCenter::XmppCenter()
{
    m_client = NULL;
    m_from = "2013@im.uskee.org";
    m_passwd = "2013";
}

XmppCenter::~XmppCenter()
{
    SAFE_DEL(m_client);
}

bool XmppCenter::Init()
{
    JID jid(m_from);
    m_client = new Client(jid, m_passwd);
    m_client->registerMessageHandler( this );
   
    return true;
}

bool XmppCenter::Connect()
{
    m_client->connect();
    return true;
}
    
void XmppCenter::handleMessage( const Message& stanza, MessageSession* session)
{
    Message msg( stanza.subtype(), stanza.to(), "hello world" );
    m_client->send( msg );
}