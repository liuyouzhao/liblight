#ifndef UDPBASE_H
#define UDPBASE_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

namespace udpp
{

class UdpBase
{
public:
    virtual int init(int port = -1) = 0;
    virtual int deinit() = 0;
    int recv(void* (*recv_hook)(UdpBase*, char*, int, char*, int), void* (*error_hook)(int));
    int recv(void* (*recv_hook)(UdpBase*, char*, int, char*, int, void*), void* (*error_hook)(int), void *param);
    int send(std::string targetAddress, int port, const char *buffer, int len);
    inline char *getRecvCache() {  return mRecvCache;  }
protected:
    char mRecvCache[BUFSIZ];
    int mSocketfd;
    int mSocketLen;
};

} // namespace udpp
#endif // UDPBASE_H
