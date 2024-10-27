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

    std::cout << "get line ok!" << std::endl;
    m_lineChecked += (ret - m_inBuf);

    m_inBuf[m_lineChecked - 1] = (m_inBuf[m_lineChecked - 1] == '\r') ? '\0'
                                 : m_inBuf[m_lineChecked - 1];
    m_inBuf[m_lineChecked++] = '\0';
    return LineCode::LINE_OK;
}

bool HttpConn::parseReqLine()
{
    //Method
    char* sep = strchr(m_inBuf + m_lineStart, ' ');
    if(sep == nullptr)
        return false;
    
    *sep = '\0';
    if(strcasecmp(m_inBuf + m_lineStart, "GET") == 0)
        m_method = Method::GET;
    if(strcasecmp(m_inBuf + m_lineStart, "POST") == 0)
        m_method = Method::POST;

    if(m_method == Method::NONE)
    {
        std::cout << "bad req: " << m_inBuf + m_lineStart << std::endl;
        return false;
    }
    m_lineStart += sep - (m_inBuf + m_lineStart) + 1;

    //Url
    sep = strchr(m_inBuf + m_lineStart, ' ');
    if(sep == nullptr)
        return false;

    *sep = '\0';
    m_url = m_inBuf + m_lineStart;
    m_lineStart += sep - (m_inBuf + m_lineStart) + 1;

    //Ver
    m_httpVer = m_inBuf + m_lineStart;
    m_lineStart = m_lineChecked;

    std::cout << "parse reqLine ok!" << std::endl;
    std::cout << m_method << " " << m_url << " " << m_httpVer << std::endl;
    
    return true;
}