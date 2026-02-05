include(FetchContent)

FetchContent_Declare(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
)

FetchContent_MakeAvailable(curl)

list(APPEND ADDITIONAL_LIBS
   ${CURL_LIBRARIES}
)