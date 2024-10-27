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
    
    std::cout << "recv " << bytes << " bytes: " << m_inBuf;
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
    m_lineChecked = m_lineStart;
}

HttpCode HttpConn::parseReqLine()
{
    
}

CheckState HttpConn::parseHeaders()
{
    char* sep = strstr(m_inBuf + m_lineStart, "\r\n");  //getline会将\r\n置0；后续需更改
    int offset = sep - (m_inBuf + m_lineStart) + 2;
    if(*(sep - 1) == '\0')
    {
        m_lineStart += 2;
        return m_conLen ? CheckState::CHECK_CONTENT : CheckState::CHECK_END;
    }

    *sep = '\0';
    *(sep + 1) = '\0';
    sep = strchr(m_inBuf + m_lineStart, ':');
    if(sep == nullptr)
        return CHECK_HEADERS_ERROR;
    *sep = '\0';
    if(strcasecmp(m_inBuf + m_lineStart, "Host") == 0)
    {
        size_t span = strspn(sep + 1, " \t");
        m_host = sep + 1 + span;
        std::cout << "parse Host ok: " << m_host << std::endl;
    }
    if(strcasecmp(m_inBuf + m_lineStart, "Content-Length") == 0)
    {
        size_t span = strspn(sep + 1, " \t");
        m_conLen = atoi(sep + span + 1);
        std::cout << "parse Content-Length ok: " << m_conLen << std::endl;
    }
    m_lineStart += offset;
    return CHECK_HEADERS;
}

CheckState HttpConn::parseContent()
{
    m_content = {m_inBuf + m_lineStart, m_conLen};
    std::cout << "parse Content ok: " << m_content << std::endl;
    return CHECK_CONTENT;
}

void HttpConn::sendRsp()
{
    std::string rsp;
    rsp += "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\nhello world";
    m_sock.Send(rsp.c_str(), rsp.size());
}