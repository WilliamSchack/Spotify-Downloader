#ifndef VECTORUTILS_H
#define VECTORUTILS_H

#include <vector>
#include <algorithm>

class VectorUtils
{
    public:
        template<typename T>
        static bool Contains(const std::vector<T>& vector, const T& value)
        {
            return std::find(vector.begin(), vector.end(), value) != vector.end();
        }

        // Returns -1 if not found
        template<typename T>
        static int IndexOf(const std::vector<T>& vector, const T& value)
        {
            auto it = std::find(vector.begin(), vector.end(), value);
            if (it == vector.end())
                return -1;

            return std::distance(vector.begin(), it);
        }

        template<typename T>
        static void Remove(std::vector<T>& vector, const T& value)
        {
            if (!Contains(vector, value)) return;
            vector.erase(std::find(vector.begin(), vector.end(), value));
        }
};

#endif