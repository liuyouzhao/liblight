#include "p2pswitcher.h"
#include "log.h"


static void *on_recv(UdpBase *server, char *ip, int port, char *msg, int len, void *param)
{
    P2pSwitcher *ptrThis = (P2pSwitcher*)param;
    ptrThis->swapPeerAddress(std::string(ip), port);
}

static void *tp_run_recv(void *param)
{
    P2pSwitcher *ptrThis = (P2pSwitcher*)param;
    while(1)
    {
        ptrThis->handlePeerMessage();
    }
}

P2pSwitcher::P2pSwitcher(int port):
    udpServer(port)
{
    updateIndex = 0;
    peerPortPair[0] = peerPortPair[1] = -1;
}

void P2pSwitcher::run()
{
    tp_run_recv(this);
}

void P2pSwitcher::handlePeerMessage()
{
    this->udpServer.recv(on_recv, NULL, this);
}

void P2pSwitcher::swapPeerAddress(std::string reqIp, int reqPort)
{
    static int swap[2] = {1, 0};
    int index = ipIndex(reqIp);

    if(index < 0)
    {
        updatePeerPair(reqIp, reqPort);
        return;
    }

    int indexToNotice = swap[index];
    std::string peerIp = peerAddrPair[indexToNotice];
    int peerPort = peerPortPair[indexToNotice];

    if(peerPort < 0)
    {
        return;
    }

    std::stringstream ss;
    ss << peerIp.c_str() << ":" << peerPort;
    std::string content = std::string(ss.str());

    tryNotice(reqIp, reqPort, content);
}
