#ifndef _TESTING
	#define _TESTING
#endif

#include "Jsonifier.hpp"
#include <simdjson.h>
#include "DataParsingFunctionc.hpp"
#include <fstream>

struct ActivitiesJson {
	ActivitiesJson() noexcept = default;
	ActivitiesJson(Jsonifier::JsonParser&& value) {
		this->createdAt = value["created_at"].getValue<std::string>();
		this->name = value["name"].getValue<std::string>();
		this->anotherValue = value["ANOTHER_VALUE"].getValue<int32_t>();
		this->anotherTestValue = value["ANOTHER_TEST_VALUE"].getValue<std::string>();
		this->anotherValue02 = value["ANOTHER_VALUE_02"].getValue<int32_t>();
		this->anotherValue02w = value["ANOTHER_VALUE_02w"].getValue<int32_t>();
		this->id = value["id"].getValue<std::string>();
		this->type = value["type"].getValue<int8_t>();
		this->testDouble = value["test_double"].getValue<double>();
	};
	std::string createdAt{};
	double testDouble{};
	int8_t type{};
	std::string name{};
	int32_t anotherValue{};
	std::string anotherTestValue{};
	int32_t anotherValue02{};
	std::string anotherTestValue03{};
	std::string anotherTestValue03d{};
	int32_t anotherValue02w{};
	std::string id{};
};

struct TheDJson {
	TheDJson() noexcept = default;
	TheDJson(Jsonifier::JsonParser&& value) {
		auto theArray = value["d"]["activitiess"].getValue<std::vector<Jsonifier::JsonParser>>();
		for (auto& value: theArray) {
			
			activities.emplace_back(std::move(value));
		}
	}
	std::vector<ActivitiesJson> activities{};
};

namespace Jsonifier {
	template<> TheDJson JsonParser::getValue() {
		return TheDJson{ std::move(*this) };
	}

}

struct TheValueJson {
	TheValueJson(Jsonifier::JsonParser&& value) {
		this->theD = value["d"].getValue<TheDJson>();
	}
	TheDJson theD{};
};

struct Activities {
	Activities(simdjson::ondemand::value value) {
		this->TEST_VALUE_03 = DiscordCoreAPI::getString(value, "TEST_VALUE_03");
		this->TEST_VALUE_01 = DiscordCoreAPI::getFloat(value, "TEST_VALUE_01");
		this->TEST_VALUE_02 = DiscordCoreAPI::getBool(value, "TEST_VALUE_02");
		this->TEST_VALUE_04 = DiscordCoreAPI::getInt64(value, "TEST_VALUE_04");
	};
	double TEST_VALUE_01{};
	bool TEST_VALUE_02{};
	std::string TEST_VALUE_03{};
	int64_t TEST_VALUE_04{};
};

struct TheD {
	TheD() noexcept = default;
	TheD(simdjson::ondemand::value value) {
		simdjson::ondemand::value valueNew{};
		value["d"].get(valueNew);
		auto theArray = DiscordCoreAPI::getArray(valueNew, "activitiess");
		if (theArray.didItSucceed) {
			for (auto value: theArray.arrayValue) {
				activities.emplace_back(value.value());
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

int32_t main() noexcept {
	try {
		Jsonifier::Jsonifier serializer{};
		Jsonifier::Jsonifier arrayValue{};
		arrayValue["TEST_VALUE_00"] = 0.00333423;
		arrayValue["TEST_VALUE_02"] = false;
		arrayValue["TEST_VALUE_03"] = "TESTING_VALUE0101";
		arrayValue["TEST_VALUE_04"] = 4325454;
		for (size_t x = 0; x < 19; ++x) {
			serializer["d"]["activitiess"].emplaceBack(arrayValue);
		}
		
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::string stringNew{ serializer.operator std::string&&() };

		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		size_t oldSize = stringNew.size();


		std::cout << "THE STRING: " << stringNew << std::endl;
		std::string stringNewer = stringNew;
		stopWatch.resetTimer();
		stringNew.resize(stringNew.size() + 256 - stringNew.size() % 256);
		
		
			
		Jsonifier::SimdJsonValue theParser{};
		for (size_t x = 0ull; x < 2048ull * 64ull; ++x) {
			auto jsonData = theParser.getJsonData(stringNew);
			//jsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
			//std::cout << "THE DATA: " << jsonData.operator std::basic_string_view<char, std::char_traits<char>>() << std::endl;
			TheValueJson value{ std::move(jsonData) };
			//std::cout << "THE VALUE: " << value.theD.activities.back().name << std::endl;
			//std::cout << "ANOTHER_VALUE_02w: " << jsonData.operator std::string_view() << std::endl;
			totalSize += oldSize;
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;



		
		totalSize = 0;
		totalTime = 0;

		stopWatch.resetTimer();


		
			
			
			
			stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
		simdjson::ondemand::parser parser{};
		for (size_t x = 0ull; x < 2048ull * 64ull; ++x) {
			auto newDocument = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheValue value{ newDocument };
			//std::cout << "THE VALUE: " << value.theD.activities.back().name << std::endl;
			totalSize += oldSize;
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;


	} catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }


	return 0;
};
