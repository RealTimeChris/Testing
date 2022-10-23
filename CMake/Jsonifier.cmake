include(FetchContent)

fetchcontent_declare(
	Jsonifier
	URL https://github.com/RealTimeChris/Jsonifier/releases/download/Master/Jsonifier-src.zip
	GIT_TAG d3a1de8f5d9bfc01a52c20ae4ede7acfe2306b50
)

fetchcontent_makeavailable(Jsonifier)