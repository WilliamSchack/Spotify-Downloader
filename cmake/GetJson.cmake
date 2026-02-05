include (FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
)

FetchContent_MakeAvailable(nlohmann_json)

list(APPEND ADDITIONAL_LIBS
    nlohmann_json::nlohmann_json
)