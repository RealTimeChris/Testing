#
#	Copyright 2021, 2022 Chris M. (RealTimeChris)
#
#	This library is free software; you can redistribute it and/or
#	modify it under the terms of the GNU Lesser General Public
#	License as published by the Free Software Foundation; either
#	version 2.1 of the License, or (at your option) any later version.
#
#	This library is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#	Lesser General Public License for more details.
#
#	You should have received a copy of the GNU Lesser General Public
#	License along with this library; if not, write to the Free Software
#	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
#	USA
#
# FindFFMPEG.cmake - This file locates the FFMPEG libraries, using a couple of provided paths for searching.
# May 13, 2021
# https://discordcoreapi.com

# Usage:
#	Set the following directories: FFMPEG_RELEASE_ROOT, FFMPEG_DEBUG_ROOT, FFMPEG_INCLUDE_DIR
# Where:
#	FFMPEG_RELEASE_ROOT = The directory containing the RELEASE version of the library, or library's linker file.
#	FFMPEG_DEBUG_ROOT = The directory containing the DEBUG version of the library, or library's linker file.
#	FFMPEG_INCLUDE_DIR = The directory containing the public headers.
# What it produces:
#	FFMPEG::AVCodec, FFMPEG::AVFormat, FFMPEG::AVUtil, and FFMPEG::SWResample
if (UNIX)
	set(LIB_SUFFIX ".a")
	set(LIB_PREFIX "lib")
else()
	set(LIB_SUFFIX ".lib")
	set(LIB_PREFIX "")
endif()
find_library(
	AVCODEC_DEBUG_LIBRARY 
	NAMES "${LIB_PREFIX}avcodec${LIB_SUFFIX}" 
	PATHS "${FFMPEG_DEBUG_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	AVCODEC_RELEASE_LIBRARY 
	NAMES "${LIB_PREFIX}avcodec${LIB_SUFFIX}" 
	PATHS "${FFMPEG_RELEASE_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	AVFORMAT_DEBUG_LIBRARY 
	NAMES "${LIB_PREFIX}avformat${LIB_SUFFIX}"
	PATHS "${FFMPEG_DEBUG_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	AVFORMAT_RELEASE_LIBRARY 
	NAMES "${LIB_PREFIX}avformat${LIB_SUFFIX}"
	PATHS "${FFMPEG_RELEASE_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	AVUTIL_DEBUG_LIBRARY 
	NAMES "${LIB_PREFIX}avutil${LIB_SUFFIX}"
	PATHS "${FFMPEG_DEBUG_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	AVUTIL_RELEASE_LIBRARY 
	NAMES "${LIB_PREFIX}avutil${LIB_SUFFIX}" 
	PATHS "${FFMPEG_RELEASE_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	SWRESAMPLE_DEBUG_LIBRARY 
	NAMES "${LIB_PREFIX}swresample${LIB_SUFFIX}"
	PATHS "${FFMPEG_DEBUG_ROOT}"
	NO_DEFAULT_PATH
)
find_library(
	SWRESAMPLE_RELEASE_LIBRARY 
	NAMES "${LIB_PREFIX}swresample${LIB_SUFFIX}"
	PATHS "${FFMPEG_RELEASE_ROOT}"
	NO_DEFAULT_PATH
)
if (EXISTS "${AVCODEC_DEBUG_LIBRARY}" AND EXISTS "${AVCODEC_RELEASE_LIBRARY}" AND EXISTS "${AVFORMAT_DEBUG_LIBRARY}"
	AND EXISTS "${AVFORMAT_RELEASE_LIBRARY}" AND EXISTS "${AVUTIL_DEBUG_LIBRARY}" AND EXISTS "${AVUTIL_RELEASE_LIBRARY}"
	AND EXISTS "${SWRESAMPLE_DEBUG_LIBRARY}" AND EXISTS "${SWRESAMPLE_RELEASE_LIBRARY}" AND EXISTS "${FFMPEG_INCLUDE_DIR}")
	message(STATUS "Found FFMPEG: TRUE")
else()
	message(FATAL_ERROR "Found FFMPEG: FALSE")
endif()
cmake_path(GET AVCODEC_DEBUG_LIBRARY PARENT_PATH AVCODEC_DEBUG_FILE_PATH)
find_file(
	AVCODEC_DEBUG_DLL
	NAMES "avcodec-58.dll" "libavcodec-58.dll"
	PATHS "${AVCODEC_DEBUG_FILE_PATH}/" "${AVCODEC_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET AVCODEC_RELEASE_LIBRARY PARENT_PATH AVCODEC_RELEASE_FILE_PATH)
find_file(
	AVCODEC_RELEASE_DLL
	NAMES "avcodec-58.dll" "libavcodec-58.dll"
	PATHS "${AVCODEC_RELEASE_FILE_PATH}/" "${AVCODEC_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET AVFORMAT_DEBUG_LIBRARY PARENT_PATH AVFORMAT_DEBUG_FILE_PATH)
find_file(
	AVFORMAT_DEBUG_DLL
	NAMES "avformat-58.dll" "avformat-58.dll"
	PATHS "${AVFORMAT_DEBUG_FILE_PATH}/" "${AVFORMAT_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET AVFORMAT_RELEASE_LIBRARY PARENT_PATH AVFORMAT_RELEASE_FILE_PATH)
find_file(
	AVFORMAT_RELEASE_DLL
	NAMES "avformat-58.dll" "libavformat-58.dll"
	PATHS "${AVFORMAT_RELEASE_FILE_PATH}/" "${AVFORMAT_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET AVUTIL_DEBUG_LIBRARY PARENT_PATH AVUTIL_DEBUG_FILE_PATH)
find_file(
	AVUTIL_DEBUG_DLL
	NAMES "avutil-56.dll" "libavutil-56.dll"
	PATHS "${AVUTIL_DEBUG_FILE_PATH}/" "${AVUTIL_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET AVUTIL_RELEASE_LIBRARY PARENT_PATH AVUTIL_RELEASE_FILE_PATH)
find_file(
	AVUTIL_RELEASE_DLL
	NAMES "avutil-56.dll" "libavutil-56.dll"
	PATHS "${AVUTIL_RELEASE_FILE_PATH}/" "${AVUTIL_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET SWRESAMPLE_DEBUG_LIBRARY PARENT_PATH SWRESAMPLE_DEBUG_FILE_PATH)
find_file(
	SWRESAMPLE_DEBUG_DLL
	NAMES "swresample-3.dll" "libswresample-3.dll"
	PATHS "${SWRESAMPLE_DEBUG_FILE_PATH}/" "${SWRESAMPLE_DEBUG_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
cmake_path(GET SWRESAMPLE_RELEASE_LIBRARY PARENT_PATH SWRESAMPLE_RELEASE_FILE_PATH)
find_file(
	SWRESAMPLE_RELEASE_DLL
	NAMES "swresample-3.dll" "libswresample-3.dll"
	PATHS "${SWRESAMPLE_RELEASE_FILE_PATH}/" "${SWRESAMPLE_RELEASE_FILE_PATH}/../bin/"
	NO_DEFAULT_PATH
)
if (EXISTS "${AVCODEC_DEBUG_DLL}" AND EXISTS "${AVCODEC_RELEASE_DLL}" AND EXISTS "${AVFORMAT_DEBUG_DLL}" AND EXISTS "${AVFORMAT_RELEASE_DLL}"
	AND EXISTS "${AVUTIL_DEBUG_DLL}" AND EXISTS "${AVUTIL_RELEASE_DLL}" AND EXISTS "${SWRESAMPLE_DEBUG_DLL}" AND EXISTS "${SWRESAMPLE_RELEASE_DLL}")
	add_library(FFMPEG::AVCodec SHARED IMPORTED GLOBAL)
	set_target_properties(
		FFMPEG::AVCodec PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${AVCODEC_DEBUG_DLL}" IMPORTED_LOCATION_RELEASE "${AVCODEC_RELEASE_DLL}"
		IMPORTED_IMPLIB_DEBUG "${AVCODEC_DEBUG_LIBRARY}" IMPORTED_IMPLIB_RELEASE "${AVCODEC_RELEASE_LIBRARY}"
	)
	target_include_directories(FFMPEG::AVCodec INTERFACE "${FFMPEG_INCLUDE_DIR}")
	add_library(FFMPEG::AVFormat SHARED IMPORTED GLOBAL)
	set_target_properties(
		FFMPEG::AVFormat PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${AVFORMAT_DEBUG_DLL}" IMPORTED_LOCATION_RELEASE "${AVFORMAT_RELEASE_DLL}"
		IMPORTED_IMPLIB_DEBUG "${AVFORMAT_DEBUG_LIBRARY}" IMPORTED_IMPLIB_RELEASE "${AVFORMAT_RELEASE_LIBRARY}"
	)
	target_include_directories(FFMPEG::AVFormat INTERFACE "${FFMPEG_INCLUDE_DIR}")
	add_library(FFMPEG::AVUtil SHARED IMPORTED GLOBAL)
	set_target_properties(
		FFMPEG::AVUtil PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${AVUTIL_DEBUG_DLL}" IMPORTED_LOCATION_RELEASE "${AVUTIL_RELEASE_DLL}"
		IMPORTED_IMPLIB_DEBUG "${AVUTIL_DEBUG_LIBRARY}" IMPORTED_IMPLIB_RELEASE "${AVUTIL_RELEASE_LIBRARY}"
	)
	target_include_directories(FFMPEG::AVUtil INTERFACE "${FFMPEG_INCLUDE_DIR}")
	add_library(FFMPEG::SWResample SHARED IMPORTED GLOBAL)
	set_target_properties(
		FFMPEG::SWResample PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${SWRESAMPLE_DEBUG_DLL}" IMPORTED_LOCATION_RELEASE "${SWRESAMPLE_RELEASE_DLL}"
		IMPORTED_IMPLIB_DEBUG "${SWRESAMPLE_DEBUG_LIBRARY}" IMPORTED_IMPLIB_RELEASE "${SWRESAMPLE_RELEASE_LIBRARY}"
	)
	target_include_directories(FFMPEG::SWResample INTERFACE "${FFMPEG_INCLUDE_DIR}")
	message(STATUS "Found FFMPEG Dlls: TRUE")
else()
	add_library(FFMPEG::AVCodec STATIC IMPORTED GLOBAL)
	set_target_properties(
		FFMPEG::AVCodec PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${AVCODEC_DEBUG_LIBRARY}" IMPORTED_LOCATION_RELEASE "${AVCODEC_RELEASE_LIBRARY}"
	)
	target_include_directories(FFMPEG::AVCodec INTERFACE "${FFMPEG_INCLUDE_DIR}")
	add_library(FFMPEG::AVFormat STATIC IMPORTED GLOBAL)
	set_target_properties(
		FFMPEG::AVFormat PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${AVFORMAT_DEBUG_LIBRARY}" IMPORTED_LOCATION_RELEASE "${AVFORMAT_RELEASE_LIBRARY}"
	)
	target_include_directories(FFMPEG::AVFormat INTERFACE "${FFMPEG_INCLUDE_DIR}")
	add_library(FFMPEG::AVUtil STATIC IMPORTED GLOBAL)
	set_target_properties(
		FFMPEG::AVUtil PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${AVUTIL_DEBUG_LIBRARY}" IMPORTED_LOCATION_RELEASE "${AVUTIL_RELEASE_LIBRARY}"
	)
	target_include_directories(FFMPEG::AVUtil INTERFACE "${FFMPEG_INCLUDE_DIR}")
	add_library(FFMPEG::SWResample STATIC IMPORTED GLOBAL)
	set_target_properties(
		FFMPEG::SWResample PROPERTIES 
		IMPORTED_LOCATION_DEBUG "${SWRESAMPLE_DEBUG_LIBRARY}" IMPORTED_LOCATION_RELEASE "${SWRESAMPLE_RELEASE_LIBRARY}"
	)
	target_include_directories(FFMPEG::SWResample INTERFACE "${FFMPEG_INCLUDE_DIR}")
	unset(AVCODEC_DEBUG_DLL CACHE)
	unset(AVCODEC_RELEASE_DLL CACHE)
	unset(AVFORMAT_DEBUG_DLL CACHE)
	unset(AVFORMAT_RELEASE_DLL CACHE)
	unset(AVUTIL_DEBUG_DLL CACHE)
	unset(AVUTIL_RELEASE_DLL CACHE)
	unset(SWRESAMPLE_DEBUG_DLL CACHE)
	unset(SWRESAMPLE_RELEASE_DLL CACHE)
	message(STATUS "Found FFMPEG Dlls: FALSE - linking statically")
endif()