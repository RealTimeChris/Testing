#ifndef _TESTING
	#define _TESTING
#endif

#include "Jsonifier.hpp"
#include <simdjson.h>
#include "DataParsingFunctionc.hpp"
#include <fstream>

Jsonifier ::StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
int64_t iterationCount{};
int64_t totalTime{};

struct ActivitiesJson {
	ActivitiesJson() noexcept = default;
	ActivitiesJson(Jsonifier::Array&& value) {
		this->TEST_VALUE_00 = value["TEST_VALUE_00"].getValue<double>();
		//std::cout << "CURRENT TYPE: " << ( int32_t )value["TEST_VALUE_01"].getType() << std::endl;
		this->TEST_VALUE_01 = value["TEST_VALUE_01"].getValue<bool>();
		this->TEST_VALUE_02 = value["TEST_VALUE_02"].getValue<std::string>();
		this->TEST_VALUE_03 = value["TEST_VALUE_03"].getValue<uint64_t>();
		this->TEST_VALUE_04 = value["TEST_VALUE_04"].getValue<double>();
		//std::cout << "CURRENT TYPE: " << ( int32_t )value["TEST_VALUE_05"].getType() << std::endl;
		this->TEST_VALUE_05 = value["TEST_VALUE_05"].getValue<bool>();
		this->TEST_VALUE_06 = value["TEST_VALUE_06"].getValue<std::string>();
		this->TEST_VALUE_07 = value["TEST_VALUE_07"].getValue<uint64_t>();
	};
	double TEST_VALUE_00{};
	bool TEST_VALUE_01{};
	std::string TEST_VALUE_02{};
	int64_t TEST_VALUE_03{};
	double TEST_VALUE_04{};
	bool TEST_VALUE_05{};
	std::string TEST_VALUE_06{};
	int64_t TEST_VALUE_07{};
};

struct TheDJson {
	TheDJson() noexcept = default;
	TheDJson(Jsonifier::JsonIterator&& value) {
		auto document = value.getDocument();
		//auto theArray = std::move(theObject["activitiess"]);
		auto object = document.getObject("TEST_VALUE_11");
		auto array = object.getArray();
		auto sizeNew = array.size();
		std::cout << "CURRENT SIZE: " << sizeNew << std::endl;
		int32_t index{};
		//for (auto iter = array.begin(); iter != array.end();++iter) {
		//index++;
		//			auto newObject = iter->getValue<double>();
		//std::cout << "NEW SIZE: " << array.size() << std::endl;
		//			std::cout << "NEW INDEX: " << index << std::endl;
		//			std::cout << "NEW VALUE: " << newObject<< std::endl;
		//}
			
		iterationCount = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		//for (size_t x = 0; x < 12; ++x) {
			//iterationCount++;

			//activities.emplace_back(std::move(value));
			//totalTime += stopWatch.totalTimePassed().count();
			//activities.emplace_back(ActivitiesJson{});
			//}
		//std::cout << "THE TOTAL TIME: " << totalTime / iterationCount << std::endl;
	}
	std::vector<ActivitiesJson> activities{};
};


struct TheValueJson {
	TheValueJson(Jsonifier::JsonIterator&&  value) {
		this->theD = TheDJson{ std::move(value) };
	}
	TheDJson theD{};
};

struct Activities {
	Activities(simdjson::ondemand::value value) {
		this->TEST_VALUE_07 = DiscordCoreAPI::getInt64(value, "TEST_VALUE_07");
		this->TEST_VALUE_06 = DiscordCoreAPI::getString(value, "TEST_VALUE_06");
		this->TEST_VALUE_05 = DiscordCoreAPI::getBool(value, "TEST_VALUE_05");
		this->TEST_VALUE_04 = DiscordCoreAPI::getFloat(value, "TEST_VALUE_04");
		this->TEST_VALUE_03 = DiscordCoreAPI::getInt64(value, "TEST_VALUE_03");
		this->TEST_VALUE_00 = DiscordCoreAPI::getFloat(value, "TEST_VALUE_00");
		this->TEST_VALUE_02 = DiscordCoreAPI::getString(value, "TEST_VALUE_02");
		this->TEST_VALUE_01 = DiscordCoreAPI::getBool(value, "TEST_VALUE_01");
	};
	double TEST_VALUE_00{};
	bool TEST_VALUE_01{};
	std::string TEST_VALUE_02{};
	int64_t TEST_VALUE_03{};
	double TEST_VALUE_04{};
	bool TEST_VALUE_05{};
	std::string TEST_VALUE_06{};
	int64_t TEST_VALUE_07{};
};

struct TheD {
	TheD() noexcept = default;
	TheD(simdjson::ondemand::value value) {
		simdjson::ondemand::value valueNew{};
		value["d"].get(valueNew);
		auto theArray = DiscordCoreAPI::getArray(valueNew, "TEST_02");
		if (theArray.didItSucceed) {
			iterationCount = 0;
			totalTime = 0;
			iterationCount++;
			stopWatch.resetTimer();
			for (auto value : theArray.arrayValue) {
				activities.emplace_back(std::move(value));
				totalTime += stopWatch.totalTimePassed().count();
			}
			
		}
		//std::cout << "THE TOTAL TIME: " << totalTime / iterationCount << std::endl;
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
		arrayValueNew["TEST_VALUE_08"] = 0.00333423;
		arrayValueNew["TEST_VALUE_09"] = true;
		arrayValueNew["TEST_VALUE_10"] = "TESTING_VALUE0101";
		arrayValueNew["TEST_VALUE_11"] = 4325454;
		auto& arrayValue = arrayValueNew;
		//arrayValueNew["TEST_VALUE_95"] = arrayValue;
		for (size_t x = 0; x <2; ++x) {
			serializer["TEST_VALUE_11"].emplaceBack(double{ 4325454.00434 });
		}
		
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::string stringNew{ serializer.operator std::string() };

		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		size_t oldSize = stringNew.size();

		std::cout << "THE STRING: " << stringNew << std::endl;
		std::cout << "THE STRING LENGTH: " << stringNew.size() << std::endl;
		std::string stringNewer = stringNew;
		
		
		stopWatch.resetTimer();
		Jsonifier::SimdJsonValue theParser{};
		for (size_t x = 0ull; x < 2048ull * 1ull; ++x) {
			auto jsonData = theParser.getJsonData(stringNew);
			TheValueJson value{ std::move(jsonData) };
			//std::cout << "VALUE00: " << value.theD.activities.begin().operator*().TEST_VALUE_00 << std::endl;
			//std::cout << "VALUE01: " << value.theD.activities.begin().operator*().TEST_VALUE_01 << std::endl;
			//std::cout << "VALUE02: " << value.theD.activities.begin().operator*().TEST_VALUE_02 << std::endl;
			//std::cout << "VALUE03: " << value.theD.activities.begin().operator*().TEST_VALUE_03 << std::endl;
			//std::cout << "VALUE04: " << value.theD.activities.begin().operator*().TEST_VALUE_04 << std::endl;
			//std::cout << "VALUE06: " << value.theD.activities.begin().operator*().TEST_VALUE_06 << std::endl;
			//std::cout << "VALUE07: " << value.theD.activities.begin().operator*().TEST_VALUE_07 << std::endl;
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
			//std::cout << "VALUE00: " << value.theD.activities.begin().operator*().TEST_VALUE_00 << std::endl;
			//std::cout << "VALUE01: " << value.theD.activities.begin().operator*().TEST_VALUE_01 << std::endl;
			//std::cout << "VALUE02: " << value.theD.activities.begin().operator*().TEST_VALUE_02 << std::endl;
			//std::cout << "VALUE03: " << value.theD.activities.begin().operator*().TEST_VALUE_03 << std::endl;
			//std::cout << "VALUE04: " << value.theD.activities.begin().operator*().TEST_VALUE_04 << std::endl;
			//std::cout << "VALUE06: " << value.theD.activities.begin().operator*().TEST_VALUE_06 << std::endl;
			//std::cout << "VALUE07: " << value.theD.activities.begin().operator*().TEST_VALUE_07 << std::endl;
			totalSize += oldSize;
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;


	} catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }


	return 0;
};
