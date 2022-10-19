include(fetchcontent)

fetchcontent_declare(
	Jsonifier
	URL https://github.com/RealTimeChris/Jsonifier/releases/download/Master/Jsonifier-src.zip
	GIT_TAG ee9b44fa63be0d3f05ed1627835c0222d509a15cw
)

fetchcontent_makeavailable(Jsonifier)