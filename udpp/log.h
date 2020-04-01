#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#include <string>
#include <pthread.h>
#include <sstream>

#define DO_LOG(color, format, va) \
    std::string contentFormat = std::string(format);\
    std::stringstream ss;\
    ss << "[Thread-" << pthread_self() << "] " << contentFormat;\
    contentFormat = std::string(ss.str());\
    contentFormat = std::string(color) + contentFormat + std::string(KNRM "\n");\
    vprintf(contentFormat.c_str(), va);

class Log
{
public:

    static void i(const char *format, ...)
    {
        va_list va;
        va_start(va, format);
        DO_LOG(KWHT, format, va)
        va_end(va);
    }

    static void d(const char *format, ...)
    {
        va_list va;
        va_start(va, format);
        DO_LOG(KNRM, format, va)
        va_end(va);
    }

    static void w(const char *format, ...)
    {
        va_list va;
        va_start(va, format);
        DO_LOG(KYEL, format, va)
        va_end(va);
    }

    static void e(const char *format, ...)
    {
        va_list va;
        va_start(va, format);
        DO_LOG(KRED, format, va)
        va_end(va);
    }

    static void ok(const char *format, ...)
    {
        va_list va;
        va_start(va, format);
        DO_LOG(KGRN, format, va)
        va_end(va);
    }

private:
};

#endif // LOG_H
