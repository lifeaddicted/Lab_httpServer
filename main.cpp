#include "WebServer.h"

INITIALIZE_EASYLOGGINGPP

int main()
{
    WebServer server;

    //启动监听
    server.eventListen();
    LoggerMgr::init();
    NW_DBG() << "listening..." << 80;

    //事件循环
    server.eventLoop();

    return 0;
}