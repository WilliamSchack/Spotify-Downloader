#ifndef LOGGER_H
#define LOGGER_H

#include "EnvironmentUtils.h"

#include <iostream>
#include <fstream>
#include <mutex>
#include <ctime>
#include <exception>

#include <QDebug>

class Logger
{
    public:
        enum class ELogLevel
        {
            Debug,
            Info,
            Warning,
            Error,
            Fatal,
            Out
        };
    public:
        static void Init();
        static void Flush();

        static void Debug(const std::string& message);
        static void Info(const std::string& message);
        static void Warning(const std::string& message);
        static void Error(const std::string& message);
        static void Fatal(const std::string& message);
        static void Out(const std::string& message);

        static std::filesystem::path GetFilePath();
        static std::filesystem::path GetLogsDir();
    private:
        static void Log(const ELogLevel& logLevel, const std::string& message);

        static void TerminateHandler();
        static void SignalHandler(int signal);
        static void QtMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message);

        static std::string GetLogLevelString(const ELogLevel& logLevel);
        static std::string GetTimestamp();
    private:
        static inline const std::string LOGS_FOLDER_NAME = "Logs";
        static inline const std::string FILE_NAME = "Log.log";

        static inline std::filesystem::path _filePathCache = "";
        static inline std::ofstream _file;
        static inline std::mutex _mutex;
};

#endif