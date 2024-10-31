#include "http_conn.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/epoll.h>

int HttpConn::m_clientCnt = 0;
const char* RscRoot = "../root/";

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

int HttpConn::handleOutput()
{
    while(m_bytesToSend)
    {
        int num = m_sock.Send(m_outbuf, 2);
        if(num == -1 && errno == EAGAIN)
        {
            epoll_event event;
            event.data.fd = m_sock.getSockFd();
            event.events = EPOLLOUT | EPOLLONESHOT;
            epoll_ctl(m_epollfd, EPOLL_CTL_MOD, event.data.fd, &event);
            break;
        }
        
        m_bytesToSend -= num;
        if (m_outbuf[0].iov_len)
        {
            if(m_outbuf[0].iov_len > num)
            {
                m_outbuf[0].iov_base += num;
                m_outbuf[0].iov_len -= num;
                num = 0;
            }
            else
            {
                num -= m_outbuf[0].iov_len;
                m_outbuf[0].iov_len = 0;
                m_outbuf[0].iov_base = nullptr;
            }
        }
        if (m_outbuf[1].iov_len)
        {
            if(m_outbuf[1].iov_len > num)
            {
                m_outbuf[1].iov_base += num;
                m_outbuf[1].iov_len -= num;
                num = 0;
            }
            else
            {
                num -= m_outbuf[1].iov_len;
                m_outbuf[1].iov_len = 0;
                m_outbuf[1].iov_base = nullptr;
            }
        }
    }

    if(m_bytesToSend)
    {
        epoll_event event;
        event.data.fd = m_sock.getSockFd();
        event.events = EPOLLOUT | EPOLLONESHOT;
        epoll_ctl(m_epollfd, EPOLL_CTL_MOD, event.data.fd, &event);
    }
    else
    {
        resetOutBuf();
        epoll_event event;
        event.data.fd = m_sock.getSockFd();
        event.events = EPOLLIN;
        epoll_ctl(m_epollfd, EPOLL_CTL_MOD, event.data.fd, &event);
    }
    return m_bytesToSend;
}

LineCode HttpConn::getLine()
{
    char* ret = strchr(m_inBuf + m_lineStart, '\n');
    if(ret == nullptr)
        return LineCode::LINE_OPEN;

    std::cout << "get line ok!" << std::endl;
    m_lineChecked += (ret - m_inBuf - m_lineStart);

    m_inBuf[m_lineChecked - 1] = (m_inBuf[m_lineChecked - 1] == '\r') ? '\0'
                                 : m_inBuf[m_lineChecked - 1];
    m_inBuf[m_lineChecked++] = '\0';
    return LineCode::LINE_OK;
}

CheckState HttpConn::parseReqLine()
{
    //Method
    char* sep = strchr(m_inBuf + m_lineStart, ' ');
    if(sep == nullptr)
        return CHECK_ERROR;
    
    *sep = '\0';
    if(strcasecmp(m_inBuf + m_lineStart, "GET") == 0)
        m_method = Method::GET;
    if(strcasecmp(m_inBuf + m_lineStart, "POST") == 0)
        m_method = Method::POST;

    if(m_method == Method::NONE)
    {
        std::cout << "bad req: " << m_inBuf + m_lineStart << std::endl;
        return CHECK_ERROR;
    }
    m_lineStart += sep - (m_inBuf + m_lineStart) + 1;

    //Url
    sep = strchr(m_inBuf + m_lineStart, ' ');
    if(sep == nullptr)
        return CHECK_ERROR;

    *sep = '\0';
    m_url = m_inBuf + m_lineStart;
    m_lineStart += sep - (m_inBuf + m_lineStart) + 1;

    //Ver
    m_httpVer = m_inBuf + m_lineStart;
    m_lineStart = m_lineChecked;

    std::cout << "parse reqLine ok!" << std::endl;
    std::cout << m_method << " " << m_url << " " << m_httpVer << std::endl;
    
    return CHECK_HEADERS;
}

CheckState HttpConn::parseHeaders()
{
    // char* sep = strstr(m_inBuf + m_lineStart, "\r\n");  //getline会将\r\n置0；后续需更改
    // int offset = sep - (m_inBuf + m_lineStart) + 2;
    if( m_inBuf[m_lineChecked - 3] == '\0')
    {
        m_lineStart += 2;
        return m_conLen ? CheckState::CHECK_CONTENT : CheckState::CHECK_END;
    }

    char* sep = strchr(m_inBuf + m_lineStart, ':');
    if(sep == nullptr)
        return CHECK_ERROR;
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
    m_lineStart = m_lineChecked;
    return CHECK_HEADERS;
}

CheckState HttpConn::parseContent()
{
    m_content = {m_inBuf + m_lineStart, m_conLen};
    std::cout << "parse Content ok: " << m_content << std::endl;
    return CHECK_END;
}

void HttpConn::doRequset()
{
    //注册
    if(m_url[1] == '0')
    {
        m_rscPath += RscRoot;
        m_rscPath += "register.html";
    }
    else if(m_url[1] == '1')
    {
        m_rscPath += RscRoot;
        m_rscPath += "log.html";
    }
    // if(m_url[1] == '0')
    // {
    //     m_rscPath += RscRoot;
    //     m_rscPath += "register.html";
    // }
    // if(m_url[1] == '0')
    // {
    //     m_rscPath += RscRoot;
    //     m_rscPath += "register.html";
    // }
    else
    {
        m_rscPath += RscRoot;
        m_rscPath += "judge.html";
    }
}

void HttpConn::processWrite()
{
    int fd = open(m_rscPath.c_str(), O_RDONLY);
    if(fd == -1)
        perror("open");

    if(stat(m_rscPath.c_str(), &m_fileStat) == -1)
        perror("stat");

    std::string rsp("HTTP/1.1 200 OK\r\nContent-Length: ");//11\r\n\r\n");
    rsp += m_fileStat.st_size;
    rsp += "\r\n\r\n";
    memcpy(m_rspHead, rsp.c_str(), rsp.size());
    m_outbuf[0].iov_base = m_rspHead;
    m_outbuf[0].iov_len = rsp.size();


    void* map = mmap(NULL, m_fileStat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(map == MAP_FAILED)
        perror("mmap");
    close(fd);
    m_outbuf[1].iov_base = map;
    m_outbuf[1].iov_len = m_fileStat.st_size;

    m_bytesToSend = m_outbuf[0].iov_len + m_outbuf[1].iov_len;

    epoll_event event;
    event.data.fd = m_sock.getSockFd();
    event.events = EPOLLOUT | EPOLLONESHOT;
    epoll_ctl(m_epollfd, EPOLL_CTL_MOD, event.data.fd, &event);
}

void HttpConn::process()
{
    HttpCode ret = processRead();
    if(ret == NO_REQ)
        return;
    doRequset();
    resetInBuf();
    processWrite();
}

HttpCode HttpConn::processRead()
{
    while( (getLine() == LINE_OK) 
            || (m_conLen > 0))
    {
        switch (m_checkState)
        {
            case CHECK_REQ_LINE:
                m_checkState = parseReqLine();
                break;
            case CHECK_HEADERS:
                m_checkState = parseHeaders();
                break;
            case CHECK_CONTENT:
                m_checkState = parseContent();
                break;
            //POST请求
            case CHECK_END:
                return GET_REQ;
            case CHECK_ERROR:
                return BAD_REQ;
        }
    }
    
    //GET请求
    return GET_REQ;
}

void HttpConn::resetInBuf()
{
    m_checkState = CHECK_REQ_LINE;
    m_lineStart = 0;
    m_lineChecked = 0;
    m_method = Method::NONE;
    m_url.clear();
    m_httpVer.clear();
    m_host.clear();
    m_conLen = 0;
    m_content.clear();
}

void HttpConn::resetOutBuf()
{
    m_rscPath.clear();
    memset(&m_fileStat, 0, sizeof(struct stat));
    memset(m_rspHead, 0, BUF_SIZE);
    memset(m_outbuf, 0, sizeof(struct iovec));
    memset(m_outbuf + 1, 0, sizeof(struct iovec));
}
