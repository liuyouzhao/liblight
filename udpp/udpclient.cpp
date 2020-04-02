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

int UdpClient::init(int port)
{
    memset(mRecvCache, 0, sizeof(mRecvCache));
    /* socket: create the socket */
    mSocketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSocketfd < 0)
    {
        Log::e("( %s:%d) ERROR opening socket", __FUNCTION__, __LINE__);
        return UdpTransState::UDP_DISCONN;
    }
    return UdpTransState::UDP_OK;
}

int UdpClient::deinit()
{
    close(mSocketfd);
    return UdpTransState::UDP_OK;
}

}
