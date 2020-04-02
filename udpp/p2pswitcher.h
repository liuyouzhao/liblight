#ifndef P2PSWITCHER_H
#define P2PSWITCHER_H

#include "udpserver.h"
#include "threadpool.h"

using namespace udpp;

class P2pSwitcher
{
public:
    P2pSwitcher(int port);
    void run();
    void handlePeerMessage();
    void swapPeerAddress(std::string reqIp, int reqPort);
private:
    inline int ipIndex(std::string ip)
    {
        for(int i = 0; i < 2; i ++)
        {
            if(peerAddrPair[i] == ip)
                return i;
        }
        return -1;
    }

    inline void tryNotice(std::string ip, int port, std::string content)
    {
        if(!ip.empty())
        {
            this->udpServer.send(ip, port, content.c_str(), content.length());
        }
    }

    inline void updatePeerPair(std::string ip, int port)
    {
        peerAddrPair[updateIndex] = ip;
        peerPortPair[updateIndex] = port;
        updateIndex = (++updateIndex) % 2;
    }

    std::string peerAddrPair[2];
    int peerPortPair[2];
    int updateIndex;

    UdpServer udpServer;
};

#endif // P2PSWITCHER_H
