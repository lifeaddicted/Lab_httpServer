#include "http_conn.h"
#include <unistd.h>

int HttpConn::m_clientCnt = 0;

HttpConn::HttpConn(int sock, const sockaddr_in& addr)
: m_sockfd(sock), m_addr(addr)
{}

void HttpConn::Close()
{
    if(m_sockfd != -1)
        close(m_sockfd);
}

