erm_library_project(0.0.1 "ERM System lib")

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		ERM_Utils
		ERM_FileSystem
	PUBLIC
		ERM_Math
		refl-cpp
)
