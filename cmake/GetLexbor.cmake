include (FetchContent)

FetchContent_Declare(
    lexbor
    GIT_REPOSITORY https://github.com/lexbor/lexbor.git
)

FetchContent_MakeAvailable(lexbor)

list(APPEND ADDITIONAL_LIBS
    lexbor
)