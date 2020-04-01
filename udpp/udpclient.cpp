#include "udpclient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "log.h"
#include "udp.h"

namespace udpp
{

UdpClient::UdpClient()
{
    memset(mRecvCache, 0, sizeof(mRecvCache));
    /* socket: create the socket */
    mSocketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSocketfd < 0)
    {
        Log::e("( %s:%d) ERROR opening socket", __FUNCTION__, __LINE__);
        return;
    }
}

UdpClient::~UdpClient()
{
    close(mSocketfd);
}

void UdpClient::target(std::string serverAddress, int port)
{
    /* build the server's Internet address */
    Log::w("client bind %s %d", serverAddress.c_str(), port);
    bzero((char *) & mTargetAddress, sizeof( mTargetAddress));
    mTargetAddress.sin_family = AF_INET;
    mTargetAddress.sin_port = htons(port);
    mTargetAddress.sin_addr.s_addr = inet_addr(serverAddress.c_str());
}

int UdpClient::send(const char *buffer, int len)
{
    /* Returns the number sent, or -1 for errors. */
    mSocketLen = sizeof( mTargetAddress);
    int n = sendto(mSocketfd, buffer, len, 0, (const struct sockaddr*) & mTargetAddress, mSocketLen);
    if(n < 0)
    {
        Log::e("Socket error number:%d %s", errno, strerror(errno));
        return UdpTransState::UDP_SEND_FAILED;
    }
    return UdpTransState::UDP_SEND_OK;
}

int UdpClient::recv(void* (*recv_hook)(UdpClient*, char*, int, char*, int), void* (*error_hook)(int))
{
    int socketlen = 0;
    struct sockaddr_in anyWhere;
    int n = 0;
    memset(mRecvCache, 0, sizeof(mRecvCache));
    n = recvfrom(mSocketfd, mRecvCache, sizeof(mRecvCache), 0, (struct sockaddr*) &anyWhere, (socklen_t*) &socketlen);
    if(n < 0)
    {
        if(error_hook)
            error_hook(UdpTransState::UDP_RECV_FAILED);
        return UdpTransState::UDP_RECV_FAILED;
    }
    char *addr = inet_ntoa(anyWhere.sin_addr);
    int port = ntohs(anyWhere.sin_port);
    if(recv_hook)
        recv_hook(this, addr, port, mRecvCache, n);
    return UdpTransState::UDP_RECV_OK;
}

}
