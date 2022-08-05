project(
	"ERM_Modules"
	VERSION 1.0.0
	DESCRIPTION "ERM Modules"
)

file(GLOB MODULES LIST_DIRECTORIES true *)

foreach(MODULE ${MODULES})
    if(IS_DIRECTORY "${MODULE}")
        add_subdirectory("${MODULE}")
    endif()
endforeach()
