#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <nlohmann/json.hpp>

#include <vector>
#include <variant>

class JsonUtils
{
    public:
        static void ExtendArray(nlohmann::json& source, const nlohmann::json& other);
        static nlohmann::json SafelyNavigate(const nlohmann::json& json, std::vector<std::variant<std::string, int>> keys);

        template <typename T>
        static T SafelyNavigate(const nlohmann::json& json, std::vector<std::variant<std::string, int>> keys)
        {
            nlohmann::json newJson = SafelyNavigate(json, keys);
			if (newJson.empty()) return T();
            return newJson.get<T>();
        }
};

#endif