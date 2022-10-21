cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME "Testing")
set(PRODUCT_VERSION "1.0.0.9")
set(DESCRIPTION "A testing program for the Jsonifier library.")

project(
	"${PROJECT_NAME}"
	VERSION "${PRODUCT_VERSION}"
	LANGUAGES CXX
	HOMEPAGE_URL "https://github.com/RealTimeChris/Jsonifier"
	DESCRIPTION "${DESCRIPTION}"
)

set(CMAKE_MODULE_PATH "/usr/share/CMake-3.22/Modules;${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/CMake;") 
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

if (WIN32)
	set(nlohmann_json_DIR "C:/vcpkg/installed/x64-windows/share/nlohmann_json")
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		cmake_path(GET CMAKE_ROOT ROOT_PATH ROOT_DRIVE)
		set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "The CMake install prefix." FORCE)
	endif()
elseif(UNIX)
set(nlohmann_json_DIR "/home/chris/vcpkg/installed/x64-windows/share/nlohmann_json")
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "The CMake install prefix." FORCE)
	endif()
else()
		message(FATAL_ERROR "Sorry, but your OS is not supported!")
endif()

add_executable("${PROJECT_NAME}" "main.cpp")

target_compile_features(
	"${PROJECT_NAME}" PRIVATE
	"cxx_std_20"
)

target_compile_definitions(
	"${PROJECT_NAME}" PRIVATE
	"$<$<STREQUAL:${VCPKG_TARGET_TRIPLET},x64-uwp>:UWP>"
	"$<$<PLATFORM_ID:Windows>:SIMDJSON_USING_WINDOWS_DYNAMIC_LIBRARY=1>"
)

target_compile_options(
	"${PROJECT_NAME}" PRIVATE
	"$<$<NOT:$<PLATFORM_ID:Linux>>:/bigobj>"
	"$<$<PLATFORM_ID:Windows>:$<$<STREQUAL:$ENV{EnableASAN},true>:/fsanitize=address>>"
	"$<$<PLATFORM_ID:Windows>:$<$<STREQUAL:$ENV{EnableASAN},true>:/Zi>>"
	"$<$<PLATFORM_ID:Windows>:/MP>"
	"$<$<PLATFORM_ID:Linux>:$<$<STREQUAL:$ENV{EnableASAN},true>:-fsanitize=address>>"
	#"$<$<PLATFORM_ID:Linux>:-fpermissive>"
)

target_link_options(
	"${PROJECT_NAME}" PRIVATE
	"$<$<PLATFORM_ID:Windows>:$<$<STREQUAL:$ENV{EnableASAN},true>:/Debug>>"
	"$<$<PLATFORM_ID:Windows>:/DEBUG>"
	"$<$<PLATFORM_ID:Linux>:$<$<STREQUAL:$ENV{EnableASAN},true>:-fsanitize=address>>"
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

include(Jsonifier)
find_package(nlohmann_json REQUIRED)
target_link_libraries("${PROJECT_NAME}" PUBLIC Jsonifier::Jsonifier)
target_link_libraries("${PROJECT_NAME}" PUBLIC nlohmann_json::nlohmann_json)
target_include_directories("${PROJECT_NAME}" PUBLIC "${jsonifier_SOURCE_DIR}")

if (WIN32)
	install(
		FILES 
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
		DESTINATION "${Testing_BINARY_DIR}/$<CONFIG>"
	)
	install(
		FILES 
		"$<IF:$<CONFIG:Debug>,${DEBUG_PDB_FILE_PATH},${RELEASE_PDB_FILE_PATH}>" 
		DESTINATION "${Testing_BINARY_DIR}/$<CONFIG>"
	)	
	install(
		FILES 
		"$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
		DESTINATION "${Testing_BINARY_DIR}/$<CONFIG>"
	)
endif()

install(
	FILES 
	"$<TARGET_FILE:${PROJECT_NAME}>"
	DESTINATION "${Testing_BINARY_DIR}/$<CONFIG>"
)
