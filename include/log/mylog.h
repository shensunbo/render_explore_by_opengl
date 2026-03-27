#pragma once

/**
 * @brief Logging interface for the renderer.
 *
 * Wraps spdlog. Call Log::init() once in main() before any other logging.
 * Use the LOG_D/I/W/E macros throughout the codebase.
 *
 * Compile-time minimum level is controlled by SPDLOG_ACTIVE_LEVEL:
 *   DEBUG builds:   SPDLOG_LEVEL_DEBUG
 *   Release builds: SPDLOG_LEVEL_INFO  (default if not set)
 */

#ifndef SPDLOG_ACTIVE_LEVEL
#  ifdef NDEBUG
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#  else
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#  endif
#endif

#include <spdlog/spdlog.h>

#ifdef __ANDROID__
#  include <spdlog/sinks/android_sink.h>
#else
#  include <spdlog/sinks/stdout_color_sinks.h>
#endif

#include <spdlog/sinks/basic_file_sink.h>

#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace Log {

/**
 * @brief Initialize the default spdlog logger.
 *        Call exactly once from main() before any LOG_* usage.
 * @param level  Runtime minimum log level.
 * @param logFile  Optional path to a log file (empty = no file sink).
 */
inline void init(spdlog::level::level_enum level = spdlog::level::debug,
                 const std::string& logFile = "") {
#ifdef __ANDROID__
    auto consoleSink = std::make_shared<spdlog::sinks::android_sink_mt>("renderer");
#else
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#endif
    consoleSink->set_pattern("[%H:%M:%S.%e][%^%-5l%$][%s:%#] %v");

    std::vector<spdlog::sink_ptr> sinks{consoleSink};

    if (!logFile.empty()) {
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
        fileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%-5l][%s:%#] %v");
        sinks.push_back(fileSink);
    }

    auto logger = std::make_shared<spdlog::logger>("renderer", sinks.begin(), sinks.end());
    logger->set_level(level);
    logger->flush_on(spdlog::level::err);
    spdlog::set_default_logger(logger);
    spdlog::set_pattern("[%H:%M:%S.%e][%^%-5l%$][%s:%#] %v");
}

/**
 * @brief Shut down spdlog (flush all sinks). Call at program exit.
 */
inline void shutdown() {
    spdlog::shutdown();
}

} // namespace Log

// ---------------------------------------------------------------------------
// Convenience macros — source file and line captured automatically
// ---------------------------------------------------------------------------
#define LOG_D(...) SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_I(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_W(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_E(...) SPDLOG_ERROR(__VA_ARGS__)

#define MY_ASSERT(cond, ...) \
    do { \
        if (!(cond)) { \
            SPDLOG_ERROR(__VA_ARGS__); \
            assert(cond); \
        } \
    } while (0)
