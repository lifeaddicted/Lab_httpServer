#include "WebServer.h"
#include "http_conn.h"
#include <assert.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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
                //连接对象
            }
        }
    }
}