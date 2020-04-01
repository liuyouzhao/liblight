#include "udpterminal.h"
#include "udpclient.h"
#include "udpserver.h"
#include "log.h"
#include <iostream>
#include <string.h>

namespace udpp
{

UdpTerminal::UdpTerminal():
    threadPool(10, 20)
{
}

void *show_clients_by_server(UdpServer *server, char *ip, int port, char *msg, int len)
{
    Log::d("%s:%d sent %s length %d", ip, port, msg, len);
    char buf[BUFSIZ] = {0};
    sprintf(buf, "[Server Recv] %s", msg);
    server->send(std::string(ip), port, buf, strlen(buf));
}

void *show_anywhere_by_client(UdpClient *client, char *ip, int port, char *msg, int len)
{
    Log::d("%s:%d sent %s length %d", ip, port, msg, len);
}


void *on_server_error(int err)
{
    Log::e("Error number %d with inner errno %d %s", err, errno, strerror(errno));
}

void *thread_recv_server(void *param)
{
    void **ptrArr = (void**)param;
    UdpServer *udpServer = (UdpServer*)ptrArr[0];
    UdpTerminal *udpTermial = (UdpTerminal*)ptrArr[1];
    while(1)
    {
        udpServer->recv(show_clients_by_server, on_server_error);
    }
}

void *thread_recv_client(void *param)
{
    void **ptrArr = (void**)param;
    UdpClient *udpClient = (UdpClient*)ptrArr[0];
    UdpTerminal *udpTermial = (UdpTerminal*)ptrArr[1];
    while(1)
    {
        udpClient->recv(show_anywhere_by_client, on_server_error);
    }
}

void UdpTerminal::dumpLines()
{
    for(int i = 0; i < this->recvLines.size(); i ++)
    {
        std::string line = recvLines[i];
        std::cout << line.c_str() << std::endl;
    }
}

void *threadpool_broadcast(void *param)
{
    void **ptrArr = (void**)param;
    UdpClient *udpClient = (UdpClient*)ptrArr[0];
    std::string targetAddress = *(std::string*)ptrArr[1];
    int i = *(int*)ptrArr[2];
    udpClient->target(targetAddress, i);
    char *tosend = "xxxxxxxxx";
    udpClient->send(tosend, sizeof(tosend));
}

void UdpTerminal::loopRun()
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
                int port;
                memset(input, 0, sizeof(input));
                std::cout << "Target IP Address:";
                std::cin >> input;
                std::string targetAddress = std::string(input);

                memset(input, 0, sizeof(input));
                std::cout << "Port:";
                std::cin >> input;
                port = atoi(input);

                udpClient.target(targetAddress, port);
            }
            else if(strcmp("s", input) == 0)
            {
                std::cout << "Message to send:";
                std::cin >> input;
                int st = udpClient.send(input, strlen(input));
                if(st < 0)
                {
                    Log::e("udp send failed with error code %d", st);
                }
            }
        }
    }
    else if(strcmp("3", input) == 0)
    {
        UdpClient udpClient;
        int port;
        memset(input, 0, sizeof(input));
        std::cout << "Target IP Address:";
        std::cin >> input;
        std::string targetAddress = std::string(input);

        udpClient.target(std::string("35.183.10.230"), 9001);
        udpClient.send("hi", 2);


        ThreadPool tp(500, 1000);
        for(int i = 0; i < 65535; i ++)
        {
            void* pointers[3] = {&udpClient, &targetAddress, &i};
            threadpool_broadcast(pointers);
        }

        void* pointers[2] = {&udpClient, this};
        threadPool.run(thread_recv_client, pointers);

    }
}

} // namespace udpp
