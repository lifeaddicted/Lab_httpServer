#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#include <sys/epoll.h>
#include <arpa/inet.h>
#include <map>
#include <sys/socket.h>
#include "http_conn.h"
#include "Sock.h"
#include "LoggerMgr.h"
#include "ThreadPool.h"

class WebServer{
    public:
        WebServer(): m_listenSock() {}
        ~WebServer();

        void eventListen();
        void eventLoop();

        void initThreadPool(int num);

        inline void newConnetion(int sock);

    public:
        const static int MAX_EVENT_NUM = 10000;
        
    private:
        Sock m_listenSock;
        int m_epollfd{-1};
        epoll_event m_events[MAX_EVENT_NUM];

        std::map<int, ConnPtr> m_mapConn;

        ThreadPool<ConnPtr>* m_threadPool;
};

#endif