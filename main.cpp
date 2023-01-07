#ifndef _TESTING
	#define _TESTING
#endif

#include "DataParsingFunctions.hpp"
#include <jsonifier/Jsonifier.hpp>
#include <simdjson.h>
#include <fstream>

StopWatch stopWatchNew{ std::chrono::nanoseconds{ 1 } };
int64_t timeValueDouble{};
int64_t timeValueBool{};
int64_t timeValueString{};
int64_t timeValueInt64{};
int64_t iterationCountNew{};

struct ActivitiesJson {
	ActivitiesJson() noexcept = default;
	ActivitiesJson(Jsonifier::Value value) {
		iterationCountNew++;
		stopWatchNew.resetTimer();
		this->TEST_VALUE_00 = Jsonifier::getFloat(value, "TEST_VALUE_00");
		timeValueDouble += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_01 = Jsonifier::getBool(value, "TEST_VALUE_01");
		timeValueBool += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_02 = Jsonifier::getString(value, "TEST_VALUE_02");
		timeValueString += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_03 = Jsonifier::getInt64(value, "TEST_VALUE_03");
		timeValueInt64 += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_04 = Jsonifier::getFloat(value, "TEST_VALUE_04");
		timeValueDouble += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_05 = Jsonifier::getBool(value, "TEST_VALUE_05");
		timeValueBool += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_06 = Jsonifier::getString(value, "TEST_VALUE_06");
		timeValueString += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_07 = Jsonifier::getInt64(value, "TEST_VALUE_07");
		timeValueInt64 += stopWatchNew.totalTimePassed().count();
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
		Jsonifier::StopWatch stopWatchNew{ std::chrono::nanoseconds{ 1 } };
		int64_t totalTime{};
		int64_t totalIterations{};
		Jsonifier::Array valueNew{};
		auto newObject = value["TEST_VALUE_11"];
		newObject = newObject["d"]; 
		newObject = newObject["TEST_VALUES"];
		//std::cout << "TYPE: " << ( int32_t )newObject.type().valueUnsafe() << std::endl;
		
		stopWatchNew.resetTimer();
		timeValueDouble = 0;	
		timeValueBool = 0;
		timeValueString = 0;
		timeValueInt64 = 0;
		iterationCountNew = 0;
		for (auto valueIterator: newObject) {
			stopWatchNew.resetTimer();//
			//std::cout << "FIELD COUNT: " << +valueNewer.countFields().error() << std::endl;
			//std::cout << "THE TYPE: " << ( int32_t )valueNewer.type().value_unsafe() << std::endl;
			strings.emplace_back(std::move(valueIterator.valueUnsafe()));
			totalTime += stopWatchNew.totalTimePassed().count();
			totalIterations++;
		}
		//std::cout << "TOTAL TIME PASSED:(JSON) (double) " << timeValueDouble / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED:(JSON) (Bool) " << timeValueBool / iterationCountNew << std::endl;
		//		std::cout << "TOTAL TIME PASSED:(JSON) (String) " << timeValueString / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED:(JSON) (int64_t) " << timeValueInt64 / iterationCountNew << std::endl;

		//std::cout << "TOTAL TIME PASSED: (JSON) " << totalTime << ", THE COUNT: " << this->strings.size() << std::endl;
	}
	std::vector<ActivitiesJson> strings{};
};

struct TheValueJson {
	TheValueJson(Jsonifier::Document value) {
		//std::cout << "THE DATA: " << value.rawJson().value_unsafe() << std::endl;
		this->theD = TheDJson{ std::forward<Jsonifier::Document>(value) };
	}
	TheDJson theD{};
};

struct Activities {
	Activities() noexcept = default;
	Activities(simdjson::ondemand::value value) {
		iterationCountNew++;
		stopWatchNew.resetTimer();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_00 = Jsonifier::getFloat(value, "TEST_VALUE_00");
		timeValueDouble += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_01 = Jsonifier::getBool(value, "TEST_VALUE_01");
		timeValueBool += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_02 = Jsonifier::getString(value, "TEST_VALUE_02");
		timeValueString += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_03 = Jsonifier::getInt64(value, "TEST_VALUE_03");
		timeValueInt64 += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_04 = Jsonifier::getFloat(value, "TEST_VALUE_04");
		timeValueDouble += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_05 = Jsonifier::getBool(value, "TEST_VALUE_05");
		timeValueBool += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_06 = Jsonifier::getString(value, "TEST_VALUE_06");
		timeValueString += stopWatchNew.totalTimePassed().count();
		stopWatchNew.resetTimer();
		this->TEST_VALUE_07 = Jsonifier::getInt64(value, "TEST_VALUE_07");
		timeValueInt64 += stopWatchNew.totalTimePassed().count();
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
	TheD(simdjson::ondemand::document value) {
		Jsonifier::StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
		int64_t totalTime{};
		int64_t totalIterations{};
		simdjson::ondemand::array valueNew{};
		value["TEST_VALUE_11"]["d"]["TEST_VALUES"].get(valueNew);
		stopWatch.resetTimer();
		timeValueDouble = 0;
		timeValueBool = 0;
		timeValueString = 0;
		timeValueInt64 = 0;
		iterationCountNew = 0;
		for (auto valueIterator: valueNew) {
			stopWatch.resetTimer();//
			//std::cout << "FIELD COUNT: " << +valueNewer.countFields().error() << std::endl;
			//std::cout << "THE TYPE: " << ( int32_t )valueNewer.type().value_unsafe() << std::endl;
			strings.emplace_back(std::move(valueIterator.value_unsafe()));
			totalTime += stopWatch.totalTimePassed().count();
			totalIterations++;
		}
		//std::cout << "TOTAL TIME PASSED: (double) " << timeValueDouble / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED: (Bool) " << timeValueBool / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED: (String) " << timeValueString / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED: (int64_t) " << timeValueInt64 / iterationCountNew << std::endl;

		//std::cout << "TOTAL TIME PASSED: " << totalTime << ", THE COUNT: " << this->strings.size() << std::endl;
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
		for (size_t x = 0; x < 150; ++x) {
			serializer["TEST_VALUE_11"]["d"]["TEST_VALUES"].emplaceBack(arrayValueNew);
		}
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		//std::string stringNew{ FileLoader{ "C:/users/chris/source/repos/Jsonifier/benchmarking/twitter.json" } };
		std::string stringNew{ serializer.operator std::string&&() };
		size_t totalTime{};
		size_t totalSize{};
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t oldSize = stringNew.size();

		//std::cout << "THE STRING: " << stringNew << std::endl;
		std::cout << "THE STRING LENGTH: " << stringNew.size() << std::endl;
		std::string stringNewer = stringNew;
				
		
		iterationCountNew = 0;
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
		simdjson::ondemand::parser parser{};
		for (size_t x = 0ull; x < 5; ++x) {
			auto newDocument = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheValue value{ std::move(newDocument) };
			//std::cout << "VALUE00 (TESTING): " << value.theD.strings.begin().operator*().TEST_VALUE_00 << std::endl;
			//std::cout << "VALUE01: " << value.theD.strings.begin().operator*().TEST_VALUE_01 << std::endl;
			//std::cout << "VALUE02: " << value.theD.strings.begin().operator*().TEST_VALUE_02 << std::endl;
			//std::cout << "VALUE03: " << value.theD.strings.begin().operator*().TEST_VALUE_03 << std::endl;
			//std::cout << "VALUE04: " << value.theD.strings.begin().operator*().TEST_VALUE_04 << std::endl;
			//std::cout << "VALUE06: " << value.theD.strings.begin().operator*().TEST_VALUE_06 << std::endl;
			//std::cout << "VALUE07: " << value.theD.strings.begin().operator*().TEST_VALUE_07 << std::endl;
			iterationCountNew++;
			totalSize += oldSize;
		}
		totalTime = stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime/iterationCountNew << "ns TO PARSE THROUGH IT: " << totalSize/iterationCountNew << " BYTES!" << std::endl;
		totalSize = 0;
		totalTime = 0;
		iterationCountNew = 0;
		stopWatch.resetTimer();

		Jsonifier::Parser parserOld{};
		for (size_t x = 0ull; x < 5; ++x) {
			auto jsonData = parserOld.parseJson(stringNew);
			TheValueJson value{ std::move(jsonData) };
			//std::cout << "VALUE00: " << value.theD.strings.begin().operator*().TEST_VALUE_00 << std::endl;
			//std::cout << "VALUE01: " << value.theD.strings.begin().operator*().TEST_VALUE_01 << std::endl;
			//std::cout << "VALUE02: " << value.theD.strings.begin().operator*().TEST_VALUE_02 << std::endl;
			//std::cout << "VALUE03: " << value.theD.strings.begin().operator*().TEST_VALUE_03 << std::endl;
			//std::cout << "VALUE04: " << value.theD.strings.begin().operator*().TEST_VALUE_04 << std::endl;
			//			std::cout << "VALUE06: " << value.theD.strings.begin().operator*().TEST_VALUE_06 << std::endl;
			//std::cout << "VALUE07: " << value.theD.strings.begin().operator*().TEST_VALUE_07 << std::endl;
			iterationCountNew++;
			totalSize += oldSize;
		}
		totalTime = stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime / iterationCountNew << "ns TO PARSE THROUGH IT: " << totalSize / iterationCountNew << " BYTES!"
				  << std::endl;
		
		
	} catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	return 0;
};