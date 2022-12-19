#file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(
	"${PROJECT_NAME}"
	#"${SOURCES}"
	"../main.cpp"
	"../Jsonifier.cpp"
	"../DataParsingFunctionc.cpp"
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
	simdjson::simdjson
	#Jsonifier::Jsonifier
)

#target_include_directories("${PROJECT_NAME}" PUBLIC "../src/generic/" "../src/" "../include/simdjson")


target_compile_options(
	"${PROJECT_NAME}" PUBLIC
	"$<$<CXX_COMPILER_ID:MSVC>:/fp:fast>"
	"$<$<CXX_COMPILER_ID:MSVC>:/Oi>"
	"$<$<CXX_COMPILER_ID:MSVC>:/Ot>"
	"$<$<CXX_COMPILER_ID:MSVC>:$<$<STREQUAL:$ENV{EnableASAN},true>:/fsanitize=address>>"
	"$<$<CXX_COMPILER_ID:MSVC>:$<$<STREQUAL:$ENV{EnableASAN},true>:/Zi>>"
	"$<$<CXX_COMPILER_ID:MSVC>:/MP>"
	"$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>"
	"$<$<CXX_COMPILER_ID:GNU>:$<$<STREQUAL:$ENV{EnableASAN},true>:-fsanitize=address>>"
	"$<$<CXX_COMPILER_ID:GNU>:-fpermissive>"
	"$<$<CXX_COMPILER_ID:GNU>:-mavx2>"
)

target_compile_features(
	"${PROJECT_NAME}" PUBLIC
	"cxx_std_20"
)

if (WIN32)
	install(
		FILES 
		"$<IF:$<CONFIG:Debug>,${DEBUG_PDB_FILE_PATH},${RELEASE_PDB_FILE_PATH}>" 
		DESTINATION "${CMAKE_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE}/Executable/${CMAKE_BUILD_TYPE}"
	)	
	install(
		FILES 
		"$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
		DESTINATION "${CMAKE_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE}/Executable/${CMAKE_BUILD_TYPE}"
	)
endif()

install(
		FILES 
		"$<TARGET_FILE:${PROJECT_NAME}>"
		DESTINATION "${CMAKE_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE}/Executable/${CMAKE_BUILD_TYPE}"
)