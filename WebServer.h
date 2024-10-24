#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#include <sys/epoll.h>
#include <arpa/inet.h>
#include <map>
#include <sys/socket.h>
#include "http_conn.h"

class WebServer{
    public:
        void eventListen();
        void eventLoop();

        inline void newConnetion(int sock, const sockaddr_in& addr);

    public:
        const static int MAX_EVENT_NUM = 10000;
        
    private:
        int m_listenfd{-1};
        int m_epollfd{-1};
        epoll_event m_events[MAX_EVENT_NUM];

        std::map<int, HttpConn> m_mapConn;
};

#endif