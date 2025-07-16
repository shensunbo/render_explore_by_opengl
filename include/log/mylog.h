#ifndef __MYLOG_H__
#define __MYLOG_H__

#include <iostream>
#include <cerrno>
#include <cstring>
#include <cassert>

enum class LogLevel {
    E,
    W,
    I,
    D,
};

//set log level
#define LOG_LEVEL LogLevel::D

#define GET_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#define mylog(level, M, ...) \
    do { \
        if ((int)level <= (int)LOG_LEVEL) { \
            switch (level) { \
                case LogLevel::D: \
                fprintf(stderr, "[DEBUG][%s:%d %s]: " M "\n", GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                    break; \
                case LogLevel::E: \
                fprintf(stderr, "[ERROR][%s:%d %s]: " M "\n", GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                    break; \
                case LogLevel::W: \
                fprintf(stderr, "[WARN][%s:%d %s]: " M "\n", GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                    break; \
                case LogLevel::I: \
                fprintf(stderr, "[INFO][%s:%d %s]: " M "\n", GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                    break; \
            } \
        } \
    } while(0)

#define MY_ASSERT(cond, M, ...) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[msg]" M "\n",##__VA_ARGS__); \
            assert(cond); \
        } \
    } while(0)


#endif // __MYLOG_H__