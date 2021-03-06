function(target_setup_project TARGET)
	set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${TARGET}")

	set_target_properties(
		"${TARGET}" PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
	)

	target_compile_definitions(
		"${TARGET}"
		PRIVATE 
			_USE_MATH_DEFINES
			NOMINMAX
	)

	target_compile_options(
		"${TARGET}"
		PRIVATE 
			$<$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">:/W3 /WX>
		PUBLIC
			$<$<STREQUAL:"${CMAKE_BUILD_TYPE}","Release">:/ZI>
	)

	target_link_options(
		"${TARGET}"
		PUBLIC
			$<$<STREQUAL:"${CMAKE_BUILD_TYPE}","Release">:/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF>
	)

	if("${TARGET_API}" STREQUAL "Vulkan")
		set(SHADERS_COMPILER "$ENV{VULKAN_SDK}\\Bin\\glslc.exe")
		string(REPLACE "\\" "/" SHADERS_COMPILER "${SHADERS_COMPILER}")
		set(SHADERS_COMPILER "${SHADERS_COMPILER}" PARENT_SCOPE)
		set(ERM_FLIP_VIEWPORT ON CACHE BOOL "" FORCE)
		set(ERM_FLIP_PROJECTION OFF CACHE BOOL "" FORCE)
	else()
		set(ERM_FLIP_VIEWPORT OFF CACHE BOOL "" FORCE)
		set(ERM_FLIP_PROJECTION OFF CACHE BOOL "" FORCE)
	endif()
endfunction()
