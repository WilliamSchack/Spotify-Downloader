#ifndef VECTORUTILS_H
#define VECTORUTILS_H

#include <vector>

class VectorUtils
{
    public:
        template<typename T>
        static bool Contains(const std::vector<T>& vector, const T& value)
        {
            return std::find(vector.begin(), vector.end(), value) != vector.end();
        }

        template<typename T>
        static void Remove(std::vector<T>& vector, const T& value)
        {
            if (!Contains(vector, value)) return;
            vector.erase(std::find(vector.begin(), vector.end(), value));
        }
};

#endif