cmake_minimum_required(VERSION 2.8)

project(udp_punching_test)


add_executable(${PROJECT_NAME}  "main.cpp"
                                "udp.h"
                                "udpbase.h"
                                "udpbase.cpp"
                                "udpclient.h"
                                "udpclient.cpp"
                                "udpserver.h"
                                "udpserver.cpp"
                                "threadpool.h"
                                "threadpool.cpp"
                                "udpterminal.cpp"
                                "udpterminal.h"
                                "udppuncher.h"
                                "udppuncher.cpp"
                                "p2pswitcher.h"
                                "p2pswitcher.cpp"
                                "log.h")
target_link_libraries(udp_punching_test pthread)
