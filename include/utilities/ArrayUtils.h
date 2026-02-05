#ifndef ARRAYUTILS_H
#define ARRAYUTILS_H

#include <algorithm>

class ArrayUtils
{
    public:
        template<class C, typename T>
        static bool Contains(const C& a, const T& e)
        {
            return std::find(std::begin(a), std::end(a), e) != std::end(a);
        }
};

#endif