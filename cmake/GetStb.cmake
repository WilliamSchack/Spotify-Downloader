include (FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)

FetchContent_MakeAvailable(stb)

add_library(stb)

target_include_directories(stb PUBLIC
    ${stb_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    stb
)