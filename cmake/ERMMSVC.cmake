string(JOIN " " CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS}"
	/W3							# Enables level 3 warnings
	/WX							# Treats warnings as errors
	/Zc:preprocessor			# Enables preprocessor conformance mode
	/GR-						# Disables run-time type information
	/wd4065						# Disable warning for no case label
	/MP							# Enables multi threaded builds
	/D_WINDOWS 
	/D_HAS_EXCEPTIONS=0 		# Disables exceptions
	/D_CRT_SECURE_NO_WARNINGS 
	/D_USE_MATH_DEFINES 
	/DNOMINMAX
	/DERM_HOST_WINDOWS
	/D${ERM_RENDERING_API_COMPILE_DEF}
)

string(JOIN " " CMAKE_CXX_FLAGS_DEBUG
	"${CMAKE_CXX_FLAGS_DEBUG}"
	/RTC1
	/Ob0
	/Od
)

string(JOIN " " CMAKE_CXX_FLAGS_RELEASE
	"${CMAKE_CXX_FLAGS_RELEASE}"
	/Zi     					# Include debug info
	/O2
	/Ob2
	/DNDEBUG
)

string(JOIN " " CMAKE_EXE_LINKER_FLAGS_DEBUG
	"${CMAKE_EXE_LINKER_FLAGS_DEBUG}"
	/INCREMENTAL
	/DEBUG
)

string(JOIN " " CMAKE_EXE_LINKER_FLAGS_RELEASE
	"${CMAKE_EXE_LINKER_FLAGS_RELEASE}"
	/INCREMENTAL:NO
	/OPT:REF
	/OPT:ICF
)

set(CMAKE_GENERATOR_PLATFORM "x64" CACHE STRING "" FORCE)
