#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <nlohmann/json.hpp>

#include <vector>

class JsonUtils
{
    public:
        static void ExtendArray(nlohmann::json& source, const nlohmann::json& other);
        static nlohmann::json SafelyNavigate(const nlohmann::json& json, std::vector<std::string> keys);
};

#endif