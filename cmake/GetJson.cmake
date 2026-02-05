include (FetchContent)

FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)

FetchContent_MakeAvailable(nlohmann_json)

list(APPEND ADDITIONAL_LIBS
    nlohmann_json::nlohmann_json
)