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
    this->init(port);
}

int UdpServer::init(int port)
{
    memset(mRecvCache, 0, sizeof(mRecvCache));
    /* socket: create the socket */
    mSocketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSocketfd < 0)
    {
        Log::e("( %s:%d) ERROR opening socket", __FUNCTION__, __LINE__);
        return UdpTransState::UDP_CONN_FAILED;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(mSocketfd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0)
    {
        Log::e("( %s:%d) ERROR bind socket", __FUNCTION__, __LINE__);
        return UdpTransState::UDP_CONN_FAILED;
    }
    return UdpTransState::UDP_OK;
}

int UdpServer::deinit()
{
    close(mSocketfd);
    return UdpTransState::UDP_OK;
}

} // namespace udpp
