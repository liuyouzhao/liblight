#ifndef UDPTERMINAL_H
#define UDPTERMINAL_H

#include <vector>
#include <string>
#include "threadpool.h"

namespace udpp
{

#define CACHE_OVERFLOW 1024

class UdpTerminal
{
public:
    UdpTerminal();
    void loopRun();
    inline void pushLine(std::string line) {    recvLines.push_back(line); }
    inline void solveOverflow() {   if(recvLines.size() > CACHE_OVERFLOW) recvLines.erase(recvLines.begin());   }
protected:
    void dumpLines();
private:
    std::vector<std::string> recvLines;
    ThreadPool threadPool;
};


}


#endif // UDPTERMINAL_H
