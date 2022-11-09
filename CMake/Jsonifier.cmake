include(FetchContent)

fetchcontent_declare(
	Jsonifier
	URL https://github.com/RealTimeChris/Jsonifier/releases/download/Master/Jsonifier-src.zip
	GIT_TAG cbd26d7efe963f59829cd5bcaa9d6740ddd5daec
)

fetchcontent_makeavailable(Jsonifier)