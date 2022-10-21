add_executable(
	"${PROJECT_NAME}"
	"../main.cpp"
	"${PRODUCT_INFO}"
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

set(nlohmann_json_DIR "C:/vcpkg/installed/x64-windows/share/nlohmann_json")

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(
	"${PROJECT_NAME}" PUBLIC
	${FFMPEG_LIBRARIES}
	DiscordCoreAPI::DiscordCoreAPI
	Jsonifier::Jsonifier
	nlohmann_json::nlohmann_json 
)

target_compile_features(
	"${PROJECT_NAME}" PUBLIC
	"cxx_std_20"
)

set_target_properties(
	"${PROJECT_NAME}" PROPERTIES 
	OUTPUT_NAME "${PROJECT_NAME}-Cpp"
	VS_GLOBAL_VcpkgEnabled true
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/$<CONFIG>"
)

if (WIN32 AND NOT EXISTS "${VCPKG_ROOT}")
	install(
		FILES 
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
		DESTINATION "$<CONFIG>"
	)
	install(
		FILES 
		"$<IF:$<CONFIG:Debug>,${DEBUG_PDB_FILE_PATH},${RELEASE_PDB_FILE_PATH}>" 
		DESTINATION "$<CONFIG>"
	)	
	install(
		FILES 
		"$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
		DESTINATION "$<CONFIG>"
	)
endif()

install(
	FILES 
	"$<TARGET_FILE:${PROJECT_NAME}>"
	DESTINATION "$<CONFIG>"
)
