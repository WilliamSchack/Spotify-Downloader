include (FetchContent)

FetchContent_Declare(
    lexbor
    GIT_REPOSITORY https://github.com/lexbor/lexbor.git
)

FetchContent_MakeAvailable(lexbor)

target_link_libraries(${PROJECT_NAME} PRIVATE
    lexbor
)

if(WIN32)
    # Copy dll
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:lexbor>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()