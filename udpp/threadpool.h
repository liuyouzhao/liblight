#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <map>
#include <pthread.h>
#include "log.h"

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

    void* (*thread_job_hook)(void*, ThreadPool*);
    void* param;

    ThreadPool *poolRef;
    unsigned long long threadId;

    inline void terminate()
    {
        this->started = 0;
        this->notify();
        pthread_join(thread, NULL);
    }
};

class ThreadWaitQueue
{
public:
    ThreadWaitQueue(void* (*thread_job_hook)(void* p, ThreadPool*), void *param)
    {
        this->thread_job_hook = thread_job_hook;
        this->param = param;
    }
    void* (*thread_job_hook)(void* p, ThreadPool*);
    void* param;
};

class ThreadPool
{
friend class ThreadObject;
public:
    ThreadPool(int maxThreadsNumber, int waitQueueSize);
    ~ThreadPool();

    THREAD_SAFE void run(void* (*thread_job_hook)(void*, ThreadPool*), void *param);
    THREAD_SAFE void dump();

    /**
     * @brief waitPool
     * Wait until busy threads turn to idle
     */
    THREAD_SAFE void waitPool();
    THREAD_SAFE void consumeQueue();

    pthread_cond_t intrinsicCond;
    pthread_mutex_t intrinsicMutex;
    inline bool ialive() {  return intrinsicAlive;  }
    inline void lockUser() {    pthread_mutex_lock(&userMutex); }
    inline void unlockUser() {    pthread_mutex_unlock(&userMutex); }
protected:
    THREAD_SAFE int idleThread(ThreadObject *threadObject);
    inline void lockSystem(const char *position = NULL, int line = -1)
    {
#if DEBUG
        if(position != NULL)
            Log::d("lock[sys] %s:%d", position, line);
        else
            Log::d("lock[sys]");
#endif
        pthread_mutex_lock(&systemMutex);
    }

    inline void unlockSystem(const char *position = NULL, int line = -1)
    {
#if DEBUG
        if(position != NULL)
            Log::d("unlock[sys] %s:%d", position, line);
        else
            Log::d("unlock[sys]");
#endif
        pthread_mutex_unlock(&systemMutex);
    }

    std::list<ThreadObject*> mPoolIdle;
    std::list<ThreadObject*> mPoolBusy;
    std::list<ThreadWaitQueue> mQueue;
    int mWaitQueueSize;

    pthread_t intrinsicThread;
    bool intrinsicAlive;

    pthread_mutex_t systemMutex;
    pthread_mutex_t userMutex;
};

} // namespace udpp

#endif // THREADPOOL_H
