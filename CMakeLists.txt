if (WIN32)
	set(nlohmann_json_DIR "C:/vcpkg/installed/x64-windows/share/nlohmann_json")
	set(Jsonifier_DIR "C:/users/chris/cmake/Jsonifier/share/jsonifier")
elseif(UNIX)
	set(nlohmann_json_DIR "/home/chris/vcpkg/installed/x64-linux/share/nlohmann_json")
endif()

set(DOWNLOAD_EXTRACT_TIMESTAMP true)

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
set(CMAKE_CXX_FLAGS_DEBUG "/O2")
if (UNIX)
	set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Ofast")
endif()
find_package(Jsonifier CONFIG REQUIRED)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/CMake") 
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")
add_subdirectory(Executable)