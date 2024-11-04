#include "WebServer.h"
#include "DBConnPool.h"

INITIALIZE_EASYLOGGINGPP

int main()
{
    WebServer server;

    //启动监听
    server.eventListen();
    LoggerMgr::init();
    NW_DBG() << "listening..." << 80;

    server.initThreadPool(4);
    DB().Init("127.0.0.1", "root", "123456", "test", 3306);

    //事件循环
    server.eventLoop();

    return 0;
}