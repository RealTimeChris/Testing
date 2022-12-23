#ifndef _TESTING
	#define _TESTING
#endif

#include "Jsonifier.hpp"
#include <simdjson.h>
#include "DataParsingFunctionc.hpp"
#include <fstream>

struct ActivitiesJson {
	ActivitiesJson(Jsonifier::JsonParser&& value) {
		this->TEST_VALUE_03 = value["TEST_VALUE_03"].getValue<std::string>();
		this->TEST_VALUE_01 = value["TEST_VALUE_01"].getValue<double>();
		this->TEST_VALUE_02 = value["TEST_VALUE_02"].getValue<bool>();
		this->TEST_VALUE_04 = value["TEST_VALUE_04"].getValue<int64_t>();
		
	};
	double TEST_VALUE_01{};
	bool TEST_VALUE_02{};
	std::string TEST_VALUE_03{};
	int64_t TEST_VALUE_04{};
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
		this->TEST_VALUE_00 = DiscordCoreAPI::getFloat(value, "TEST_VALUE_00");
		this->TEST_VALUE_01 = DiscordCoreAPI::getBool(value, "TEST_VALUE_01");
		this->TEST_VALUE_02 = DiscordCoreAPI::getString(value, "TEST_VALUE_02");
		this->TEST_VALUE_03 = DiscordCoreAPI::getInt64(value, "TEST_VALUE_03");
		this->TEST_VALUE_00 = DiscordCoreAPI::getFloat(value, "TEST_VALUE_04");
		this->TEST_VALUE_01 = DiscordCoreAPI::getBool(value, "TEST_VALUE_05");
		this->TEST_VALUE_02 = DiscordCoreAPI::getString(value, "TEST_VALUE_06");
		this->TEST_VALUE_03 = DiscordCoreAPI::getInt64(value, "TEST_VALUE_07");
	};
	double TEST_VALUE_00{};
	bool TEST_VALUE_01{};
	std::string TEST_VALUE_02{};
	int64_t TEST_VALUE_03{};
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
		arrayValueNew["TEST_VALUE_01"] = false;
		arrayValueNew["TEST_VALUE_02"] = "TESTING_VALUE0101";
		arrayValueNew["TEST_VALUE_03"] = 4325454;
		arrayValueNew["TEST_VALUE_04"] = 0.00333423;
		arrayValueNew["TEST_VALUE_05"] = false;
		arrayValueNew["TEST_VALUE_06"] = "TESTING_VALUE0101";
		arrayValueNew["TEST_VALUE_07"] = 4325454;
		auto arrayValue = arrayValueNew;
		arrayValueNew["TEST_VALUE_95"] = arrayValue;
		arrayValueNew["TEST_VALUE_92"] = arrayValue;
		arrayValueNew["TEST_VALUE_93"] = arrayValue;
		for (size_t x = 0; x < 60; ++x) {
			serializer["d"]["activitiess"].emplaceBack(arrayValueNew);
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
		for (size_t x = 0ull; x < 2048ull * 64ull; ++x) {
			auto newDocument = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheValue value{ newDocument };
			totalSize += oldSize;
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;


	} catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }


	return 0;
};
