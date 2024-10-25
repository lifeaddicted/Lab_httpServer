#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

#include <arpa/inet.h>
#include "Sock.h"
#include "Handler.h"

class HttpConn: public Handler
{
    const static int BUF_SIZE = 2048;

    public:
        HttpConn(int sock): m_sock(sock) {}

        void Close() { m_sock.Close(); }

    //IO处理
    public:
        int handleInput() override;
        int handleOutput() override {};
        void handleClose() override {};


    public:
        static int m_clientCnt;
        const static int m_maxClientCnt = 10000;
    
    private:
        Sock m_sock;

        char m_inBuf[BUF_SIZE];
        int m_inOff{0};
};

#endif