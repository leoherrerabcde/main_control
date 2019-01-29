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


enum SocketError
{
    NoError = 0,
    OpeningSocket,
    BindingSocket,
    AcceptError,
    SetSockOptError
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
        CSocket(const std::string& hostName, const unsigned int port);
        CSocket(const int sock);
        CSocket();
        virtual ~CSocket();

        void init();
        bool isError();
        /*void setError(const SocketError sockErrCode);
        void setError(const int sockErrCode);
        void setError(const SocketError sockErrCode, const std::string& strErrMsg);*/

        bool isConnected() {return m_bConnected;}
        SocketState getSocketState() {return m_sckState;}

        void setLocalPort(const int port);
        void setRemotePort(const int port);

        int getLocalPort() const;
        int getRemotePort() const;

        void setRemoteHost(const std::string& hostName);
        std::string getLocalHost() const;
        std::string getRemoteHost() const;

        void setState(SocketState newState, const std::string& msg = "");
        SocketState getState() const {return m_sckState;}

        int getSocketHdl() const {return sockfd;};
        struct sockaddr_in* getCliAddrPtr() {return &cli_addr;}
        int getCliAddrLen()const {return clilen;}

        void addSocket(CSocket* newSocket) {m_SockConnectedList.push(newSocket);}
        void addSocket(int newSocket);
        CSocket* getSocket() {CSocket* newSocket = m_SockConnectedList.front(); m_SockConnectedList.pop(); return newSocket;}

        void listen();
        void connect();
        void disconnect();
        bool listening();

        int send(const std::string& msg);
        std::string& receive();

    protected:

        void throwError(const SocketError& sockErrCode, const std::string& strErrorMsg);
        void throwError(const SocketError& sockErrCode);
        void throwError(const int sockErrCode);

    private:

        int enableKeepAlive(const int sock);
        std::string getStateDescript();

        std::thread* m_pListeningThread;

        bool m_bConnected;
        int sockfd;
        int newsockfd;
        int portno;
        int m_iRemotePort;
        std::string m_strRemoteHost;
        socklen_t clilen;
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;
        struct hostent* server;
        int m_sckError;
        SocketState m_sckState;
        std::string m_strID;
        std::queue<CSocket*> m_SockConnectedList;
};

#endif // CSOCKET_H
