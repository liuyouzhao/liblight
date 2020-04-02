#ifndef UDPPUNCHER_H
#define UDPPUNCHER_H

#include <string.h>
#include <string>
#include "udpserver.h"
#include "threadpool.h"
#include "log.h"

namespace udpp
{
class UdpPuncher
{
public:
    UdpPuncher(std::string name,
               int selfPort,
               std::string switchServerAddress,
               int switchServerPort);
    void run(ThreadPool *threadPool);
    void recv();
    inline void updatePeer(std::string ip, int port)
    {
        if(peerAddress == ip && peerPort == port)
            return;
        peerAddress = ip;
        peerPort = port;
        Log::d("Update peerAddress to %s:%d", peerAddress.c_str(), peerPort);
    }
    inline bool isSwitchServer(const char *ip, int port)
    {
        return !strcmp(ip, switchServerAddress.c_str()) && port == switchServerPort;
    }

    inline bool isPeer(const char *ip, int port)
    {
        return !strcmp(ip, peerAddress.c_str()) && port == peerPort;
    }
private:
    UdpBase *udp;
    std::string name;
    std::string switchServerAddress;
    std::string peerAddress;
    int switchServerPort;
    int peerPort;
};
}
#endif // UDPPUNCHER_H
