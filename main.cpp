#ifndef _TESTING
	#define _TESTING
#endif

#include "DataParsingFunctions.hpp"
#include "Jsonifier.hpp"
#include <simdjson.h>
#include <fstream>	
Jsonifier::StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
int64_t iterationCount{};
int64_t totalTimeNew{};

struct ActivitiesJson {
	ActivitiesJson() noexcept = default;
	ActivitiesJson(Jsonifier::Value value) {
		this->TEST_VALUE_00 = Jsonifier::getFloat(value, "TEST_VALUE_00");
		this->TEST_VALUE_01 = Jsonifier::getBool(value, "TEST_VALUE_01");
		this->TEST_VALUE_02 = Jsonifier::getString(value, "TEST_VALUE_02");
		this->TEST_VALUE_03 = Jsonifier::getInt64(value, "TEST_VALUE_03");
		this->TEST_VALUE_04 = Jsonifier::getFloat(value, "TEST_VALUE_04");
		this->TEST_VALUE_05 = Jsonifier::getBool(value, "TEST_VALUE_05");
		this->TEST_VALUE_06 = Jsonifier::getString(value, "TEST_VALUE_06");
		this->TEST_VALUE_07 = Jsonifier::getInt64(value, "TEST_VALUE_07");
		//std::cout << "CURRENT VALUE 00: " << this->TEST_VALUE_00 << std::endl;
		//std::cout << "CURRENT VALUE 01: " << this->TEST_VALUE_01 << std::endl;
		//std::cout << "CURRENT VALUE 02: " << this->TEST_VALUE_02 << std::endl;
		//std::cout << "CURRENT VALUE 03: " << this->TEST_VALUE_03 << std::endl;
		//std::cout << "CURRENT VALUE 04: " << this->TEST_VALUE_04 << std::endl;
		//std::cout << "CURRENT VALUE 05: " << this->TEST_VALUE_05 << std::endl;
		//std::cout << "CURRENT VALUE 06: " << this->TEST_VALUE_06 << std::endl;
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

struct Activities {
	Activities() noexcept = default;
	Activities(simdjson::ondemand::value value) {
		this->TEST_VALUE_00 = Jsonifier::getFloat(value, "TEST_VALUE_00");
		this->TEST_VALUE_01 = Jsonifier::getBool(value, "TEST_VALUE_01");
		this->TEST_VALUE_02 = Jsonifier::getString(value, "TEST_VALUE_02");
		this->TEST_VALUE_03 = Jsonifier::getInt64(value, "TEST_VALUE_03");
		this->TEST_VALUE_04 = Jsonifier::getFloat(value, "TEST_VALUE_04");
		this->TEST_VALUE_05 = Jsonifier::getBool(value, "TEST_VALUE_05");
		this->TEST_VALUE_06 = Jsonifier::getString(value, "TEST_VALUE_06");
		this->TEST_VALUE_07 = Jsonifier::getInt64(value, "TEST_VALUE_07");
		//std::cout << "CURRENT VALUE 00: " << this->TEST_VALUE_00 << std::endl;
		//std::cout << "CURRENT VALUE 01: " << this->TEST_VALUE_01 << std::endl;
		//std::cout << "CURRENT VALUE 02: " << this->TEST_VALUE_02 << std::endl;
		//std::cout << "CURRENT VALUE 03: " << this->TEST_VALUE_03 << std::endl;
		//std::cout << "CURRENT VALUE 04: " << this->TEST_VALUE_04 << std::endl;
		//std::cout << "CURRENT VALUE 05: " << this->TEST_VALUE_05 << std::endl;
		//std::cout << "CURRENT VALUE 06: " << this->TEST_VALUE_06 << std::endl;
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
		//std::cout << "ELEMENT COUNT: " << valueNew.countElements().value_unsafe() << std::endl;
		
		for (auto valueIterator : valueNew) {
			stopWatch.resetTimer();
			//std::cout << "FIELD COUNT: " << +valueNewer.countFields().error() << std::endl;
			//std::cout << "THE TYPE: " << ( int32_t )valueNewer.type().value_unsafe() << std::endl;
			strings.emplace_back(valueIterator.valueUnsafe());
			totalTimeNew += stopWatch.totalTimePassed().count();
			iterationCount++;
			
			
		}
		std::cout << "TOTAL TIME PASSED: (JSON) " << totalTimeNew / iterationCount << std::endl;
	}
	std::vector<ActivitiesJson> strings{};
};

struct TheValueJson {
	TheValueJson(Jsonifier::Document value) {
		//std::cout << "THE DATA: " << value.rawJson().value_unsafe() << std::endl;
		this->theD = TheDJson{ std::move(value) };
	}
	TheDJson theD{};
};

struct TheD {
	TheD() noexcept = default;
	TheD(simdjson::ondemand::document value) {
		simdjson::ondemand::array valueNew{};
		value["TEST_VALUE_11"]["d"].get(valueNew);

		for (auto valueNewer: valueNew) {
			stopWatch.resetTimer();
			//std::cout << "FIELD COUNT: " << +valueNewer.countFields().error() << std::endl;
			//std::cout << "THE TYPE: " << ( int32_t )valueNewer.type().value_unsafe() << std::endl;
			strings.emplace_back(valueNewer.value_unsafe());
			totalTimeNew += stopWatch.totalTimePassed().count();
			iterationCount++;
		}
		std::cout << "TOTAL TIME PASSED: " << totalTimeNew / iterationCount << std::endl;
	}
	std::vector<Activities> strings{};
};

struct TheValue {
	TheValue(simdjson::ondemand::document value) {
		this->theD = TheD{ std::move(value) };
	}
	TheD theD{};
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
	operator std::string() {
		return this->getFileContents();
	}
  protected:
	std::string fileContents{};
};

int32_t main() {
	try {
		Jsonifier::Serializer serializer{};
		Jsonifier::Serializer arrayValueNew{};
		arrayValueNew["TEST_VALUE_00"] = 0.00333423;
		arrayValueNew["TEST_VALUE_01"] = true;
		arrayValueNew["TEST_VALUE_02"] = "TESTING_VALUE112323";
		arrayValueNew["TEST_VALUE_03"] = 4325454;
		arrayValueNew["TEST_VALUE_04"] = 0.00333423;
		arrayValueNew["TEST_VALUE_05"] = true;
		arrayValueNew["TEST_VALUE_06"] = "TESTING_VALUE";
		arrayValueNew["TEST_VALUE_07"] = 4325454;
		auto& arrayValue = arrayValueNew;
		for (size_t x = 0; x < 15; ++x) {
			serializer["TEST_VALUE_11"]["d"].emplaceBack(arrayValueNew);
		}
		std::cout << "CURRENT SIZE: " << serializer.size() << std::endl;
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		//std::string stringNew{ FileLoader{ "C:/users/chris/downloads/refsnp-unsupported35000.json" } };
		std::string stringNew{ serializer.operator std::string&&() };
		size_t totalTime{};
		size_t totalSize{};
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		std::unique_ptr<uint64_t[]> objectBuffer{};
		//objectBuffer.reset(new (std::nothrow) uint64_t[1024 * 1024]);	}totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		{
			Jsonifier::ObjectBuffer<uint64_t> objectBuffer{};
			//objectBuffer.reset(1024 * 1024);
		};
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		size_t oldSize = stringNew.size();

		//std::cout << "THE STRING: " << stringNew << std::endl;/
		std::cout << "THE STRING LENGTH: " << stringNew.size() << std::endl;
		std::string stringNewer = stringNew;

		totalSize = 0;
		totalTime = 0;

		
		iterationCount = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
		simdjson::ondemand::parser parser{};
		for (size_t x = 0ull; x < 256; ++x) {
			auto newDocument = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheValue value{ std::move(newDocument) };
			//std::cout << "VALUE00 (TESTING): " << value.theD.strings.begin().operator*().TEST_VALUE_00 << std::endl;
			//std::cout << "VALUE01: " << value.theD.strings.begin().operator*().TEST_VALUE_01 << std::endl;
			//std::cout << "VALUE02: " << value.theD.strings.begin().operator*().TEST_VALUE_02 << std::endl;
			//std::cout << "VALUE03: " << value.theD.strings.begin().operator*().TEST_VALUE_03 << std::endl;
			//std::cout << "VALUE04: " << value.theD.strings.begin().operator*().TEST_VALUE_04 << std::endl;
			//std::cout << "VALUE06: " << value.theD.strings.begin().operator*().TEST_VALUE_06 << std::endl;
			//std::cout << "VALUE07: " << value.theD.strings.begin().operator*().TEST_VALUE_07 << std::endl;
			totalSize += oldSize;
		}
		totalTime = stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		totalSize = 0;
		totalTime = 0;
		iterationCount = 0;
		totalTimeNew = 0;
		totalTime = 0; 
		stopWatch.resetTimer();

		Jsonifier::Parser parserOld{};		
		for (size_t x = 0ull; x < 256; ++x) {
			auto jsonData = parserOld.parseJson(stringNew);
			TheValueJson value{ std::move(jsonData) };
			//std::cout << "VALUE00: " << value.theD.strings.begin().operator*().TEST_VALUE_00 << std::endl;
			//std::cout << "VALUE01: " << value.theD.strings.begin().operator*().TEST_VALUE_01 << std::endl;
			//std::cout << "VALUE02: " << value.theD.strings.begin().operator*().TEST_VALUE_02 << std::endl;
			//std::cout << "VALUE03: " << value.theD.strings.begin().operator*().TEST_VALUE_03 << std::endl;
			//std::cout << "VALUE04: " << value.theD.strings.begin().operator*().TEST_VALUE_04 << std::endl;
			//std::cout << "VALUE06: " << value.theD.strings.begin().operator*().TEST_VALUE_06 << std::endl;
			//std::cout << "VALUE07: " << value.theD.strings.begin().operator*().TEST_VALUE_07 << std::endl;
			totalSize += oldSize;
		}
		totalTime = stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
	} catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	return 0;
};