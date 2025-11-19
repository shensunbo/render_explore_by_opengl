#ifndef __MYLOG_H__
#define __MYLOG_H__

#include <cerrno>
#include <cstring>
#include <cassert>

// Android logging support
#ifdef __ANDROID__
#include <android/log.h>
#define TAG "MY_APP"
#else
#include <iostream>
#include <cstdio>
#endif

enum class LogLevel {
    E,
    W,
    I,
    D,
};

//set log level
#define LOG_LEVEL LogLevel::D

#define GET_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


#ifdef __ANDROID__
// Android logging implementation
#define mylog(level, M, ...) \
    do { \
        if ((int)level <= (int)LOG_LEVEL) { \
            switch (level) { \
                case LogLevel::D: \
                    __android_log_print(ANDROID_LOG_DEBUG, TAG, "[%s:%d %s]: " M, GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                    break; \
                case LogLevel::E: \
                    __android_log_print(ANDROID_LOG_ERROR, TAG, "[%s:%d %s]: " M, GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                    break; \
                case LogLevel::W: \
                    __android_log_print(ANDROID_LOG_WARN, TAG, "[%s:%d %s]: " M, GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                    break; \
                case LogLevel::I: \
                    __android_log_print(ANDROID_LOG_INFO, TAG, "[%s:%d %s]: " M, GET_FILENAME, __LINE__, __func__, ##__VA_ARGS__); \
                    break; \
            } \
        } \
    } while(0)

#define MY_ASSERT(cond, M, ...) \
    do { \
        if (!(cond)) { \
            __android_log_print(ANDROID_LOG_ERROR, TAG, "[ASSERT] " M, ##__VA_ARGS__); \
            assert(cond); \
        } \
    } while(0)

#else
// Standard logging implementation for non-Android platforms
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
            fprintf(stderr, "[msg]" M "\n", ##__VA_ARGS__); \
            assert(cond); \
        } \
    } while(0)

#endif // __ANDROID__


#endif // __MYLOG_H__