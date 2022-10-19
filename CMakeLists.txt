# Set this value...
set(VCPKG_ROOT_DIR "")
# OR this value...
set(CMAKE_CONFIG_FILE_DIR "C:/Users/Chris/CMake/DiscordCoreAPI/share/discordcoreapi")
# AND these values.
set(FFMPEG_DIR "C:/Vcpkg/installed/x64-windows/share/ffmpeg") # Set this one to the folder location of the file "FindFFMPEG.cmake".
set(Opus_DIR "C:/Vcpkg/installed/x64-windows/share/opus") # Set this one to the folder location of the file "OpusConfig.cmake".
set(simdjson_DIR "C:/Vcpkg/installed/x64-windows/share/simdjson") # Set this one to the folder location of the file "simdjson-config.cmake".
set(unofficial-sodium_DIR "C:/Vcpkg/installed/x64-windows/share/unofficial-sodium") # Set this one to the folder location of the file "unofficial-sodiumConfig.cmake".
set(OPENSSL_ROOT_DIR "C:/Vcpkg/installed/x64-windows/") # Set this one to the folder location of the include folder and library folders of OpenSSL.
# Also, set this value to install the output files to a particular location.

cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME "Testing")
set(PRODUCT_VERSION "1.0.0.9")
set(DESCRIPTION "A Discord bot, written in C++, using the DiscordCoreAPI Discord bot library.")
set(DiscordCoreAPI_DIR "${CMAKE_CONFIG_FILE_DIR}")
project(
	"${PROJECT_NAME}"
	VERSION "${PRODUCT_VERSION}"
	LANGUAGES CXX
	HOMEPAGE_URL "https://discordcoreapi.com"
	DESCRIPTION "${DESCRIPTION}"
)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/CMake;") 
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")
if (WIN32)
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		cmake_path(GET CMAKE_ROOT ROOT_PATH ROOT_DRIVE)
		message("WEVE GOT THE DEFAULT CMAKE INSTALL PREFIX!")
		set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "The CMake install prefix." FORCE)
		message("${CMAKE_INSTALL_PREFIX}")
	endif()
elseif(UNIX)
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE PATH "The CMake install prefix." FORCE)
	endif()
else()
		message(FATAL_ERROR "Sorry, but your OS is not supported!")
endif()
message("${CMAKE_INSTALL_PREFIX}")

add_executable("${PROJECT_NAME}" "./main.cpp")

include(Jsonifier)

target_link_libraries("${PROJECT_NAME}" PUBLIC Jsonifier::Jsonifier)

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
