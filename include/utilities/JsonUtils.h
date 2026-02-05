#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <nlohmann/json.hpp>

class JsonUtils
{
    public:
        static void ExtendArray(nlohmann::json& source, const nlohmann::json& other);
};

#endif