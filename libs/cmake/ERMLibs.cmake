function(_erm_target_source_files TARGET_NAME)
	target_sources(
		${TARGET_NAME}
		PRIVATE
			${${PROJECT_NAME}_CPP_SOURCES}
			${${PROJECT_NAME}_GENERATED_SOURCES}
			${${PROJECT_NAME}_NN_SOURCES}
			${${PROJECT_NAME}_NN_DATA}
	)

	if(ERM_USE_MODULES)
		target_sources(
			${TARGET_NAME}
			PUBLIC
				FILE_SET CXX_MODULES
				BASE_DIRS
					"${CMAKE_CURRENT_SOURCE_DIR}/Sources/common"
					"${CMAKE_CURRENT_SOURCE_DIR}/Sources/${ERM_RENDERING_API}"
				FILES
					${${PROJECT_NAME}_CPP_MODULES}
		)
	endif()
endfunction()

function(_erm_target_header_files TARGET_NAME SCOPE)
	target_sources(
		${TARGET_NAME}
		${SCOPE}
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
endfunction()

function(_erm_target_common_compile_definitions TARGET_NAME SCOPE)
	target_compile_definitions(
		${TARGET_NAME}
		${SCOPE}
			$<$<BOOL:${ERM_ASSIMP_ENABLED}>:ERM_ASSIMP_ENABLED>
			$<$<BOOL:${ERM_RAY_TRACING_ENABLED}>:ERM_RAY_TRACING_ENABLED>
			$<$<BOOL:${ERM_TRACY_ENABLED}>:TRACY_ENABLE>
	)
endfunction()

function(_erm_target_link_libraries TARGET_NAME)
	target_link_libraries(
		${TARGET_NAME}
		INTERFACE
			${${PROJECT_NAME}_INTERFACE_DEPS}
		PRIVATE
			${${PROJECT_NAME}_PRIVATE_DEPS}
		PUBLIC
			${${PROJECT_NAME}_PUBLIC_DEPS}
	)
endfunction()

function(_erm_target_setup_nn TARGET_NAME)
	if(${PROJECT_NAME}_NN_DATA)
		add_dependencies(${TARGET_NAME} ERM_NN_exe)
	endif()
endfunction()

function(_erm_setup_target TARGET_NAME SCOPE)
	_erm_target_setup_nn(${TARGET_NAME})
	_erm_target_header_files(${TARGET_NAME} ${SCOPE})
	_erm_target_common_compile_definitions(${TARGET_NAME} ${SCOPE})
endfunction()

function(_erm_setup_object_lib TARGET_NAME SOURCES)
	add_library(${TARGET_NAME} OBJECT ${SOURCES})
	_erm_target_source_files(${TARGET_NAME})
	_erm_setup_target(${TARGET_NAME} PUBLIC)
	_erm_target_link_libraries(${TARGET_NAME})
endfunction()

function(_erm_setup_interface_lib TARGET_NAME)
	add_library(${TARGET_NAME} INTERFACE)
	_erm_setup_target(${TARGET_NAME} INTERFACE)
endfunction()

function(_erm_setup_static_lib TARGET_NAME SOURCES)
	add_library(${TARGET_NAME} STATIC ${SOURCES})
	_erm_setup_target(${TARGET_NAME} PUBLIC)
	_erm_target_link_libraries(${TARGET_NAME})
endfunction()

function(_erm_setup_shared_lib TARGET_NAME SOURCES)
	add_library(${TARGET_NAME} SHARED ${SOURCES})
	_erm_setup_target(${TARGET_NAME} PUBLIC)
	_erm_target_link_libraries(${TARGET_NAME})
endfunction()

function(_erm_setup_executable)
	# ===========================================================================
	# Parse options
	# ===========================================================================
	set(EXE_OPTIONS LINK_INTERFACE_LIB)
	set(EXE_ONE_VALUE_ARGS MAIN_SOURCE)
	set(EXE_MULTI_VALUE_ARGS SOURCES)
	
	cmake_parse_arguments(
		EXE
		"${EXE_OPTIONS}"
		"${EXE_ONE_VALUE_ARGS}"
		"${EXE_MULTI_VALUE_ARGS}"
		${ARGN}
	)

	string(REGEX REPLACE ".*/(.*)main\\.cpp" "${PROJECT_NAME}_\\1exe" EXE_TARGET_NAME "${MAIN_SOURCE}")

	add_executable(${EXE_TARGET_NAME} ${EXE_MAIN_SOURCE} ${EXE_SOURCES})
	_erm_setup_target(${EXE_TARGET_NAME} PRIVATE)
	_erm_target_link_libraries(${EXE_TARGET_NAME})

	add_custom_command(
		TARGET ${EXE_TARGET_NAME} POST_BUILD 
		COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>"
		COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_FILE:${EXE_TARGET_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>/${EXE_TARGET_NAME}"
		BYPRODUCTS "${CMAKE_CURRENT_SOURCE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>/${EXE_TARGET_NAME}"
	)
	
	if(ERM_HOST_OSX)
		add_custom_command(
			TARGET ${EXE_TARGET_NAME} POST_BUILD 
			COMMAND  codesign --force --deep --sign - "${CMAKE_CURRENT_SOURCE_DIR}/bin/${ERM_HOST_PLATFORM}/$<CONFIG>/${EXE_TARGET_NAME}"
		)
	endif()

	if(ERM_HOST_WINDOWS AND $<TARGET_RUNTIME_DLLS:${EXE_TARGET_NAME}>)
		# Copy DLLs
		add_custom_command(
			TARGET ${EXE_TARGET_NAME} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy "$<TARGET_RUNTIME_DLLS:${EXE_TARGET_NAME}>" "$<TARGET_FILE_DIR:${EXE_TARGET_NAME}>"
			COMMAND_EXPAND_LISTS
		)
	endif()

	if(EXE_LINK_INTERFACE_LIB)
		target_link_libraries(${EXE_TARGET_NAME} PRIVATE ${PROJECT_NAME}_int)
	endif()
endfunction()

# Using macro in order to inherit the PROJECT_NAME variable from the calling site
macro(erm_lib_setup)
	# ===========================================================================
	# Get dir name
	# ===========================================================================
	get_filename_component(DIR_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

	# ===========================================================================
	# Parse options
	# ===========================================================================
	set(ERM_LIB_OPTIONS STATIC SHARED EXECUTABLES)
	set(ERM_LIB_ONE_VALUE_ARGS VERSION DESCRIPTION)
	set(ERM_LIB_MULTI_VALUE_ARGS LANGUAGES PRIVATE_DEPS PUBLIC_DEPS INTERFACE_DEPS)
	
	cmake_parse_arguments(
		${DIR_NAME}
		"${ERM_LIB_OPTIONS}"
		"${ERM_LIB_ONE_VALUE_ARGS}"
		"${ERM_LIB_MULTI_VALUE_ARGS}"
		${ARGN}
	)

	# ===========================================================================
	# Setup project
	# ===========================================================================
	project(
		${DIR_NAME}
		VERSION ${${DIR_NAME}_VERSION}
		DESCRIPTION ${${DIR_NAME}_DESCRIPTION}
		LANGUAGES ${${DIR_NAME}_LANGUAGES}
	)

	# ===========================================================================
	# Determine if object lib is needed
	# ===========================================================================
	if(${PROJECT_NAME}_STATIC AND ${PROJECT_NAME}_SHARED)
		set(${PROJECT_NAME}_OBJECT ON)
	elseif(${PROJECT_NAME}_STATIC AND ${PROJECT_NAME}_EXECUTABLES)
		set(${PROJECT_NAME}_OBJECT ON)
	elseif(${PROJECT_NAME}_SHARED AND ${PROJECT_NAME}_EXECUTABLES)
		set(${PROJECT_NAME}_OBJECT ON)
	elseif(NOT ${PROJECT_NAME}_STATIC AND NOT ${PROJECT_NAME}_SHARED AND NOT ${PROJECT_NAME}_EXECUTABLES)
		message(FATAL_ERROR "When calling erm_lib_setup at least one of [STATIC, SHARED, EXECUTABLES] needs to be specified")
	else()
		set(${PROJECT_NAME}_OBJECT OFF)
	endif()

	# ===========================================================================
	# Create groups
	# ===========================================================================
	foreach(FILE ${${PROJECT_NAME}_ALL_SOURCES})
		erm_get_group_for_file("${FILE}")
		source_group("${GROUP}" FILES "${FILE}")
	endforeach()

	# ===========================================================================
	# Generate lib header
	# ===========================================================================
	set(ERM_LIB_HEADER "#pragma once\n\n")
	string(APPEND ERM_LIB_HEADER "")

	file(MAKE_DIRECTORY "${ERM_GENERATED_DIR}/${PROJECT_NAME}/common/erm/")
	file(WRITE "${ERM_GENERATED_DIR}/${PROJECT_NAME}/common/erm/${PROJECT_NAME}.h" "${ERM_LIB_HEADER}")

	# ===========================================================================
	# NN Setup
	# ===========================================================================
	foreach(FILE ${${PROJECT_NAME}_NN_DATA})
		string(REGEX REPLACE ".*/NN_Data/" "${ERM_GENERATED_DIR}/${PROJECT_NAME}/" NN_OUTPUT_DIR "${FILE}")
		get_filename_component(NN_OUTPUT_DIR "${NN_OUTPUT_DIR}" DIRECTORY)
		get_filename_component(NN_OUTPUT_FILE_NAME "${FILE}" NAME_WE)
		
		set(NN_OUTPUT_HEADER "${NN_OUTPUT_DIR}/${NN_OUTPUT_FILE_NAME}.h")
		set(NN_OUTPUT_SOURCE "${NN_OUTPUT_DIR}/${NN_OUTPUT_FILE_NAME}.cpp")

		set(NN_GENERATE_COMMAND "${CMAKE_INSTALL_PREFIX}/bin/$<CONFIG>/$<TARGET_FILE_NAME:ERM_NN_exe>")

		list(APPEND NN_GENERATE_ARGS ${FILE} ${NN_OUTPUT_DIR})

		add_custom_command(
			OUTPUT ${NN_OUTPUT_HEADER} ${NN_OUTPUT_SOURCE}
			COMMAND ${NN_GENERATE_COMMAND} ${NN_GENERATE_ARGS}
			DEPENDS ${FILE}
		)
	endforeach()

	# ===========================================================================
	# Setup libs and exes
	# ===========================================================================
	if(${PROJECT_NAME}_OBJECT)
		_erm_setup_interface_lib(${PROJECT_NAME}_int)
		_erm_setup_object_lib(${PROJECT_NAME}_obj ${${PROJECT_NAME}_ALL_SOURCES})

		set(ERM_DUMMY_DIR "${ERM_GENERATED_DIR}/${PROJECT_NAME}/common/erm")
		set(ERM_DUMMY_FILE "${ERM_DUMMY_DIR}/dummy.cpp")
		file(MAKE_DIRECTORY "${ERM_DUMMY_DIR}")
		file(WRITE "${ERM_DUMMY_FILE}" "")

		erm_get_group_for_file("${ERM_DUMMY_FILE}")
		source_group("${GROUP}" FILES "${ERM_DUMMY_FILE}")

		if(${PROJECT_NAME}_STATIC)
			_erm_setup_static_lib(${PROJECT_NAME}_static "$<TARGET_OBJECTS:${PROJECT_NAME}_obj>;${ERM_DUMMY_FILE}")
			target_link_libraries(${PROJECT_NAME}_static PRIVATE ${PROJECT_NAME}_int)
		endif()

		if(${PROJECT_NAME}_SHARED)
			_erm_setup_shared_lib(${PROJECT_NAME}_shared "$<TARGET_OBJECTS:${PROJECT_NAME}_obj>;${ERM_DUMMY_FILE}")
			target_link_libraries(${PROJECT_NAME}_shared PRIVATE ${PROJECT_NAME}_int)
		endif()
		
		if(${PROJECT_NAME}_EXECUTABLES)
			foreach(MAIN_SOURCE ${${PROJECT_NAME}_MAIN_SOURCES})
				_erm_setup_executable(
					LINK_INTERFACE_LIB
					MAIN_SOURCE ${MAIN_SOURCE}
					SOURCES $<TARGET_OBJECTS:${PROJECT_NAME}_obj>
				)
			endforeach()
		endif()
	elseif(${PROJECT_NAME}_STATIC)
		_erm_setup_static_lib(${PROJECT_NAME}_static ${${PROJECT_NAME}_ALL_SOURCES})
		_erm_target_source_files(${PROJECT_NAME}_static)
	elseif(${PROJECT_NAME}_SHARED)
		_erm_setup_static_lib(${PROJECT_NAME}_shared ${${PROJECT_NAME}_ALL_SOURCES})
		_erm_target_source_files(${PROJECT_NAME}_shared)
	elseif(${PROJECT_NAME}_EXECUTABLES)
		foreach(MAIN_SOURCE ${${PROJECT_NAME}_MAIN_SOURCES})
			_erm_setup_executable(
				TARGET_NAME ${EXE_TARGET_NAME}
				MAIN_SOURCE ${MAIN_SOURCE}
				SOURCES ${${PROJECT_NAME}_ALL_SOURCES}
			)
		endforeach()
	endif()
endmacro()

option(ERM_USE_MODULES "If on it uses cxx modules instead of header files" OFF)
