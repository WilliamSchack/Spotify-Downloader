#ifndef JSONUTILS_H
#define JSONUTILS_H

#include "VectorUtils.h"

#include <nlohmann/json.hpp>

#include <vector>
#include <variant>

class JsonUtils
{
    public:
        static void ExtendArray(nlohmann::json& source, const nlohmann::json& other);
        static nlohmann::json SafelyNavigate(const nlohmann::json& json, const std::vector<std::variant<std::string, int>>& keys);

        template <typename T>
        static T SafelyNavigate(const nlohmann::json& json, const std::vector<std::variant<std::string, int>>& keys, T defaultValue = T())
        {
            nlohmann::json newJson = SafelyNavigate(json, keys);
            if (newJson.is_null() || newJson.empty()) return defaultValue;
            return newJson.get<T>();
        }

        // Reads an enum stored as a string, assumes the enumStrings has the same order as the enum
        template <typename T>
        static T ReadStringEnum(const nlohmann::json& json, const std::vector<std::variant<std::string, int>>& keys, const std::vector<std::string>& enumStrings, T defaultValue)
        {
            std::string enumString = JsonUtils::SafelyNavigate<std::string>(json, keys);
            int indexOfEnum = VectorUtils::IndexOf(enumStrings, enumString);
            if (indexOfEnum >= 0)
                return static_cast<T>(indexOfEnum);

            return defaultValue;
        }
};

#endif