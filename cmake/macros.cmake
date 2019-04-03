function(CREATE_GROUPS SRCS)
	foreach(FILE ${SRCS}) 
		# Get the directory of the source file
		get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

		# Remove common directory prefix to make the group
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
		string(REPLACE "/include/" "" GROUP "${GROUP}")
		string(REPLACE "/src/" "" GROUP "${GROUP}")
		string(REPLACE "/" "\\" GROUP "${GROUP}")

		source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endfunction()
