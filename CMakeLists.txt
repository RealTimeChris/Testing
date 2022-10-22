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
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/CMake") 
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")

if (WIN32)
	set(Jsonifier_DIR "C:/Users/Chris/CMake/Jsonifier/share/jsonifier/")
	set(nlohmann_json_DIR "C:/vcpkg/installed/x64-windows/share/nlohmann_json")
	set(RapidJSON_DIR "C:/vcpkg/installed/x64-windows/share/rapidjson")
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		cmake_path(GET CMAKE_ROOT ROOT_PATH ROOT_DRIVE)
		if (DEFINED ENV{EnableASAN})
			set(CMAKE_INSTALL_PREFIX "${ROOT_DRIVE}Users/$ENV{USERNAME}/CMake/${PROJECT_NAME}/ASAN" CACHE PATH "The CMake install prefix." FORCE)
		else()
			set(CMAKE_INSTALL_PREFIX "${ROOT_DRIVE}Users/$ENV{USERNAME}/CMake/${PROJECT_NAME}" CACHE PATH "The CMake install prefix." FORCE)
		endif()
	endif()
elseif(UNIX)
	set(nlohmann_json_DIR "/home/chris/vcpkg/installed/x64-linux/share/nlohmann_json")
	set(Jsonifier_DIR "/usr/local/share/CMake/Jsonifier/share/jsonifier/")
	set(RapidJSON_DIR "/home/chris/vcpkg/installed/x64-windows/share/rapidjson")
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		if (DEFINED ENV{EnableASAN})
			set(CMAKE_INSTALL_PREFIX "/home/$ENV{USERNAME}/CMake/${PROJECT_NAME}/ASAN" CACHE PATH "The CMake install prefix." FORCE)
		else()
			set(CMAKE_INSTALL_PREFIX "/home/$ENV{USERNAME}/CMake/${PROJECT_NAME}" CACHE PATH "The CMake install prefix." FORCE)
		endif()
	endif()
endif()
 
add_subdirectory(Executable)