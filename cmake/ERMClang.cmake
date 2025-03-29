string(JOIN " " CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS}"
	-fno-exceptions				# Disables exceptions
	-fno-rtti					# Disables rtti
	-Wall						# Enable all warnings
	-Werror						# Treat warnings as errors
	-Wextra						# Enable extra warnings
	-pedantic-errors			# Error if a feature from a later standard revision is used in an earlier mode.
	-Wno-gnu					# 
	-D${ERM_RENDERING_API_COMPILE_DEF}
)

if(ERM_HOST_OSX)
	string(JOIN " " CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS}"
		-xobjective-c++
		-DERM_HOST_OSX
	)

	string(JOIN " " CMAKE_C_FLAGS
		"${CMAKE_C_FLAGS}"
		-xobjective-c
	)
elseif(ERM_HOST_WINDOWS)
	string(JOIN " " CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS}"
		-DERM_HOST_WINDOWS
	)
elseif(ERM_HOST_LINUX)
	string(JOIN " " CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS}"
		-DERM_HOST_LINUX
	)
endif()
