#include "JsonUtils.h"

void JsonUtils::ExtendArray(nlohmann::json& source, const nlohmann::json& other)
{
    if (source.is_null()) {
        source = other;
        return;
    }

    source.insert(source.end(), other.begin(), other.end());
}