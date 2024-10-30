#ifndef __LOCKER_H__
#define __LOCKER_H__

#include <pthread.h>
#include <semaphore.h>

class Mutex
{
    public:
        Mutex();
        ~Mutex();

        void Lock();
        void Unlock();

    private:
        pthread_mutex_t m_mutex;
};

class Semaphore
{
    public:
        Semaphore(int val = 0);
        ~Semaphore();

        void Post();
        void Wait();

    private:
        sem_t m_sem;
};

#endif