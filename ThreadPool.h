#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include "Locker.h"
#include <list>

template<typename T>
class ThreadPool
{
    public:
        ThreadPool(int num);
        ~ThreadPool();

        bool append(T);

    protected:
        static void* worker(void* arg);
        void run();

    private:
        int m_threadNum;
        pthread_t* m_idVec;

        int m_maxJob{10000};
        std::list<T> m_jobQue;
        Mutex m_queMtx;
        Semaphore m_queSem;
};

template<typename T>
ThreadPool<T>::ThreadPool(int num)
{
    m_idVec = new pthread_t[num];
    for(int i = 0; i < num; i++)
    {
        pthread_create(m_idVec + i, NULL, worker, this);
    }
}

template<typename T>
ThreadPool<T>::~ThreadPool()
{
    delete [] m_idVec;
}

template<typename T>
void* ThreadPool<T>::worker(void* arg)
{
    ThreadPool* pool = (ThreadPool*)arg;
    pool->run();
    return pool;
}

template<typename T>
void ThreadPool<T>::run()
{
    while(true) {

        //从任务队列中取任务
        m_queSem.Wait();
        m_queMtx.Lock();
        if (m_jobQue.empty())
        {
            m_queMtx.Unlock();
            continue;
        }

        T job = m_jobQue.front();
        m_jobQue.pop_front();
        m_queMtx.Unlock();

        if(!job)
            continue;
        
        //proactor
        job->process();
    }
}

template<typename T>
bool ThreadPool<T>::append(T job)
{
    m_queMtx.Lock();
    if(m_jobQue.size() >= m_maxJob)
    {
        m_queMtx.Unlock();
        //Log
        return false;
    }
    m_jobQue.emplace_back(job);
    m_queMtx.Unlock();
    m_queSem.Post();
    return true;
}

#endif