include(FetchContent)

fetchcontent_declare(
	Jsonifier
	URL https://github.com/RealTimeChris/Jsonifier/releases/download/Master/Jsonifier-src.zip
	GIT_TAG d90bca72cacab78e1488770115cbd2da555bcbe6
)

fetchcontent_makeavailable(Jsonifier)