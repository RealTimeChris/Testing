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
	TheDJson(Jsonifier::Document value) {
		Jsonifier::Array valueNew{};
		value["TEST_VALUE_11"]["d"].get(valueNew);
		for (auto value: valueNew) {
			strings.push_back(static_cast<std::string>(value.get_string().value_unsafe()));
		}
	}
	std::vector<std::string> strings{};
};
 
struct TheValueJson {
	TheValueJson(Jsonifier::Document value) {
		//std::cout << "THE DATA: " << value.raw_json().value_unsafe() << std::endl;
		this->theD = TheDJson{ std::move(value) };
	}
	TheDJson theD{};
};

struct TheD {
	TheD() noexcept = default;
	TheD(simdjson::ondemand::document value) {
		simdjson::ondemand::array valueNew{};
		value["TEST_VALUE_11"]["d"].get(valueNew);
		for (auto value: valueNew) {
			strings.push_back(static_cast<std::string>(value.get_string().value_unsafe()));
		}
	}
	std::vector<std::string> strings{};
};

struct TheValue {
	TheValue(simdjson::ondemand::document value) {
		this->theD = TheD{ std::move(value) };
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

class FileLoader {
  public:
	FileLoader(const char* filePath) {
		std::ofstream theStream{ filePath, std::ios::out | std::ios::in };
		std::stringstream inputStream{};
		inputStream << theStream.rdbuf();
		this->fileContents = inputStream.str();
	}

	std::string& getFileContents() {
		return this->fileContents;
	}

  protected:
	std::string fileContents{};
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
			serializer["TEST_VALUE_11"]["d"].emplaceBack(std::string{ "TESTING VALUE11111111111" });
		}
		std::cout << "CURRENT SIZE: " << serializer.size() << std::endl;
		FileLoader jsonFile{ "C:/users/chris/downloads/refsnp-unsupported35000.json" };
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::string stringNew{ jsonFile.getFileContents() };
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

		//std::cout << "THE STRING: " << stringNew << std::endl;
		std::cout << "THE STRING LENGTH: " << stringNew.size() << std::endl;
		std::string stringNewer = stringNew;
		
		totalSize = 0;
		totalTime = 0;		
					

		stopWatch.resetTimer();
		stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
		simdjson::ondemand::parser parser{};
		for (size_t x = 0ull; x < 1ull * 35; ++x) {
			auto newDocument = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheValue value{ std::move(newDocument) };
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
		
		
			
			
			Jsonifier::JsonifierCore parserOld{};
		for (size_t x = 0ull; x < 1* 35; ++x) {
			auto jsonData = parserOld.parseJson(stringNew);
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
		


	} catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }


	return 0;
};