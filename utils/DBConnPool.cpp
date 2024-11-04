#include "DBConnPool.h"
#include <iostream>

bool DBConnPool::Init(std::string host, std::string user, std::string passwd, std::string db, int port, int num) {
    m_host = host;
    m_user = user;
    m_passwd = passwd;
    m_db = db;
    m_port = port;
    m_maxConn = num;

    for(int i = 0; i < m_maxConn; i++) {
        MYSQL* conn = mysql_init(NULL);
        if(conn == NULL)
        {
            std::cout << "mysql connection init failed!" << std::endl;
            return false;
        }
        conn = mysql_real_connect(conn, m_host.c_str(), m_user.c_str(), m_passwd.c_str(), m_db.c_str(), m_port, NULL, 0);
        if(conn == NULL)
        {
            std::cout << "mysql connect to " << m_user << "@" << m_host << ":" << m_port << " failed!" << std::endl;
            return false;
        }

        m_connPool.push_back(conn);
        std::cout << "mysql connection established!" << std::endl;
    }
    m_poolSem.reInit(m_maxConn);

    return true;
}

void DBConnPool::Destroy()
{
    m_poolMtx.Lock();
    for(auto it: m_connPool) {
        mysql_close(it);
    }
    m_connPool.clear();
    m_poolMtx.Unlock();
}

MYSQL* DBConnPool::getConn()
{
    m_poolSem.Wait();
    m_poolMtx.Lock();
    MYSQL* conn = m_connPool.front();
    m_connPool.pop_front();
    m_poolMtx.Unlock();
    return conn;
}

void DBConnPool::releaseConn(MYSQL* conn)
{
    m_poolMtx.Lock();
    m_connPool.push_back(conn);
    m_poolSem.Post();
    m_poolMtx.Unlock();
}

DBConn::DBConn()
{
    m_conn = DB().getConn();
}

DBConn::~DBConn()
{
    DB().releaseConn(m_conn);
}