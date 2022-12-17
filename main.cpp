#include "Jsonifier.hpp"
#include "DataParsingFunctionc.hpp"
#include <simdjson.h>
//#include "src/generic/stage2/tape_builder.h"

struct ActivitiesJson {
	ActivitiesJson() noexcept = default;
	ActivitiesJson(Jsonifier::Jsonifier&& value) {
		this->createdAt = value["created_at"].getValue<std::string>();
		this->id = value["id"].getValue<std::string>();
		this->testDouble = value["test_double"].getValue<double>();
		//this->createdAt02 = value["created_at02"].getValue<std::string>();
		//this->id02 = value["id02"].getValue<std::string>();
	};
	std::string createdAt{};
	double testDouble{};
	//std::string createdAt02{};
	//std::string id02{};
	std::string id{};
	operator Jsonifier::Jsonifier() {
		Jsonifier::Jsonifier data{};
		data["test_double"] = this->testDouble;
		data["created_at02"] = this->createdAt;
		data["id02"] = this->id;
		return data;
	}
};

struct TheDJson {
	TheDJson() noexcept = default;
	TheDJson(Jsonifier::Jsonifier& value) {
		//value["d"]["activitiess"].refreshString(Jsonifier::JsonifierSerializeType::Json);
		//std::cout << "THE SIZE: " << value["d"]["activitiess"].operator std::basic_string_view<char, std::char_traits<char>>() << std::endl;
		auto theArray = value["d"]["activitiess"].getValue<std::vector<Jsonifier::Jsonifier>>();
		for (auto& value: theArray) {
			activities.emplace_back(std::move(value));
		}
	}
	operator Jsonifier::Jsonifier() {
		Jsonifier::Jsonifier serializer{};
		for (auto& value: this->activities) {
			serializer["d"]["activitiess"].emplaceBack(value);
		}
		return serializer;
	}
	std::vector<ActivitiesJson> activities{};
};

namespace Jsonifier {
	template<> TheDJson Jsonifier::getValue() {
		return TheDJson{ *this };
	}

}

struct TheValueJson {
	TheValueJson(Jsonifier::Jsonifier& value) {
		this->theD = value.getValue<TheDJson>();
	}
	TheDJson theD{};
};

struct Activities {
	Activities(simdjson::ondemand::value value) {
		this->createdAt = DiscordCoreAPI::getString(value, "created_at");
		this->id = DiscordCoreAPI::getString(value, "id");
		this->testDouble = DiscordCoreAPI::getFloat(value, "test_double");
		//this->createdAt02 = DiscordCoreAPI::getString(value, "created_at02");
		//this->id02 = DiscordCoreAPI::getString(value, "id02");
	};
	//std::string createdAt02{};
	//std::string id02{};
	std::string createdAt{};
	double testDouble{};
	std::string id{};
};

struct TheD {
	TheD() noexcept = default;
	TheD(simdjson::ondemand::value value) {
		simdjson::ondemand::value valueNew{};
		value["d"]["activitiess"].get(valueNew);
		auto theArray = DiscordCoreAPI::getArray(value["d"], "activitiess");
		if (theArray.didItSucceed) {
			for (auto value: theArray.arrayValue) {
				activities.push_back(value.value());
			}
		}
	}
	std::vector<Activities> activities{};
};

struct TheValue {
	TheValue(simdjson::ondemand::value value) {
		this->theD = TheD{ value };
	}
	TheD theD{};
};

template<typename OTy> void prepStringForParsing(std::basic_string<OTy>& string) {
	string.resize(string.size() + 256 - string.size() % 256);
}

int32_t main() noexcept {
	try {
		std::string stringNew{ "{\"d\":{\"activitiess\":[{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|\",\"test_double\":334.4545,"
			"\"type\":3,\"ANOTHER_VALUE\":3434,\"ANOTHER_TEST_VALUE\":\"TESTING-TESTING\",\"ANOTHER_VALUE_02\":3434,\"ANOTHER_TEST_"
			"VALUE_03\":\"TESTING-TESTING_031\",\"ANOTHER_VALUE_02w\":3434,\"ANOTHER_TEST_VALUE_03d\":\"TESTING-TESTING_031d\"}]}}"
		};
		//std::string stringNew{ "{\"d\":{\"activitiess\":[{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"created_at02\":\"1669495273631\",\"id2\":\"ec0b28a579ecb4bd\"}]}}" };

		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		size_t oldSize = stringNew.size();


		totalSize = 0;
		totalTime = 0;

		//TheDJson newValue{};
		//newValue.activities.push_back(ActivitiesJson{});
		//auto jsonDataNew = newValue.operator Jsonifier::JsonSerializer();
		//jsonDataNew.refreshString(Jsonifier::JsonifierSerializeType::Json);

		std::cout << "THE STRING: " << stringNew << std::endl;
		prepStringForParsing(stringNew);
		Jsonifier::Jsonifier jsonDataNew{};
		jsonDataNew["testing"] = Jsonifier::JsonType::Array;
		jsonDataNew["testing"].emplaceBack("TESTING");
		jsonDataNew.refreshString(Jsonifier::JsonifierSerializeType::Json);
		
		std::cout << "THE NEWER JSON DATA: " << jsonDataNew.operator std::basic_string_view<char, std::char_traits<char>>() << std::endl;
		stopWatch.resetTimer();
		for (size_t x = 0ull; x < 2048ull * 64ull; ++x) {
			Jsonifier::SimdJsonValue stringScanner{ stringNew.data(), stringNew.size() };
			Jsonifier::Jsonifier jsonData = std::move(stringScanner.getJsonData());
			//jsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
			//std::cout << "THE DATA" << jsonData.operator std::basic_string_view<char, std::char_traits<char>>() << std::endl;
			TheValueJson theValue{ jsonData };
			//std::cout << "THE VALUE 01: " << theValue.theD.activities.back().createdAt << std::endl;
			std::cout << "THE VALUE 02: " << theValue.theD.activities.back().testDouble << std::endl;
			//std::cout << "THE VALUE 03: " << theValue.theD.activities.back().id<< std::endl;
			//std::cout << "THE VALUE 04: " << theValue.theD.activities.back().id02 << std::endl;
			totalSize += oldSize;
		}
		totalTime += stopWatch.totalTimePassed().count();
		//jsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
		//std::cout << "THE DATA" << jsonData.operator std::basic_string_view<char, std::char_traits<char>>() << std::endl;
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		//		jsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
		//std::cout << "THE DATA" << jsonData.operator std::basic_string_view<char, std::char_traits<char>>() << std::endl;
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		auto stringNewer = stringNew;


		stringNewer.reserve(stringNewer.size() + simdjson::SIMDJSON_PADDING);
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		simdjson::ondemand::parser parser{};
		parser.allocate(1024 * 1024);
		for (size_t x = 0ull; x < 2048ull * 64ull; ++x) {
			auto newDocument = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheValue theValue{ newDocument };
			//std::cout << "THE VALUE 01: " << theValue.theD.activities.back().createdAt << std::endl;
			////std::cout << "THE VALUE 02: " << theValue.theD.activities.back().createdAt02<< std::endl;
			//std::cout << "THE VALUE 03: " << theValue.theD.activities.back().id<< std::endl;
			//std::cout << "THE VALUE 04: " << theValue.theD.activities.back().id02 << std::endl;
			totalSize += oldSize;
		}

		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		std::cout << "THE STRING: " << stringNew << std::endl;
		std::cout << "THE STRING SIZE: " << stringNew.size() << std::endl;


	} catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}


	return 0;
};