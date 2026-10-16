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
    const nlohmann::json* currentJson = &json;

    for (const std::variant<std::string, int>& key : keys) {
        // Object Key
        if (std::holds_alternative<std::string>(key)) {
            const std::string& stringKey = std::get<std::string>(key);
            if (!currentJson->contains(stringKey))
                return nlohmann::json::object();

            auto it = currentJson->find(stringKey);
            if (it == currentJson->end() || it->is_null())
                return nlohmann::json::object();

            currentJson = &*it;
        }

        // Array Index
        else {
            int intKey = std::get<int>(key);
            if (!currentJson->is_array() || intKey < 0 || currentJson->size() <= static_cast<size_t>(intKey) || (*currentJson)[intKey].is_null())
                return nlohmann::json::object();

            currentJson = &(*currentJson)[intKey];
        }
    }

    return *currentJson;
}