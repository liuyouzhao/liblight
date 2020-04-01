#include "threadpool.h"
#include <pthread.h>
#include "log.h"

namespace udpp
{

void *thread_func(void *param)
{
    ThreadObject *threadObject = (ThreadObject*)param;

    while(threadObject->started)
    {
        pthread_mutex_lock(&(threadObject->__mutex));
        while(!threadObject->alive)
        {
            pthread_cond_wait(&(threadObject->__cond), &(threadObject->__mutex));
        }
        pthread_mutex_unlock(&(threadObject->__mutex));

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
    poolRef->idleThread(this);
    alive = 0;
    thread_job_hook = NULL;
}

void ThreadObject::run()
{
    if(thread_job_hook != NULL)
        thread_job_hook(param);
}

void ThreadObject::notify()
{
    alive = 1;
    pthread_mutex_unlock(&__mutex);
    pthread_cond_signal(&__cond);
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
    pthread_mutex_init(&mutex, &attr);


    mWaitQueueSize = waitQueueSize;
    for(int i = 0; i < maxThreadsNumber; i ++)
    {
        ThreadObject* threadObject = new ThreadObject(this);
        mPoolIdle.push_back(threadObject);
    }
}

ThreadPool::~ThreadPool()
{
    pthread_mutex_lock(&mutex);
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
        threadObject->started = 0;
        threadObject->notify();
        pthread_join(threadObject->thread, NULL);
        delete threadObject;
    }
    pthread_mutex_unlock(&mutex);
}

void ThreadPool::consumeQueue()
{
    pthread_mutex_lock(&mutex);
    while(mQueue.size() > 0 && mPoolIdle.size() > 0)
    {
        ThreadWaitQueue threadWaitQueue = mQueue.front();
        mQueue.pop_front();
        run(threadWaitQueue.thread_job_hook, threadWaitQueue.param);
    }
    pthread_mutex_unlock(&mutex);
}

void ThreadPool::run(void* (*thread_job_hook)(void* p), void *param)
{
    consumeQueue();

    pthread_mutex_lock(&mutex);

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

    pthread_mutex_unlock(&mutex);
}

int ThreadPool::idleThread(ThreadObject *threadObject)
{
    pthread_mutex_lock(&mutex);

    mPoolBusy.remove(threadObject);
    mPoolIdle.push_back(threadObject);
    pthread_mutex_unlock(&mutex);
}

void ThreadPool::dump()
{
    pthread_mutex_lock(&mutex);
    Log::d("TP - B[%d] I[%d] W[%d]", mPoolBusy.size(), mPoolIdle.size(), mQueue.size());
    pthread_mutex_unlock(&mutex);
}

} // namespace udpp
