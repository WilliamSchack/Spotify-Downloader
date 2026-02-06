#include "JsonUtils.h"

void JsonUtils::ExtendArray(nlohmann::json& source, const nlohmann::json& other)
{
    if (source.is_null()) {
        source = other;
        return;
    }

    source.insert(source.end(), other.begin(), other.end());
}

nlohmann::json JsonUtils::SafelyNavigate(const nlohmann::json& json, const std::vector<std::variant<std::string, int>>& keys)
{
    nlohmann::json currentJson = json;

    for (std::variant<std::string, int> key : keys) {
        // Object Key
        if (std::holds_alternative<std::string>(key)) {
            std::string stringKey = std::get<std::string>(key);
            if (!currentJson.contains(stringKey) || currentJson[stringKey].is_null())
                return nlohmann::json::object();

            currentJson = currentJson[stringKey];
        }

        // Array Index
        else {
            int intKey = std::get<int>(key);
            if (!currentJson.is_array() || currentJson.size() <= intKey || currentJson[intKey].is_null())
                return nlohmann::json::object();

            currentJson = currentJson[intKey];
        }
    }

    return currentJson;
}