#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <map>
#include <pthread.h>

namespace udpp
{
#define THREAD_SAFE
#define MAX_THREAD_NUMBER 4096
class ThreadPool;

class ThreadObject
{
public:
    ThreadObject(ThreadPool *ref);
    THREAD_SAFE ~ThreadObject();

    void run();
    void done();
    void notify();

    pthread_mutex_t __mutex;
    pthread_cond_t __cond;
    pthread_t thread;
    int alive;
    int started;

    void* (*thread_job_hook)(void* p);
    void* param;

    ThreadPool *poolRef;
};

class ThreadWaitQueue
{
public:
    ThreadWaitQueue(void* (*thread_job_hook)(void* p), void *param)
    {
        this->thread_job_hook = thread_job_hook;
        this->param = param;
    }
    void* (*thread_job_hook)(void* p);
    void* param;
};

class ThreadPool
{
friend class ThreadObject;
public:
    ThreadPool(int maxThreadsNumber, int waitQueueSize);
    ~ThreadPool();

    THREAD_SAFE void run(void* (*thread_job_hook)(void* p), void *param);
    THREAD_SAFE void dump();
protected:
    THREAD_SAFE int idleThread(ThreadObject *threadObject);
    THREAD_SAFE void consumeQueue();

    std::list<ThreadObject*> mPoolIdle;
    std::list<ThreadObject*> mPoolBusy;
    std::list<ThreadWaitQueue> mQueue;
    int mWaitQueueSize;

    pthread_mutex_t mutex;
};

} // namespace udpp

#endif // THREADPOOL_H
