#ifndef PORTSCANSENDING_H
#define PORTSCANSENDING_H
#include <string>
#include "udpbase.h"
#include "log.h"

using namespace udpp;
class PortScanSending;

class PssPair
{
public:
    PortScanSending *ptr;
    int i;
};

class PortScanSending
{
public:
    PortScanSending(std::string ip, int f, int t, int selfPort);
    void trySend(std::string ip, int port)
    {
        udp->send(ip, port, "hi", 2);
    }
    inline std::string getAddr() { return addr; }
private:
    UdpBase *udp;
    int from;
    int to;
    std::string addr;
};

#endif // PORTSCANSENDING_H
