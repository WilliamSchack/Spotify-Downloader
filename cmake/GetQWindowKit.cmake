include (FetchContent)

set(QWINDOWKIT_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(QWINDOWKIT_BUILD_WIDGETS OFF CACHE BOOL "" FORCE)
set(QWINDOWKIT_BUILD_QUICK ON CACHE BOOL "" FORCE)
set(QWINDOWKIT_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    QWindowKit
    GIT_REPOSITORY https://github.com/stdware/qwindowkit.git
    GIT_TAG 1.5.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(QWindowKit)

target_link_libraries(${PROJECT_NAME} PRIVATE QWindowKit::Quick)