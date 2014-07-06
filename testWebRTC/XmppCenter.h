#ifndef _XMPP_CENTER_H_
#define _XMPP_CENTER_H_

#include <gloox/client.h>
#include <gloox/message.h>
#include <gloox/messagehandler.h>
#include <gloox/presencehandler.h>
#include <gloox/connectionlistener.h>

using namespace gloox;

#include <string>

class XmppCenter : public MessageHandler//, public PresenceHandler, public ConnectionListener
{
public:
    XmppCenter();
    ~XmppCenter();
    
    bool Init();
    bool Connect();
    
    virtual void handleMessage( const Message& stanza, MessageSession* session);
    //virtual void handlePresence( const Presence& presence );
    //virtual void onConnect();
    //virutal bool onTLSConnect( const CertInfo& info );
    
private:
    Client* m_client;
    std::string m_from;
    std::string m_passwd;
};

extern XmppCenter *g_xmpp;

#endif
