#include "Logger.h"

void Logger::Init()
{
    std::lock_guard<std::mutex> lock(_mutex);

    std::filesystem::path path = GetFilePath();
    if (path.empty())
        return;

    // Remove previous log
    if (std::filesystem::exists(path)) {
        std::filesystem::path newPath = path.parent_path() / "PreviousLog.log";

        // FileUtils::MoveFile
        try {
            std::filesystem::rename(path, newPath);
        } catch (...) {
            // If rename fails, assume it is between drives, in that case copy
            bool copied = std::filesystem::copy_file(path, newPath);
            std::filesystem::remove(path);

            // Move error
        }
    }

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
    std::cerr << message << "\n";
}

void Logger::Info(const std::string& message)
{
    Log(ELogLevel::Info, message);
    std::cerr << message << "\n";
}

void Logger::Warning(const std::string& message)
{
    Log(ELogLevel::Warning, message);
    std::cerr << message << "\n";
}

void Logger::Error(const std::string& message)
{
    Log(ELogLevel::Error, message);
    std::cerr << message << "\n";
}

void Logger::Fatal(const std::string& message)
{
    Log(ELogLevel::Fatal, message);
    std::cerr << message << "\n";
}

void Logger::Out(const std::string& message)
{
    Log(ELogLevel::Out, message);
    std::cout << message << "\n";
}

void Logger::Log(const ELogLevel& logLevel, const std::string& message)
{
    std::lock_guard<std::mutex> lock(_mutex);

    std::ostringstream entry;
    entry << "[" << GetTimestamp() << " | " << GetLogLevelString(logLevel) << "] " << message << "\n";

    if (_file.is_open()) {
        _file << entry.str();

        // Make sure the log file is up to date with errors
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

    std::filesystem::path configPath = GetLogsDir();
    if (configPath.empty())
        return "";

    _filePathCache = configPath / FILE_NAME;
    return _filePathCache;
}

std::filesystem::path Logger::GetLogsDir()
{
    // Would use paths but I want to use the logger in FileUtils so its defined here
    // just means a little bit of duplicate code

    // Paths::GetConfigDir
    std::filesystem::path configDir = "";

#ifdef WIN32
    EnvironmentVariable<std::filesystem::path> appdataPath = EnvironmentUtils::GetVariablePath("APPDATA");
    configDir = appdataPath.Value / APP_NAME;
#elif __APPLE__
    EnvironmentVariable<std::filesystem::path> homePath = EnvironmentUtils::GetVariablePath("HOME");
    std::filesystem::path configPath = homePath.Value / ".config";
    configDir = configPath / APP_NAME;
#else
    EnvironmentVariable<std::filesystem::path> xgdConfigPath = EnvironmentUtils::GetVariablePath("XDG_CONFIG_HOME");
    std::filesystem::path configPath = xgdConfigPath.Value;
    if (!xgdConfigPath.Exists) {
        EnvironmentVariable<std::filesystem::path> homePath = EnvironmentUtils::GetVariablePath("HOME");
        configPath = homePath.Value / ".config";
    }

    configDir = configPath / APP_NAME;
#endif

    if (!std::filesystem::exists(configDir))
        std::filesystem::create_directories(configDir);

    // Get logs dir
    std::filesystem::path logsFolder = configDir / LOGS_FOLDER_NAME;
    if (!std::filesystem::exists(logsFolder))
        std::filesystem::create_directory(logsFolder);

    return logsFolder;
}