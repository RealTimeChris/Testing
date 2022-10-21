include(FetchContent)

fetchcontent_declare(
	Jsonifier
	URL https://github.com/RealTimeChris/Jsonifier/releases/download/Master/Jsonifier-src.zip
	GIT_TAG e51516f178006f1fb73e4584cc2938509cbfa20f
)

fetchcontent_makeavailable(Jsonifier)