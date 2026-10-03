#ifndef EASYQPROPERTIES_H
#define EASYQPROPERTIES_H

// These just make it easy to repeat properties without manually defining each function
// Mainly created to avoid rewriting each config property

#include <QMetaEnum>
#include <QStringList>

class EasyQProperties {
    template<typename T>
    static QStringList GetEnumOptions()
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<T>();
        QStringList options;

        for (int i = 0; i < metaEnum.keyCount(); i++)
            options.append(QString::fromLatin1(metaEnum.key(i)));

        return options;
    }
};

#define SIMPLE_QPROPERTY(type, qmlName, functionName, variable) \
    Q_PROPERTY(type qmlName READ Get##functionName WRITE Set##functionName NOTIFY functionName##Changed) \
    signals: \
        void functionName##Changed(); \
    public: \
        type Get##functionName() const { return variable; } \
        void Set##functionName(type value) { \
            if (variable == value) return; \
            variable = value; \
            emit functionName##Changed(); \
        }

#define ENUM_QPROPERTY(type, qmlName, functionName, variable) \
    Q_ENUM(type) \
    SIMPLE_QPROPERTY(type, qmlName, functionName, variable) \
    public: \
        Q_INVOKABLE QStringList Get##functionName##Options() const { return EasyQProperties::GetEnumOptions<type>(); }

#endif