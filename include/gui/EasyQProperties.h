#ifndef EASYQPROPERTIES_H
#define EASYQPROPERTIES_H

// These just make it easy to repeat properties without manually defining each function
// Mainly created to avoid rewriting each config property

#include "Config.h"
#include "TrackData.h"
#include "FileUtils.h"

#include <QStringList>

class EasyQProperties {
    public:
        // Function is really just a convert vector to stringlist but whatever
        static QStringList GetEnumStrings(std::vector<std::string> enumStrings)
        {
            QStringList options;
            for (const std::string& enumName : enumStrings)
                options.append(QString::fromStdString(enumName));

            return options;
        }
};

// Needs to use Q_SIGNALS, signals: does not work for some reason
#define SIMPLE_QPROPERTY(type, qmlName, functionName, variable) \
    Q_PROPERTY(type qmlName READ Get##functionName WRITE Set##functionName NOTIFY functionName##Changed) \
    Q_SIGNALS: \
        void functionName##Changed(); \
    public: \
        type Get##functionName() const { return variable; } \
        void Set##functionName(type value) { \
            if (variable == value) return; \
            variable = value; \
            emit functionName##Changed(); \
        }

// Converts std::string to QString
#define STRING_QPROPERTY(qmlName, functionName, variable) \
    Q_PROPERTY(QString qmlName READ Get##functionName WRITE Set##functionName NOTIFY functionName##Changed) \
    Q_SIGNALS: \
        void functionName##Changed(); \
    public: \
        QString Get##functionName() const { return QString::fromStdString(variable); } \
        void Set##functionName(QString value) { \
            std::string castedValue = value.toStdString(); \
            if (variable == castedValue) return; \
            variable = castedValue; \
            emit functionName##Changed(); \
        }

// Converts std::filesystem::path to QString
#define PATH_QPROPERTY(qmlName, functionName, variable) \
    Q_PROPERTY(QString qmlName READ Get##functionName WRITE Set##functionName NOTIFY functionName##Changed) \
    Q_SIGNALS: \
        void functionName##Changed(); \
    public: \
        QString Get##functionName() const { return QString::fromStdString(FileUtils::PathToUtf8(variable)); } \
        void Set##functionName(QString value) { \
            std::filesystem::path castedValue = FileUtils::PathFromUtf8(value.toStdString()); \
            if (variable == castedValue) return; \
            variable = castedValue; \
            emit functionName##Changed(); \
        }

// Converts char to QString, storing as ' ' when empty. This also makes whitespaces invalid characters
#define CHAR_QPROPERTY(qmlName, functionName, variable) \
    Q_PROPERTY(QString qmlName READ Get##functionName WRITE Set##functionName NOTIFY functionName##Changed) \
    Q_SIGNALS: \
        void functionName##Changed(); \
    public: \
        QString Get##functionName() const { return std::isblank(variable) ? "" : QString(QChar(variable)); } \
        void Set##functionName(QString value) { \
            char castedValue = value.isEmpty() ? ' ' : value.at(0).toLatin1(); \
            bool isBlank = std::isblank(castedValue); \
            if (isBlank) castedValue = ' '; \
            if (variable == castedValue && !isBlank) return; \
            variable = castedValue; \
            emit functionName##Changed(); \
        }

// Makes the options of an enum available to qml
#define REGISTER_QENUM(enumStrings, functionName)  \
    public: \
        Q_INVOKABLE QStringList Get##functionName##Options() const { return EasyQProperties::GetEnumStrings(enumStrings); }

// Makes an int property
// Could use Q_ENUM but I would need to change every enum file and pollute the c++ only code with qt
#define ENUM_QPROPERTY(enumType, enumStrings, qmlName, functionName, variable) \
    Q_PROPERTY(int qmlName READ Get##functionName WRITE Set##functionName NOTIFY functionName##Changed) \
    REGISTER_QENUM(enumStrings, functionName) \
    Q_SIGNALS: \
        void functionName##Changed(); \
    public: \
        int Get##functionName() const { return static_cast<int>(variable); } \
        void Set##functionName(int value) { \
            enumType castedValue = static_cast<enumType>(value); \
            if (variable == castedValue) return; \
            variable = castedValue; \
            emit functionName##Changed(); \
        } \
        

// Used to easily create the tag entries since there is a fair bit to do for them
// This is really only for the SettingsScreenGUI but its cleaner to keep it here
// Extra args after tagHandlerType are used in the constructor as tagHandlerType(a, b, c, ...)
// Creates private variables:
// char _{qmlName}TagsOpeningChar
// char _{qmlName}TagsOpeningChar
// std::string _{qmlName}
// bool _{qmlName}Valid
// Creates private functions to setup the variables, and save them to the config
// Setup{functionName}
// Save{functionName}
// Uses the following config vars:
// char {configName}TagsOpeningChar
// char {configName}TagsClosingChar
// std::string {configName}
#define TAG_QPROPERTY(qmlName, configName, functionName, className, tagHandlerType, ...) \
    CHAR_QPROPERTY(qmlName##TagsOpeningChar, functionName##TagsOpeningChar, _##qmlName##TagsOpeningChar) \
    CHAR_QPROPERTY(qmlName##TagsClosingChar, functionName##TagsClosingChar, _##qmlName##TagsClosingChar) \
    STRING_QPROPERTY(qmlName, functionName, _##qmlName) \
    Q_PROPERTY(QString qmlName##Error READ Get##functionName##Error NOTIFY functionName##ErrorChanged) \
    Q_SIGNALS: \
        void functionName##ErrorChanged(); \
    public: \
        QString Get##functionName##Error() { \
            tagHandlerType tagHandler{__VA_ARGS__}; \
            TagHandlerResult result = tagHandler.FormatString(_##qmlName, _##qmlName##TagsOpeningChar, _##qmlName##TagsClosingChar); \
            _##qmlName##Error = QString::fromStdString(result.ErrorString); \
            return _##qmlName##Error; \
        }; \
    private: \
        void Setup##functionName() { \
            connect(this, &className::functionName##TagsOpeningCharChanged, this, &className::functionName##ErrorChanged); \
            connect(this, &className::functionName##TagsClosingCharChanged, this, &className::functionName##ErrorChanged); \
            connect(this, &className::functionName##Changed, this, &className::functionName##ErrorChanged); \
            _##qmlName##TagsOpeningChar = Config::configName##TagsOpeningChar; \
            _##qmlName##TagsClosingChar = Config::configName##TagsClosingChar; \
            _##qmlName = Config::configName; \
        }; \
        void Save##functionName() { \
            Config::configName##TagsOpeningChar = _##qmlName##TagsOpeningChar; \
            Config::configName##TagsClosingChar = _##qmlName##TagsClosingChar; \
            Config::configName = _##qmlName; \
        }; \
    private: \
        char _##qmlName##TagsOpeningChar; \
        char _##qmlName##TagsClosingChar; \
        std::string _##qmlName; \
        QString _##qmlName##Error;

#endif