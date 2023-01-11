#ifndef _TESTING
	#define _TESTING
#endif

#include "DataParsingFunctions.hpp"
#include <simdjson.h>
#include <fstream>
#include <random>

Jsonifier::StopWatch stopWatchNew{ std::chrono::nanoseconds{ 1 } };
int64_t timeValueDouble{};
int64_t timeValueBool{};
int64_t timeValueString{};
int64_t timeValueInt64{};
int64_t iterationCountNew{};
struct TestObject01Json {
	double TEST_VALUE_00{};
	bool TEST_VALUE_01{};
	std::string TEST_VALUE_02{};
	int64_t TEST_VALUE_03{};
	uint64_t TEST_VALUE_04{};
	double TEST_VALUE_05{};
	bool TEST_VALUE_06{};
	std::string TEST_VALUE_07{};
	int64_t TEST_VALUE_08{};
	uint64_t TEST_VALUE_09{};
};

struct TestObject02Json {
	TestObject01Json testObject{};
};

struct ActivitiesJson {
	ActivitiesJson() noexcept = default;
	ActivitiesJson(Jsonifier::Value value) {
		Jsonifier::Value newValue01{};
		value["TEST_OBJECT_01"].get(newValue01);
		newValue01["TEST_OBJECT_02"].get(newValue01);
		//iterationCountNew++;
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_00 = Jsonifier::getFloat(newValue01, "TEST_VALUE_00");
		//timeValueDouble += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_01 = Jsonifier::getBool(newValue01, "TEST_VALUE_01");
		//timeValueBool += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_02 = Jsonifier::getString(newValue01, "TEST_VALUE_02");
		//timeValueString += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_03 = Jsonifier::getInt64(newValue01, "TEST_VALUE_03");
		//timeValueInt64 += stopWatchNew.totalTimePassed().count();
		this->testObject.testObject.TEST_VALUE_04 = Jsonifier::getUint64(newValue01, "TEST_VALUE_04");
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_05 = Jsonifier::getFloat(newValue01, "TEST_VALUE_05");
		//timeValueDouble += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_06 = Jsonifier::getBool(newValue01, "TEST_VALUE_06");
		//timeValueBool += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_07 = Jsonifier::getString(newValue01, "TEST_VALUE_07");
		//timeValueString += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_08 = Jsonifier::getInt64(newValue01, "TEST_VALUE_08");
		//timeValueInt64 += stopWatchNew.totalTimePassed().count();
		this->testObject.testObject.TEST_VALUE_09 = Jsonifier::getUint64(newValue01, "TEST_VALUE_09");
	};
	TestObject02Json testObject{};
};

struct TheDJson {
	TheDJson() noexcept = default;
	TheDJson(Jsonifier::Document value) {
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatchNew{ std::chrono::nanoseconds{ 1 } };
		int64_t totalTime{};
		int64_t totalIterations{};
		Jsonifier::Array valueNew{};
		value["d"]["TEST_VALUES"].get(valueNew);
		timeValueDouble = 0;
		timeValueBool = 0;
		timeValueString = 0;
		timeValueInt64 = 0;
		iterationCountNew = 0;
		for (auto valueIterator: valueNew) {
			//std::cout << "FIELD COUNT: " << +valueNewer.countFields().error() << std::endl;
			//std::cout << "THE TYPE: " << ( int32_t )valueNewer.type().value_unsafe() << std::endl;
			strings.emplace_back(std::move(valueIterator.valueUnsafe()));
		}
		//std::cout << "TOTAL TIME PASSED:(JSON) (double) " << timeValueDouble / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED:(JSON) (Bool) " << timeValueBool / iterationCountNew << std::endl;
		//std::cout << "TOTAL TIME PASSED:(JSON) (String) " << timeValueString / iterationCountNew << std::endl;
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

struct TestObject01 {
	double TEST_VALUE_00{};
	bool TEST_VALUE_01{};
	std::string TEST_VALUE_02{};
	int64_t TEST_VALUE_03{};
	uint64_t TEST_VALUE_04{};
	double TEST_VALUE_05{};
	bool TEST_VALUE_06{};
	std::string TEST_VALUE_07{};
	int64_t TEST_VALUE_08{};
	uint64_t TEST_VALUE_09{};
};

struct TestObject02 {
	TestObject01 testObject{};
};

struct Activities {
	Activities() noexcept = default;
	Activities(simdjson::ondemand::value value) {
		simdjson::ondemand::value newValue01{};
		value["TEST_OBJECT_01"].get(newValue01);
		newValue01["TEST_OBJECT_02"].get(newValue01);
		//iterationCountNew++;
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_00 = Jsonifier::getFloat(newValue01, "TEST_VALUE_00");
		//timeValueDouble += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_01 = Jsonifier::getBool(newValue01, "TEST_VALUE_01");
		//timeValueBool += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_02 = Jsonifier::getString(newValue01, "TEST_VALUE_02");
		//timeValueString += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_03 = Jsonifier::getInt64(newValue01, "TEST_VALUE_03");
		//timeValueInt64 += stopWatchNew.totalTimePassed().count();
		this->testObject.testObject.TEST_VALUE_04 = Jsonifier::getUint64(newValue01, "TEST_VALUE_04");
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_05 = Jsonifier::getFloat(newValue01, "TEST_VALUE_05");
		//timeValueDouble += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_06 = Jsonifier::getBool(newValue01, "TEST_VALUE_06");
		//timeValueBool += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_07 = Jsonifier::getString(newValue01, "TEST_VALUE_07");
		//timeValueString += stopWatchNew.totalTimePassed().count();
		//stopWatchNew.resetTimer();
		this->testObject.testObject.TEST_VALUE_08 = Jsonifier::getInt64(newValue01, "TEST_VALUE_08");
		//timeValueInt64 += stopWatchNew.totalTimePassed().count();
		this->testObject.testObject.TEST_VALUE_09 = Jsonifier::getUint64(newValue01, "TEST_VALUE_09");
	};
	TestObject02 testObject{};
};

struct TheD {
	TheD() noexcept = default;
	TheD(simdjson::ondemand::document value) {
		Jsonifier::StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
		int64_t totalTime{};
		int64_t totalIterations{};
		simdjson::ondemand::array valueNew{};
		value["d"]["TEST_VALUES"].get(valueNew);
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


uint64_t randomUint64() {
	std::mt19937_64 randomGenerator{};
	randomGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	uint64_t returnValue{ randomGenerator() };
	return returnValue;
}

std::string randomStringInternal() {
	std::mt19937_64 randomGenerator{};
	randomGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	uint64_t stringLength{ static_cast<uint64_t>((static_cast<float>(randomGenerator()) / static_cast<float>(randomGenerator.max())) * 30) };


	std::string returnValue{};
	for (size_t x = 0; x < stringLength; ++x) {
		returnValue.push_back(static_cast<char>(((static_cast<float>(randomGenerator()) / static_cast<float>(randomGenerator.max())) * 89) + 38));
	}

	return returnValue;
}

std::string randomString() {
	bool testValue{ false };
	std::string returnValue{};
	while (!testValue) {
		returnValue = randomStringInternal();
		testValue = simdjson::validate_utf8(returnValue.data(), returnValue.size());
	}
	return returnValue;
}

bool randomBool() {
	std::mt19937_64 randomGenerator{};
	randomGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	bool returnValue{ static_cast<bool>(static_cast<int32_t>((static_cast<float>(randomGenerator()) / static_cast<float>(randomGenerator.max())) * 2.0f)) };
	return returnValue;
}

int64_t randomInt64() {
	std::mt19937_64 randomGenerator{};
	randomGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	int64_t returnValue{ static_cast<int64_t>(randomGenerator()) };
	return returnValue;
}

double randomDouble() {
	std::mt19937_64 randomGenerator{};
	randomGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	double returnValue{ static_cast<double>(randomGenerator()) };
	return returnValue;
}

int32_t main() {
	try {
		Jsonifier::Serializer serializer{};
		for (size_t x = 0; x < 30; ++x) {
			Jsonifier::Serializer testObject{};
			Jsonifier::Serializer arrayValueNew{};
			testObject["TEST_VALUE_00"] = randomDouble();
			testObject["TEST_VALUE_01"] = randomBool();
			testObject["TEST_VALUE_02"] = randomString();
			testObject["TEST_VALUE_03"] = randomInt64();
			testObject["TEST_VALUE_04"] = randomUint64();
			testObject["TEST_VALUE_05"] = randomDouble();
			testObject["TEST_VALUE_06"] = randomBool();
			testObject["TEST_VALUE_07"] = randomString();
			testObject["TEST_VALUE_08"] = randomInt64();
			testObject["TEST_VALUE_09"] = randomUint64();
			arrayValueNew["TEST_OBJECT_01"] = testObject;
			arrayValueNew["TEST_OBJECT_01"]["TEST_OBJECT_02"] = testObject;
			serializer["d"]["TEST_VALUES"].emplaceBack(arrayValueNew);
		}
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		//std::string stringNew{ FileLoader{ "C:/users/chris/source/repos/jsonifier/benchmarking/test_data.json" } };
		std::string stringNew{ serializer.operator std::string&&() };
		
		size_t totalTime{};
		size_t totalSize{};
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t oldSize = stringNew.size();
		std::cout << "THE STRING: " << stringNew << std::endl;
		std::cout << "THE STRING LENGTH: " << stringNew.size() << std::endl;
		std::string stringNewer = stringNew;
				
		int32_t iterationCount{};

		iterationCount = 0;
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
		simdjson::ondemand::parser parser{};
		for (size_t x = 0ull; x < 15; ++x) {
			auto jsonData = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheD theValue{ std::move(jsonData) };
			//std::cout << "VALUE01: " << theValue.strings[x].testObject.testObject.TEST_VALUE_01 << std::endl;
			//std::cout << "VALUE02: " << theValue.strings[x].testObject.testObject.TEST_VALUE_02 << std::endl;
			//std::cout << "VALUE03: " << theValue.strings[x].testObject.testObject.TEST_VALUE_03 << std::endl;
			//std::cout << "VALUE04: " << theValue.strings[x].testObject.testObject.TEST_VALUE_04 << std::endl;
			//std::cout << "VALUE06: " << theValue.strings[x].testObject.testObject.TEST_VALUE_06 << std::endl;
			//std::cout << "VALUE07: " << theValue.strings[x].testObject.testObject.TEST_VALUE_07 << std::endl;
			//std::cout << "VALUE08: " << theValue.strings[x].testObject.testObject.TEST_VALUE_08 << std::endl;
			//std::cout << "VALUE09: " << theValue.strings[x].testObject.testObject.TEST_VALUE_09 << std::endl;
			iterationCount++;
			totalSize += oldSize;
		}
		std::cout << "IT TOOK: " << stopWatch.totalTimePassed().count() / iterationCount << "ns TO PARSE THROUGH IT: " << totalSize / iterationCount
				  << " BYTES!" << std::endl;
		iterationCount = 0;
		totalSize = 0;
		totalTime = 0;
		
		stopWatch.resetTimer();
		Jsonifier::Parser parserOld{};
		for (size_t x = 0ull; x < 15 ; ++x) {
			auto jsonData = parserOld.parseJson(stringNew.data(), stringNew.size());
			TheDJson theValue{ std::move(jsonData) };
			//std::cout << "VALUE01: " << theValue.strings[x].testObject.testObject.TEST_VALUE_01 << std::endl;
			//std::cout << "VALUE02: " << theValue.strings[x].testObject.testObject.TEST_VALUE_02 << std::endl;
			//std::cout << "VALUE03: " << theValue.strings[x].testObject.testObject.TEST_VALUE_03 << std::endl;
			//std::cout << "VALUE04: " << theValue.strings[x].testObject.testObject.TEST_VALUE_04 << std::endl;
			//std::cout << "VALUE06: " << theValue.strings[x].testObject.testObject.TEST_VALUE_06 << std::endl;
			//std::cout << "VALUE07: " << theValue.strings[x].testObject.testObject.TEST_VALUE_07 << std::endl;
			//std::cout << "VALUE08: " << theValue.strings[x].testObject.testObject.TEST_VALUE_08 << std::endl;
			//std::cout << "VALUE09: " << theValue.strings[x].testObject.testObject.TEST_VALUE_09 << std::endl;
			iterationCount++;
			totalSize += oldSize;
		}
		totalTime = stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << stopWatch.totalTimePassed().count() / iterationCount << "ns TO PARSE THROUGH IT: " << totalSize / iterationCount
				  << " BYTES!" << std::endl;
		
	} catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	return 0;
};


