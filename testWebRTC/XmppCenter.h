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

#include "ubase/ubase.h"
#include "xrtc_api.h"
#import "RtcCenter.h"


///> xmpp task
struct XmppTask{
    XmppTask(){}
    XmppTask(std::string subject, std::string body) : subject(subject), body(body) {};
    
    std::string subject;
    std::string body;
};


///> This is Xmpp client
class XmppCenter : public MessageSessionHandler, ConnectionListener, LogHandler,
                        MessageEventHandler, MessageHandler, ChatStateHandler
{
public:
    XmppCenter();
    ~XmppCenter();
    
    void PushTask(std::string subject, std::string body);
    void PushTask(XmppTask & task);

    bool Init();
    void Uninit();
    bool Start();
    void Stop();
    
    ///> for gloox
    void SetAccount(std::string fromuser, std::string passwd)  { m_from = fromuser + "@" + m_domain; m_passwd = passwd; }
    void SetRemote(std::string touser)      { m_to = touser + "@" + m_domain;}
    
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
    
    ///> for librtc
protected:
    bool InitRtc();
    void UninitRtc();
    bool SetLocalStream();
    
protected:
    static void * threadStart(void *arg);
    void Run();
    
    static void * signalStart(void *arg);
    void Loop();
    
private:
    IRtcCenter * m_rtc;
    IRtcSink * m_sink;
    bool m_rtc_init;
    
    Client* m_client;
    MessageSession *m_session;
    MessageEventFilter *m_messageEventFilter;
    ChatStateFilter *m_chatStateFilter;
    
    std::string m_from;
    std::string m_passwd;
    std::string m_to;
    std::string m_domain;

    bool m_connected;
    bool m_quit;
    ubase::Queue<XmppTask> m_queue;
};

extern XmppCenter *g_xmpp;

#endif
