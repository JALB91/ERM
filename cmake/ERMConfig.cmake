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

function(erm_setup_shaders_compiler)
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

		set(ERM_SHADERS_COMPILER "${ERM_SHADERS_COMPILER}" PARENT_SCOPE)
	endif()
endfunction()

function(erm_setup_resources_pipeline)
	set(ERM_PIPELINE_ARGS --res-dest ${ERM_RES_DEST} --res-src ${ERM_RES_SOURCE})

	if(ERM_VULKAN)
		list(APPEND ERM_PIPELINE_ARGS --shaders-compiler ${ERM_SHADERS_COMPILER})
	endif()

	if(ERM_RAY_TRACING_ENABLED)
		list(APPEND ERM_PIPELINE_ARGS --rtx-enabled True)
	endif()

	set(ERM_PIPELINE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/resources_pipeline.py")

	add_custom_target(
		ERM_RESOURCES_PIPELINE
		COMMAND ${PYTHON_EXECUTABLE} ${ERM_PIPELINE_SCRIPT} ${ERM_PIPELINE_ARGS}
		COMMENT "Resources Pipeline"
	)
	add_dependencies(ERM_Main ERM_RESOURCES_PIPELINE)
endfunction()

function(erm_target_setup_common_defaults)
	# Target common compile definitions
	target_compile_definitions(
		"${PROJECT_NAME}"
		PRIVATE
			$<$<BOOL:${ERM_VULKAN}>:GLM_FORCE_DEPTH_ZERO_TO_ONE>
			$<$<BOOL:${ERM_VULKAN}>:IMGUI_DISABLE_OBSOLETE_FUNCTIONS>
			$<$<BOOL:${ERM_FLIP_PROJECTION}>:ERM_FLIP_PROJECTION>
			$<$<BOOL:${ERM_FLIP_VIEWPORT}>:ERM_FLIP_VIEWPORT>
			$<$<BOOL:${ERM_FBX_ENABLED}>:ERM_FBX_ENABLED>
			$<$<BOOL:${ERM_ASSIMP_ENABLED}>:ERM_ASSIMP_ENABLED>
			$<$<BOOL:${ERM_RAY_TRACING_ENABLED}>:ERM_RAY_TRACING_ENABLED>
			$<$<BOOL:${ERM_TRACY_ENABLED}>:ERM_TRACY_ENABLED>
			${ERM_TARGET_API_COMPILE_DEF}
			$<$<BOOL:${ERM_OPEN_GL}>:GLEW_STATIC>
			$<$<BOOL:${ERM_VULKAN}>:ERM_SHADERS_COMPILER="${ERM_SHADERS_COMPILER}">
	)

	# Target common features
	target_compile_features(
		"${PROJECT_NAME}"
		PUBLIC 
			cxx_std_17
	)

	# Target common properties
	set_target_properties(
		"${PROJECT_NAME}" 
		PROPERTIES
			CXX_STANDARD 17
			CXX_EXTENSIONS OFF
	)
endfunction()


option(ERM_FLIP_PROJECTION "If ON the projection matrix will be flipped over the Y axis" ON)
option(ERM_FLIP_VIEWPORT "If ON the viewport will be flipped over the Y axis" ${ERM_VULKAN})
option(ERM_ASSIMP_ENABLED "If ON assimp library will be used to load models" OFF)
option(ERM_RAY_TRACING_ENABLED "If ON it will try to enable ray tracing (if supported by the API" ON)
