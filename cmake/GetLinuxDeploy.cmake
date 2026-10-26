# Assumes an OS that supports the AppImage format as LinuxDeploy is an AppImage
# Creates variables:
# LINUXDEPLOY_EXE   | Path to linuxdeployqt
# LINUXDEPLOYQT_EXE | Path to linuxdeploy qt plugin
# Assumed variables:
# BINARIES_DIR      | Directory to external binaries, assumes folder is created

# Setup paths
set(LD_URL "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy")
set(LD_QT_URL "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt")

if(IS_ARM_CPU)
    set(LD_URL "${LD_URL}-aarch64.AppImage")
    set(LD_QT_URL "${LD_QT_URL}-aarch64.AppImage")
else()
    set(LD_URL "${LD_URL}-x86_64.AppImage")
    set(LD_QT_URL "${LD_QT_URL}-x86_64.AppImage")
endif()

set(LD_DIR "${BINARIES_DIR}/linuxdeploy" )
set(LD_PATH "${LD_DIR}/linuxdeploy.AppImage")
set(LD_QT_PATH "${LD_DIR}/linuxdeploy-plugin-qt.AppImage")

file(MAKE_DIRECTORY ${LD_DIR})

if(NOT EXISTS ${LD_PATH})
	message(STATUS "Downloading linuxdeploy...")
	file(DOWNLOAD ${LD_URL} ${LD_PATH} SHOW_PROGRESS)
	message(STATUS "Downloaded linuxdeploy to ${LD_PATH}")

	# Give executable permissions
	file(CHMOD ${LD_PATH} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
endif()

if(NOT EXISTS ${LD_QT_PATH})
	message(STATUS "Downloading linuxdeploy qt plugin...")
	file(DOWNLOAD ${LD_QT_URL} ${LD_QT_PATH} SHOW_PROGRESS)
	message(STATUS "Downloaded linuxdeploy qt plugin to ${LD_QT_PATH}")

	# Give executable permissions
	file(CHMOD ${LD_QT_PATH} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
endif()

# Create variables
set(LINUXDEPLOY_EXE ${LD_PATH} CACHE INTERNAL "Path to linuxdeploy")
set(LINUXDEPLOYQT_EXE ${LD_QT_PATH} CACHE INTERNAL "Path to linuxdeploy qt plugin")