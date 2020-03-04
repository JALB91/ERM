function(target_setup_project TARGET)
	set_target_properties(
		"${TARGET}" PROPERTIES
		VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${CONFIG}"
	)

	target_compile_definitions(
		"${TARGET}"
		PRIVATE 
			_USE_MATH_DEFINES
	)

	target_compile_options(
		"${TARGET}"
		PRIVATE 
			$<$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">:/W4>
	)
endfunction()