#include <iostream>
#include <unistd.h>
#include "log.h"
#include "threadpool.h"
#include "udpterminal.h"
#include "p2pswitcher.h"
#include "portscansending.h"
#include "udppuncher.h"
#include <string.h>


using namespace std;
using namespace udpp;

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

    if(!strcmp("scan", argv[1]))
    {
        int selfPort = atoi(argv[2]);
        PortScanSending portScanSending(std::string(argv[3]), atoi(argv[4]), atoi(argv[5]), selfPort);
    }
    else if(!strncmp("s", argv[1], 1))
    {
        int port = atoi(argv[2]);
        P2pSwitcher p2pSwitcher(port);
        p2pSwitcher.run();
    }
    else if(!strncmp("p", argv[1], 1))
    {

        int port = atoi(argv[2]);
        std::string ssIp = std::string(argv[3]);
        int ssPort = atoi(argv[4]);
        std::string name = std::string(argv[5]);
        UdpPuncher udpPuncher(name, port, ssIp, ssPort);
        ThreadPool threadpool(2, 5);
        udpPuncher.run(&threadpool);
    }

    cout << "Usage: udp_punching_test [s/p] [self-port] [swicher-ip] [swicher-port] [name]" << endl;
    /// punching try
    //udpp::UdpTerminal udpTerminal;
    //udpTerminal.loopRun(argv + 1, argc - 1);

    return 0;
}
