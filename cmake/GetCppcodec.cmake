include (FetchContent)

FetchContent_Declare(
    ccpcodec
    GIT_REPOSITORY https://github.com/tplgy/cppcodec.git
)

FetchContent_MakeAvailable(ccpcodec)

add_library(ccpcodec)

target_include_directories(ccpcodec PUBLIC
    ${ccpcodec_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ccpcodec
)