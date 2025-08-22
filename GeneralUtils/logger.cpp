#include "Logger.h"
#include <ctime>

Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    if (file_stream_.is_open())
        file_stream_.close();
}

void Logger::SetLogFile(const std::string& file_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
    file_stream_.open(file_path, std::ios::app);
    if (!file_stream_.is_open()) {
        std::cerr << "[ERROR] Failed to open log file: " << file_path << std::endl;
    }
}

void Logger::SetPrefix(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(mutex_);
    prefix_ = prefix;
}

void Logger::Log(LogLevel level, const std::string& message,
    const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::ostringstream oss;
    oss << GetCurrentTime() << " ";
    oss << "[" << LogLevelToString(level) << "] ";
    if (!file.empty()) {
        oss << "[" << file << ":" << line << "] ";
    }
    if (!prefix_.empty()) {
        oss << prefix_ << " ";
    }
    oss << message;


    std::cerr << oss.str() << std::endl;


    if (file_stream_.is_open()) {
        file_stream_ << oss.str() << std::endl;
    }
}

std::string Logger::GetCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    std::tm tm_time;
#ifdef _WIN32
    localtime_s(&tm_time, &time);
#else
    localtime_r(&time, &tm_time);
#endif
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string Logger::LogLevelToString(LogLevel level) {
    switch (level) {
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::ERROR: return "ERROR";
    default: return "UNKNOWN";
    }
}
