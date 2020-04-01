#include "udpserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "log.h"
#include "udp.h"

namespace udpp
{

UdpServer::UdpServer(int port)
{
    memset(mRecvCache, 0, sizeof(mRecvCache));
    /* socket: create the socket */
    mSocketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSocketfd < 0)
    {
        Log::e("( %s:%d) ERROR opening socket", __FUNCTION__, __LINE__);
        return;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(mSocketfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        Log::e("( %s:%d) ERROR bind socket", __FUNCTION__, __LINE__);
        return;
    }
}

int UdpServer::send(std::string clientAddress, int port, const char *buffer, int len)
{
    Log::w("server will send %s %d", clientAddress.c_str(), port);
    struct sockaddr_in addr;
    bzero((char *) &addr, sizeof( addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(clientAddress.c_str());
    int addrLen = sizeof( addr);
    int n = sendto(mSocketfd, buffer, len, 0, (const struct sockaddr*) &addr, addrLen);
    if(n < 0)
    {
        Log::e("Socket error number:%d %s", errno, strerror(errno));
        return UdpTransState::UDP_SEND_FAILED;
    }
    return UdpTransState::UDP_SEND_OK;
}


int UdpServer::recv(void* (*recv_hook)(UdpServer*, char*, int, char*, int), void* (*error_hook)(int))
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

} // namespace udpp
