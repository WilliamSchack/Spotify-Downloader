#ifndef ENVIRONMENTVARIABLE_H
#define ENVIRONMENTVARIABLE

template<typename T>
struct EnvironmentVariable
{
    T Value;
    bool Exists;
};

#endif