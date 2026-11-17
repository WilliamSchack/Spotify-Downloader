#ifndef ENVIRONMENTVARIABLE_H
#define ENVIRONMENTVARIABLE_H

template<typename T>
struct EnvironmentVariable
{
    T Value;
    bool Exists;
};

#endif