set(CMAKE_BUILD_TYPE Debug)

include_directories( ../ )
link_directories( ../ )

add_executable(test_threadpool test_base.h test_threadpool.cpp)
target_link_libraries(test_threadpool pthread libudpp2p.so)

add_executable(test_portscansending test_base.h test_portscansending.cpp)
target_link_libraries(test_portscansending pthread libudpp2p.so)

add_test(thread_pool test_threadpool)
add_test(ports_scan_sending test_portscansending)
