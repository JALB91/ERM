project(
	"ERM_Libs"
	VERSION 1.0.0
	DESCRIPTION "ERM Libraries"
)

file(GLOB ERM_LIBS LIST_DIRECTORIES true *)

foreach(ERM_LIB ${ERM_LIBS})
    if(IS_DIRECTORY "${ERM_LIB}")
        add_subdirectory("${ERM_LIB}")
    endif()
endforeach()
