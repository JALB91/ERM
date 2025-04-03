function(erm_gather_sources DIR_PATH)
	erm_get_dir_name_from_path(${DIR_PATH})

	# Clear eventual cached data
	unset(${DIR_NAME}_GENERATED_CPP_MODULES CACHE)
	unset(${DIR_NAME}_GENERATED_CPP_SOURCES CACHE)
	unset(${DIR_NAME}_GENERATED_CPP_HEADERS CACHE)
	unset(${DIR_NAME}_CPP_MODULES CACHE)
	unset(${DIR_NAME}_CPP_SOURCES CACHE)
	unset(${DIR_NAME}_CPP_HEADERS CACHE)
	unset(${DIR_NAME}_MAIN_CPP_SOURCES CACHE)
	unset(MAIN_CPP_SOURCES CACHE)
	unset(GENERATED_CPP_MODULES CACHE)
	unset(GENERATED_CPP_SOURCES CACHE)
	unset(GENERATED_CPP_HEADERS CACHE)
	unset(CPP_MODULES CACHE)
	unset(CPP_SOURCES CACHE)
	unset(CPP_HEADERS CACHE)

	# Gather cpp module files
	if(ERM_USE_CPP_MODULES)
		file(GLOB_RECURSE CPP_MODULES
			"${DIR_PATH}/Sources/common/*.cppm"
			"${DIR_PATH}/Sources/${ERM_RENDERING_API}/*.cppm"
		)
	endif()

	# Gather header files
	file(GLOB_RECURSE CPP_HEADERS
		"${DIR_PATH}/Sources/common/*.h"
		"${DIR_PATH}/Sources/common/*.hpp"
		"${DIR_PATH}/Sources/${ERM_RENDERING_API}/*.h"
		"${DIR_PATH}/Sources/${ERM_RENDERING_API}/*.hpp"
	)

	# Gather source files
	file(GLOB_RECURSE CPP_SOURCES
		"${DIR_PATH}/Sources/common/*.cpp"
		"${DIR_PATH}/Sources/${ERM_RENDERING_API}/*.cpp"
	)

	# Remove raytracing files if not enabled
	if(NOT ERM_RAY_TRACING_ENABLED)
		list(FILTER CPP_MODULES EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER CPP_HEADERS EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER CPP_SOURCES EXCLUDE REGEX ".*/ray_tracing/.*")
	endif()

	# Find main sources
	set(MAIN_CPP_SOURCES ${CPP_SOURCES})
	list(FILTER MAIN_CPP_SOURCES INCLUDE REGEX ".*main.cpp")
	list(FILTER CPP_SOURCES EXCLUDE REGEX ".*main.cpp")

	set(${DIR_NAME}_GENERATED_CPP_MODULES ${GENERATED_CPP_MODULES} CACHE STRING "" FORCE)
	set(${DIR_NAME}_GENERATED_CPP_SOURCES ${GENERATED_CPP_SOURCES} CACHE STRING "" FORCE)
	set(${DIR_NAME}_GENERATED_CPP_HEADERS ${GENERATED_CPP_HEADERS} CACHE STRING "" FORCE)
	set(${DIR_NAME}_CPP_MODULES ${CPP_MODULES} CACHE STRING "" FORCE)
	set(${DIR_NAME}_CPP_SOURCES ${CPP_SOURCES} CACHE STRING "" FORCE)
	set(${DIR_NAME}_CPP_HEADERS ${CPP_HEADERS} CACHE STRING "" FORCE)
	set(${DIR_NAME}_MAIN_CPP_SOURCES ${MAIN_CPP_SOURCES} CACHE STRING "" FORCE)
endfunction()

function(erm_source_group_for_file FILE)
	cmake_path(GET FILE PARENT_PATH PARENT_DIR)
	cmake_path(GET FILE EXTENSION LAST_ONLY FILE_EXTENSION)

	string(REGEX REPLACE ".*/common/" "" GROUP "${PARENT_DIR}")
	string(REGEX REPLACE ".*/${ERM_RENDERING_API}/" "" GROUP "${GROUP}")

	string(FIND "${FILE}" "/generated/" IS_GENERATED)
	string(FIND "${FILE}" "/NN_Sources/" IS_NN_SOURCE)
	string(REGEX MATCH "/.*main.cpp" IS_MAIN "${FILE}")

	if(IS_GENERATED GREATER_EQUAL 0)
		set(GROUP "Generated/${GROUP}")
	elseif(IS_NN_SOURCE GREATER_EQUAL 0)
		set(GROUP "NN_Sources/${GROUP}")
	elseif(IS_MAIN)
		set(GROUP "Main/${GROUP}")
	elseif(FILE_EXTENSION STREQUAL ".h" OR FILE_EXTENSION STREQUAL ".hpp" OR FILE_EXTENSION STREQUAL ".cpp")
		set(GROUP "Sources/${GROUP}")
	elseif(FILE_EXTENSION STREQUAL ".nn")
		set(GROUP "NN_Data/${GROUP}")
	else()
		message(WARNING "Unknown file extension ${FILE}")
	endif()

	source_group("${GROUP}" FILES "${FILE}")
endfunction()

set(ERM_GENERATED_HEADER_WARNING "/*
===============================================================
| 		  This file have been automatically generated  		  |
|            any local changes will be overridden			  |
===============================================================
*/")
