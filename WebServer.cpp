#include "WebServer.h"
#include <assert.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

void WebServer::eventListen()
{
    m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(m_listenfd != -1);
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(m_listenfd, (sockaddr*)&addr, sizeof(addr));

    listen(m_listenfd, 16);

    m_epollfd = epoll_create(1);
    assert(m_epollfd != -1);

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = m_listenfd;
    epoll_ctl(m_epollfd, EPOLL_CTL_ADD, m_listenfd, &event);   
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
            if(m_events[i].data.fd == m_listenfd)
            {
                sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int clientfd = accept(m_listenfd, (sockaddr*)&clientaddr, &len);
                if(clientfd == -1)
                    continue;
                newConnetion(clientfd, clientaddr);
                continue;
            }
            if(m_events[i].events & EPOLLIN)
            {
                char buf[1024];
                int recvbytes = recv(m_events[i].data.fd, buf, 1023, 0);
                if(recvbytes == 0)
                {
                    std::cout << "conn closed by peer" << std::endl;
                    m_mapConn.erase(m_events[i].data.fd);
                    close(m_events[i].data.fd);
                }
            }
        }
    }
}

void WebServer::newConnetion(int sock, const sockaddr_in& addr)
{
    std::cout << "new connection" << std::endl;
    HttpConn conn(sock, addr);
    m_mapConn.insert({sock, conn});

    epoll_event event;
    event.data.fd = sock;
    event.events = EPOLLIN | EPOLLOUT;
    int ret = epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sock, &event);
    std::cout << "add ret: " << ret << std::endl;
}