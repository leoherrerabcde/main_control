#ifndef CSOCKET_H
#define CSOCKET_H

#include <iostream>
#include <future>
#include <functional>
#include <queue>
#include <thread>
#include <pthread.h>

#include <errno.h>
#include <string.h>

#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/signal.h>
#include <mutex>


enum SocketError
{
    NoError = 0,
    OpeningSocket,
    BindingSocket,
    AcceptError,
    SetSockOptError,
    MaxSendDataTries,
    ReadTimeOut,
    ReadError,
    SocketDisconected,
};

enum SocketState
{
    sckClosed =0,
    sckOpened,
    sckListening,
    sckConnectionPending,
    sckResolvingHost,
    sckHostResolved,
    sckConnecting,
    sckConnected,
    sckDisconnecting,
    sckDisconnected,
    sckClosing,
    sckError
};

class CSocket
{
    public:
        CSocket(const std::string& hostName, const unsigned int port, const size_t iBufferSize = 0);
        CSocket(const int sock, const size_t iBufferSize = 0);
        CSocket();
        virtual ~CSocket();

        void init();
        bool isError();
        /*void setError(const SocketError sockErrCode);
        void setError(const int sockErrCode);
        void setError(const SocketError sockErrCode, const std::string& strErrMsg);*/

        bool isConnected()
        {
        if (!m_bConnected)
            std::cout << "It is disconnected" << std::endl;
        return m_bConnected;}
        SocketState getSocketState() {return m_sckState;}

        void setLocalPort(const int port);
        void setRemotePort(const int port);

        int getLocalPort() const;
        int getRemotePort() const;

        void setRemoteHost(const std::string& hostName);
        std::string getLocalHost() const;
        std::string getRemoteHost() const;

        void setBufferSize(const size_t iBufferSize);
        std::string getIDClient() { return m_strID;}
        void setIDClient(const std::string& name = "") {m_strID = name;}

        void setState(SocketState newState, const std::string& msg = "");
        //SocketState getState() const {return m_sckState;}

        void setConnected() { m_bConnected = true; }
        int getSocketHdl() const {return sockfd;};
        struct sockaddr_in* getCliAddrPtr() {return &cli_addr;}
        int getCliAddrLen()const {return clilen;}

        void addSocket(CSocket* newSocket) {m_SockConnectedList.push(newSocket);}
        void addSocket(int newSocket);
        CSocket* getSocket() {CSocket* newSocket = m_SockConnectedList.front(); m_SockConnectedList.pop(); return newSocket;}

        void listen();
        void connect();
        void connect(const std::string& remoteHost, const int& remotePort);
        void disconnect();
        void socketClose();

        bool sendData(std::string msg);
        std::string getData();

        bool listening();
        bool connecting();

        int send(std::string msg);
        std::string& receive();

        void runRcvLoop();
        void setSocketName(const std::string& name) {m_strSckName=name;}

        std::string getStateDescript();

    protected:

        void receivingLoop();

        void throwError(const int line, const SocketError& sockErrCode, const std::string& strErrorMsg);
        void throwError(const int line, const SocketError& sockErrCode);
        void throwError(const int line, const int sockErrCode);

        void printError(const SocketError& sockErrCode, const std::string& strErrorMsg = "");
        void printError(const int& sockErrCode, const std::string& strErrorMsg = "");

        char* allocateBuffer(char* pBuf, size_t size);
        void deallocateBuffer(char* pBuf);

    private:

        int enableKeepAlive(const int sock);
        void clearBuffer();

        std::thread* m_pListeningThread;
        std::thread* m_pReceivingThread;

        bool m_bConnected;
        int sockfd;
        int newsockfd;
        int portno;
        int m_iRemotePort;
        std::string m_strRemoteHost;
        socklen_t clilen;
        char* m_pRcvBuffer;
        size_t m_iBufferSize;
        struct sockaddr_in serv_addr, cli_addr;
        struct hostent* server;
        int m_sckError;
        SocketState m_sckState;
        std::string m_strID;
        std::queue<CSocket*> m_SockConnectedList;
        int m_iMaxSendDataTries;
        bool m_bReceiveEvent;
        std::string m_bufferIn;
        std::mutex m_RecMutex;
        std::string m_strSckName;
};

#endif // CSOCKET_H
