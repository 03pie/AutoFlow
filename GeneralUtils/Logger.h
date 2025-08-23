/******************************************************************************
 * �ļ���    : Logger.h
 * ��Ŀ      : AutoFlow
 * ����      : ��־ϵͳͷ�ļ�
 * ����      : 03pei
 * ����      : 2025-08-22
 * �汾      : 1.0
 * ˵��      :
 *      1. GetInstance()         ��ȡ Logger ʵ��
 *      2. SetPrefix()           ������־ǰ׺
 *      3. SetLogLevel()         ������־�ȼ�
 *      4. SetLogFile()          ������־�ļ�
 *      5. EnableConsoleOutput() ���ÿ���̨���
 *      6. SetMaxFileSize()      ��������ļ���С
 *      7. SetMaxBackupFiles()   ������󱸷��ļ���
 *      8. Log()                 ��¼��־
 * ����      :
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
  * @brief ��־�ȼ�
  *****************************************************/
enum class LogLevel
{
    INFO_LOG = 0,
    WARNING_LOG,
    ERROR_LOG
};

/*****************************************************
 * @brief ��־��
 *****************************************************/
class Logger
{
public:
    /*****************************************************
     * @brief ��ȡ Logger ʵ��
     * @return Logger& ���� Logger ʵ��������
     *****************************************************/
    static Logger& GetInstance()
    {
        static Logger instance;
        return instance;
    }
    /*****************************************************
     * @brief Logger ��������
     *****************************************************/
    ~Logger()
    {
        if (file_stream_.is_open())
            file_stream_.close();
    }
    /*****************************************************
     * @brief ������־ǰ׺
     * @param prefix ��־ǰ׺�ַ���
     *****************************************************/
    void SetPrefix(const std::string& prefix) { prefix_ = prefix; UpdateMaxPrefixWidth(prefix_); }
    /*****************************************************
     * @brief ������־�ȼ�
     * @param level ��־�ȼ�
     *****************************************************/
    void SetLogLevel(LogLevel level) { min_level_ = level; }
    /*****************************************************
     * @brief ������־�ļ�
     * @param file_path ��־�ļ�·��
     *****************************************************/
    void SetLogFile(const std::string& file_path)
    {
        log_file_ = file_path;
        OpenLogFile();
    }
    /*****************************************************
     * @brief ���ÿ���̨���
     * @param enable �Ƿ����ÿ���̨���
     *****************************************************/
    void EnableConsoleOutput(bool enable) { console_output_ = enable; }
    /*****************************************************
     * @brief ��������ļ���С
     * @param bytes ����ļ���С���ֽڣ�
     *****************************************************/
    void SetMaxFileSize(size_t bytes) { max_file_size_ = bytes; }
    /*****************************************************
     * @brief ������󱸷��ļ���
     * @param n ��󱸷��ļ���
     *****************************************************/
    void SetMaxBackupFiles(int n) { max_backup_files_ = n; }
    /*****************************************************
     * @brief ��¼��־
     * @param level ��־�ȼ�
     * @param message ��־��Ϣ
     * @param file Դ�ļ���
     * @param line �к�
     * @param func ������
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
            << std::setw(20) << GetCurrentTime()                 // ʱ�������
            << std::setw(12) << LogLevelToString(level)          // �ȼ��̶���
            << std::setw(max_file_width_) << file_func_line      // �ļ�:��:�����������Ӧ
            << std::setw(max_prefix_width_) << prefix_           // ǰ׺�������Ӧ
            << message;

        if (console_output_)
            std::cerr << oss.str() << std::endl;

        if (file_stream_.is_open())
        {
            RotateLogIfNeeded(); // ����С��������־
            file_stream_ << oss.str() << std::endl;
        }
    }

private:
    /*****************************************************
     * @brief Logger ���캯��
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
     * @brief ���ÿ�������͸�ֵ����
     *****************************************************/
    Logger(const Logger&) = delete;
    /*****************************************************
     * @brief �����ƶ�����͸�ֵ����
     *****************************************************/
    Logger(Logger&&) = delete;
    /*****************************************************
     * @brief ���ÿ�����ֵ����
     *****************************************************/
    Logger& operator=(const Logger&) = delete;
    /*****************************************************
     * @brief �����ƶ���ֵ����
     *****************************************************/
    Logger& operator=(Logger&&) = delete;
    /*****************************************************
     * @brief ����־�ļ�
     *****************************************************/
    void OpenLogFile()
    {
        // ����ļ��в����ڣ��ȴ���
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
     * @brief ��鲢��ת��־�ļ�
     *****************************************************/
    void RotateLogIfNeeded()
    {
        if (!std::filesystem::exists(log_file_)) return;
        auto size = std::filesystem::file_size(log_file_);
        if (size < max_file_size_) return;

        file_stream_.close();
        // ���ݾ���־
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
     * @brief ��ȡ��ǰʱ��
     * @return ��ǰʱ���ַ���
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
     * @brief ����־����ת��Ϊ�ַ���
     * @param level ��־����
     * @return ��־�����ַ���
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
     * @brief ��������ļ����
     * @param s ��־��Ϣ
     *****************************************************/
    void UpdateMaxFileWidth(const std::string& s)
    {
        size_t len = s.length();
        if (len > max_file_width_)
            max_file_width_ = static_cast<int>(len) + 2; // +2 �ո�
    }

    /*****************************************************
     * @brief �������ǰ׺���
     * @param s ��־��Ϣ
     *****************************************************/
    void UpdateMaxPrefixWidth(const std::string& s)
    {
        size_t len = s.length();
        if (len > max_prefix_width_)
            max_prefix_width_ = static_cast<int>(len) + 2; // +2 �ո�
    }

private:
    std::ofstream file_stream_{};              // ��־�ļ���
    std::string log_file_{};                   // ��־�ļ���
    std::string prefix_{};                     // ��־ǰ׺
    LogLevel min_level_{ LogLevel::INFO_LOG };   // ��С��־����
    bool console_output_{ true };                // �Ƿ����������̨
    int max_file_width_{ 0 };                    // ����ļ����
    int max_prefix_width_{ 0 };                  // ���ǰ׺���
    size_t max_file_size_{ 0 };                  // ����ļ���С
    int max_backup_files_{ 0 };                  // ��󱸷��ļ���
};

/*****************************************************
 * @brief ��Ϣ��־��
 *****************************************************/
#define LOG_INFO(msg) Logger::GetInstance().Log(LogLevel::INFO_LOG, msg, __FILE__, __LINE__, __FUNCTION__)
 /*****************************************************
  * @brief ������־��
  *****************************************************/
#define LOG_WARN(msg) Logger::GetInstance().Log(LogLevel::WARNING_LOG, msg, __FILE__, __LINE__, __FUNCTION__)
  /*****************************************************
   * @brief ������־��
   *****************************************************/
#define LOG_ERROR(msg) Logger::GetInstance().Log(LogLevel::ERROR_LOG, msg, __FILE__, __LINE__, __FUNCTION__)