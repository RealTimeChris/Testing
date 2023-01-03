#ifndef _TESTING
	#define _TESTING
#endif

#include "DataParsingFunctions.hpp"
#include "Jsonifier.hpp"
#include <simdjson.h>
#include <fstream>

Jsonifier ::StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
int64_t iterationCount{};
int64_t totalTime{};

struct ActivitiesJson {
	ActivitiesJson() noexcept = default;
	ActivitiesJson(Jsonifier::Object&& value) {
		//this->TEST_VALUE_00 = value["TEST_VALUE_00"].get<double>().getValue();
		//std::cout << "CURRENT TYPE: " << ( int32_t )value["TEST_VALUE_01"].getType() << std::endl;
		//this->TEST_VALUE_01 = value["TEST_VALUE_01"].get<bool>().getValue();
		//this->TEST_VALUE_02 = value["TEST_VALUE_02"].get<std::string>().getValue();
		//this->TEST_VALUE_03 = value["TEST_VALUE_03"].get<uint64_t>().getValue();
		//this->TEST_VALUE_04 = value["TEST_VALUE_04"].get<double>().getValue();
		//std::cout << "CURRENT TYPE: " << ( int32_t )va["TEST_VALUE_05"].getType() << std::endl;
		//this->TEST_VALUE_05 = value["TEST_VALUE_05"].get<bool>().getValue();
		//this->TEST_VALUE_06 = value["TEST_VALUE_06"].get<std::string>().getValue();
		//this->TEST_VALUE_07 = value["TEST_VALUE_07"].get<uint64_t>().getValue();
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
	TheDJson(Jsonifier::Document&& value) {
		//std::cout << "CURRENT KEY: " << value.countFields() << std::endl;
		//Jsonifier::Object objectNew{ value.getObject() };
		//objectNew.findField("TEST_VALUE_11");
		//std::cout << "CURRENT FIELD COUNT: " << objectNew.countFields() << std::endl;
		//auto newArray = objectNew.getArray();
		//std::cout << "CURRENT SIZE: " << newArray.countElements() << std::endl;
		//		std::cout << "CURRENT TYPE: " << ( int32_t )objectNew.type() << std::endl;
		//for (auto iterator = newArray.begin(); iterator != newArray.end(); ++iterator) {
			//std::cout << "CURRENT SIZE: " << newArray.countElements() << std::endl;
		//auto newValue = *iterator;
		//			std::string_view theValueNew{};
		////			(int32_t) newValue.get<std::string_view>(theValueNew);
		//			std::cout << "CURRENT TYPE: " << theValueNew << std::endl;
		//}
		//std::cout << "CURRENT COUNT OF THE ARRAY: " << newArray.countElements() << std::endl;
		//std::cout << "CURRENT SIZE: " << ( int32_t )value.getObject().findFieldUnordered("TEST_VALUE_11").countFields() << std::endl;
		//auto newKey = value["TEST_VALUE_11"].fieldKey();
		
		//std::cout << "CURRENT KEY: " << newKey << std::endl;
		//int32_t index{};

		//for (auto iterator = arrayNewer.begin(); iterator != arrayNewer.end(); ++iterator) {
		//index++;

		//	std::string_view newValueDouble = iterator->get<std::string_view>().getValue();
			//std::cout << "NEW INDEX: " << newValueDouble << std::endl;
			//}
		std::cout << "NEW INDEX: WERE DONE" << std::endl;
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
	TheValueJson(Jsonifier::Document&& value) {
		this->theD = TheDJson{ std::move(value) };
	}
	TheDJson theD{};
};

struct Activities {
	Activities(simdjson::ondemand::value value) {
		this->TEST_VALUE_07 = Jsonifier::getInt64(value, "TEST_VALUE_07");
		this->TEST_VALUE_06 = Jsonifier::getString(value, "TEST_VALUE_06");
		this->TEST_VALUE_05 = Jsonifier::getBool(value, "TEST_VALUE_05");
		this->TEST_VALUE_04 = Jsonifier::getFloat(value, "TEST_VALUE_04");
		this->TEST_VALUE_03 = Jsonifier::getInt64(value, "TEST_VALUE_03");
		this->TEST_VALUE_00 = Jsonifier::getFloat(value, "TEST_VALUE_00");
		this->TEST_VALUE_02 = Jsonifier::getString(value, "TEST_VALUE_02");
		this->TEST_VALUE_01 = Jsonifier::getBool(value, "TEST_VALUE_01");
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
		simdjson::ondemand::array valueNew{};
		value["TEST_VALUE_11"].get(valueNew);
		for (auto value: valueNew) {
			auto newDouble = value.get_string().take_value();
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
class TestClass {
  public:
	inline static int32_t counter{};
	TestClass() {
		counter++;
		//std::cout << "I'M BEING CONSTRUCTED!:  NUMBER: " << counter << std::endl;
	}
	~TestClass() {
		counter--;
		//std::cout << "I'M BEING DESTROYED!:  NUMBER: " << counter << std::endl;
	}
};

class TestClass02 {
  public:
	inline static int32_t counter{};
	TestClass02() {
		counter++;
		//std::cout << "I'M BEING CONSTRUCTED! (SECOND):  NUMBER: "<< counter << std::endl;
	}
	~TestClass02() {
		counter--;
		//std::cout << "I'M BEING DESTROYED! (SECOND):  NUMBER: " << counter << std::endl;
	}
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
		for (size_t x = 0; x < 273; ++x) {
			serializer["TEST_VALUE_11"].emplaceBack(std::string{ "TESTING VALUE11111111111" });
		}
		std::cout << "CURRENT SIZE: " << serializer.size() << std::endl;
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::string stringNew{ serializer.operator std::string&&() };
		size_t totalTime{};
		size_t totalSize{};
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		{
			std::unique_ptr<uint64_t[]> objectBuffer{};
			objectBuffer.reset(new (std::nothrow) uint64_t[1024 * 1024]);
		}

		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer(); 
		{
			Jsonifier::ObjectBuffer<uint64_t> objectBuffer{};
			objectBuffer.reset(1024 * 1024);
		};
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		size_t oldSize = stringNew.size();

		std::cout << "THE STRING: " << stringNew << std::endl;
		std::cout << "THE STRING LENGTH: " << stringNew.size() << std::endl;
		std::string stringNewer = stringNew;
		
		totalSize = 0;
		totalTime = 0;		
					

		stopWatch.resetTimer();
		for (size_t x = 0ull; x < 2048ull * 1; ++x) {
			stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
			simdjson::ondemand::parser parser{};
			auto newDocument = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			//TheValue value{ newDocument };
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
		
		
			
		for (size_t x = 0ull; x < 2048ull * 1; ++x) {
			Jsonifier::JsonifierCore parserOld{};	
			auto jsonData = parserOld.parseJson(stringNew);
			//TheValueJson value{ std::move(jsonData) };
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