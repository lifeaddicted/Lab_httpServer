#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

#include <arpa/inet.h>

class HttpConn
{
    public:
        HttpConn();
        ~HttpConn();

    public:
        static int m_clientCnt;
        const static int m_maxClientCnt;
    
    private:
        int m_sockfd{-1};
        sockaddr_in clientAddr{0};
};

#endif