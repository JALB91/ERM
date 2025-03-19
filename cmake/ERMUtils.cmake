function(erm_setup_api)
	if(ERM_OPEN_GL)
		set(ERM_TARGET_API_COMPILE_DEF "ERM_OPEN_GL" PARENT_SCOPE)
		find_package(OpenGL REQUIRED)
		set(ERM_TARGET_API_LIB "${OPENGL_gl_LIBRARY}" PARENT_SCOPE)
		set(ERM_RAY_TRACING_ENABLED OFF CACHE BOOL "" FORCE)
	elseif(ERM_VULKAN)
		set(ERM_TARGET_API_COMPILE_DEF "ERM_VULKAN" PARENT_SCOPE)
		find_package(Vulkan REQUIRED)
		set(ERM_TARGET_API_LIB "${Vulkan_LIBRARIES}" PARENT_SCOPE)
		set(ERM_TARGET_API_INCLUDE_DIR "${Vulkan_INCLUDE_DIR}" PARENT_SCOPE)
	elseif(ERM_DX12)
		set(ERM_TARGET_API_COMPILE_DEF "ERM_DX12" PARENT_SCOPE)
		set(ERM_RAY_TRACING_ENABLED OFF CACHE BOOL "" FORCE)
	endif()
endfunction()

function(erm_setup_pipeline_script)
	if(ERM_VULKAN)
		if(ERM_WINDOWS)
			set(ERM_SHADERS_COMPILER "$ENV{VULKAN_SDK}\\Bin\\glslc.exe")
			string(REPLACE "\\" "/" ERM_SHADERS_COMPILER "${ERM_SHADERS_COMPILER}")
		elseif(ERM_MAC)
			set(ERM_SHADERS_COMPILER "glslc")
		endif()

		if(ERM_RAY_TRACING_ENABLED)
			set(ERM_SHADERS_COMPILER "${ERM_SHADERS_COMPILER} --target-spv=spv1.5 -I ${ERM_RES_DEST}/shaders/Vulkan/imports/")
		else()
			set(ERM_SHADERS_COMPILER "${ERM_SHADERS_COMPILER} --target-spv=spv1.3 -I ${ERM_RES_DEST}/shaders/Vulkan/imports/")
		endif()
	endif()

	set(ERM_PIPELINE_ARGS --res-dest ${ERM_RES_DEST} --res-src ${ERM_RES_SOURCE})

	if(ERM_SHADERS_COMPILER)
		list(APPEND ERM_PIPELINE_ARGS --shaders-compiler ${ERM_SHADERS_COMPILER})
	endif()

	if(ERM_RAY_TRACING_ENABLED)
		list(APPEND ERM_PIPELINE_ARGS --rtx-enabled True)
	endif()

	set(ERM_PIPELINE_ARGS "${ERM_PIPELINE_ARGS}" PARENT_SCOPE)
	set(ERM_PIPELINE_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/resources_pipeline.py" PARENT_SCOPE)
endfunction()

function(erm_setup_resources_pipeline)
	# add_custom_command(
	# 	TARGET "${PROJECT_NAME}"
	# 	POST_BUILD
	# 	COMMAND ${Python3_EXECUTABLE} ${ERM_PIPELINE_SCRIPT} ${ERM_PIPELINE_ARGS}
	# 	COMMENT "Resources Pipeline"
	# )
endfunction()

function(erm_gather_directories OUT_VAR)
	file(GLOB GLOBBED_FILES LIST_DIRECTORIES true *)
	foreach(FILE ${GLOBBED_FILES})
		if(IS_DIRECTORY "${FILE}")
			list(APPEND DIRECTORIES ${FILE})
		endif()
	endforeach()
	set("${OUT_VAR}" ${DIRECTORIES} PARENT_SCOPE)
endfunction()

function(erm_get_dir_name_from_path DIR_PATH)
	if(NOT IS_DIRECTORY "${DIR_PATH}")
		message(FATAL_ERROR "No such directory: ${DIR_PATH}")
	endif()
	cmake_path(GET DIR_PATH STEM DIR_NAME)
	set(DIR_NAME ${DIR_NAME} PARENT_SCOPE)
endfunction()