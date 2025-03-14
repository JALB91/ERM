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
	string(REGEX REPLACE ".*/${ERM_TARGET_API}" "" GROUP "${GROUP}")

	string(FIND ${FILE} "generated" IS_GENERATED)
	string(FIND ${FILE} "NN_Sources" IS_NN_SOURCE)

	if(IS_GENERATED GREATER_EQUAL 0)
		set(GROUP "Generated/${GROUP}")
	elseif(IS_NN_SOURCE GREATER_EQUAL 0)
		set(GROUP "NN_Sources/${GROUP}")
	elseif(FILE_EXTENSION STREQUAL ".h" OR FILE_EXTENSION STREQUAL ".cpp" OR FILE_EXTENSION STREQUAL ".hpp")
		set(GROUP "Sources/${GROUP}")
	else()
		set(GROUP "NN_Data/${GROUP}")
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
	unset(CPP_SOURCES)
	unset(CPP_HEADERS)
	unset(NN_DATA)
	unset(NN_SOURCES)
	unset(NN_GEN_FILES)
	unset(ALL_SOURCES)

	# Gather header files
	file(GLOB_RECURSE CPP_HEADERS
		"${DIR_PATH}/Sources/common/*.h"
		"${DIR_PATH}/Sources/${ERM_TARGET_API}/*.h"
	)

	# Gather source files
	file(GLOB_RECURSE CPP_SOURCES
		"${DIR_PATH}/Sources/common/*.cpp"
		"${DIR_PATH}/Sources/${ERM_TARGET_API}/*.cpp"
	)

	# Gather nn data files
	file(GLOB_RECURSE NN_DATA
		"${DIR_PATH}/NN_Data/common/*.nn"
		"${DIR_PATH}/NN_Data/${ERM_TARGET_API}/*.nn"
	)

	# Gather nn source files
	file(GLOB_RECURSE NN_SOURCES
		"${DIR_PATH}/NN_Sources/common/*.h"
		"${DIR_PATH}/NN_Sources/common/*.cpp"
		"${DIR_PATH}/NN_Sources/${ERM_TARGET_API}/*.h"
		"${DIR_PATH}/NN_Sources/${ERM_TARGET_API}/*.cpp"
	)

	# Remove raytracing files if not enabled
	if(NOT ERM_RAY_TRACING_ENABLED)
		list(FILTER CPP_HEADERS EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER CPP_SOURCES EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER NN_DATA EXCLUDE REGEX ".*/ray_tracing/.*")
		list(FILTER NN_SOURCES EXCLUDE REGEX ".*/ray_tracing/.*")
	endif()

	# Gather generated files
	foreach(FILE ${NN_DATA})
		string(REGEX REPLACE ".*/${DIR_NAME}/" "${ERM_GENERATED_DIR}/${DIR_NAME}/" FILE "${FILE}")
		string(REPLACE "/NN_Data" "" FILE "${FILE}")
		string(REPLACE ".nn" ".h" NN_GEN_HEADER "${FILE}")
		string(REPLACE ".nn" ".cpp" NN_GEN_SOURCE "${FILE}")

		list(APPEND CPP_HEADERS ${NN_GEN_HEADER})
		list(APPEND CPP_SOURCES ${NN_GEN_SOURCE})
		list(APPEND NN_GEN_FILES ${NN_GEN_HEADER} ${NN_GEN_SOURCE})
	endforeach()

	list(APPEND ALL_SOURCES ${CPP_HEADERS} ${CPP_SOURCES} ${NN_DATA} ${NN_GEN_FILES})
	
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
	list(APPEND NN_EXECUTABLE_COMMAND 
		"cp"
		"$<TARGET_FILE:${PROJECT_NAME}>"
		"${CMAKE_CURRENT_SOURCE_DIR}/bin/${ERM_PLATFORM}/$<CONFIG>/${PROJECT_NAME}")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${NN_EXECUTABLE_COMMAND})
endfunction()

function(erm_add_library)
	if(NOT ${PROJECT_NAME}_CPP_SOURCES)
		add_library("${PROJECT_NAME}" INTERFACE ${${PROJECT_NAME}_ALL_SOURCES})
		set_target_properties(
			"${PROJECT_NAME}" 
			PROPERTIES
				IS_INTERFACE ON
		)
	else()
		add_library("${PROJECT_NAME}" ${${PROJECT_NAME}_ALL_SOURCES})
		set_target_properties(
			"${PROJECT_NAME}" 
			PROPERTIES
				IS_INTERFACE OFF
		)
	endif()
endfunction()

function(erm_add_executable)
	add_executable("${PROJECT_NAME}" ${${PROJECT_NAME}_ALL_SOURCES})
endfunction()

function(erm_target_setup_common_defaults)
	get_target_property(IS_INTERFACE "${PROJECT_NAME}" IS_INTERFACE)
	if(IS_INTERFACE)
		target_include_directories(
			"${PROJECT_NAME}"
			INTERFACE
				${ERM_TARGET_API_INCLUDE_DIR}
				"${ERM_GENERATED_DIR}/${PROJECT_NAME}/common"
				"${ERM_GENERATED_DIR}/${PROJECT_NAME}/${ERM_TARGET_API}"
				"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common"
				"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_TARGET_API}"
		)

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
				${ERM_TARGET_API_COMPILE_DEF}
		)
	else()
		target_include_directories(
			"${PROJECT_NAME}"
			PRIVATE
				${ERM_TARGET_API_INCLUDE_DIR}
			PUBLIC
				"${ERM_GENERATED_DIR}/${PROJECT_NAME}/common"
				"${ERM_GENERATED_DIR}/${PROJECT_NAME}/${ERM_TARGET_API}"
				"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common"
				"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_TARGET_API}"
		)

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
				${ERM_TARGET_API_COMPILE_DEF}
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
