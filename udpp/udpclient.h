#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

namespace udpp
{
class UdpClient
{
public:
    UdpClient();
    ~UdpClient();

    void target(std::string serverAddress, int port);
    int send(const char *buffer, int len);
    int recv(void* (*recv_hook)(UdpClient*, char*, int, char*, int), void* (*error_hook)(int));

    inline char *getRecvCache() {  return mRecvCache;  }
private:
    int mSocketfd;
    struct sockaddr_in  mTargetAddress;
    int mSocketLen;
    char mRecvCache[BUFSIZ];
};
}

#endif // UDPCLIENT_H
