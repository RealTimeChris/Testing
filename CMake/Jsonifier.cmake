include(FetchContent)

fetchcontent_declare(
	Jsonifier
	URL https://github.com/RealTimeChris/Jsonifier/releases/download/Master/Jsonifier-src.zip
	GIT_TAG aa509b143c70023b55fe157dec6ab9f39e822490
)

fetchcontent_makeavailable(Jsonifier)