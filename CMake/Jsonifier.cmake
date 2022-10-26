include(FetchContent)

fetchcontent_declare(
	Jsonifier
	URL https://github.com/RealTimeChris/Jsonifier/releases/download/Master/Jsonifier-src.zip
	GIT_TAG 1f52dc8042ca3e1f488d08170f08d2e88fba04e1
)

fetchcontent_makeavailable(Jsonifier)