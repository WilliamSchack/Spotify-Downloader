#ifndef MAPUTILS_H
#define MAPUTILS_H

#include <map>

class MapUtils
{
    public:
        template<typename K, typename V>
        static bool ContainsKey(const std::map<K, V>& map, const K& value)
        {
            for (auto element : map) {
                if (element.first == value)
                    return true;
            }

            return false;
        };
};

#endif