#include "http_conn.h"
#include <unistd.h>
#include <iostream>
#include <cstring>

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

LineCode HttpConn::getLine()
{
    char* ret = strchr(m_inBuf + m_lineStart, '\n');
    if(ret == nullptr)
        return LineCode::LINE_OPEN;

    std::cout << "line parse ok!" << std::endl;
    m_lineStart += (ret - m_inBuf + 1);
}

HttpCode HttpConn::parseReqLine()
{
    
}