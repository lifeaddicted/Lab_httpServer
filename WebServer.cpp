#include "WebServer.h"
#include <assert.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

void WebServer::eventListen()
{
    if(!m_listenSock.Bind(8080))
        throw "bind failed!";

    if(!m_listenSock.Listen(16))
        throw "listen failed!";

    m_epollfd = epoll_create(1);
    assert(m_epollfd != -1);

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = m_listenSock.getSockFd();
    if(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenSock.getSockFd(), &event) == -1)
        std::cout << "add listen event failed" << std::endl;
}

void WebServer::eventLoop()
{
    bool running{true};
    while(running)
    {
        int num = epoll_wait(m_epollfd, m_events, MAX_EVENT_NUM, -1);
        for(int i = 0; i < num; ++i)
        {
            //新连接
            if(m_events[i].data.fd == m_listenSock.getSockFd())
            {
                int clientfd = m_listenSock.Accpect();
                if(clientfd == -1)
                    continue;
                newConnetion(clientfd);
                continue;
            }
            if(m_events[i].events & EPOLLIN)
            {
                auto it = m_mapConn.find(m_events[i].data.fd);
                if(it == m_mapConn.end())
                    ;
                int recvbytes = it->second.handleInput();
                if(recvbytes == 0)
                {
                    std::cout << "conn closed by peer" << std::endl;
                    m_mapConn.erase(m_events[i].data.fd);
                    close(m_events[i].data.fd);
                    continue;
                }
                it->second.getLine();
            }
        }
    }
}

void WebServer::newConnetion(int sock)
{
    std::cout << "new connection" << std::endl;
    HttpConn conn(sock);
    m_mapConn.insert({sock, conn});

    epoll_event event;
    event.data.fd = sock;
    event.events = EPOLLIN | EPOLLOUT;
    int ret = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sock, &event);
    std::cout << "add ret: " << ret << std::endl;
}