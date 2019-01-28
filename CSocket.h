#ifndef CSOCKET_H
#define CSOCKET_H

#include <iostream>
#include <future>
#include <functional>

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
        void setError(const SocketError sockErrCode);
        void setError(const SocketError sockErrCode, const std::string& strErrMsg);

        bool isConnected() {return m_bConnected;}
        SocketState getSocketState() {return m_sckState;}

        void setLocalPort(const int port);
        void setRemotePort(const int port);

        int getLocalPort() const;
        int getRemotePort() const;

        void setRemoteHost(const std::string& hostName);
        std::string getLocalHost() const;
        std::string getRemoteHost() const;

        void listen();
        void connect();
        void disconnect();

        int send(const std::string& msg);
        std::string& receive();


    protected:

        void throwError(const SocketError& socketErr);

    private:

        int enableKeepAlive(const int sock);

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
        SocketError m_sckError;
        SocketState m_sckState;
};

#endif // CSOCKET_H
