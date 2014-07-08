#include "XmppCenter.h"
#include <sstream>

#define SAFE_DEL(p) if (p) { delete p; p = NULL;}
#define assert_return(p, v) if(!(p)) { return (v);}

XmppCenter *g_xmpp = new XmppCenter;


XmppCenter::XmppCenter()
{
    m_sink = NULL;
    m_rtc = NULL;
    
    m_client = NULL;
    m_session = NULL;
    m_messageEventFilter = NULL;
    m_chatStateFilter = NULL;
    
    m_from = "2015@im.uskee.org";
    m_passwd = "2015";
    m_to = "2013@im.uskee.org";
    
    m_connected = false;
    m_quit = true;
}

XmppCenter::~XmppCenter()
{
    SAFE_DEL(m_client);
    SAFE_DEL(m_session);
    SAFE_DEL(m_messageEventFilter);
    SAFE_DEL(m_chatStateFilter);
}

bool XmppCenter::Init()
{
    bool bret = true;
    
    bret = InitRtc();
    assert_return(bret, false);
    
    JID jid(m_from);
    m_client = new Client(jid, m_passwd);
    m_client->registerMessageHandler( this );
    m_client->registerConnectionListener( this );
    m_client->registerMessageSessionHandler( this, 0 );
    
    m_client->disco()->setVersion( "messageTest", GLOOX_VERSION, "Linux" );
    m_client->disco()->setIdentity( "client", "bot" );
    m_client->disco()->addFeature( XMLNS_CHAT_STATES );
   
    return true;
}

bool XmppCenter::Start()
{
    assert_return(!m_connected, true);
    
    pthread_t tid;
    int iret = pthread_create(&tid, NULL, threadStart, (void *)this);
    assert_return(iret == 0, false);
    
    iret = pthread_create(&tid, NULL, signalStart, (void *)this);
    assert_return(iret == 0, false);
    
    return true;
}

void XmppCenter::Stop()
{
    m_quit = true;
    m_client->disconnect();
}

void XmppCenter::PushTask(std::string subject, std::string body)
{
    XmppTask task(subject, body);
    m_queue.push(task);
}

void XmppCenter::PushTask(XmppTask & task)
{
    m_queue.push(task);
}

bool XmppCenter::SendMessage(std::string subject, std::string body)
{
    assert_return(m_client != NULL, false);

    JID jid(m_to);
    Message message(Message::Chat, jid, body, subject);
    m_client->send(message);
    return true;
}

void XmppCenter::onConnect()
{
    printf( "connected!!!\n" );
    m_connected = true;
}

void XmppCenter::onDisconnect( ConnectionError e )
{
    printf( "message_test: disconnected: %d\n", e );
    if( e == ConnAuthenticationFailed )
        printf( "auth failed. reason: %d\n", m_client->authError() );
    m_connected = false;
}

bool XmppCenter::onTLSConnect( const CertInfo& info )
{
    time_t from( info.date_from );
    time_t to( info.date_to );
    printf( "status: %d\nissuer: %s\npeer: %s\nprotocol: %s\nmac: %s\ncipher: %s\ncompression: %s\n"
           "from: %s\nto: %s\n",
           info.status, info.issuer.c_str(), info.server.c_str(),
           info.protocol.c_str(), info.mac.c_str(), info.cipher.c_str(),
           info.compression.c_str(), ctime( &from ), ctime( &to ) );
    m_connected = true;
    return true;
}

void XmppCenter::handleMessage( const Message& msg, MessageSession* session)
{
    printf( "type: %d, subject: %s, message: %s, thread id: %s\n", msg.subtype(),
           msg.subject().c_str(), msg.body().c_str(), msg.thread().c_str() );
    
    do {
        if (!m_sink) break;
        
        std::string subject = msg.subject();
        std::string body = msg.body();
        if (subject == "sdp")
        {
            PushTask("remotesdp", body);
        }
        else if (subject == "candidate")
        {
            PushTask("candidate", body);
        
        }
    }while(0);
}

void XmppCenter::handleMessageEvent( const JID& from, MessageEventType event )
{
    printf( "received event: %d from: %s\n", event, from.full().c_str() );
}

void XmppCenter::handleChatState( const JID& from, ChatStateType state )
{
    printf( "received state: %d from: %s\n", state, from.full().c_str() );
}

void XmppCenter::handleMessageSession( MessageSession *session )
{
    printf( "got new session\n");
    
    // this example can handle only one session. so we get rid of the old session
    m_client->disposeMessageSession( m_session );
    m_session = session;
    m_session->registerMessageHandler( this );
    m_messageEventFilter = new MessageEventFilter( m_session );
    m_messageEventFilter->registerMessageEventHandler( this );
    m_chatStateFilter = new ChatStateFilter( m_session );
    m_chatStateFilter->registerChatStateHandler( this );
}

void XmppCenter::handleLog( LogLevel level, LogArea area, const std::string& message )
{
    printf("log: level: %d, area: %d, %s\n", level, area, message.c_str() );
}

void * XmppCenter::threadStart(void *arg)
{
    XmppCenter *thiz = (XmppCenter *)arg;
    if (thiz)
        thiz->Run();
    return NULL;
}

void XmppCenter::Run()
{
    StringList ca;
    ca.push_back( "/path/to/cacert.crt" );
    m_client->setCACerts( ca );
    m_client->logInstance().registerLogHandler( LogLevelDebug, LogAreaAll, this );
    m_client->connect(true);
    m_connected = false;
}


///===========================================


void * XmppCenter::signalStart(void *arg)
{
    XmppCenter *thiz = (XmppCenter *)arg;
    if (thiz)
        thiz->Loop();
    return NULL;
}

void XmppCenter::Loop()
{
    m_quit = false;
    
    do {
        XmppTask task;
        if(!m_queue.pop(task)) {
            usleep(100 * 1000);
            continue;
        }
        
        if (task.subject == "initrtc")
        {
        }
        else if (task.subject == "initlocalstream")
        {
            SetLocalStream();
        }
        else if (task.subject == "localsdp")
        {
            m_rtc->SetLocalDescription(task.body);
        }
        else if (task.subject == "remotesdp")
        {
            m_rtc->SetRemoteDescription(task.body);
            m_rtc->AnswerCall();
        }
        else if (task.subject == "candidate")
        {
            m_rtc->AddIceCandidate(task.body);
        }
        else if (task.subject == "setupcall")
        {
            m_rtc->SetupCall();
        }
    }while(!m_quit);
}

bool XmppCenter::InitRtc()
{
    bool bret = xrtc_init();
    assert_return(bret, false);
    
    m_sink = [[RtcSink alloc]init];
    assert_return(m_sink, false);
    
    bret = xrtc_create(m_rtc);
    assert_return(bret, false);
    m_rtc->SetSink(m_sink);
    
    return true;
}

bool XmppCenter::SetLocalStream()
{
    long lret = m_rtc->GetUserMedia(true, false);
    assert_return(lret == 0, false);
    
    lret = m_rtc->CreatePeerConnection();
    assert_return(lret == 0, false);
    
    lret = m_rtc->AddLocalStream();
    assert_return(lret == 0, false);
    
    return true;
}
