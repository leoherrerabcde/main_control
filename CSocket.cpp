#include "CSocket.h"

#include <vector>

static std::vector<std::string> stErrorDescriptList = {
    "Error opening socket",
    "Error binding socket"
    };

CSocket::CSocket(const string& hostName, const unsigned int port)
: m_bConnected(false), m_strRemoteHost(hostName) , m_iRemotePort(port) , m_sckError(NoError) , m_sckState(sckClosed)
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

int CSocket::getLocalPort()
{
    return portno;
}

int CSocket::getRemotePort()
{
    return m_RemotePort;
}

void CSocket::setRemoteHost(const std::string& hostName)
{
    m_strRemoteHost = hostName;
}

std::string CSocket::getLocalHost()
{
    return "";
}

std::string CSocket::getRemoteHost()
{
    return m_strRemoteHost;
}

void CSocket::throwError(const SocketError& socketErr)
{
    throw(stErrorDescriptList[socketErr]);
}

void CSocket::init()
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

void CSocket::listen()
{
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
          sizeof(serv_addr)) < 0)
          error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
      error("ERROR on accept");
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
    return 0;
}

void CSocket::connect()
{
    if (m_sckState == sckClosed)
        init();
    if (m_sckState == sckConnected)
        disconnect();

}
