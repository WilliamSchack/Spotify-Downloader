#ifndef EASYQPROPERTIES_H
#define EASYQPROPERTIES_H

// These just make it easy to repeat properties without manually defining each function
// Mainly created to avoid rewriting each config property

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

// Makes an int property
// Could use Q_ENUM but I would need to change every enum file and pollute the c++ only code with qt
#define ENUM_QPROPERTY(enumType, enumStrings, qmlName, functionName, variable) \
    Q_PROPERTY(int qmlName READ Get##functionName WRITE Set##functionName NOTIFY functionName##Changed) \
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
        Q_INVOKABLE QStringList Get##functionName##Options() const { return EasyQProperties::GetEnumStrings(enumStrings); }

#endif