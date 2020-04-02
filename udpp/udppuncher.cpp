#include "udppuncher.h"
#include "log.h"
#include <unistd.h>

namespace udpp
{

static void *on_recv(UdpBase *server, char *ip, int port, char *msg, int len, void *param)
{
    UdpPuncher *ptrThis = (UdpPuncher*)param;

    // update peer address
    if(ptrThis->isSwitchServer(ip, port))
    {
        std::string content = std::string(msg);
        int colonIndex = content.find(":");
        std::string ipStr = content.substr(0, colonIndex - 1);
        std::string portStr = content.substr(colonIndex + 1);
        int peerPortUpdate = atoi(portStr.c_str());
        ptrThis->updatePeer(ipStr, peerPortUpdate);
    }
    else if(ptrThis->isPeer(ip, port))
    {
        Log::d("PEER RECV: %s", msg);
    }
}

static void *tp_run_recv(void *param)
{
    UdpPuncher *ptrThis = (UdpPuncher*)param;
    while(1)
    {
        ptrThis->recv();
    }
}

UdpPuncher::UdpPuncher(std::string name, int selfPort, std::string switchServerAddress, int switchServerPort)
{
    udp = new UdpServer(selfPort);
    this->name = name;
    this->switchServerAddress = switchServerAddress;
    this->switchServerPort = switchServerPort;

    this->peerAddress = std::string("");
    this->peerPort = -1;
}

void UdpPuncher::run(ThreadPool *threadPool)
{
    threadPool->run(tp_run_recv, this);
    while(1)
    {
        this->udp->send(this->switchServerAddress,
                             this->switchServerPort,
                             this->name.c_str(),
                             this->name.length());
        if(this->peerPort > 0)
        {
            this->udp->send(peerAddress, peerPort, name.c_str(), name.length());
        }
        sleep(1);
    }
}


void UdpPuncher::recv()
{
    this->udp->recv(on_recv, NULL, this);
}

}
