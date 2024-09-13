#pragma once

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

enum LogLevel {
    D,
    E,
    W,
    I,
};

#define GET_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#define mylog(level, M, ...) \
    do { \
        switch ((int)level) { \
            case D: \
                fprintf(stderr, "[DEBUG][%s:%d %s]: " M "\n", GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                break; \
            case E: \
                fprintf(stderr, "[ERROR][%s:%d %s]: " M "\n", GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                break; \
            case W: \
                fprintf(stderr, "[WARN][%s:%d %s]: " M "\n", GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                break; \
            case I: \
                fprintf(stderr, "[INFO][%s:%d %s]: " M "\n", GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                break; \
        } \
    } while(0)

#define MY_ASSERT(cond, M, ...) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[msg]" M "\n",##__VA_ARGS__); \
            assert(cond); \
        } \
    } while(0)
