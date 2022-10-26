add_executable(
	"${PROJECT_NAME}"
	"../main.cpp"
)

if (DEFINED ENV{EnableASAN})
	if(MSVC)
		target_compile_options("${PROJECT_NAME}" PUBLIC "/fsanitize=address" "/Zi" "/Debug")
		target_link_options("${PROJECT_NAME}" PUBLIC "/incremental:no" "/Debug")	
	else()
		target_compile_options("${PROJECT_NAME}" PUBLIC "-fsanitize=address")
		target_link_options("${PROJECT_NAME}" PUBLIC "-fsanitize=address")
	endif()
endif()

#include(Jsonifier)

find_package(nlohmann_json CONFIG REQUIRED)

target_include_directories(
	"${PROJECT_NAME}" 
	PUBLIC 
	Jsonifier_INTERFACE_INCLUDE_DIR
)

target_link_libraries(
	"${PROJECT_NAME}" PUBLIC
	Jsonifier::Jsonifier
	nlohmann_json::nlohmann_json 
)

target_compile_options(
	"${PROJECT_NAME}" PRIVATE
	"$<$<PLATFORM_ID:Windows>:/MP>"
	"$<$<PLATFORM_ID:Linux>:-fpermissive>"
	"$<$<PLATFORM_ID:Linux>:-mavx2>"
	"$<$<PLATFORM_ID:Linux>:-mavx>"
)

target_compile_features(
	"${PROJECT_NAME}" PUBLIC
	"cxx_std_20"
)

if (WIN32)
	install(
		FILES 
		"$<IF:$<CONFIG:Debug>,${DEBUG_PDB_FILE_PATH},${RELEASE_PDB_FILE_PATH}>" 
		DESTINATION "${CMAKE_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE}/Executable/${CMAKE_BUILD_TYPE}"
	)	
	install(
		FILES 
		"$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
		DESTINATION "${CMAKE_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE}/Executable/${CMAKE_BUILD_TYPE}"
	)
endif()

install(
		FILES 
		"$<TARGET_FILE:${PROJECT_NAME}>"
		DESTINATION "${CMAKE_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE}/Executable/${CMAKE_BUILD_TYPE}"
)