include (FetchContent)

FetchContent_Declare(
    lexbor
    GIT_REPOSITORY https://github.com/lexbor/lexbor.git
)

FetchContent_MakeAvailable(lexbor)

target_link_libraries(${PROJECT_NAME} PRIVATE
    lexbor
)