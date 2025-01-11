#pragma once

#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip>


enum class VerboseLevel
{
    NONE = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4
};

extern VerboseLevel g_verboseLevel;

#define LOG(level, message) \
    do { \
        auto now = std::chrono::system_clock::now(); \
        auto time = std::chrono::system_clock::to_time_t(now); \
        std::cout << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] " \
                << "[" << level << "] " \
                << "[" << __FILE__ << ":" << __LINE__ << "]    " \
                << message << std::endl; \
    } while (0)

#define LOG_LEVEL(requiredLevel, level, message) \
    do { \
        if (requiredLevel <= g_verboseLevel) { \
            LOG(level, message); \
        } \
    } while (0)

#define LOG_DEBUG(message)      LOG_LEVEL(VerboseLevel::DEBUG, "DEBUG",    message)
#define LOG_INFO(message)       LOG_LEVEL(VerboseLevel::INFO,  "INFO",     message)
#define LOG_WARN(message)       LOG_LEVEL(VerboseLevel::WARN,  "WARN",     message)
#define LOG_ERROR(message)      LOG_LEVEL(VerboseLevel::ERROR, "ERROR",    message)
#define LOG_CRITICAL(message)   LOG_LEVEL(VerboseLevel::ERROR, "CRITICAL", message)
#define LOG_FATAL(message)      LOG_LEVEL(VerboseLevel::ERROR, "FATAL",    message)