#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

namespace udpp
{

class UdpServer
{
public:
    UdpServer(int port);
    int recv(void* (*recv_hook)(UdpServer*, char*, int, char*, int), void* (*error_hook)(int));
    int send(std::string clientAddress, int port, const char *buffer, int len);
    inline char *getRecvCache() {  return mRecvCache;  }
private:
    int mSocketfd;
    int mSocketLen;
    char mRecvCache[BUFSIZ];
};

} // namespace udpp

#endif // UDPSERVER_H
