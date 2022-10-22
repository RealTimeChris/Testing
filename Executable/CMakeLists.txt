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
find_package(RapidJSON CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Jsonifier CONFIG REQUIRED)
target_link_libraries(
	"${PROJECT_NAME}" PUBLIC
	Jsonifier::Jsonifier
	nlohmann_json::nlohmann_json 
)

target_compile_features(
	"${PROJECT_NAME}" PUBLIC
	"cxx_std_20"
)

if (WIN32)
	install(
		FILES 
		"$<IF:$<CONFIG:Debug>,${DEBUG_PDB_FILE_PATH},${RELEASE_PDB_FILE_PATH}>" 
		DESTINATION "${CMAKE_BINARY_DIR}/Executable/Release"
	)	
	install(
		FILES 
		"$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
		DESTINATION "${CMAKE_BINARY_DIR}/Executable/Release"
	)
endif()
set(OUT_VAR "")
get_target_property(OUT_VAR "${PROJECT_NAME}" COMPILE_DEFINITIONS  )
