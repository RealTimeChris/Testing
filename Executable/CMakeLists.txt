add_executable(
	"${PROJECT_NAME}"
	"../main.cpp"
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

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(
	"${PROJECT_NAME}" PUBLIC
	${FFMPEG_LIBRARIES}
	Jsonifier::Jsonifier
	nlohmann_json::nlohmann_json 
)

target_compile_features(
	"${PROJECT_NAME}" PUBLIC
	"cxx_std_20"
)

if (WIN32 AND NOT EXISTS "${VCPKG_ROOT}")
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
