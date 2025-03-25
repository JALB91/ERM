function(erm_gather_libs OUT_VAR)
	erm_gather_directories(DIRS)
	foreach(DIR_PATH ${DIRS})
		cmake_path(GET DIR_PATH STEM DIR_NAME)
		if(NOT "${DIR_NAME}" STREQUAL "cmake")
			list(APPEND LIBS "${DIR_PATH}")
		endif()
	endforeach()
	set("${OUT_VAR}" ${LIBS} PARENT_SCOPE)
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

function(erm_create_groups SOURCES)
	foreach(FILE ${SOURCES})
		erm_get_group_for_file("${FILE}")
		source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endfunction()

function(erm_gather_sources DIR_PATH)
	erm_get_dir_name_from_path(${DIR_PATH})

	# Clear eventual cached data
	unset(MAIN_SOURCES)
	unset(CPP_SOURCES)
	unset(CPP_HEADERS)
	unset(NN_DATA)
	unset(NN_SOURCES)
	unset(NN_GEN_FILES)
	unset(ALL_SOURCES)

	# Gather header files
	file(GLOB_RECURSE CPP_HEADERS
		"${DIR_PATH}/Sources/common/*.h"
		"${DIR_PATH}/Sources/common/*.hpp"
		"${DIR_PATH}/Sources/${ERM_RENDERING_API}/*.h"
		"${DIR_PATH}/Sources/${ERM_RENDERING_API}/*.hpp"
	)

	if(ERM_USE_MODULES AND CPP_HEADERS)
		message(FATAL_ERROR "Found header files while using modules ${PROJECT_NAME}\n\t${CPP_HEADERS}")
	endif()

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
		list(FILTER CPP_HEADERS EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER CPP_SOURCES EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER NN_DATA EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER NN_SOURCES EXCLUDE REGEX ".*/ray_tracing/.*")
	endif()

	# Find main sources
	set(MAIN_SOURCES ${CPP_SOURCES})
	list(FILTER MAIN_SOURCES INCLUDE REGEX ".*main.cpp")
	list(FILTER CPP_SOURCES EXCLUDE REGEX ".*main.cpp")

	# Gather generated files
	foreach(FILE ${NN_DATA})
		string(REGEX REPLACE ".*/${DIR_NAME}/" "${ERM_GENERATED_DIR}/${DIR_NAME}/" FILE "${FILE}")
		string(REPLACE "/NN_Data" "" FILE "${FILE}")
		string(REPLACE ".nn" ".cpp" NN_GEN_SOURCE "${FILE}")

		if(NOT ERM_USE_MODULES)
			string(REPLACE ".nn" ".h" NN_GEN_HEADER "${FILE}")
			list(APPEND CPP_HEADERS ${NN_GEN_HEADER})
			list(APPEND NN_GEN_FILES ${NN_GEN_HEADER})
		endif()
		list(APPEND CPP_SOURCES ${NN_GEN_SOURCE})
		list(APPEND NN_GEN_FILES ${NN_GEN_SOURCE})
	endforeach()

	list(APPEND ALL_SOURCES ${MAIN_SOURCES} ${CPP_HEADERS} ${CPP_SOURCES} ${NN_DATA} ${NN_GEN_FILES})
	
	set(${DIR_NAME}_MAIN_SOURCES ${MAIN_SOURCES} PARENT_SCOPE)
	set(${DIR_NAME}_CPP_HEADERS ${CPP_HEADERS} PARENT_SCOPE)
	set(${DIR_NAME}_CPP_SOURCES ${CPP_SOURCES} PARENT_SCOPE)
	set(${DIR_NAME}_NN_DATA ${NN_DATA} PARENT_SCOPE)
	set(${DIR_NAME}_NN_SOURCES ${NN_SOURCES} PARENT_SCOPE)
	set(${DIR_NAME}_NN_GEN_FILES ${NN_GEN_FILES} PARENT_SCOPE)
	set(${DIR_NAME}_ALL_SOURCES ${ALL_SOURCES} PARENT_SCOPE)
endfunction()

function(erm_setup_custom_commands)
	if(${PROJECT_NAME}_NN_DATA)
		foreach(FILE ${${PROJECT_NAME}_NN_DATA})
			string(REGEX REPLACE ".*/NN_Data/" "${ERM_GENERATED_DIR}/${PROJECT_NAME}/" NN_OUTPUT_DIR "${FILE}")
			get_filename_component(NN_OUTPUT_DIR "${NN_OUTPUT_DIR}" DIRECTORY)
			get_filename_component(NN_OUTPUT_FILE_NAME "${FILE}" NAME_WE)
			
			set(NN_OUTPUT_HEADER "${NN_OUTPUT_DIR}/${NN_OUTPUT_FILE_NAME}.h")
			set(NN_OUTPUT_SOURCE "${NN_OUTPUT_DIR}/${NN_OUTPUT_FILE_NAME}.cpp")

			set(NN_GENERATE_COMMAND "${CMAKE_INSTALL_PREFIX}/bin/$<CONFIG>/$<TARGET_FILE_NAME:ERM_NN>")

			list(APPEND NN_GENERATE_ARGS ${FILE} ${NN_OUTPUT_DIR})

			add_custom_command(
				OUTPUT ${NN_OUTPUT_HEADER} ${NN_OUTPUT_SOURCE}
				COMMAND ${NN_GENERATE_COMMAND} ${NN_GENERATE_ARGS}
				DEPENDS ${FILE}
			)
		endforeach()
	endif()
endfunction()

function(erm_setup_executable_custom_commands)
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD 
		COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>"
		COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>/${PROJECT_NAME}"
		BYPRODUCTS "${CMAKE_CURRENT_SOURCE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>/${PROJECT_NAME}"
	)
	
	if(ERM_HOST_OSX)
		add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD 
			COMMAND  codesign --force --deep --sign - "${CMAKE_CURRENT_SOURCE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>/${PROJECT_NAME}"
		)
	endif()

	if(ERM_HOST_WINDOWS AND $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>)
		# Copy DLLs
		add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
			COMMAND_EXPAND_LISTS
		)
	endif()
endfunction()

function(erm_target_sources)
	if(ERM_USE_MODULES)
		target_sources(
			"${PROJECT_NAME}"
			PUBLIC
				FILE_SET CXX_MODULES
				BASE_DIRS
					"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common"
					"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_RENDERING_API}"
					"${CMAKE_CURRENT_SOURCE_DIR}/NN_Sources/common"
					"${CMAKE_CURRENT_SOURCE_DIR}/NN_Sources/${ERM_RENDERING_API}"
					"${CMAKE_CURRENT_SOURCE_DIR}/NN_Data/common"
					"${CMAKE_CURRENT_SOURCE_DIR}/NN_Data/${ERM_RENDERING_API}"
					"${ERM_GENERATED_DIR}/${PROJECT_NAME}/common"
					"${ERM_GENERATED_DIR}/${PROJECT_NAME}/${ERM_RENDERING_API}"
				FILES
					${${PROJECT_NAME}_CPP_SOURCES}
		)
	else()
		if(${PROJECT_NAME}_CPP_SOURCES)
			target_sources(
				"${PROJECT_NAME}"
				PRIVATE
					${${PROJECT_NAME}_CPP_SOURCES}
				PUBLIC
					FILE_SET HEADERS
					BASE_DIRS
						"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common"
						"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_RENDERING_API}"
						"${CMAKE_CURRENT_SOURCE_DIR}/NN_Sources/common"
						"${CMAKE_CURRENT_SOURCE_DIR}/NN_Sources/${ERM_RENDERING_API}"
						"${CMAKE_CURRENT_SOURCE_DIR}/NN_Data/common"
						"${CMAKE_CURRENT_SOURCE_DIR}/NN_Data/${ERM_RENDERING_API}"
						"${ERM_GENERATED_DIR}/${PROJECT_NAME}/common"
						"${ERM_GENERATED_DIR}/${PROJECT_NAME}/${ERM_RENDERING_API}"
					FILES
						${${PROJECT_NAME}_CPP_HEADERS}
			)
		else()
			target_sources(
				"${PROJECT_NAME}"
				INTERFACE
					FILE_SET HEADERS
					BASE_DIRS
						"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common"
						"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_RENDERING_API}"
						"${CMAKE_CURRENT_SOURCE_DIR}/NN_Sources/common"
						"${CMAKE_CURRENT_SOURCE_DIR}/NN_Sources/${ERM_RENDERING_API}"
						"${CMAKE_CURRENT_SOURCE_DIR}/NN_Data/common"
						"${CMAKE_CURRENT_SOURCE_DIR}/NN_Data/${ERM_RENDERING_API}"
						"${ERM_GENERATED_DIR}/${PROJECT_NAME}/common"
						"${ERM_GENERATED_DIR}/${PROJECT_NAME}/${ERM_RENDERING_API}"
					FILES
						${${PROJECT_NAME}_CPP_HEADERS}
			)
		endif()
	endif()

	if(${PROJECT_NAME}_NN_DATA)
		target_sources(
			"${PROJECT_NAME}"
			PUBLIC
				${${PROJECT_NAME}_NN_DATA}
		)
	endif()

	if(${PROJECT_NAME}_NN_SOURCES)
		target_sources(
			"${PROJECT_NAME}"
			PUBLIC
				${${PROJECT_NAME}_NN_SOURCES}
		)
	endif()

	if(${PROJECT_NAME}_NN_GEN_FILES)
		target_sources(
			"${PROJECT_NAME}"
			PUBLIC
				${${PROJECT_NAME}_NN_GEN_FILES}
		)
	endif()
endfunction()

function(erm_add_library)
	if(NOT ${PROJECT_NAME}_CPP_SOURCES)
		add_library("${PROJECT_NAME}" INTERFACE)
		set_target_properties(
			"${PROJECT_NAME}" 
			PROPERTIES
				IS_INTERFACE ON
		)
	else()
		add_library("${PROJECT_NAME}")
		set_target_properties(
			"${PROJECT_NAME}" 
			PROPERTIES
				IS_INTERFACE OFF
		)
	endif()

	erm_target_sources()
endfunction()

function(erm_add_executable)
	if(NOT ${PROJECT_NAME}_MAIN_SOURCES)
		message(FATAL_ERROR "No main.cpp file found for executable ${PROJECT_NAME}")
	endif()
	
	add_executable("${PROJECT_NAME}" ${${PROJECT_NAME}_MAIN_SOURCES})

	erm_target_sources()
endfunction()

function(erm_target_setup_common_defaults)
	get_target_property(IS_INTERFACE "${PROJECT_NAME}" IS_INTERFACE)
	if(IS_INTERFACE)
		target_compile_definitions(
			"${PROJECT_NAME}"
			INTERFACE
				$<$<BOOL:${ERM_VULKAN}>:GLM_FORCE_DEPTH_ZERO_TO_ONE>
				$<$<BOOL:${ERM_VULKAN}>:IMGUI_DISABLE_OBSOLETE_FUNCTIONS>
				$<$<BOOL:${ERM_FLIP_PROJECTION}>:ERM_FLIP_PROJECTION>
				$<$<BOOL:${ERM_FLIP_VIEWPORT}>:ERM_FLIP_VIEWPORT>
				$<$<BOOL:${ERM_ASSIMP_ENABLED}>:ERM_ASSIMP_ENABLED>
				$<$<BOOL:${ERM_RAY_TRACING_ENABLED}>:ERM_RAY_TRACING_ENABLED>
				$<$<BOOL:${ERM_TRACY_ENABLED}>:TRACY_ENABLE>
				$<$<BOOL:${ERM_OPEN_GL}>:GLEW_STATIC>
				$<$<BOOL:${ERM_VULKAN}>:ERM_SHADERS_COMPILER="${ERM_SHADERS_COMPILER}">
				$<$<BOOL:${ERM_VULKAN}>:VULKAN_HPP_NO_EXCEPTIONS>
				$<$<CONFIG:Debug>:ERM_DEBUG>
				$<$<CONFIG:Release>:ERM_RELEASE>
				SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS
				${ERM_RENDERING_API_COMPILE_DEF}
		)
	else()
		target_compile_definitions(
			"${PROJECT_NAME}"
			PRIVATE
				$<$<BOOL:${ERM_VULKAN}>:GLM_FORCE_DEPTH_ZERO_TO_ONE>
				$<$<BOOL:${ERM_VULKAN}>:IMGUI_DISABLE_OBSOLETE_FUNCTIONS>
				$<$<BOOL:${ERM_FLIP_PROJECTION}>:ERM_FLIP_PROJECTION>
				$<$<BOOL:${ERM_FLIP_VIEWPORT}>:ERM_FLIP_VIEWPORT>
				$<$<BOOL:${ERM_ASSIMP_ENABLED}>:ERM_ASSIMP_ENABLED>
				$<$<BOOL:${ERM_RAY_TRACING_ENABLED}>:ERM_RAY_TRACING_ENABLED>
				$<$<BOOL:${ERM_TRACY_ENABLED}>:TRACY_ENABLE>
				$<$<BOOL:${ERM_OPEN_GL}>:GLEW_STATIC>
				$<$<BOOL:${ERM_VULKAN}>:ERM_SHADERS_COMPILER="${ERM_SHADERS_COMPILER}">
				$<$<BOOL:${ERM_VULKAN}>:VULKAN_HPP_NO_EXCEPTIONS>
				$<$<CONFIG:Debug>:ERM_DEBUG>
				$<$<CONFIG:Release>:ERM_RELEASE>
				SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS
				${ERM_RENDERING_API_COMPILE_DEF}
		)
	endif()

	if(${PROJECT_NAME}_NN_DATA)
		add_dependencies("${PROJECT_NAME}" ERM_NN)
	endif()
endfunction()

function(erm_setup_library)
	erm_create_groups("${${PROJECT_NAME}_ALL_SOURCES}")
	erm_setup_custom_commands()
	erm_add_library()
	erm_target_setup_project()
	erm_target_setup_common_defaults()
endfunction()

function(erm_setup_executable)
	erm_create_groups("${${PROJECT_NAME}_ALL_SOURCES}")
	erm_setup_custom_commands()
	erm_add_executable()
	erm_target_setup_project()
	erm_target_setup_common_defaults()
	erm_setup_executable_custom_commands()
endfunction()

# Using macros in order to inherit the PROJECT_NAME variable from the calling site
macro(erm_library_project VERSION DESCRIPTION)
	get_filename_component(DIR_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
	project(
		"${DIR_NAME}"
		VERSION ${VERSION}
		DESCRIPTION "${DESCRIPTION}"
		LANGUAGES CXX
	)
	erm_setup_library()
endmacro()

macro(erm_executable_project VERSION DESCRIPTION)
	get_filename_component(DIR_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
	project(
		"${DIR_NAME}"
		VERSION ${VERSION}
		DESCRIPTION "${DESCRIPTION}"
		LANGUAGES CXX
	)
	erm_setup_executable()
endmacro()

option(ERM_USE_MODULES "If on it uses cxx modules instead of header files" OFF)
