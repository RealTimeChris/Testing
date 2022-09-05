﻿include(GenerateProductVersion)
generate_product_version(
	PRODUCT_INFO
	NAME "${PROJECT_NAME}"
	ICON "${CMAKE_SOURCE_DIR}/Chris 01.ico"
	ORIGINAL_FILENAME "${PROJECT_NAME}-Cpp.exe"
	VERSION_MAJOR "1"
	VERSION_MINOR "0"
	VERSION_PATCH "0"
	VERSION_REVISION "9"
)

add_executable(
	"${PROJECT_NAME}"
	"../main.cpp"
	"${PRODUCT_INFO}"
)
target_sources(
	"${PROJECT_NAME}"
PUBLIC "../ErlPacker.cpp"
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

target_link_libraries(
	"${PROJECT_NAME}" PUBLIC
	DiscordCoreAPI::DiscordCoreAPI
	simdjson::simdjson
)

target_include_directories(
"${PROJECT_NAME}" PUBLIC
	"C:/vcpkg/installed/x64-windows/include")

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