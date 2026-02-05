include(FetchContent)

FetchContent_Declare(
    curl
    URL https://github.com/curl/curl/releases/download/curl-8_1_2/curl-8.1.2.zip
)

FetchContent_MakeAvailable(curl)

list(APPEND ADDITIONAL_LIBS
   ${CURL_LIBRARIES}
)