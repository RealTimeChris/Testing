#ifndef _TESTING
	#define _TESTING
#endif

#include "Jsonifier.hpp"
#include <simdjson.h>
#include "DataParsingFunctionc.hpp"
#include <fstream>

struct ActivitiesJson {
	ActivitiesJson(Jsonifier::JsonParser&& value) {
		std::cout << "THE TYPE: " << ( int32_t )value.getType() << std::endl;
		this->TEST_VALUE_03 = value["TEST_VALUE_02"].getValue<std::string>();
		std::cout << "THE TYPE: " << this->TEST_VALUE_03 << std::endl;
		//this->TEST_VALUE_01 = value["TEST_VALUE_01"].getValue<double>();
		//this->TEST_VALUE_02 = value["TEST_VALUE_02"].getValue<bool>();
		//this->TEST_VALUE_04 = value["TEST_VALUE_04"].getValue<int64_t>();
		
	};
	double TEST_VALUE_01{};
	bool TEST_VALUE_02{};
	std::string TEST_VALUE_03{};
	int64_t TEST_VALUE_04{};
};

struct TheDJson {
	TheDJson() noexcept = default;
	TheDJson(Jsonifier::JsonParser&& value) {
		auto theObject = std::move(value["d"]);
		std::cout << "THE CURRENT SIZE: (REAL) " << theObject.size() << std::endl;
		auto theArray = std::move(theObject["activitiess"]);
		std::cout << "THE CURRENT SIZE: (REAL2) " << theArray.size() << std::endl;
		for (auto& value: theArray.getValue<std::vector<Jsonifier::JsonParser>>()) {
			
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
		this->theD = TheDJson{ std::move(value) };
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
		Jsonifier::Jsonifier arrayValueNew{};
		arrayValueNew["TEST_VALUE_00"] = 0.00333423;
		arrayValueNew["TEST_VALUE_01"] = true;
		arrayValueNew["TEST_VALUE_02"] = "TESTING_VALUE0101";
		arrayValueNew["TEST_VALUE_03"] = 4325454;
		arrayValueNew["TEST_VALUE_04"] = 0.00333423;
		arrayValueNew["TEST_VALUE_05"] = true;
		arrayValueNew["TEST_VALUE_06"] = "TESTING_VALUE0101";
		arrayValueNew["TEST_VALUE_07"] = 4325454;
		auto arrayValue = arrayValueNew;
		//arrayValueNew["TEST_VALUE_95"] = arrayValue;
		for (size_t x = 0; x < 1; ++x) {
			serializer["d"]["activitiess"].emplaceBack(arrayValue);
		}
		
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::string stringNew{ serializer.operator std::string() };

		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		size_t oldSize = stringNew.size();


		std::cout << "THE STRING: " << stringNew << std::endl;
		std::string stringNewer = stringNew;
		stopWatch.resetTimer();
		Jsonifier::SimdJsonValue theParser{};
		for (size_t x = 0ull; x < 1ull; ++x) {
			auto jsonData = theParser.getJsonData(stringNew);
			TheValueJson value{ std::move(jsonData) };
			totalSize += oldSize;
		} 
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;

		totalSize = 0;
		totalTime = 0;

		stopWatch.resetTimer();
		stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
		simdjson::ondemand::parser parser{};
		for (size_t x = 0ull; x < 2048ull * 1ull; ++x) {
			auto newDocument = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheValue value{ newDocument };
			totalSize += oldSize;
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;


	} catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }


	return 0;
};
