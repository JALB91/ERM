project(
	"ERM_Modules"
	VERSION 1.0.0
	DESCRIPTION "ERM Modules"
)

# Updates cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include erm modules and utils
include(ERMModulesUtils)
include(ERMModules)

# Unset variables
unset(ERM_MODULES CACHE)
unset(ALL_DEPS CACHE)

# Add ERM modules subdirectories
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_ArgsParser")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Assets")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Audio")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_ECS")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Editor")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Engine")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_FileSystem")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Input")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Log")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Main")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Math")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_NN")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Rendering")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Setup")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_System")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Utils")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Window")

foreach(ERM_MODULE ${ERM_MODULES})
	unset(${ERM_MODULE}_ALL_SOURCES CACHE)

	# Call pre generate callback if any
	if(${ERM_MODULE}_PRE_GENERATE_CALLBACK)
		message(STATUS "Pre generate callback ${ERM_MODULE}")
		erm_call_function(FN_NAME "${${ERM_MODULE}_PRE_GENERATE_CALLBACK}")
	endif()

	# Gather all dependencies
	list(APPEND ALL_DEPS 
		"${${ERM_MODULE}_INTERFACE_DEPS}" 
		"${${ERM_MODULE}_PRIVATE_DEPS}" 
		"${${ERM_MODULE}_PUBLIC_DEPS}"
	)

	# Create module header
	set(MODULE_HEADER "#pragma once\n\n")
	string(APPEND MODULE_HEADER "// We'll see")
	file(WRITE "${${ERM_MODULE}_GENERATED_DIR}/common/erm/${ERM_MODULE}.h" "${MODULE_HEADER}")
endforeach()

foreach(ERM_MODULE ${ERM_MODULES})
	message(STATUS "Generating ${ERM_MODULE}")

	# Gather all sources
	list(APPEND ${ERM_MODULE}_ALL_SOURCES
		${${ERM_MODULE}_GENERATED_CPP_MODULES}
		${${ERM_MODULE}_GENERATED_CPP_SOURCES}
		${${ERM_MODULE}_GENERATED_CPP_HEADERS}
		${${ERM_MODULE}_CPP_MODULES}
		${${ERM_MODULE}_CPP_SOURCES}
		${${ERM_MODULE}_CPP_HEADERS}
	)
	set(${ERM_MODULE}_ALL_SOURCES ${${ERM_MODULE}_ALL_SOURCES} CACHE STRING "" FORCE)
	
	# Source groups
	foreach(FILE ${${ERM_MODULE}_ALL_SOURCES})
		erm_source_group_for_file("${FILE}")
	endforeach()

	foreach(FILE ${${ERM_MODULE}_MAIN_CPP_SOURCES})
		erm_source_group_for_file("${FILE}")
	endforeach()
	

	# Determine multi target requirements
	list(FIND ALL_DEPS "${ERM_MODULE}_static" STATIC)
	list(FIND ALL_DEPS "${ERM_MODULE}_shared" SHARED)

	if(${STATIC} GREATER_EQUAL 0)
		set(${ERM_MODULE}_STATIC ON CACHE BOOL "" FORCE)
	else()
		set(${ERM_MODULE}_STATIC OFF CACHE BOOL "" FORCE)
	endif()

	if(${SHARED} GREATER_EQUAL 0)
		set(${ERM_MODULE}_SHARED ON CACHE BOOL "" FORCE)
	else()
		set(${ERM_MODULE}_SHARED OFF CACHE BOOL "" FORCE)
	endif()

	if((${ERM_MODULE}_STATIC AND ${ERM_MODULE}_SHARED)) 
		set(${ERM_MODULE}_MULTI_TARGET ON CACHE BOOL "" FORCE)
		set(${ERM_MODULE}_OBJECT ON CACHE BOOL "" FORCE)
	elseif(${ERM_MODULE}_STATIC AND ${ERM_MODULE}_MAIN_CPP_SOURCES)
		set(${ERM_MODULE}_MULTI_TARGET ON CACHE BOOL "" FORCE)
		set(${ERM_MODULE}_OBJECT ON CACHE BOOL "" FORCE)
	elseif(${ERM_MODULE}_SHARED AND ${ERM_MODULE}_MAIN_CPP_SOURCES)
		set(${ERM_MODULE}_MULTI_TARGET ON CACHE BOOL "" FORCE)
		set(${ERM_MODULE}_OBJECT ON CACHE BOOL "" FORCE)
	else()
		set(${ERM_MODULE}_MULTI_TARGET OFF CACHE BOOL "" FORCE)
	endif()

	_erm_setup_interface_lib(${ERM_MODULE})

	# Setup libs and exes
	if(${ERM_MODULE}_MULTI_TARGET)
		_erm_setup_object_lib(${ERM_MODULE})

		# Setup dummy cpp source
		set(${ERM_MODULE}_DUMMY_CPP_SOURCE "${${ERM_MODULE}_GENERATED_DIR}/common/erm/dummy.cpp" CACHE FILEPATH
		"" FORCE)
		file(TOUCH "${${ERM_MODULE}_DUMMY_CPP_SOURCE}" "")
		list(APPEND ${ERM_MODULE}_ALL_SOURCES "${${ERM_MODULE}_DUMMY_CPP_SOURCE}")
		erm_source_group_for_file("${${ERM_MODULE}_DUMMY_CPP_SOURCE}")

		if(${ERM_MODULE}_STATIC)
			_erm_setup_static_lib(${ERM_MODULE} "$<TARGET_OBJECTS:${ERM_MODULE}_obj>;${${ERM_MODULE}_DUMMY_CPP_SOURCE}")
		endif()

		if(${ERM_MODULE}_SHARED)
			_erm_setup_shared_lib(${ERM_MODULE} "$<TARGET_OBJECTS:${ERM_MODULE}_obj>;${${ERM_MODULE}_DUMMY_CPP_SOURCE}")
		endif()
		
		if(${ERM_MODULE}_MAIN_CPP_SOURCES)
			foreach(MAIN_SOURCE ${${ERM_MODULE}_MAIN_CPP_SOURCES})
				_erm_setup_executable(
					ERM_MODULE ${ERM_MODULE}
					MAIN_SOURCE "${MAIN_SOURCE}"
					SOURCES $<TARGET_OBJECTS:${ERM_MODULE}_obj>
				)
			endforeach()
		endif()
	elseif(${ERM_MODULE}_STATIC)
		_erm_setup_static_lib(${ERM_MODULE} ${${ERM_MODULE}_ALL_SOURCES})
		_erm_target_source_files(${ERM_MODULE} ${ERM_MODULE}_static)
	elseif(${ERM_MODULE}_SHARED)
		_erm_setup_static_lib(${ERM_MODULE} ${${ERM_MODULE}_ALL_SOURCES})
		_erm_target_source_files(${ERM_MODULE} ${ERM_MODULE}_shared)
	elseif(${ERM_MODULE}_MAIN_CPP_SOURCES)
		foreach(MAIN_SOURCE ${${ERM_MODULE}_MAIN_CPP_SOURCES})
			_erm_setup_executable(
				ERM_MODULE ${ERM_MODULE}
				MAIN_SOURCE "${MAIN_SOURCE}"
				SOURCES ${${ERM_MODULE}_ALL_SOURCES}
			)
		endforeach()
	endif()
endforeach()

foreach(ERM_MODULE ${ERM_MODULES})
	# Call post generate callback if any
	if(${ERM_MODULE}_POST_GENERATE_CALLBACK)
		message(STATUS "Post generate callback ${ERM_MODULE}")
		erm_call_function(FN_NAME "${${ERM_MODULE}_POST_GENERATE_CALLBACK}")
	endif()
endforeach()
