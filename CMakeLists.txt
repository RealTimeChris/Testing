if (WIN32)
	set(simdjson_DIR "C:/Program Files (x86)/simdjson/lib/cmake/simdjson")
	set(Jsonifier_DIR "C:/users/chris/cmake/jsonifier/share/jsonifier")
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
#set(CMAKE_CXX_FLAGS_DEBUG "/O2")
if (UNIX)
	set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Ofast")
endif()
find_package(simdjson CONFIG REQUIRED)
find_package(Jsonifier CONFIG REQUIRED)
message("${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/CMake") 
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_BUILD_TYPE}")
add_subdirectory(Executable)