#ifndef __HTTP_CONN_H__
#define __HTTP_CONN_H__

#include <arpa/inet.h>
#include "Sock.h"
#include "Handler.h"
#include <string>
#include <sys/uio.h>
#include <sys/stat.h>
#include <memory>

enum Method{
    NONE,
    GET,
    POST
};

enum HttpCode{
    NO_REQ,
    GET_REQ,
    BAD_REQ,
    INTERNAL_ERROR
};

enum CheckState{
    CHECK_REQ_LINE,
    CHECK_HEADERS,
    CHECK_CONTENT,
    CHECK_ERROR,
    CHECK_END
};

enum LineCode{
    LINE_OPEN,
    LINE_OK,
    LINE_BAD
};

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

    //报文解析
    public:
        LineCode getLine();
        CheckState parseReqLine();
        CheckState parseHeaders();
        CheckState parseContent();

        void doRequset();

        void sendRsp();

        void process();
        HttpCode processRead();
        void processWrite();

        void resetInBuf();

    public:
        static int m_clientCnt;
        const static int m_maxClientCnt = 10000;
    
    private:
        Sock m_sock;

        char m_inBuf[BUF_SIZE];
        int m_inOff{0};

        int m_lineStart{0};
        int m_lineChecked{0};
        CheckState m_checkState{CHECK_REQ_LINE};
        Method m_method{Method::NONE};
        std::string m_url;
        std::string m_httpVer;

        std::string m_host;
        int m_conLen{0};
        std::string m_content;

        std::string m_rscPath;
        struct stat m_fileStat;
        char m_rspHead[BUF_SIZE];
        struct iovec m_outbuf[2];
};

typedef std::shared_ptr<HttpConn> ConnPtr;

#endif