# - Try to find ZBar
# This will define
#
# ZBAR_FOUND -
# ZBAR_LIBRARY_DIR -
# ZBAR_INCLUDE_DIR -
# ZBAR_LIBRARIES -
#

if(WIN32)
	find_library(ZBAR_LIBRARIES NAMES libzbar64-0
	HINTS ${PC_ZBAR_LIBDIR} ${PC_ZBAR_LIBRARY_DIRS} )
	find_path(ZBAR_INCLUDE_DIR zbar.h
	HINTS ${PC_ZBAR_INCLUDEDIR} ${PC_ZBAR_INCLUDE_DIRS})
		
else()
	find_package(PkgConfig)
	pkg_check_modules(PC_ZBAR QUIET zbar)
	set(ZBAR_DEFINITIONS ${PC_ZBAR_CFLAGS_OTHER})
	find_library(ZBAR_LIBRARIES NAMES zbar
	HINTS ${PC_ZBAR_LIBDIR} ${PC_ZBAR_LIBRARY_DIRS} )
	find_path(ZBAR_INCLUDE_DIR Decoder.h
	HINTS ${PC_ZBAR_INCLUDEDIR} ${PC_ZBAR_INCLUDE_DIRS})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ZBAR DEFAULT_MSG ZBAR_LIBRARIES ZBAR_INCLUDE_DIR)