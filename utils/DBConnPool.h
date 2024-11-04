#ifndef __DBCONNPOOL_H__
#define __DBCONNPOOL_H__

#include <string>
#include <list>
#include <mysql/mysql.h>
#include "Locker.h"

class DBConnPool {
    public:
        static DBConnPool& Instance() {
            static DBConnPool pool;
            return pool;
        }

        bool Init(std::string host, std::string user, std::string passwd, std::string db, int port, int num = 8);
        void Destroy();

        MYSQL* getConn();
        void releaseConn(MYSQL*);

        int getFree() const { return m_connPool.size(); }

    protected:
        DBConnPool() {}
        ~DBConnPool() { Destroy(); }

    private:
        int m_maxConn{0};
        std::list<MYSQL*> m_connPool;
        Mutex m_poolMtx;
        Semaphore m_poolSem;

    private:
        std::string m_host{"127.0.0.1"};
        std::string m_user{"root"};
        std::string m_passwd{"123456"};
        std::string m_db{"test"};
        int m_port{3306};
};

class DBConn {
    public:
        DBConn();
        ~DBConn();

        MYSQL* getConn() const { return m_conn; }

    private:
        MYSQL* m_conn;
};

#define DB() DBConnPool::Instance()

#endif