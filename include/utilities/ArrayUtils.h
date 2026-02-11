#ifndef ARRAYUTILS_H
#define ARRAYUTILS_H

#include <vector>
#include <algorithm>

class ArrayUtils
{
    public:
        template<class C, typename T>
        static bool Contains(const C& a, const T& e)
        {
            return std::find(std::begin(a), std::end(a), e) != std::end(a);
        }

        template<typename T>
        static void ExtendVector(std::vector<T>& original, const std::vector<T>& src)
        {
            original.reserve(original.size() + distance(src.begin(),src.end()));
            original.insert(original.end(),src.begin(),src.end());
        }

        template<typename T>
        static void RemoveAt(std::vector<T>& vector, const unsigned int& index)
        {
            vector.erase(vector.begin() + index);
        }
};

#endif