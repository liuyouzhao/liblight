#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "udpbase.h"

namespace udpp
{
class UdpClient : public UdpBase
{
public:
    virtual int init(int port = -1);
    virtual int deinit();
};

}

#endif // UDPCLIENT_H
