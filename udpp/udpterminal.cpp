#include "udpterminal.h"
#include "udpclient.h"
#include "udpserver.h"
#include "log.h"
#include <iostream>
#include <unistd.h>
#include <string.h>

namespace udpp
{
static std::vector<TempUdpClientAddress> recvLines;
void pushLine(std::string ip, int port)
{
    for(int i = 0; i < recvLines.size(); i ++)
    {
        TempUdpClientAddress t = recvLines[i];
        if(strcmp(t.ip.c_str(), ip.c_str()) == 0)
        {
            return;
        }
    }
    TempUdpClientAddress t1(ip, port);
    recvLines.push_back(t1);
}

UdpTerminal::UdpTerminal():
    threadPool(10, 20)
{
}

void *show_clients_by_server(UdpBase *server, char *ip, int port, char *msg, int len)
{
    Log::d("%s:%d sent %s length %d", ip, port, msg, len);

#if PING_PONG
    pushLine(std::string(ip), port);

    for(int i = 0; i < recvLines.size(); i ++)
    {
        TempUdpClientAddress t = recvLines[i];
        std::stringstream ss;
        ss << t.ip << " " << t.port << std::endl;
        std::string tosend = std::string(ss.str());
        server->send(std::string(ip), port, tosend.c_str(), tosend.length());
    }
#endif
    server->send(std::string(ip), port, msg, len);
}

void *show_anywhere_by_client(UdpBase *client, char *ip, int port, char *msg, int len)
{
    Log::d("%s:%d sent %s length %d", ip, port, msg, len);
    client->send(std::string(ip), port, msg, len);
}


void *on_server_error(int err)
{
    Log::e("Error number %d with inner errno %d %s", err, errno, strerror(errno));
}

void *thread_recv_server(void *param, ThreadPool* tp)
{
    void **ptrArr = (void**)param;
    UdpServer *udpServer = (UdpServer*)ptrArr[0];
    UdpTerminal *udpTermial = (UdpTerminal*)ptrArr[1];
    while(1)
    {
        udpServer->recv(show_clients_by_server, on_server_error);
    }
}

void *thread_recv_client(void *param, ThreadPool* tp)
{
    void **ptrArr = (void**)param;
    UdpClient *udpClient = (UdpClient*)ptrArr[0];
    UdpTerminal *udpTermial = (UdpTerminal*)ptrArr[1];
    while(1)
    {
        udpClient->recv(show_anywhere_by_client, on_server_error);
    }
}

void *show_any_by_any(UdpBase *client, char *ip, int port, char *msg, int len)
{
    Log::d("RECV FROM [%s:%d], MSG: %s", ip, port, msg);
}

void *thread_recv_base(void *param, ThreadPool* tp)
{
    UdpBase *udpBase = (UdpBase*)param;
    while(1)
        udpBase->recv(show_any_by_any, NULL);
}

void UdpTerminal::interactiveRun()
{
    char input[512] = {0};
    std::cout << "[1] Server" << std::endl << "[2] Client" << std::endl << "1 or 2 (default 1):";
    std::cin >> input;

    if(strcmp("1", input) == 0)
    {
        memset(input, 0, sizeof(input));
        std::cout << "Local port:";
        std::cin >> input;

        int lport = atoi(input);
        UdpServer udpServer(lport);
        void* pointers[2] = {&udpServer, this};
        threadPool.run(thread_recv_server, pointers);
        Log::ok("SERVER are created.");

        std::string targetAddress;
        int port = 0;

        while(1)
        {
            memset(input, 0, sizeof(input));
            std::cout << "t(target)/s(send)" << std::endl;
            std::cout << "$";
            std::cin >> input;
            if(strcmp("t", input) == 0)
            {
                memset(input, 0, sizeof(input));
                std::cout << "Target IP Address:";
                std::cin >> input;
                targetAddress = std::string(input);

                memset(input, 0, sizeof(input));
                std::cout << "Port:";
                std::cin >> input;
                port = atoi(input);
            }
            else if(strcmp("s", input) == 0)
            {
                std::cout << "Message to send:";
                std::cin >> input;
                int st = udpServer.send(targetAddress, port, input, strlen(input));
                if(st < 0)
                {
                    Log::e("udp send failed with error code %d", st);
                }
            }
        }
    }
    else if(strcmp("2", input) == 0)
    {
        UdpClient udpClient;
        std::string targetAddress;
        int port;
        void* pointers[2] = {&udpClient, this};
        threadPool.run(thread_recv_client, pointers);
        while(1)
        {
            memset(input, 0, sizeof(input));
            std::cout << "t(target)/s(send)" << std::endl;
            std::cout << "$";
            std::cin >> input;
            if(strcmp("t", input) == 0)
            {
                memset(input, 0, sizeof(input));
                std::cout << "Target IP Address:";
                std::cin >> input;
                targetAddress = std::string(input);

                memset(input, 0, sizeof(input));
                std::cout << "Port:";
                std::cin >> input;
                port = atoi(input);
            }
            else if(strcmp("s", input) == 0)
            {
                std::cout << "Message to send:";
                std::cin >> input;
                int st = udpClient.send(targetAddress, port, input, strlen(input));
                if(st < 0)
                {
                    Log::e("udp send failed with error code %d", st);
                }
            }
        }
    }
}

void UdpTerminal::loopRun(char **arr, int len)
{
    int i = 0;
    if(len > 0)
    {
        UdpBase *pUdp = NULL;
        std::string targetAddr;
        int targetPort = 0;
        std::string message;
        char msg[512] = {0};

        if(!strncmp("1", *arr, 1) ||
           !strncmp("s", *arr, 1))
        {
            pUdp = new UdpServer(atoi(*(arr + 1)));
            arr = arr + 2;
        }
        else if(!strncmp("2", *arr, 1) ||
                !strncmp("c", *arr, 1))
        {
            pUdp = new UdpClient();
            arr = arr + 1;
        }
        targetAddr = std::string(*arr);
        targetPort = atoi(*(arr + 1));
        if(*(arr + 2) && strlen(*(arr + 2)) > 0)
            message = std::string(*(arr + 2));
        else
        {
            std::cin >> msg;
            message = std::string(msg);
        }
        threadPool.run(thread_recv_base, pUdp);

        /// loop send
        while(1)
        {
            pUdp->send(targetAddr, targetPort, message.c_str(), message.length());
            sleep(1);
        }
    }
    else
    {
        interactiveRun();
    }
}

} // namespace udpp
