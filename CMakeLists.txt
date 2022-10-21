# Set this value...
set(VCPKG_ROOT_DIR "")
# Also, set this value to install the output files to a particular location.
set(CMAKE_INSTALL_PREFIX)

cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME "Testing")
set(PRODUCT_VERSION "1.0.0.9")
set(DESCRIPTION "A Discord bot, written in C++, using the DiscordCoreAPI Discord bot library.")

project(
	"${PROJECT_NAME}"
	VERSION "${PRODUCT_VERSION}"
	LANGUAGES CXX
	HOMEPAGE_URL "https://discordcoreapi.com"
	DESCRIPTION "${DESCRIPTION}"
)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/CMake;${FFMPEG_DIR}") 
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

if (WIN32)
	set(nlohmann_json_DIR "C:/vcpkg/installed/x64-windows/share/nlohmann_json")
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		cmake_path(GET CMAKE_ROOT ROOT_PATH ROOT_DRIVE)
		set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/Output Files/" CACHE PATH "The CMake install prefix." FORCE)
	endif()
elseif(UNIX)
	set(nlohmann_json_DIR "/home/chris/vcpkg/installed/x64-linux/share/nlohmann_json")
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/Output Files/" CACHE PATH "The CMake install prefix." FORCE)
	endif()
else()
		message(FATAL_ERROR "Sorry, but your OS is not supported!")
endif()

add_subdirectory(Executable)