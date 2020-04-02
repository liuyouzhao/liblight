#ifndef UDPTERMINAL_H
#define UDPTERMINAL_H

#include <vector>
#include <string>
#include "threadpool.h"

namespace udpp
{

#define CACHE_OVERFLOW 1024
class TempUdpClientAddress
{
public:
    TempUdpClientAddress(std::string ip, int pt)
    {
        this->ip = ip;
        port = pt;
    }
    std::string ip;
    int port;
};

class UdpTerminal
{
public:
    UdpTerminal();
    void loopRun(char **arr, int len);
private:
    void interactiveRun();
    ThreadPool threadPool;
};


}


#endif // UDPTERMINAL_H
