include(FetchContent)

fetchcontent_declare(
	Jsonifier
	URL https://github.com/RealTimeChris/Jsonifier/releases/download/Master/Jsonifier-src.zip
	GIT_TAG 5421d1fecd48475079c0363d9b5d10597d1da493
)

fetchcontent_makeavailable(Jsonifier)