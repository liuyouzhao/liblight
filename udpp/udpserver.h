#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "udpbase.h"

namespace udpp
{

class UdpServer : public UdpBase
{
public:
    UdpServer() {};
    UdpServer(int port);
    virtual int init(int port = -1);
    virtual int deinit();
};

} // namespace udpp

#endif // UDPSERVER_H
