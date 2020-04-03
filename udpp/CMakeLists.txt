cmake_minimum_required(VERSION 2.8)
set(CMAKE_BUILD_TYPE Debug)

project(udpp2p)
set(EXEC_NAME p2prun)

add_library(${PROJECT_NAME}   SHARED
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
                            "portscansending.cpp"
                            "portscansending.h"
                            "log.h")
target_link_libraries(${PROJECT_NAME} pthread)

add_executable(${EXEC_NAME}  "main.cpp")
target_link_libraries(${EXEC_NAME} pthread ${PROJECT_NAME})



enable_testing()
add_subdirectory( tests )
