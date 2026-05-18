#include "Logger.h"
#include <filesystem>
#include <iostream>

void Logger::WriteLog(const char* file, int line, LogLevel level, const std::string& message) {

    if (mExcludedLevels.find(level) != mExcludedLevels.end())
        return;

    const std::lock_guard<std::mutex> lock(mLogMutex);

    std::string formattedMessage = FormatLogMessage(file, line, level, message);

    // Write to console if enabled
    if (static_cast<int>(mOutput) & static_cast<int>(LogOutput::Console)) {
        WriteToConsole(formattedMessage, level);
    }

    // Write to file if enabled
    if (static_cast<int>(mOutput) & static_cast<int>(LogOutput::File)) {
        WriteToFile(formattedMessage);
    }
}

void Logger::AttachConsole() {
#ifdef _WIN32
    if (!mConsoleAttached) {
        AllocConsole();

        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
        SetConsoleOutputCP(CP_UTF8);

        // Enable virtual terminal processing for ANSI color support
        HANDLE hOut   = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD  dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }

        mConsoleAttached = true;
    }
#endif
}

void Logger::DetachConsole() {
#ifdef _WIN32
    if (mConsoleAttached) {
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        FreeConsole();
        mConsoleAttached = false;
    }
#endif
}

void Logger::ClearConsole() {
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE)
        return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(h, &csbi))
        return;

    DWORD size  = csbi.dwSize.X * csbi.dwSize.Y;
    COORD coord = {0, 0};
    DWORD written;

    FillConsoleOutputCharacter(h, TEXT(' '), size, coord, &written);
    GetConsoleScreenBufferInfo(h, &csbi);
    FillConsoleOutputAttribute(h, csbi.wAttributes, size, coord, &written);
    SetConsoleCursorPosition(h, coord);
#else
    std::cout << "\033[2J\033[H" << std::flush;
#endif
}

char Logger::ConsoleReadKey() {
    return std::cin.get();
}

bool Logger::PrepareFileLogging(const std::string& directory) {
    try {
        // Create directory if it doesn't exist
        if (!std::filesystem::exists(directory)) {
            if (!std::filesystem::create_directories(directory)) {
                return false;
            }
        }

        // Generate filename with timestamp
        time_t now = time(NULL);

        struct tm tm_buf;
#ifdef _WIN32
        localtime_s(&tm_buf, &now);
#else
        localtime_r(&now, &tm_buf);
#endif

        char timeBuffer[64];

        // Create filename with timestamp using simple string formatting
        sprintf_s(
            timeBuffer,
            sizeof(timeBuffer),
            "log_%04d-%02d-%02d_%02d-%02d-%02d.txt",
            tm_buf.tm_year + 1900,
            tm_buf.tm_mon + 1,
            tm_buf.tm_mday,
            tm_buf.tm_hour,
            tm_buf.tm_min,
            tm_buf.tm_sec
        );

        std::string filename = timeBuffer;

        mLogFilePath = directory + "/" + filename;

        // Close existing file if open
        if (mLogFile && mLogFile->is_open()) {
            mLogFile->close();
        }

        // Open new log file
        mLogFile = std::make_unique<std::ofstream>(mLogFilePath, std::ios::out | std::ios::app);
        if (!mLogFile->is_open()) {
            mLogFile.reset();
            return false;
        }

        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

void Logger::CloseFileLogging() {
    if (mLogFile && mLogFile->is_open()) {
        mLogFile->close();
        mLogFile.reset();
    }
}

void Logger::WriteToConsole(const std::string& formattedMessage, LogLevel level)
{
#ifdef _WIN32
    if (!mEnableColors) {
        std::cout << formattedMessage << std::endl;
        return;
    }

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) {
        std::cout << formattedMessage << std::endl;
        return;
    }

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD def = csbi.wAttributes;

    WORD levelColor = GetLevelColor(level);
    WORD fileColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD lineColor = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;

    size_t pos = 0;
    int idx = 0;

    while (pos < formattedMessage.size())
    {
        size_t l = formattedMessage.find('[', pos);
        if (l == std::string::npos)
        {
            std::cout << formattedMessage.substr(pos);
            break;
        }

        if (l > pos)
            std::cout << formattedMessage.substr(pos, l - pos);

        size_t r = formattedMessage.find(']', l);
        if (r == std::string::npos)
        {
            std::cout << formattedMessage.substr(l);
            break;
        }

        std::string content = formattedMessage.substr(l + 1, r - l - 1);

        if (idx == 0)
        {
            std::cout << "[" << content << "]";
        }
        else if (idx == 1)
        {
            std::cout << "[";
            SetConsoleTextAttribute(hConsole, levelColor);
            std::cout << content;
            SetConsoleTextAttribute(hConsole, def);
            std::cout << "]";
        }
        else if (idx == 2)
        {
            std::cout << "[";

            size_t colon = content.find(':');
            size_t dot = content.find('.');

            if (colon != std::string::npos && dot != std::string::npos && colon > dot)
            {
                SetConsoleTextAttribute(hConsole, fileColor);
                std::cout << content.substr(0, colon);

                SetConsoleTextAttribute(hConsole, def);
                std::cout << ":";

                SetConsoleTextAttribute(hConsole, lineColor);
                std::cout << content.substr(colon + 1);

                SetConsoleTextAttribute(hConsole, def);
            }
            else
            {
                std::cout << content;
            }

            std::cout << "]";
        }
        else
        {
            std::cout << "[" << content << "]";
        }

        pos = r + 1;
        idx++;
    }

    std::cout << std::endl;
    SetConsoleTextAttribute(hConsole, def);

#else
    std::cout << formattedMessage << std::endl;
#endif
}

void Logger::WriteToFile(const std::string& formattedMessage) {
    if (mLogFile && mLogFile->is_open()) {
        *mLogFile << formattedMessage << std::endl;
        mLogFile->flush();
    }
}

std::string Logger::FormatLogMessage(const char* file, int line, LogLevel level, const std::string& message) {
    std::string result;

    // Add timestamp if enabled
    if (mShowTimeStamp) {
        result += "[" + GetCurrentTimeString() + "] ";
    }

    // Add level
    result += "[" + GetLevelString(level) + "] ";

    // Add file info if enabled
    if (mShowFileName && file && std::strlen(file) > 0) {
        std::string filename = std::filesystem::path(file).filename().string();

        result += "[" + filename;

        if (mShowLineNumber && line > 0) {
            result += ":" + std::to_string(line);
        }

        result += "] ";
    }

    result += message;

    return result;
}

std::string Logger::GetLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
        default:
            return "LOG";
    }
}

std::string Logger::GetCurrentTimeString()
{
    time_t now = time(NULL);
    struct tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &now);
#else
    localtime_r(&now, &tm_buf);
#endif

    char buffer[32] = { 0 };

    if (mShowDate)
    {
        // YYYY-MM-DD
        sprintf_s(buffer, sizeof(buffer), "%04d-%02d-%02d ", tm_buf.tm_year + 1900, tm_buf.tm_mon + 1, tm_buf.tm_mday);
    }

    char timeBuf[16];
	// HH:MM:SS
    sprintf_s(timeBuf, sizeof(timeBuf),"%02d:%02d:%02d", tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec);
    strcat_s(buffer, sizeof(buffer), timeBuf);
    return std::string(buffer);
}

#ifdef _WIN32
WORD Logger::GetLevelColor(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;  // Magenta
        case LogLevel::Info:
            return FOREGROUND_GREEN | FOREGROUND_INTENSITY;  // Green
        case LogLevel::Warning:
            return FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;  // Yellow
        case LogLevel::Error:
            return FOREGROUND_RED | FOREGROUND_INTENSITY;  // Red
        default:
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;  // White
    }
}
#endif

void __cdecl Logger::LogCFmt(const char* file, int line, LogLevel level, const char* fmt, ...)
{
    if (!fmt)
        return;

    va_list args;
    va_start(args, fmt);

    int len = _vscprintf(fmt, args);
    if (len <= 0)
    {
        va_end(args);
        Instance().WriteLog(file, line, level, " [FORMAT ERROR]");
        return;
    }

    std::string buffer;
    buffer.resize(len);
    vsprintf_s(buffer.data(), buffer.size() + 1, fmt, args);
    va_end(args);

    Instance().WriteLog(file, line, level, buffer);
}

void __cdecl Logger::InfoCFmt(const char* fmt, ...)
{
    if (!fmt)
        return;

    va_list args;
    va_start(args, fmt);
    int len = _vscprintf(fmt, args);
    if (len <= 0)
    {
        va_end(args);
        LogCFmt("", 0, LogLevel::Info, " [FORMAT ERROR]");
        return;
    }

    char* buffer = (char*)malloc(len + 1);
    if (!buffer)
    {
        va_end(args);
        LogCFmt("", 0, LogLevel::Info, " [ALLOC FAIL]");
        return;
    }

    vsprintf_s(buffer, len + 1, fmt, args);
    va_end(args);
    LogCFmt("", 0, LogLevel::Info, buffer);
    free(buffer);
}

void __cdecl Logger::DebugCFmt(const char* fmt, ...)
{
    if (!fmt)
        return;

    va_list args;
    va_start(args, fmt);
    int len = _vscprintf(fmt, args);
    if (len <= 0)
    {
        va_end(args);
        LogCFmt("", 0, LogLevel::Debug, " [FORMAT ERROR]");
        return;
    }

    char* buffer = (char*)malloc(len + 1);
    if (!buffer)
    {
        va_end(args);
        LogCFmt("", 0, LogLevel::Debug, " [ALLOC FAIL]");
        return;
    }

    vsprintf_s(buffer, len + 1, fmt, args);
    va_end(args);
    LogCFmt("", 0, LogLevel::Debug, buffer);
    free(buffer);
}

void __cdecl Logger::ErrorCFmt(const char* fmt, ...)
{
    if (!fmt)
        return;

    va_list args;
    va_start(args, fmt);
    int len = _vscprintf(fmt, args);
    if (len <= 0)
    {
        va_end(args);
        LogCFmt("", 0, LogLevel::Error, " [FORMAT ERROR]");
        return;
    }

    char* buffer = (char*)malloc(len + 1);
    if (!buffer)
    {
        va_end(args);
        LogCFmt("", 0, LogLevel::Error, " [ALLOC FAIL]");
        return;
    }

    vsprintf_s(buffer, len + 1, fmt, args);
    va_end(args);
    LogCFmt("", 0, LogLevel::Error, buffer);
    free(buffer);
}

void __cdecl Logger::WarnCFmt(const char* fmt, ...)
{
    if (!fmt)
        return;

    va_list args;
    va_start(args, fmt);
    int len = _vscprintf(fmt, args);
    if (len <= 0)
    {
        va_end(args);
        LogCFmt("", 0, LogLevel::Warning, " [FORMAT ERROR]");
        return;
    }

    char* buffer = (char*)malloc(len + 1);
    if (!buffer)
    {
        va_end(args);
        LogCFmt("", 0, LogLevel::Warning, " [ALLOC FAIL]");
        return;
    }

    vsprintf_s(buffer, len + 1, fmt, args);
    va_end(args);
    LogCFmt("", 0, LogLevel::Warning, buffer);
    free(buffer);
}
