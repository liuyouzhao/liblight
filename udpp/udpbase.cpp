#include "udpbase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "log.h"
#include "udp.h"

namespace udpp
{

int UdpBase::send(std::string targetAddress, int port, const char *buffer, int len)
{
#if DEBUG
    Log::w("server will send %s %d", targetAddress.c_str(), port);
#endif
    struct sockaddr_in addr;
    bzero((char *) &addr, sizeof( addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(targetAddress.c_str());
    int addrLen = sizeof( addr);
    int n = sendto(mSocketfd, buffer, len, 0, (const struct sockaddr*) &addr, addrLen);
    if(n < 0)
    {
        Log::e("Socket error number:%d %s", errno, strerror(errno));
        return UdpTransState::UDP_SEND_FAILED;
    }
    return UdpTransState::UDP_SEND_OK;
}

int UdpBase::recv(void* (*recv_hook)(UdpBase*, char*, int, char*, int), void* (*error_hook)(int))
{
    struct sockaddr_in addrClient;
    int socketlen = sizeof(addrClient);
    int n = 0;
    memset(mRecvCache, 0, sizeof(mRecvCache));
    n = recvfrom(mSocketfd, mRecvCache, sizeof(mRecvCache), 0, (struct sockaddr*) &addrClient, (socklen_t*) &socketlen);
    if(n < 0)
    {
        if(error_hook)
            error_hook(UdpTransState::UDP_RECV_FAILED);
        return UdpTransState::UDP_RECV_FAILED;
    }
    char *clientAddr = inet_ntoa(addrClient.sin_addr);
    int clientPort = ntohs(addrClient.sin_port);
    if(recv_hook)
        recv_hook(this, clientAddr, clientPort, mRecvCache, n);
    return UdpTransState::UDP_RECV_OK;
}

int UdpBase::recv(void* (*recv_hook)(UdpBase*, char*, int, char*, int, void*), void* (*error_hook)(int), void *param)
{
    struct sockaddr_in addrClient;
    int socketlen = sizeof(addrClient);
    int n = 0;
    memset(mRecvCache, 0, sizeof(mRecvCache));
    n = recvfrom(mSocketfd, mRecvCache, sizeof(mRecvCache), 0, (struct sockaddr*) &addrClient, (socklen_t*) &socketlen);
    if(n < 0)
    {
        if(error_hook)
            error_hook(UdpTransState::UDP_RECV_FAILED);
        return UdpTransState::UDP_RECV_FAILED;
    }
    char *clientAddr = inet_ntoa(addrClient.sin_addr);
    int clientPort = ntohs(addrClient.sin_port);
    if(recv_hook)
        recv_hook(this, clientAddr, clientPort, mRecvCache, n, param);
    return UdpTransState::UDP_RECV_OK;
}

} // namespace udpp
