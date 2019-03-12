#include "CSocket.h"
#include "SCCLog.h"

#include <vector>


//extern std::string globalMyDeviceName;


static std::vector<std::string> stErrorDescriptList = {
    "No Error",
    "Error opening socket",
    "Error binding socket",
    "Error on Accept",
    "SetSockOptError",
    "Exceed Tries to Send Data",
    "Read Time Out",
    "Error during reading process.",
    "Socket Disconected"
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

CSocket::CSocket(const std::string& hostName, const unsigned int port, const size_t bufferSize)
: m_pListeningThread(NULL),
    m_pReceivingThread(NULL),
    m_bConnected(false),
    m_iRemotePort(port) ,
    m_strRemoteHost(hostName) ,
    m_iBufferSize(bufferSize),
    m_sckError(NoError) ,
    m_sckState(sckClosed)
{
    //ctor
    m_pRcvBuffer = new char[m_iBufferSize];
}

CSocket::CSocket(const int sock, const size_t bufferSize)
: m_pListeningThread(NULL),
    m_pReceivingThread(NULL),
    m_bConnected(true),
    sockfd(sock) ,
    m_iBufferSize(bufferSize),
    m_sckError(NoError) ,
    m_sckState(sckConnected)
{
    //ctor
    if (m_iBufferSize)
        m_pRcvBuffer = new char[m_iBufferSize];
}

CSocket::CSocket()
: m_pListeningThread(NULL),
    m_pReceivingThread(NULL),
    m_bConnected(false),
    m_iBufferSize(0),
    m_sckError(NoError) ,
    m_sckState(sckClosed)
{
    //ctor
}

CSocket::~CSocket()
{
    //dtor
    disconnect();
    clearBuffer();
}

void CSocket::clearBuffer()
{
    if (m_iBufferSize)
    {
        delete [] m_pRcvBuffer;
        m_iBufferSize = 0;
    }
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

void CSocket::throwError(const int line, const SocketError& sockErrCode)
{
    m_sckError = sockErrCode;
    printError(errno, strerror(errno));
    printError(sockErrCode);
    throw(stErrorDescriptList[sockErrCode]);
}

void CSocket::throwError(const int line, const int sockErrCode)
{
    if (m_sckError != sockErrCode)
    {
        m_sckError = sockErrCode;

        printError(sockErrCode);
        //SCCLog.print("");
        throw(strerror(sockErrCode));
    }
}

void CSocket::throwError(const int line, const SocketError& sockErrCode, const std::string& strErrorMsg)
{
    m_sckError = sockErrCode;
    printError(sockErrCode, strErrorMsg);
    throw(strErrorMsg);
}

void CSocket::setBufferSize(const size_t iBufferSize)
{
    clearBuffer();
    m_iBufferSize = iBufferSize;
    m_pRcvBuffer = new char[m_iBufferSize];
}

void CSocket::init()
{
    if (m_sckState == sckClosed)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            setState(sckError);
            throwError(__LINE__, OpeningSocket);
            /*m_sckError = OpeningSocket;
            throw("ERROR opening socket");*/
        }
        setState(sckOpened);
    }
}

void CSocket::setState(const SocketState newState, const std::string& msg)
{
    m_sckState = newState;
    /*SCCLog::print(getStateDescript());
    if (msg != "")
        SCCLog::print(msg);*/
}

void CSocket::addSocket(int newSocket)
{
    CSocket* sckClient = new CSocket(newSocket);
    sckClient->setState(sckConnected);
    sckClient->setConnected();
    m_SockConnectedList.push(sckClient);
}

void CSocket::listen()
{
    if (m_sckState != sckHostResolved)
    {
        if (m_sckState != sckClosed)
            disconnect();

        init();

        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
            throwError(__LINE__, SocketError::SetSockOptError, strerror(errno));

        enableKeepAlive(sockfd);

        bzero(&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(portno);

        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
              //perror("ERROR on binding");
              throwError(__LINE__, SocketError::BindingSocket);
    }
    ::listen(sockfd, 5);

    /*std::string msg("Listening by port ");
    msg += std::to_string(portno);

    setState(sckListening, msg);*/

    clilen = sizeof(cli_addr);

    //CSocket* self = this;
    if (m_pListeningThread != NULL)
    {
        m_pListeningThread->join();
        delete m_pListeningThread;
    }
    m_pListeningThread = new std::thread(&CSocket::listening, this);

    while (m_sckState == sckClosed || m_sckState == sckHostResolved)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

bool CSocket::listening()
{
    std::string msg("Listening by port ");
    msg += std::to_string(portno);
    setState(sckListening, msg);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
      //error("ERROR on accept");
      throwError(__LINE__, SocketError::AcceptError);

    if (newsockfd == -1)
    {
        throwError(__LINE__, errno);
        return false;
    }
    else
    {
        addSocket(newsockfd);
        setState(SocketState::sckHostResolved);
    }
    return true;
}

void CSocket::connect(const std::string& host, const int& port)
{
    setRemoteHost(host);
    setRemotePort(port);
    connect();
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

    setState(sckConnectionPending);

    enableKeepAlive(sockfd);

    m_pListeningThread = new std::thread(&CSocket::connecting, this);
}

bool CSocket::connecting()
{
    if (m_sckState == sckConnectionPending)
    {
        setState(sckConnecting);
        for(size_t i = 0; i < 15; i++)
        { //try to connect 3 times
            if(::connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
            {
                //cerr << "Error on connecting: " << errno << "  " << strerror(errno) << endl;
                //throwError(errno);
                setState(sckError);
            }
            else
            {
                setConnected();
                setState(sckConnected);
                m_pReceivingThread = new std::thread(&CSocket::receivingLoop, this);
                break;
            }
        }
    }
    return true;
}

void CSocket::runRcvLoop()
{
    if (getState() == sckConnected)
        m_pReceivingThread = new std::thread(&CSocket::receivingLoop, this);
}

void CSocket::disconnect()
{
    if (m_sckState != sckClosed)
    {
        close(sockfd);
        setState(sckClosed);
        if (m_pListeningThread != NULL && !m_bConnected)
        {
            m_pListeningThread->join();
            delete m_pListeningThread;
            m_pListeningThread = NULL;
        }
        m_bConnected = false;
        if (m_pReceivingThread != NULL)
        {
            m_pReceivingThread->join();
            delete m_pReceivingThread;
            m_pReceivingThread = NULL;
        }
    }
}

int CSocket::enableKeepAlive(const int sock)
{
    int yes = 1;

    if(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) == -1) {
        //cerr << errno << "  " << strerror(errno) << endl;
        throwError(__LINE__, errno);
        return -1;
    }

    int idle = 1;

    if(setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) == -1) {
        //cerr << errno << "  " << strerror(errno) << endl;
        throwError(__LINE__, errno);
        return -1;
    }

    int interval = 1;

    if(setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) == -1) {
        //cerr << errno << "  " << strerror(errno) << endl;
        throwError(__LINE__, errno);
        return -1;
    }

    int maxpkt = 10;

    if(setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) == -1) {
        //cerr << errno << "  " << strerror(errno) << endl;
        throwError(__LINE__, errno);
        return -1;
    }

    return 0;
}

bool CSocket::sendData(std::string msg)
{
    if (m_bConnected != true || m_sckState != sckConnected)
        return false;

    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(sockfd, &writefds);

    int sentBytes = 0;
    m_iMaxSendDataTries = 5;
    int tries = 0;

    for(size_t i = 0; i < msg.length(); i += sentBytes)
    {
        FD_ZERO(&writefds);
        FD_SET(sockfd, &writefds);
        int rv = select(sockfd + 1, NULL, &writefds, NULL, &tv);

        if(rv == -1)
            //cerr << errno << "  " << strerror(errno) << endl;
            throwError(__LINE__, errno);
        else if(rv == 0)
        {
            sentBytes = 0;
            if (++tries > m_iMaxSendDataTries)
                throwError(__LINE__, SocketError::MaxSendDataTries);
        }
        else if(rv > 0 && FD_ISSET(sockfd, &writefds))
        {
            sentBytes = ::write(sockfd, msg.substr(i, msg.length() - i).c_str(), msg.length() - i);

            if(sentBytes == -1)
            {
                //cerr << "Error sending IDs: " << errno << "  " << strerror(errno) << endl;
                //throwError(__LINE__, SocketDisconected);
                disconnect();
                return false;
            }
            tries = 0;
        }
    }
    return true;
}

std::string CSocket::getData()
{
    std::lock_guard<std::mutex> lockBuffer(m_RecMutex);
    if (m_bReceiveEvent)
    {
        m_bReceiveEvent = false;
        std::string buf(m_bufferIn);
        m_bufferIn = "";
        return buf;
    }
    return "";
}

void CSocket::receivingLoop()
{
    bool bBreakLoop = false;
    while(m_bConnected && m_sckState == sckConnected && !bBreakLoop)
    {
        /*if (m_bConnected != true || m_sckState != sckConnected)
            return "";*/

        struct timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        //std::string resp = "";
        int n = 0;

        do
        {
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);
            int rv = select(sockfd + 1, &readfds, NULL, NULL, &tv);

            if(rv <= -1)
                //cerr << errno << "  " << strerror(errno) << endl;
                throwError(__LINE__, errno);
            else if(rv == 0)
            {
                //throwError(__LINE__, SocketError::ReadTimeOut);
                break;
            }
            else if(rv > 0 && FD_ISSET(sockfd, &readfds))
            {
                n = ::read(sockfd, m_pRcvBuffer, m_iBufferSize - 1);

                if(n > 0)
                {
                    /*n = tn;
                    m_pRcvBuffer[n] = '\0';
                    string tResp(m_pRcvBuffer, n);
                    resp += tResp;*/
                    m_pRcvBuffer[n] = '\0';
                    std::lock_guard<std::mutex> lockBuffer(m_RecMutex);
                    m_bufferIn += m_pRcvBuffer;
                    m_bReceiveEvent = true;
                }
                else if(n == -1)
                {
                    if(errno == 11)
                    { //get the good part of the received stuff also if the connection closed during receive -> happens sometimes with short messages
                        std::string tResp(m_pRcvBuffer);

                        if(tResp.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890,.{}[]'") == std::string::npos) //but only allow valid chars
                        {
                            std::lock_guard<std::mutex> lockBuffer(m_RecMutex);
                            m_bufferIn += tResp;
                            m_bReceiveEvent = true;
                        }
                    }
                    else
                    {
                        //cerr << errno << "  " << strerror(errno) << endl;
                        //throwError(__LINE__, errno);
                        bBreakLoop = true;
                    }
                    break;
                }
                else
                    break;
            }
            else
                //cerr << "ERROR: rv: " << rv << endl;
                throwError(__LINE__, SocketError::ReadError);

        }
        while(n >= (int)(m_iBufferSize - 1));
    }
    //return resp;
}

void CSocket::printError(const SocketError& sockErrCode, const std::string& strErrorMsg)
{
    std::cout << m_strSckName << "::CSocket: " << stErrorDescriptList[sockErrCode] << std::endl;
    if (strErrorMsg !="")
        std::cout << " " << strErrorMsg << std::endl;
}

void CSocket::printError(const int& sockErrCode, const std::string& strErrorMsg)
{
    std::cout << m_strSckName << "::CSocket: " << strerror(sockErrCode) << std::endl;
    if (strErrorMsg !="")
        std::cout << " " << strErrorMsg << std::endl;
}


