#include "portscansending.h"
#include <string>
#include <unistd.h>
#include "udpserver.h"
#include "threadpool.h"
#include "log.h"

using namespace udpp;

void *scanScand(void *param)
{
    PssPair *pp = (PssPair*)param;
    pp->ptr->trySend(pp->ptr->getAddr(), pp->i);

    delete param;
}

static void *on_recv(UdpBase *server, char *ip, int port, char *msg, int len)
{
    Log::d("RECV: %s", msg);
}

PortScanSending::PortScanSending(std::string ip, int f, int t, int selfPort)
{
    this->udp = new UdpServer(selfPort);
    this->addr = ip;
    this->from = f;
    this->to = t;

    //ThreadPool threadPool(500, 50000);

    for(int i = f; i < t; i ++)
    {
        PssPair *pp = new PssPair();
        pp->ptr = this;
        pp->i = i;
        Log::i("scan %s:%d", ip.c_str(), i);
        for(int j = 0; j < 100; j ++)
        {
            this->trySend(ip, i);
        }

        usleep(1000 * 1000);
        //threadPool.run(scanScand, pp);
    }

    while(1)
    {
        udp->recv(on_recv, NULL);
    }

}
