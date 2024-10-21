#include "WebServer.h"

int main()
{
    WebServer server;

    //启动监听
    server.eventListen();

    //事件循环
    server.eventLoop();

    return 0;
}