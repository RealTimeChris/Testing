# Set this value...
set(VCPKG_ROOT_DIR "")
# OR this value...
set(CMAKE_CONFIG_FILE_DIR "C:/users/chris/cmake/discordcoreapi/share/discordcoreapi")
# AND these values.
set(FFMPEG_DIR "C:/Vcpkg/installed/x64-windows/share/ffmpeg") # Set this one to the folder location of the file "FindFFMPEG.cmake".
set(Opus_DIR "C:/Vcpkg/installed/x64-windows/share/opus") # Set this one to the folder location of the file "OpusConfig.cmake".
set(simdjson_DIR "C:/Vcpkg/installed/x64-windows/share/simdjson") # Set this one to the folder location of the file "simdjson-config.cmake".
set(unofficial-sodium_DIR "C:/Vcpkg/installed/x64-windows/share/unofficial-sodium") # Set this one to the folder location of the file "unofficial-sodiumConfig.cmake".
set(OPENSSL_ROOT_DIR "C:/Vcpkg/installed/x64-windows/") # Set this one to the folder location of the include folder and library folders of OpenSSL.
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


include(FetchContent)
FetchContent_Declare(
	Jsonifier 
	URL https://github.com/RealTimeChris/Jsonifier/releases/download/Master/Jsonifier-src.zip
)

FetchContent_MakeAvailable(Jsonifier)


add_subdirectory(Executable)