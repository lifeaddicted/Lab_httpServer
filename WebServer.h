#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#include <sys/epoll.h>

class WebServer{
    public:
        void eventListen();
        
    private:
        int m_epollfd{-1};
};

#endif