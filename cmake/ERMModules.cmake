function(erm_create_groups SRCS)
	foreach(FILE ${SRCS})
		# Get the directory of the source file
		get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

		# Remove common directory prefix to make the group
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
		string(REPLACE "/include/" "" GROUP "${GROUP}")
		string(REPLACE "/src/" "erm" GROUP "${GROUP}")
		string(REPLACE "common" "" GROUP "${GROUP}")
		string(REPLACE "${ERM_TARGET_API}" "" GROUP "${GROUP}")
		string(REPLACE "/" "\\" GROUP "${GROUP}")

		source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endfunction()

function(erm_gather_sources)
	file(GLOB_RECURSE SOURCES
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_TARGET_API}/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_TARGET_API}/*.cpp"
	)

	if(NOT ERM_RAY_TRACING_ENABLED)
		list(FILTER SOURCES EXCLUDE REGEX ".*/ray_tracing/.*")
	endif()
	erm_create_groups("${SOURCES}")
	set(SOURCES "${SOURCES}" PARENT_SCOPE)
endfunction()

function(erm_add_module_library)
	add_library("${PROJECT_NAME}" "${SOURCES}")
endfunction()

function(erm_add_module_executable)
	add_executable("${PROJECT_NAME}" "${SOURCES}")
endfunction()

function(erm_target_setup_common_defaults)
	target_include_directories(
		"${PROJECT_NAME}"
		PRIVATE
			"${ERM_TARGET_API_INCLUDE_DIR}"
		PUBLIC
			"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common/"
			"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_TARGET_API}/"
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
			SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS
			${ERM_TARGET_API_COMPILE_DEF}
	)

	target_compile_features(
		"${PROJECT_NAME}"
		PUBLIC 
			cxx_std_17
	)

	set_target_properties(
		"${PROJECT_NAME}" 
		PROPERTIES
			CXX_STANDARD 17
			CXX_EXTENSIONS OFF
	)
endfunction()

function(erm_setup_module_library)
    erm_gather_sources()
    erm_add_module_library()
    erm_target_setup_project()
    erm_target_setup_common_defaults()
endfunction()

function(erm_setup_module_executable)
    erm_gather_sources()
    erm_add_module_executable()
    erm_target_setup_project()
    erm_target_setup_common_defaults()

	add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
		COMMAND_EXPAND_LISTS
	)
endfunction()
