#include "CSocket.h"
#include "SCCLog.h"

#include <vector>

static std::vector<std::string> stErrorDescriptList = {
    "Error opening socket",
    "Error binding socket",
    "Error on Accept"
    };

static std::vector<std::string> stStateDescriptList = {
    "sckClosed",
    "sckOpened",
    "sckListening",
    "sckConnectionPending",
    "sckResolvingHost",
    "sckHostResolved",
    "sckConnecting",
    "sckConnected",
    "sckDisconnecting",
    "sckDisconnected",
    "sckClosing",
    "sckError"
};

CSocket::CSocket(const std::string& hostName, const unsigned int port)
: m_pListeningThread(NULL),
    m_bConnected(false),
    m_iRemotePort(port) ,
    m_strRemoteHost(hostName) ,
    m_sckError(NoError) ,
    m_sckState(sckClosed)
{
    //ctor
}

CSocket::CSocket(const int sock)
: m_pListeningThread(NULL),
    m_bConnected(true),
    sockfd(sock) ,
    m_sckError(NoError) ,
    m_sckState(sckConnected)
{
    //ctor
}

CSocket::CSocket()
: m_pListeningThread(NULL),
    m_bConnected(false),
    m_sckError(NoError) ,
    m_sckState(sckClosed)
{
    //ctor
}

CSocket::~CSocket()
{
    //dtor
    disconnect();
}

std::string CSocket::getStateDescript()
{
    return stStateDescriptList[m_sckState];
}

void CSocket::setLocalPort(const int port)
{
    portno = port;
}

void CSocket::setRemotePort(const int port)
{
    m_iRemotePort = port;
}

int CSocket::getLocalPort() const
{
    return portno;
}

int CSocket::getRemotePort() const
{
    return m_iRemotePort;
}

void CSocket::setRemoteHost(const std::string& hostName)
{
    m_strRemoteHost = hostName;
}

std::string CSocket::getLocalHost() const
{
    return "";
}

std::string CSocket::getRemoteHost() const
{
    return m_strRemoteHost;
}

void CSocket::throwError(const SocketError& sockErrCode)
{
    m_sckError = sockErrCode;
    throw(stErrorDescriptList[sockErrCode]);
}

void CSocket::throwError(const int sockErrCode)
{
    if (m_sckError != sockErrCode)
    {
        m_sckError = sockErrCode;
        throw(strerror(sockErrCode));
    }
}

void CSocket::throwError(const SocketError& sockErrCode, const std::string& strErrorMsg)
{
    m_sckError = sockErrCode;
    throw(strErrorMsg);
}

/*void CSocket::setError(const SocketError sockErrCode)
{
    if (m_sckError != sockErrCode)
    {
        m_sckError = sockErrCode;
        throwError(sockErrCode);
    }
}*/

/*void CSocket::setError(const SocketError sockErrCode, const std::string& strErrMsg)
{
    stErrorDescriptList[sockErrCode] = strErrMsg;
    setError(sockErrCode);
}*/

/*int CSocket::getSocket() const
{
    return sockfd;
}*/

void CSocket::init()
{
    if (m_sckState == sckClosed)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            setState(sckError);
            throwError(OpeningSocket);
            /*m_sckError = OpeningSocket;
            throw("ERROR opening socket");*/
        }
        setState(sckOpened);
    }
}

void CSocket::setState(const SocketState newState, const std::string& msg)
{
    m_sckState = newState;
    SCCLog::print(getStateDescript());
    if (msg != "")
        SCCLog::print(msg);
}

void CSocket::addSocket(int newSocket)
{
    CSocket* sckClient = new CSocket(newSocket);
    m_SockConnectedList.push(sckClient);
}

void CSocket::listen()
{
    if (m_sckState != sckClosed)
        disconnect();

    init();

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        throwError(SocketError::SetSockOptError, strerror(errno));

    enableKeepAlive(sockfd);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
          //perror("ERROR on binding");
          throwError(SocketError::BindingSocket);

    ::listen(sockfd, 5);

    std::string msg("Listening by port ");
    msg += std::to_string(portno);

    setState(sckListening, msg);

    clilen = sizeof(cli_addr);

    //CSocket* self = this;
    m_pListeningThread = new std::thread(&CSocket::listening, this);
}

bool CSocket::listening()
{
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
      //error("ERROR on accept");
      throwError(SocketError::AcceptError);
    bzero(buffer,256);
    //int n = read(newsockfd,buffer,255);
    /*if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");*/
    //return 0;
    if (newsockfd == -1)
    {
        throwError(errno);
        return false;
    }
    else
    {
        addSocket(newsockfd);
        setState(SocketState::sckHostResolved);
    }
    return true;
}

void CSocket::connect()
{
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    server = gethostbyname(m_strRemoteHost.data());
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(m_iRemotePort);

    if (m_sckState != sckClosed)
        disconnect();

    init();

    enableKeepAlive(sockfd);


}

void CSocket::disconnect()
{
    if (m_sckState != sckClosed)
    {
        close(sockfd);
        m_bConnected = false;
        setState(sckClosed);
        if (m_pListeningThread != NULL)
        {
            m_pListeningThread->join();
            delete m_pListeningThread;
        }
    }
}

int CSocket::enableKeepAlive(const int sock)
{
    int yes = 1;

    if(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) == -1) {
        //cerr << errno << "  " << strerror(errno) << endl;
        throwError(errno);
        return -1;
    }

    int idle = 1;

    if(setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) == -1) {
        //cerr << errno << "  " << strerror(errno) << endl;
        throwError(errno);
        return -1;
    }

    int interval = 1;

    if(setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) == -1) {
        //cerr << errno << "  " << strerror(errno) << endl;
        throwError(errno);
        return -1;
    }

    int maxpkt = 10;

    if(setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) == -1) {
        //cerr << errno << "  " << strerror(errno) << endl;
        throwError(errno);
        return -1;
    }

    return 0;
}

