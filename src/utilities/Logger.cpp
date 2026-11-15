#include "Logger.h"

void Logger::Init()
{
    std::lock_guard<std::mutex> lock(_mutex);

    std::filesystem::path path = GetFilePath();
    if (path.empty())
        return;

    // Remove previous log
    if (std::filesystem::exists(path))
        FileUtils::MoveFile(path, path.parent_path() / "PreviousLog.log");

    _file = std::ofstream(path);
    if (!_file.is_open()) {
        std::cerr << "Failed to open the log file, logs will not be output" << std::endl;
        return;
    }

    std::set_terminate(&Logger::TerminateHandler);
    qInstallMessageHandler(&Logger::QtMessageHandler);
}

void Logger::Flush()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_file.is_open())
        return;
    
    _file.flush();
}

void Logger::Debug(const std::string& message)
{
    Log(ELogLevel::Debug, message);
    std::cerr << message << std::endl;
}

void Logger::Info(const std::string& message)
{
    Log(ELogLevel::Info, message);
    std::cerr << message << std::endl;
}

void Logger::Warning(const std::string& message)
{
    Log(ELogLevel::Warning, message);
    std::cerr << message << std::endl;
}

void Logger::Error(const std::string& message)
{
    Log(ELogLevel::Error, message);
    std::cerr << message << std::endl;
}

void Logger::Fatal(const std::string& message)
{
    Log(ELogLevel::Fatal, message);
    std::cerr << message << std::endl;
}

void Logger::Out(const std::string& message)
{
    Log(ELogLevel::Out, message);
    std::cout << message << std::endl;
}

void Logger::Log(const ELogLevel& logLevel, const std::string& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    std::ostringstream entry;
    entry << "[" << GetTimestamp() << " | " << GetLogLevelString(logLevel) << "] " << message << "\n";

    if (_file.is_open()) {
        _file << entry.str();

        // Flush on errors or higher
        // The file will flush on exit
        if (logLevel >= ELogLevel::Error)
            _file.flush();
    }
}

void Logger::TerminateHandler()
{
    std::exception_ptr exceptionPointer = std::current_exception();
    if (exceptionPointer != nullptr) {
        try {
            std::rethrow_exception(exceptionPointer);
        } catch (const std::exception& e) {
            Fatal("Exception: " + std::string(e.what()));
        } catch (...) {
            Fatal("Unknown exception");
        }
    } else {
        Fatal("terminate called without an exception");
    }

    Flush();
    std::abort();
}

void Logger::QtMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    ELogLevel logLevel = ELogLevel::Debug;
    switch (type) {
        case QtMsgType::QtDebugMsg:    logLevel = ELogLevel::Debug; break;
        case QtMsgType::QtInfoMsg:     logLevel = ELogLevel::Info; break;
        case QtMsgType::QtWarningMsg:  logLevel = ELogLevel::Warning; break;
        case QtMsgType::QtCriticalMsg: logLevel = ELogLevel::Error; break;
        case QtMsgType::QtFatalMsg:    logLevel = ELogLevel::Fatal; break;
    }
    
    std::string messageString = message.toStdString();

    Log(logLevel, messageString);
    std::cerr << messageString << std::endl;
}

std::string Logger::GetLogLevelString(const ELogLevel& logLevel)
{
    switch (logLevel) {
        case ELogLevel::Debug:   return "DEBUG";
        case ELogLevel::Info:    return "INFO";
        case ELogLevel::Out:     return "OUT";
        case ELogLevel::Warning: return "WARNING";
        case ELogLevel::Error:   return "ERROR";
        case ELogLevel::Fatal:   return "FATAL";
    }

    return "";
}

std::string Logger::GetTimestamp()
{
    std::time_t time = std::time(NULL);
    std::tm* now = std::localtime(&time);

    std::ostringstream timeString;
    timeString << std::put_time(now, "%Y-%m-%d %H:%M:%S");

    return timeString.str();
}

std::filesystem::path Logger::GetFilePath()
{
    if (!_filePathCache.empty())
        return _filePathCache;

    std::filesystem::path configPath = Paths::GetLogsDir();
    if (configPath.empty())
        return "";

    _filePathCache = configPath / FILE_NAME;
    return _filePathCache;
}