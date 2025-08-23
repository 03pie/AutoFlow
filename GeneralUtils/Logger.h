/******************************************************************************
    * @File     : Logger.h
    * @Brief    : Logger class header file
    * @Author   : 03pei
    * @Date     : 2025-08-22
    * @Version  : 1.0
    * @Note     :
    *      1. GetInstance()         Get Logger instance
    *      2. SetPrefix()           Set log prefix
    *      3. SetLogLevel()         Set log level
    *      4. SetLogFile()          Set log file
    *      5. EnableConsoleOutput() Enable console output
    *      6. SetMaxFileSize()      Set max log file size
    *      7. SetMaxBackupFiles()   Set max backup files
    *      8. Log()                 Log message
    * @Include  :
    *      - GeneralDefine.h
 ******************************************************************************/
#pragma once
#include "GeneralDefine.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <filesystem>

 /*****************************************************
  * @brief 日志级别
  *****************************************************/
enum class LogLevel
{
    INFO_LOG = 0,
    WARNING_LOG,
    ERROR_LOG
};

/*****************************************************
 * @brief 日志类
 *****************************************************/
class Logger
{
public:
    /*****************************************************
     * @brief 获取 Logger 实例
     * @return Logger& 返回 Logger 实例的引用
     *****************************************************/
    static Logger& GetInstance()
    {
        static Logger instance;
        return instance;
    }
    /*****************************************************
     * @brief Logger 析构函数
     *****************************************************/
    ~Logger()
    {
        if (file_stream_.is_open())
            file_stream_.close();
    }
    /*****************************************************
     * @brief 设置日志前缀
     * @param prefix 日志前缀字符串
     *****************************************************/
    void SetPrefix(const std::string& prefix) { prefix_ = prefix; UpdateMaxPrefixWidth(prefix_); }
    /*****************************************************
     * @brief 设置日志级别
     * @param level 日志级别
     *****************************************************/
    void SetLogLevel(LogLevel level) { min_level_ = level; }
    /*****************************************************
     * @brief 设置日志文件
     * @param file_path 日志文件路径
     *****************************************************/
    void SetLogFile(const std::string& file_path)
    {
        log_file_ = file_path;
        OpenLogFile();
    }
    /*****************************************************
     * @brief 启用/禁用控制台输出
     * @param enable 是否启用控制台输出
     *****************************************************/
    void EnableConsoleOutput(bool enable) { console_output_ = enable; }
    /*****************************************************
     * @brief 设置最大日志文件大小
     * @param bytes 最大日志文件大小（字节）
     *****************************************************/
    void SetMaxFileSize(size_t bytes) { max_file_size_ = bytes; }
    /*****************************************************
     * @brief 设置最大备份文件数
     * @param n 最大备份文件数
     *****************************************************/
    void SetMaxBackupFiles(int n) { max_backup_files_ = n; }
    /*****************************************************
     * @brief 记录日志
     * @param level 日志级别
     * @param message 日志信息
     * @param file 源文件
     * @param line 行号
     * @param func 函数名
     *****************************************************/
    void Log(LogLevel level, const std::string& message,
        const char* file = "", int line = 0,
        const char* func = "")
    {
        if (level < min_level_) return;

        std::string file_func_line;
        if (file && *file)
        {
            file_func_line = std::string(file) + ":" + std::to_string(line);
            if (func && *func)
                file_func_line += " " + std::string(func);
        }

        UpdateMaxFileWidth(file_func_line);

        std::ostringstream oss;
        oss << std::left
            << std::setw(20) << GetCurrentTime()                 // 时间戳
            << std::setw(12) << LogLevelToString(level)          // 日志级别
            << std::setw(max_file_width_) << file_func_line      // 文件:行:函数名
            << std::setw(max_prefix_width_) << prefix_           // 前缀
            << message;

        if (console_output_)
            std::cerr << oss.str() << std::endl;

        if (file_stream_.is_open())
        {
            RotateLogIfNeeded(); // 轮转日志文件
            file_stream_ << oss.str() << std::endl;
        }
    }

private:
    /*****************************************************
     * @brief Logger 构造函数
     *****************************************************/
    Logger()
        : log_file_(GeneralDefine::general_logs_path),
        prefix_(""),
        min_level_(LogLevel::INFO_LOG),
        console_output_(true),
        max_file_width_(0),
        max_prefix_width_(0),
        max_file_size_(10 * 1024 * 1024), // Ĭ��10MB
        max_backup_files_(5)
    {
        OpenLogFile();
    }
    /*****************************************************
     * @brief 复制构造函数
     *****************************************************/
    Logger(const Logger&) = delete;
    /*****************************************************
     * @brief 移动构造函数
     *****************************************************/
    Logger(Logger&&) = delete;
    /*****************************************************
     * @brief 赋值构造函数
     *****************************************************/
    Logger& operator=(const Logger&) = delete;
    /*****************************************************
     * @brief 赋值运算符
     *****************************************************/
    Logger& operator=(Logger&&) = delete;
    /*****************************************************
     * @brief 关闭日志文件
     *****************************************************/
    void OpenLogFile()
    {
        // 创建日志文件所在目录
        std::filesystem::path log_path(log_file_);
        std::filesystem::path dir = log_path.parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir))
        {
            try {
                std::filesystem::create_directories(dir);
            }
            catch (const std::exception& e) {
                std::cerr << "[ERROR] Failed to create log directory: " << dir << " - " << e.what() << std::endl;
            }
        }

        if (file_stream_.is_open()) file_stream_.close();
        file_stream_.open(log_file_, std::ios::app);
        if (!file_stream_.is_open())
            std::cerr << "[ERROR] Failed to open log file: " << log_file_ << std::endl;
    }

    /*****************************************************
     * @brief 轮转日志文件
     *****************************************************/
    void RotateLogIfNeeded()
    {
        if (!std::filesystem::exists(log_file_)) return;
        auto size = std::filesystem::file_size(log_file_);
        if (size < max_file_size_) return;

        file_stream_.close();
        // 备份日志文件
        for (int i = max_backup_files_ - 1; i >= 1; --i)
        {
            std::string old_name = log_file_ + "." + std::to_string(i);
            std::string new_name = log_file_ + "." + std::to_string(i + 1);
            if (std::filesystem::exists(old_name))
                std::filesystem::rename(old_name, new_name);
        }
        std::string first_backup = log_file_ + ".1";
        std::filesystem::rename(log_file_, first_backup);
        OpenLogFile();
    }
    /*****************************************************
     * @brief 获取当前时间
     * @return 当前时间字符串
     *****************************************************/
    std::string GetCurrentTime()
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_time;
#ifdef _WIN32
        localtime_s(&tm_time, &time);
#else
        localtime_r(&time, &tm_time);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    /*****************************************************
     * @brief 日志级别转换为字符串
     * @param level 日志级别
     * @return 日志级别字符串
     *****************************************************/
    std::string LogLevelToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::INFO_LOG: return "[INFO]";
        case LogLevel::WARNING_LOG: return "[WARNING]";
        case LogLevel::ERROR_LOG: return "[ERROR]";
        default: return "[UNKNOWN]";
        }
    }

    /*****************************************************
     * @brief 更新日志文件宽度
     * @param s 日志信息
     *****************************************************/
    void UpdateMaxFileWidth(const std::string& s)
    {
        size_t len = s.length();
        if (len > max_file_width_)
            max_file_width_ = static_cast<int>(len) + 2; // +2 for brackets
    }

    /*****************************************************
     * @brief 更新日志前缀宽度
     * @param s 日志信息
     *****************************************************/
    void UpdateMaxPrefixWidth(const std::string& s)
    {
        size_t len = s.length();
        if (len > max_prefix_width_)
            max_prefix_width_ = static_cast<int>(len) + 2; // +2 for brackets
    }

private:
    std::ofstream file_stream_{};              // 日志文件流
    std::string log_file_{};                   // 日志文件名
    std::string prefix_{};                     // 日志前缀
    LogLevel min_level_{ LogLevel::INFO_LOG };   // 最小日志级别
    bool console_output_{ true };                // 是否输出到控制台
    int max_file_width_{ 0 };                    // 日志文件宽度
    int max_prefix_width_{ 0 };                  // 日志前缀宽度
    size_t max_file_size_{ 0 };                  // 日志文件大小
    int max_backup_files_{ 0 };                  // 备份日志文件数量
};

/*****************************************************
 * @brief 信息日志
 *****************************************************/
#define LOG_INFO(msg) Logger::GetInstance().Log(LogLevel::INFO_LOG, msg, __FILE__, __LINE__, __FUNCTION__)
 /*****************************************************
  * @brief 警告日志
  *****************************************************/
#define LOG_WARN(msg) Logger::GetInstance().Log(LogLevel::WARNING_LOG, msg, __FILE__, __LINE__, __FUNCTION__)
/*****************************************************
 * @brief 错误日志
 *****************************************************/
#define LOG_ERROR(msg) Logger::GetInstance().Log(LogLevel::ERROR_LOG, msg, __FILE__, __LINE__, __FUNCTION__)