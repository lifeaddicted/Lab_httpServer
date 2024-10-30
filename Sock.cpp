#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include "Sock.h"
#include <iostream>

Sock::Sock()
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_sockfd == -1)
        throw "socket() fail";
}

bool Sock::Bind(uint16_t port, std::string ip)
{
    if(!setReuseAddr())
        std::cout << "setReuseAddr failed in Bind()" << std::endl;
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton(ip.c_str(), &addr.sin_addr);
    if(bind(m_sockfd, (sockaddr*)&addr, sizeof(addr)))
        return false;
    return true;
}

bool Sock::Listen(int backlog)
{
    if(listen(m_sockfd, backlog) == -1)
        return false;
    return true;
}

int Sock::Accpect()
{
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int fd = accept(m_sockfd, (sockaddr*)&addr, &len);
    return fd;
}

int Sock::Recv(char buf[], int len)
{
    int ret = recv(m_sockfd, buf, len, 0);
    return ret;
}

int Sock::Send(const char buf[], int len)
{
    int ret = send(m_sockfd, buf, len, 0);
    return ret;
}

int Sock::Send(const struct iovec* vec, int num)
{
    int ret = writev(m_sockfd, vec, num);
    return ret;
}

bool Sock::setNonblock()
{
    int old = fcntl(m_sockfd, F_GETFL);
    if(fcntl(m_sockfd, F_SETFL, old | O_NONBLOCK))
        return false;
    return true;
}

bool Sock::setReuseAddr()
{
    int val{1};
    if(setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)))
        return false;
    return true;
}

void Sock::Close()
{
    if(m_sockfd == -1)
        return;
    close(m_sockfd);
    m_sockfd = -1;
}