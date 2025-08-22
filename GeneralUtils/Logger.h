#pragma once

#include <string>
#include <iostream>
#include <mutex>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

enum class LogLevel { INFO, WARNING, ERROR };

class Logger {
public:
    static Logger& GetInstance();

    void SetLogFile(const std::string& file_path);
    void SetPrefix(const std::string& prefix);

    void Log(LogLevel level, const std::string& message,
        const std::string& file = "", int line = 0);

    inline void Info(const std::string& message,
        const std::string& file = "", int line = 0) {
        Log(LogLevel::INFO, message, file, line);
    }
    inline void Warning(const std::string& message,
        const std::string& file = "", int line = 0) {
        Log(LogLevel::WARNING, message, file, line);
    }
    inline void Error(const std::string& message,
        const std::string& file = "", int line = 0) {
        Log(LogLevel::ERROR, message, file, line);
    }

private:
    Logger() = default;
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string GetCurrentTime();
    std::string LogLevelToString(LogLevel level);

    std::string prefix_;
    mutable std::mutex mutex_;
    std::ofstream file_stream_;
};


inline void LogInfo(const std::string& message,
    const std::string& file = __FILE__, int line = __LINE__) {
    Logger::GetInstance().Info(message, file, line);
}

inline void LogWarning(const std::string& message,
    const std::string& file = __FILE__, int line = __LINE__) {
    Logger::GetInstance().Warning(message, file, line);
}

inline void LogError(const std::string& message,
    const std::string& file = __FILE__, int line = __LINE__) {
    Logger::GetInstance().Error(message, file, line);
}
