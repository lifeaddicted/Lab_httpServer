#include "Locker.h"

Mutex::Mutex()
{
    if(pthread_mutex_init(&m_mutex, NULL))
        throw "mutex error";
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&m_mutex);
}

void Mutex::Lock()
{
    pthread_mutex_lock(&m_mutex);
}

void Mutex::Unlock()
{
    pthread_mutex_unlock(&m_mutex);
}

Semaphore::Semaphore(int val)
{
    sem_init(&m_sem, 0, val);
}

Semaphore::~Semaphore()
{
    sem_destroy(&m_sem);
}

void Semaphore::reInit(int val)
{
    sem_destroy(&m_sem);
    sem_init(&m_sem, 0, val);
}

void Semaphore::Post()
{
    sem_post(&m_sem);
}

void Semaphore::Wait()
{
    sem_wait(&m_sem);
}