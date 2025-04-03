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
unset(ALL_DEPS CACHE)
unset(ERM_MODULES CACHE)

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
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Modules")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_NN")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Rendering")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Setup")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_System")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Utils")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ERM_Window")

foreach(ERM_MODULE ${ERM_MODULES})
	unset(${ERM_MODULE}_ALL_SOURCES CACHE)

	# Call post setup callback if any
	if(${ERM_MODULE}_POST_SETUP_CALLBACK)
		message(STATUS "Post setup callback ${ERM_MODULE}")
		erm_call_function(FN_NAME "${${ERM_MODULE}_POST_SETUP_CALLBACK}")
	endif()

	# Gather all dependencies
	list(APPEND ALL_DEPS
		${${ERM_MODULE}_LINK_INTERFACE}
		${${ERM_MODULE}_LINK_PRIVATE}
		${${ERM_MODULE}_LINK_PUBLIC}
	)

	list(APPEND ${ERM_MODULE}_ALL_DEPS
		${${ERM_MODULE}_LINK_INTERFACE}
		${${ERM_MODULE}_LINK_PRIVATE}
		${${ERM_MODULE}_LINK_PUBLIC}
	)
endforeach()

foreach(ERM_MODULE ${ERM_MODULES})
	# Call pre generate callback if any
	if(${ERM_MODULE}_PRE_GENERATE_CALLBACK)
		message(STATUS "Pre generate callback ${ERM_MODULE}")
		erm_call_function(FN_NAME "${${ERM_MODULE}_PRE_GENERATE_CALLBACK}")
	endif()
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
	list(FIND ALL_DEPS "${ERM_MODULE}_obj" OBJECT)
	list(FIND ALL_DEPS "${ERM_MODULE}_int" INTERFACE)
	list(FIND ALL_DEPS "${ERM_MODULE}_static" STATIC)
	list(FIND ALL_DEPS "${ERM_MODULE}_shared" SHARED)

	if(${OBJECT} GREATER_EQUAL 0)
		set(${ERM_MODULE}_OBJECT ON CACHE BOOL "" FORCE)
	else()
		set(${ERM_MODULE}_OBJECT OFF CACHE BOOL "" FORCE)
	endif()

	if(${INTERFACE} GREATER_EQUAL 0)
		set(${ERM_MODULE}_INTERFACE ON CACHE BOOL "" FORCE)
	else()
		set(${ERM_MODULE}_INTERFACE OFF CACHE BOOL "" FORCE)
	endif()

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

	# Always create interface lib
	_erm_setup_interface_lib(${ERM_MODULE})

	# Create object lib
	if(${ERM_MODULE}_OBJECT OR ${ERM_MODULE}_MULTI_TARGET)
		_erm_setup_object_lib(${ERM_MODULE})

		# Setup dummy cpp source
		set(${ERM_MODULE}_DUMMY_CPP_SOURCE "${${ERM_MODULE}_GENERATED_DIR}/common/erm/dummy.cpp" CACHE FILEPATH
		"" FORCE)
		file(TOUCH "${${ERM_MODULE}_DUMMY_CPP_SOURCE}" "")
		list(APPEND ${ERM_MODULE}_ALL_SOURCES "${${ERM_MODULE}_DUMMY_CPP_SOURCE}")
		erm_source_group_for_file("${${ERM_MODULE}_DUMMY_CPP_SOURCE}")
	endif()

	# Create static lib
	if(${ERM_MODULE}_STATIC)
		if(${ERM_MODULE}_OBJECT)
			_erm_setup_static_lib(
				${ERM_MODULE} 
				"$<TARGET_OBJECTS:${ERM_MODULE}_obj>;${${ERM_MODULE}_DUMMY_CPP_SOURCE}"
			)
		else()
			_erm_setup_static_lib(${ERM_MODULE} ${${ERM_MODULE}_ALL_SOURCES})
			_erm_target_source_files(${ERM_MODULE} ${ERM_MODULE}_static)
		endif()
	endif()
	
	# Create shared lib
	if(${ERM_MODULE}_SHARED)
		if(${ERM_MODULE}_OBJECT)
			_erm_setup_shared_lib(
				${ERM_MODULE} 
				"$<TARGET_OBJECTS:${ERM_MODULE}_obj>;${${ERM_MODULE}_DUMMY_CPP_SOURCE}"
			)
		else()
			_erm_setup_shared_lib(${ERM_MODULE} ${${ERM_MODULE}_ALL_SOURCES})
			_erm_target_source_files(${ERM_MODULE} ${ERM_MODULE}_shared)
		endif()
	endif()

	# Create executables
	if(${ERM_MODULE}_MAIN_CPP_SOURCES)
		if(${ERM_MODULE}_OBJECT)
			foreach(MAIN_SOURCE ${${ERM_MODULE}_MAIN_CPP_SOURCES})
				_erm_setup_executable(
					ERM_MODULE ${ERM_MODULE}
					MAIN_SOURCE "${MAIN_SOURCE}"
					SOURCES $<TARGET_OBJECTS:${ERM_MODULE}_obj>
				)
			endforeach()
		else()
			foreach(MAIN_SOURCE ${${ERM_MODULE}_MAIN_CPP_SOURCES})
				_erm_setup_executable(
					ERM_MODULE ${ERM_MODULE}
					MAIN_SOURCE "${MAIN_SOURCE}"
					SOURCES ${${ERM_MODULE}_ALL_SOURCES}
				)
			endforeach()
		endif()
	endif()
endforeach()

foreach(ERM_MODULE ${ERM_MODULES})
	# Call post generate callback if any
	if(${ERM_MODULE}_POST_GENERATE_CALLBACK)
		message(STATUS "Post generate callback ${ERM_MODULE}")
		erm_call_function(FN_NAME "${${ERM_MODULE}_POST_GENERATE_CALLBACK}")
	endif()
endforeach()
