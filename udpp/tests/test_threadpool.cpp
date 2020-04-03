#include "test_base.h"
#include "threadpool.h"
#include "log.h"
#include <unistd.h>

using namespace udpp;


void *tp_hook(void *param, ThreadPool* tp)
{
    tp->lockUser();
    (*((int*)param)) ++;
    tp->unlockUser();

    Log::i("done %d", (*((int*)param)));
}

int main()
{
    ThreadPool threadpool(1000, 70000);

    int sum = 0;
    for(int i = 0; i < 65535; i ++)
    {
        threadpool.run(tp_hook, &sum);
    }

    threadpool.waitPool();
    ASSERT_EQUAL(sum, 65535);

    Log::ok("test threadpool Success");
    return 0;
}
