#ifndef _XMPP_CENTER_H_
#define _XMPP_CENTER_H_

#import <Foundation/Foundation.h>

#include <gloox/client.h>
#include <gloox/message.h>
#include <gloox/messagehandler.h>
#include <gloox/presencehandler.h>
#include <gloox/connectionlistener.h>
#include <gloox/messageeventhandler.h>
#include <gloox/chatstatefilter.h>
#include <gloox/messagesessionhandler.h>
#include <gloox/chatstatehandler.h>
#include <gloox/messageeventfilter.h>
#include <gloox/disco.h>

using namespace gloox;



///> This is for xmpp sink of objc
@protocol IXmppSink

@required
- (void) OnXmppSessionDescription:(const std::string &)sdp;

@required
- (void) OnXmppIceCandidate:(const std::string &)candidate;

@end
typedef NSObject<IXmppSink> IXmppSink;



///> This is Xmpp client
class XmppCenter : public MessageSessionHandler, ConnectionListener, LogHandler,
                        MessageEventHandler, MessageHandler, ChatStateHandler
{
public:
    XmppCenter();
    ~XmppCenter();
    void SetSink(IXmppSink * sink) { m_sink = sink; }
    
    bool Init();
    bool Start();
    void Stop();
    
    void SetToUser(std::string touser) { m_to = touser;}
    bool SendMessage(std::string subject, std::string body);
    
    
    virtual void onConnect();
    virtual void onDisconnect( ConnectionError e );
    virtual bool onTLSConnect( const CertInfo& info );
    
    //virtual void handlePresence( const Presence& presence );
    virtual void handleMessage( const Message& stanza, MessageSession* session);
    virtual void handleMessageEvent( const JID& from, MessageEventType event );
    virtual void handleChatState( const JID& from, ChatStateType state );
    virtual void handleMessageSession( MessageSession *session );
    virtual void handleLog( LogLevel level, LogArea area, const std::string& message );
    
protected:
    static void * threadStart(void *arg);
    void Run();
    
private:
    IXmppSink * m_sink;
    
    Client* m_client;
    MessageSession *m_session;
    MessageEventFilter *m_messageEventFilter;
    ChatStateFilter *m_chatStateFilter;
    
    bool m_connected;
    
    std::string m_from;
    std::string m_passwd;
    std::string m_to;
};

extern XmppCenter *g_xmpp;

#endif
