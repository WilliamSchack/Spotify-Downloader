# Assumes an OS that supports the AppImage format as this builds to AppImage
# Creates variables:
# LINUXDEPLOYQT_EXECUTABLE | Path to linuxdeployqt
# Assumed variables:
# BINARIES_DIR             | Directory to external binaries, assumes folder is created

# Setup paths
set(LDQT_URL "https://github.com/probonopd/linuxdeployqt/releases/latest/download/linuxdeployqt-continuous-x86_64.AppImage")
set(LDQT_PATH "${BINARIES_DIR}/linuxdeployqt-continuous-x86_64.AppImage")

if(NOT EXISTS ${LDQT_PATH})
	message(STATUS "Downloading linuxdeployqt...")
	file(DOWNLOAD ${LDQT_URL} ${LDQT_PATH} SHOW_PROGRESS)
	message(STATUS "Downloaded linuxdeployqt to ${LDQT_PATH}")

	# Give executable permissions
	file(CHMOD ${LDQT_PATH} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
endif()

# Create variable for easy access
set(LINUXDEPLOYQT_EXECUTABLE ${LDQT_PATH} CACHE INTERNAL "Path to linuxdeployqt")
