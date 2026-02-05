#include "JsonUtils.h"

void JsonUtils::ExtendArray(nlohmann::json& source, const nlohmann::json& other)
{
    if (source.is_null()) {
        source = other;
        return;
    }

    source.insert(source.end(), other.begin(), other.end());
}

nlohmann::json JsonUtils::SafelyNavigate(const nlohmann::json& json, std::vector<std::string> keys)
{
    nlohmann::json currentJson = json;

    for (std::string key : keys) {
        if (!currentJson.contains(key) || currentJson[key].is_null())
            return nlohmann::json::object();

        currentJson = currentJson[key];
    }

    return currentJson;
}