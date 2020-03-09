function(target_setup_project TARGET)
	set_target_properties(
		"${TARGET}" PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/"
	)

	target_compile_definitions(
		"${TARGET}"
		PRIVATE 
			_USE_MATH_DEFINES
	)

	target_compile_options(
		"${TARGET}"
		PRIVATE 
			$<$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">:/W3 /WX>
	)

	if("${TARGET_API}" STREQUAL "Vulkan")
		set(SHADERS_COMPILER "$ENV{VULKAN_SDK}\\Bin32\\glslc.exe" PARENT_SCOPE)
	endif("${TARGET_API}" STREQUAL "Vulkan")
endfunction()