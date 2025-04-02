function(erm_gather_modules OUT_VAR)
	erm_gather_directories(DIRS)
	foreach(DIR_PATH ${DIRS})
		cmake_path(GET DIR_PATH STEM DIR_NAME)
		if(NOT "${DIR_NAME}" STREQUAL "cmake")
			list(APPEND MODULES "${DIR_PATH}")
		endif()
	endforeach()
	set("${OUT_VAR}" ${MODULES} PARENT_SCOPE)
endfunction()

function(erm_gather_sources DIR_PATH)
	erm_get_dir_name_from_path(${DIR_PATH})

	# Clear eventual cached data
	unset(MAIN_CPP_SOURCES)
	unset(CPP_MODULES)
	unset(CPP_SOURCES)
	unset(CPP_HEADERS)
	unset(NN_DATA)
	unset(NN_SOURCES)
	unset(GENERATED_CPP_MODULES)
	unset(GENERATED_CPP_SOURCES)
	unset(GENERATED_CPP_HEADERS)
	unset(ALL_SOURCES)

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

	# Gather nn data files
	file(GLOB_RECURSE NN_DATA
		"${DIR_PATH}/NN_Data/common/*.nn"
		"${DIR_PATH}/NN_Data/${ERM_RENDERING_API}/*.nn"
	)

	# Gather nn source files
	file(GLOB_RECURSE NN_SOURCES
		"${DIR_PATH}/NN_Sources/common/*.h"
		"${DIR_PATH}/NN_Sources/common/*.hpp"
		"${DIR_PATH}/NN_Sources/common/*.cpp"
		"${DIR_PATH}/NN_Sources/${ERM_RENDERING_API}/*.h"
		"${DIR_PATH}/NN_Sources/${ERM_RENDERING_API}/*.hpp"
		"${DIR_PATH}/NN_Sources/${ERM_RENDERING_API}/*.cpp"
	)

	# Remove raytracing files if not enabled
	if(NOT ERM_RAY_TRACING_ENABLED)
		list(FILTER CPP_MODULES EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER CPP_HEADERS EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER CPP_SOURCES EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER NN_DATA EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER NN_SOURCES EXCLUDE REGEX ".*/ray_tracing/.*")
	endif()

	# Add module header
	set(MODULE_HEADER "${ERM_GENERATED_DIR}/${DIR_NAME}/common/erm/${DIR_NAME}.h")
	list(APPEND CPP_HEADERS "${MODULE_HEADER}")
	list(APPEND GENERATED_CPP_SOURCES "${MODULE_HEADER}")

	# Add dummy file
	# set(DUMMY_FILE "${ERM_GENERATED_DIR}/${DIR_NAME}/common/erm/dummy.cpp")
	# list(APPEND CPP_SOURCES "${DUMMY_FILE}")
	# list(APPEND GENERATED_CPP_SOURCES "${DUMMY_FILE}")

	# Gather generated files
	foreach(FILE ${NN_DATA})
		string(REGEX REPLACE ".*/${DIR_NAME}/" "${ERM_GENERATED_DIR}/${DIR_NAME}/" FILE "${FILE}")
		string(REPLACE "/NN_Data" "" FILE "${FILE}")

		# Generated .cpp
		string(REPLACE ".nn" ".cpp" NN_GEN_SOURCE "${FILE}")
		list(APPEND CPP_SOURCES ${NN_GEN_SOURCE})
		list(APPEND GENERATED_CPP_SOURCES ${NN_GEN_SOURCE})

		if(ERM_USE_CPP_MODULES)
			# Generated .cppm
			string(REPLACE ".nn" ".cppm" NN_GEN_MODULE "${FILE}")
			list(APPEND CPP_MODULES ${NN_GEN_MODULE})
			list(APPEND GENERATED_CPP_MODULES ${NN_GEN_MODULE})
		else()
			# Generated .h
			string(REPLACE ".nn" ".h" NN_GEN_HEADER "${FILE}")
			list(APPEND CPP_HEADERS ${NN_GEN_HEADER})
			list(APPEND GENERATED_CPP_HEADERS ${NN_GEN_HEADER})
		endif()
	endforeach()

	# Find main sources
	set(MAIN_CPP_SOURCES ${CPP_SOURCES})
	list(FILTER MAIN_CPP_SOURCES INCLUDE REGEX ".*main.cpp")
	list(FILTER CPP_SOURCES EXCLUDE REGEX ".*main.cpp")

	list(APPEND ALL_SOURCES 
		${CPP_MODULES} 
		${CPP_HEADERS} 
		${CPP_SOURCES} 
		${NN_DATA} 
		${GENERATED_CPP_MODULES}
		${GENERATED_CPP_SOURCES} 
		${GENERATED_CPP_HEADERS}
	)
	
	set(${DIR_NAME}_MAIN_CPP_SOURCES ${MAIN_CPP_SOURCES} CACHE STRING "" FORCE)
	set(${DIR_NAME}_CPP_MODULES ${CPP_MODULES} CACHE STRING "" FORCE)
	set(${DIR_NAME}_CPP_HEADERS ${CPP_HEADERS} CACHE STRING "" FORCE)
	set(${DIR_NAME}_CPP_SOURCES ${CPP_SOURCES} CACHE STRING "" FORCE)
	set(${DIR_NAME}_NN_DATA ${NN_DATA} CACHE STRING "" FORCE)
	set(${DIR_NAME}_NN_SOURCES ${NN_SOURCES} CACHE STRING "" FORCE)
	set(${DIR_NAME}_GENERATED_CPP_MODULES ${GENERATED_CPP_MODULES} CACHE STRING "" FORCE)
	set(${DIR_NAME}_GENERATED_CPP_SOURCES ${GENERATED_CPP_SOURCES} CACHE STRING "" FORCE)
	set(${DIR_NAME}_GENERATED_CPP_HEADERS ${GENERATED_CPP_HEADERS} CACHE STRING "" FORCE)
	set(${DIR_NAME}_ALL_SOURCES ${ALL_SOURCES} CACHE STRING "" FORCE)
endfunction()

function(erm_get_group_for_file FILE)
	cmake_path(GET FILE PARENT_PATH PARENT_DIR)
	cmake_path(GET FILE EXTENSION LAST_ONLY FILE_EXTENSION)

	string(REGEX REPLACE ".*/common" "" GROUP "${PARENT_DIR}")
	string(REGEX REPLACE ".*/${ERM_RENDERING_API}" "" GROUP "${GROUP}")

	string(FIND ${FILE} "generated" IS_GENERATED)
	string(FIND ${FILE} "NN_Sources" IS_NN_SOURCE)

	if(IS_GENERATED GREATER_EQUAL 0)
		set(GROUP "Generated/${GROUP}")
	elseif(IS_NN_SOURCE GREATER_EQUAL 0)
		set(GROUP "NN_Sources/${GROUP}")
	elseif(FILE_EXTENSION STREQUAL ".h" OR FILE_EXTENSION STREQUAL ".hpp" OR FILE_EXTENSION STREQUAL ".cpp")
		set(GROUP "Sources/${GROUP}")
	elseif(FILE_EXTENSION STREQUAL ".nn")
		set(GROUP "NN_Data/${GROUP}")
	else()
		message(WARNING "Unknown file extension ${FILE}")
	endif()

	set(GROUP "${GROUP}" PARENT_SCOPE)
endfunction()
