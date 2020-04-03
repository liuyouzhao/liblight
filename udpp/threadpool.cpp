#include "threadpool.h"
#include <pthread.h>
#include <unistd.h>
#include "log.h"

namespace udpp
{

void *thread_func(void *param)
{
    ThreadObject *threadObject = (ThreadObject*)param;
    threadObject->threadId = pthread_self();
    while(threadObject->started)
    {
        pthread_mutex_lock(&(threadObject->__mutex));
        while(!threadObject->alive)
        {
            pthread_cond_wait(&(threadObject->__cond), &(threadObject->__mutex));
        }
        pthread_mutex_unlock(&(threadObject->__mutex));

        if(!threadObject->started)
        {
#if DEBUG
            Log::d("threadObject is FINISHED expectedly %d %d", threadObject->alive, threadObject->started);
#endif
            return NULL;
        }

        threadObject->run();
        threadObject->done();
    }
    Log::w("threadObject is FINISHED unexpectedly %d %d", threadObject->alive, threadObject->started);
    return NULL;
}

ThreadObject::ThreadObject(ThreadPool *ref)
{
    alive = 0;
    started = 1;
    poolRef = ref;
    thread_job_hook = NULL;
    param = NULL;
    pthread_mutex_init(&__mutex, NULL);
    pthread_cond_init(&__cond, NULL);
    pthread_create(&thread, NULL, thread_func, this);
}

ThreadObject::~ThreadObject()
{
    alive = 0;
    started = 0;
    pthread_join(thread, NULL);
    pthread_mutex_destroy(&__mutex);
}

void ThreadObject::done()
{
    pthread_mutex_lock(&__mutex);
    alive = 0;
    thread_job_hook = NULL;
    pthread_mutex_unlock(&__mutex);

    poolRef->idleThread(this);
}

void ThreadObject::run()
{
    if(thread_job_hook != NULL)
        thread_job_hook(param, this->poolRef);
}

void ThreadObject::notify()
{
    pthread_mutex_lock(&__mutex);
    alive = 1;
    pthread_cond_signal(&__cond);
    pthread_mutex_unlock(&__mutex);
}

void *threadpool_intrinsic_thread_func(void *param)
{
    ThreadPool *thiz = (ThreadPool*) param;
    while(1)
    {
        pthread_mutex_lock(&(thiz->intrinsicMutex));
        while(!thiz->ialive())
        {
            pthread_cond_wait(&(thiz->intrinsicCond), &(thiz->intrinsicMutex));
        }
        pthread_mutex_unlock(&(thiz->intrinsicMutex));
        thiz->consumeQueue();
    }
}

ThreadPool::ThreadPool(int maxThreadsNumber, int waitQueueSize)
{
    if(MAX_THREAD_NUMBER < maxThreadsNumber)
    {
        Log::e("Number of threads applied too big as %d, the max number is %d", maxThreadsNumber, MAX_THREAD_NUMBER);
        return;
    }
    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&systemMutex, &attr);
    pthread_mutex_init(&userMutex, &attr);

    mWaitQueueSize = waitQueueSize;
    for(int i = 0; i < maxThreadsNumber; i ++)
    {
        ThreadObject* threadObject = new ThreadObject(this);
        mPoolIdle.push_back(threadObject);
    }

    pthread_mutex_init(&intrinsicMutex, NULL);
    pthread_cond_init(&intrinsicCond, NULL);
    pthread_create(&intrinsicThread, NULL, threadpool_intrinsic_thread_func, this);
}

ThreadPool::~ThreadPool()
{
    lockSystem(__FUNCTION__, __LINE__);
    for(std::list<ThreadObject*>::iterator iter = mPoolBusy.begin(); iter != mPoolBusy.end(); iter ++)
    {
        ThreadObject* threadObject = (ThreadObject*)*iter;
        threadObject->started = 0;
        pthread_join(threadObject->thread, NULL);
        delete threadObject;
    }
    for(std::list<ThreadObject*>::iterator iter = mPoolIdle.begin(); iter != mPoolIdle.end(); iter ++)
    {
        ThreadObject* threadObject = (ThreadObject*)*iter;
        threadObject->terminate();
        delete threadObject;
    }
    unlockSystem(__FUNCTION__, __LINE__);
}

void ThreadPool::consumeQueue()
{
    while(1)
    {
        lockSystem(__FUNCTION__, __LINE__);
        if(mQueue.size() == 0)
        {
            unlockSystem(__FUNCTION__, __LINE__);
            break;
        }
        ThreadWaitQueue threadWaitQueue = mQueue.front();
        mQueue.pop_front();

        unlockSystem(__FUNCTION__, __LINE__);

        run(threadWaitQueue.thread_job_hook, threadWaitQueue.param);
    }

    pthread_mutex_lock(&this->intrinsicMutex);
    this->intrinsicAlive = 0;
    pthread_mutex_unlock(&this->intrinsicMutex);
}

void ThreadPool::run(void* (*thread_job_hook)(void*, ThreadPool*), void *param)
{
    lockSystem(__FUNCTION__, __LINE__);

    if(mPoolIdle.size() == 0)
    {
        if(mQueue.size() < mWaitQueueSize)
        {
            ThreadWaitQueue threadWaitQueue(thread_job_hook, param);
            mQueue.push_back(threadWaitQueue);
        }
    }
    else
    {
        ThreadObject *threadObject = mPoolIdle.back();
        mPoolIdle.pop_back();
        mPoolBusy.push_back(threadObject);

        threadObject->thread_job_hook = thread_job_hook;
        threadObject->param = param;
        threadObject->notify();
    }

    unlockSystem(__FUNCTION__, __LINE__);
}

int ThreadPool::idleThread(ThreadObject *threadObject)
{
    lockSystem(__FUNCTION__, __LINE__);
    mPoolBusy.remove(threadObject);
    mPoolIdle.push_back(threadObject);
    unlockSystem(__FUNCTION__, __LINE__);

    pthread_mutex_lock(&this->intrinsicMutex);
    pthread_cond_signal(&this->intrinsicCond);
    this->intrinsicAlive = 1;
    pthread_mutex_unlock(&this->intrinsicMutex);
}

void ThreadPool::dump()
{
    lockSystem(__FUNCTION__, __LINE__);
    Log::d("TP - B[%d] I[%d] W[%d]", mPoolBusy.size(), mPoolIdle.size(), mQueue.size());
    unlockSystem(__FUNCTION__, __LINE__);
}

void ThreadPool::waitPool()
{
    while(1)
    {
        lockSystem(__FUNCTION__, __LINE__);
        if(mPoolBusy.size() == 0)
        {
            unlockSystem(__FUNCTION__, __LINE__);
            return;
        }
        unlockSystem(__FUNCTION__, __LINE__);

        usleep(1000 * 100);
#if DEBUG
        dump();
#endif
    }
}

} // namespace udpp
