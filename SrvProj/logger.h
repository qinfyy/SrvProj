#pragma once

#include <string>
#include <memory>
#include <format>
#include <fstream>
#include <mutex>
#include <unordered_set>

#ifdef _WIN32
#include <Windows.h>
#endif

#define LOG(fmt, ...)       Logger_::Log(__FILE__, __LINE__, LogLevel::Info, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...)  Logger_::Log(__FILE__, __LINE__, LogLevel::Info, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...) Logger_::Log(__FILE__, __LINE__, LogLevel::Debug, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) Logger_::Log(__FILE__, __LINE__, LogLevel::Error, fmt, __VA_ARGS__)
#define LOG_WARNING(fmt, ...)  Logger_::Log(__FILE__, __LINE__, LogLevel::Warning, fmt, __VA_ARGS__)

enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error
};

enum class LogOutput
{
    Console = 1,
    File = 2,
    Both = Console | File
};

class Logger_
{
public:
    static Logger_& Instance()
    {
        static Logger_ logger;
        return logger;
    }

    static Logger_& Attach()
    {
        Instance().AttachConsole();
        return Instance();
    }

    Logger_& ShowFileName(bool show = true)
    {
        mShowFileName = show;
        return *this;
    }

    Logger_& ShowLineNumber(bool show = true)
    {
        mShowLineNumber = show;
        return *this;
    }

    Logger_& ShowTimeStamp(bool show = true)
    {
        mShowTimeStamp = show;
        return *this;
    }

    Logger_& LogToFile(const std::string& directory = "logs")
    {
        PrepareFileLogging(directory);
        m_output = LogOutput::Both;
        return *this;
    }

    Logger_& ConsoleOnly()
    {
        m_output = LogOutput::Console;
        return *this;
    }

    Logger_& FileOnly()
    {
        m_output = LogOutput::File;
        return *this;
    }

    Logger_& Exclude(LogLevel level)
    {
        mExcludedLevels.insert(level);
        return *this;
    }

    Logger_& Include(LogLevel level)
    {
        mExcludedLevels.erase(level);
        return *this;
    }

    Logger_& ClearExclusions()
    {
        mExcludedLevels.clear();
        return *this;
    }

    Logger_& EnableColors(bool enable = true)
    {
        mEnableColors = enable;
        return *this;
    }

    template<typename... Args>
    static void Log(const char* file, int line, LogLevel level, const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            Instance().WriteLog(file, line, level, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                Instance().WriteLog(file, line, level, formatted);
            }
            catch (const std::exception&)
            {
                Instance().WriteLog(file, line, level, fmt + " [FORMAT ERROR]");
            }
        }
    }

    // Direct logging methods
    template<typename... Args>
    static void Info(const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            Instance().WriteLog("", 0, LogLevel::Info, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                Instance().WriteLog("", 0, LogLevel::Info, formatted);
            }
            catch (const std::exception&)
            {
                Instance().WriteLog("", 0, LogLevel::Info, fmt + " [FORMAT ERROR]");
            }
        }
    }

    template<typename... Args>
    static void Debug(const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            Instance().WriteLog("", 0, LogLevel::Debug, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                Instance().WriteLog("", 0, LogLevel::Debug, formatted);
            }
            catch (const std::exception&)
            {
                Instance().WriteLog("", 0, LogLevel::Debug, fmt + " [FORMAT ERROR]");
            }
        }
    }

    template<typename... Args>
    static void Error(const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            Instance().WriteLog("", 0, LogLevel::Error, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                Instance().WriteLog("", 0, LogLevel::Error, formatted);
            }
            catch (const std::exception&)
            {
                Instance().WriteLog("", 0, LogLevel::Error, fmt + " [FORMAT ERROR]");
            }
        }
    }

    template<typename... Args>
    static void Warn(const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            Instance().WriteLog("", 0, LogLevel::Warning, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                Instance().WriteLog("", 0, LogLevel::Warning, formatted);
            }
            catch (const std::exception&)
            {
                Instance().WriteLog("", 0, LogLevel::Warning, fmt + " [FORMAT ERROR]");
            }
        }
    }

    static void Detach()
    {
        Instance().DetachConsole();
    }

    static void Clear()
    {
        Instance().ClearConsole();
    }

    static char ReadKey()
    {
        return Instance().ConsoleReadKey();
    }

    static void Close()
    {
        Instance().CloseFileLogging();
    }

private:
    Logger_() = default;
    ~Logger_()
    {
        CloseFileLogging();
        DetachConsole();
    }

    Logger_(const Logger_&) = delete;
    Logger_& operator=(const Logger_&) = delete;

    void WriteLog(const char* file, int line, LogLevel level, const std::string& message);
    void AttachConsole();
    void DetachConsole();
    void ClearConsole();
    char ConsoleReadKey();
    bool PrepareFileLogging(const std::string& directory);
    void CloseFileLogging();
    void WriteToConsole(const std::string& formattedMessage, LogLevel level);
    void WriteToFile(const std::string& formattedMessage);
    std::string FormatLogMessage(const char* file, int line, LogLevel level, const std::string& message);
    std::string GetLevelString(LogLevel level);
    std::string GetCurrentTimeString();

#ifdef _WIN32
    WORD GetLevelColor(LogLevel level);
#endif

    // Configuration
    bool mShowFileName = true;
    bool mShowLineNumber = true;
    bool mShowTimeStamp = false;
    bool mEnableColors = true;
    LogOutput m_output = LogOutput::Console;
    std::unordered_set<LogLevel> mExcludedLevels;

    // File logging
    std::string mLogFilePath;
    std::unique_ptr<std::ofstream> mLogFile;

    // Thread safety
    std::mutex mLogMutex;
    bool mConsoleAttached = false;
};
