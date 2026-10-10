target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ITagHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TagHandlerResult.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TagReplacerResult.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackTagHandler.h
)