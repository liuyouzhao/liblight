#include <iostream>
#include <unistd.h>
#include "log.h"
#include "threadpool.h"
#include "udpterminal.h"


using namespace std;

void *func(void *param)
{
    int index = *(int*)(param);
    Log::i("I am %d", index);
    usleep(1000 * 5000);
}

int main(int argc, char **argv)
{
#if 0
    udpp::ThreadPool threadPool(200, 500);
    int i = 0;
    while(1)
    {
        threadPool.run(func, &i);
        i ++;
        usleep(1000 * 10);
    }
#endif

    /// punching try
    udpp::UdpTerminal udpTerminal;
    udpTerminal.loopRun(argv + 1, argc - 1);

    return 0;
}
