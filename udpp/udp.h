#ifndef UDP_H
#define UDP_H
namespace udpp
{

enum UdpTransState
{
    UDP_OK = 0,
    UDP_SEND_OK = 1,
    UDP_RECV_OK = 2,
    UDP_SEND_FAILED = -1,
    UDP_RECV_FAILED = -2,
    UDP_CONN_FAILED = -8,
    UDP_DISCONN = -9
};
}
#endif // UDP_H

