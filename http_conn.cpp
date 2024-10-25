#include "http_conn.h"
#include <unistd.h>
#include <iostream>

int HttpConn::m_clientCnt = 0;

int HttpConn::handleInput()
{
    int bytes = m_sock.Recv(m_inBuf + m_inOff, BUF_SIZE - m_inOff - 1);
    if(bytes == -1)
        return -1;
    
    std::cout << "recv " << bytes << " bytes: " << m_inBuf + m_inOff;
    m_inOff += bytes;
    m_inBuf[m_inOff] = 0;
    return bytes;
}
