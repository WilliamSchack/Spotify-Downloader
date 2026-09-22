target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/gui.qrc
)

qt6_add_resources(QT_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/gui.qrc)