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

// C++ style formatted logging
#define LOG(fmt, ...)       Logger::Log(__FILE__, __LINE__, LogLevel::Info, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...)  Logger::Log(__FILE__, __LINE__, LogLevel::Info, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...) Logger::Log(__FILE__, __LINE__, LogLevel::Debug, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) Logger::Log(__FILE__, __LINE__, LogLevel::Error, fmt, __VA_ARGS__)
#define LOG_WARNING(fmt, ...)  Logger::Log(__FILE__, __LINE__, LogLevel::Warning, fmt, __VA_ARGS__)

// C style formatted logging
#define LOG_CFMT(fmt, ...) Logger::LogCFmt(__FILE__, __LINE__, LogLevel::Info, fmt, ##__VA_ARGS__)
#define LOG_INFO_CFMT(fmt, ...) Logger::LogCFmt(__FILE__, __LINE__, LogLevel::Info, fmt, ##__VA_ARGS__)
#define LOG_DEBUG_CFMT(fmt, ...) Logger::LogCFmt(__FILE__, __LINE__, LogLevel::Debug, fmt, ##__VA_ARGS__)
#define LOG_ERROR_CFMT(fmt, ...) Logger::LogCFmt(__FILE__, __LINE__, LogLevel::Error, fmt, ##__VA_ARGS__)
#define LOG_WARNING_CFMT(fmt, ...) Logger::LogCFmt(__FILE__, __LINE__, LogLevel::Warning, fmt, ##__VA_ARGS__)

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

class Logger
{
public:
    static Logger& Instance()
    {
        static Logger logger;
        return logger;
    }

    static Logger& Attach()
    {
        Instance().AttachConsole();
        return Instance();
    }

    Logger& ShowFileName(bool show = true)
    {
        mShowFileName = show;
        return *this;
    }

    Logger& ShowLineNumber(bool show = true)
    {
        mShowLineNumber = show;
        return *this;
    }

    Logger& ShowTimeStamp(bool show = true)
    {
        mShowTimeStamp = show;
        return *this;
    }

    Logger& ShowDate(bool show = true)
    {
        mShowDate = show;
        return *this;
    }

    Logger& LogToFile(const std::string& directory = "logs")
    {
        PrepareFileLogging(directory);
        mOutput = LogOutput::Both;
        return *this;
    }

    Logger& ConsoleOnly()
    {
        mOutput = LogOutput::Console;
        return *this;
    }

    Logger& FileOnly()
    {
        mOutput = LogOutput::File;
        return *this;
    }

    Logger& Exclude(LogLevel level)
    {
        mExcludedLevels.insert(level);
        return *this;
    }

    Logger& Include(LogLevel level)
    {
        mExcludedLevels.erase(level);
        return *this;
    }

    Logger& ClearExclusions()
    {
        mExcludedLevels.clear();
        return *this;
    }

    Logger& EnableColors(bool enable = true)
    {
        mEnableColors = enable;
        return *this;
    }

	// C++ style formatted logging
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
            Log("", 0, LogLevel::Info, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                Log("", 0, LogLevel::Info, formatted);
            }
            catch (const std::exception&)
            {
                Log("", 0, LogLevel::Info, fmt + " [FORMAT ERROR]");
            }
        }
    }

    template<typename... Args>
    static void Debug(const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            Log("", 0, LogLevel::Debug, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                Log("", 0, LogLevel::Debug, formatted);
            }
            catch (const std::exception&)
            {
                Log("", 0, LogLevel::Debug, fmt + " [FORMAT ERROR]");
            }
        }
    }

    template<typename... Args>
    static void Error(const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            Log("", 0, LogLevel::Error, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                Log("", 0, LogLevel::Error, formatted);
            }
            catch (const std::exception&)
            {
                Log("", 0, LogLevel::Error, fmt + " [FORMAT ERROR]");
            }
        }
    }

    template<typename... Args>
    static void Warn(const std::string& fmt, Args&&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            Log("", 0, LogLevel::Warning, fmt);
        }
        else
        {
            try
            {
                std::string formatted = std::vformat(fmt, std::make_format_args(args...));
                Log("", 0, LogLevel::Warning, formatted);
            }
            catch (const std::exception&)
            {
                Log("", 0, LogLevel::Warning, fmt + " [FORMAT ERROR]");
            }
        }
    }

	// C style formatted logging
    static void __cdecl LogCFmt(const char* file, int line, LogLevel level, const char* fmt, ...);

    static void __cdecl InfoCFmt(const char* fmt, ...);

    static void __cdecl DebugCFmt(const char* fmt, ...);

    static void __cdecl ErrorCFmt(const char* fmt, ...);

    static void __cdecl WarnCFmt(const char* fmt, ...);

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
    Logger() = default;
    ~Logger()
    {
        CloseFileLogging();
        DetachConsole();
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

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
	bool mShowDate = false;
    LogOutput mOutput = LogOutput::Console;
    std::unordered_set<LogLevel> mExcludedLevels;

    // File logging
    std::string mLogFilePath;
    std::unique_ptr<std::ofstream> mLogFile;

    // Thread safety
    std::mutex mLogMutex;
    bool mConsoleAttached = false;
};
