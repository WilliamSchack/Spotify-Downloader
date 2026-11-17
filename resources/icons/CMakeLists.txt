target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/icons.qrc
)

qt_add_resources(QT_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/icons.qrc)

if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE
	    ${CMAKE_CURRENT_SOURCE_DIR}/Icon.rc
    )
elseif (APPLE)
    target_sources(${PROJECT_NAME} PRIVATE
	    ${CMAKE_CURRENT_SOURCE_DIR}/Icon.icns
    )
endif()