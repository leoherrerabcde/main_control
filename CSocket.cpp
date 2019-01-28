#include "CSocket.h"

#include <vector>

static std::vector<std::string> stErrorDescriptList = {
    "Error opening socket",
    "Error binding socket",
    "Error on Accept"
    };

CSocket::CSocket(const std::string& hostName, const unsigned int port)
: m_bConnected(false), m_iRemotePort(port) , m_strRemoteHost(hostName) , m_sckError(NoError) , m_sckState(sckClosed)
{
    //ctor
}

CSocket::CSocket(const int sock)
: m_bConnected(false), sockfd(sock) , m_sckError(NoError) , m_sckState(sckClosed)
{
    //ctor
}

CSocket::CSocket()
: m_bConnected(false), m_sckError(NoError) , m_sckState(sckClosed)
{
    //ctor
}

CSocket::~CSocket()
{
    //dtor
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

void CSocket::throwError(const SocketError& socketErr)
{
    throw(stErrorDescriptList[socketErr]);
}

void CSocket::setError(const SocketError sockErrCode)
{
    if (m_sckError != sockErrCode)
    {
        m_sckError = sockErrCode;
        throwError(sockErrCode);
    }
}

void CSocket::setError(const SocketError sockErrCode, const std::string& strErrMsg)
{
    stErrorDescriptList[sockErrCode] = strErrMsg;
    setError(sockErrCode);
}

void CSocket::init()
{
    if (m_sckState == sckClosed)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            m_sckState = sckError;
            m_sckError = OpeningSocket;
            throw("ERROR opening socket");
        }
        m_sckState = sckOpened;
    }
}

void CSocket::listen()
{
    init();

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        setError(SocketError::SetSockOptError, strerror(errno));

    enableKeepAlive(sockfd);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
          //perror("ERROR on binding");
          setError(SocketError::BindingSocket);

    ::listen(sockfd, 5);

    m_sckState = sckListening;

    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
      //error("ERROR on accept");
      setError(SocketError::AcceptError);
    bzero(buffer,256);
    //int n = read(newsockfd,buffer,255);
    /*if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");*/
    //return 0;
}

void CSocket::connect()
{
    /*if (m_sckState == sckClosed)
        init();*/
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
        m_sckState = sckClosed;
    }
}

int CSocket::enableKeepAlive(const int sock)
{
    return 0;
}

