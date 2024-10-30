#ifndef __SOCK_H__
#define __SOCK_H__

#include <string>
#include <arpa/inet.h>
#include <sys/uio.h>

/**
 * @brief SOCK_STREAM
 */
class Sock
{
    public:
        Sock();
        Sock(int fd): m_sockfd(fd) {}

    //active
    public:
        bool Bind(uint16_t port, std::string ip = "");
        bool Listen(int backlog = 5);
        int Accpect();
        void Close();

    //IO
    public:
        int Recv(char buf[], int len);
        int Send(const char buf[], int len);
        int Send(const struct iovec*, int num);

    //opt
    public:
        bool setReuseAddr();
        bool setNonblock();


        int getSockFd() { return m_sockfd; }

    private:
        int m_sockfd;
};

#endif