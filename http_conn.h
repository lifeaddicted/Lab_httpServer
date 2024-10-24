#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

#include <arpa/inet.h>

class HttpConn
{
    public:
        HttpConn(int sock, const sockaddr_in& addr);

        void Close();

    public:
        static int m_clientCnt;
        const static int m_maxClientCnt = 10000;
    
    private:
        int m_sockfd{-1};
        sockaddr_in m_addr{0};
};

#endif