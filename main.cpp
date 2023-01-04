#ifndef _TESTING
	#define _TESTING
#endif

#include "DataParsingFunctions.hpp"
#include "Jsonifier.hpp"
#include <simdjson.h>
#include <fstream>	Jsonifier::StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
int64_t iterationCount{};
int64_t totalTime{};

struct ActivitiesJson {
	ActivitiesJson() noexcept = default;
	ActivitiesJson(Jsonifier::Object&& value) {
		this->TEST_VALUE_00 =	Jsonifier::getFloat(value, "TEST_VALUE_00");
		this->TEST_VALUE_01 =	Jsonifier::getBool(value, "TEST_VALUE_01");
		this->TEST_VALUE_02 =	Jsonifier::getString(value, "TEST_VALUE_02");
		this->TEST_VALUE_03 =	Jsonifier::getInt64(value, "TEST_VALUE_03");
		this->TEST_VALUE_04 =	Jsonifier::getFloat(value, "TEST_VALUE_04");
		this->TEST_VALUE_05 =	Jsonifier::getBool(value, "TEST_VALUE_05");
		this->TEST_VALUE_06 =	Jsonifier::getString(value, "TEST_VALUE_06");
		this->TEST_VALUE_07 =	Jsonifier::getInt64(value, "TEST_VALUE_07");
		std::cout << "CURRENT VALUE: " << this->TEST_VALUE_06 << std::endl;
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
	TheDJson(Jsonifier::Document value) {	Jsonifier::Array valueNew{};
		value["TEST_VALUE_11"]["d"].get(valueNew);
		//std::cout << "ELEMENT COUNT: " << valueNew.count_elements().value_unsafe() << std::endl;
		for (auto valueNewer: valueNew) {
			//std::cout << "FIELD COUNT: " << +valueNewer.count_fields().error() << std::endl;
			//std::cout << "THE TYPE: " << ( int32_t )valueNewer.type().value_unsafe() << std::endl;
			strings.emplace_back(valueNewer.get_object().value_unsafe());
		}
	}
	std::vector<ActivitiesJson> strings{};
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
		//std::cout << "I'M BEING CONSTRUCTED!:  NUMBER: " << counter << std::endl;	}~TestClass() {
		counter--;
		//std::cout << "I'M BEING DESTROYED!:  NUMBER: " << counter << std::endl;	}};}
	}
};
class TestClass02 {
  public:
	inline static int32_t counter{};
	TestClass02() {
		counter++;
		//std::cout << "I'M BEING CONSTRUCTED! (SECOND):  NUMBER: "<< counter << std::endl;	}~TestClass02() {
		counter--;
		//std::cout << "I'M BEING DESTROYED! (SECOND):  NUMBER: " << counter << std::endl;	}};

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
	}
};

int32_t main()  {
	try {	Jsonifier::Jsonifier serializer{};	Jsonifier::Jsonifier arrayValueNew{};
		arrayValueNew["TEST_VALUE_00"] = 0.00333423;
		arrayValueNew["TEST_VALUE_01"] = true;
		arrayValueNew["TEST_VALUE_02"] = "TESTING_VALUE2323";
		arrayValueNew["TEST_VALUE_03"] = 4325454;
		arrayValueNew["TEST_VALUE_04"] = 0.00333423;
		arrayValueNew["TEST_VALUE_05"] = true;
		arrayValueNew["TEST_VALUE_06"] = "TESTING_VALUE";
		arrayValueNew["TEST_VALUE_07"] = 4325454;
		auto& arrayValue = arrayValueNew;
		for (size_t x = 0; x < 27; ++x) {
			serializer["TEST_VALUE_11"]["d"].emplaceBack(arrayValueNew);
		}
		std::cout << "CURRENT SIZE: " << serializer.size() << std::endl;
		//FileLoader jsonFile{ "C:/users/chris/downloads/refsnp-unsupported35000.json" };
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::string stringNew{ serializer.operator std::string&&() };
		size_t totalTime{};
		size_t totalSize{};	Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		std::unique_ptr<uint64_t[]> objectBuffer{};
		//objectBuffer.reset(new (std::nothrow) uint64_t[1024 * 1024]);	}totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		{	Jsonifier::ObjectBuffer<uint64_t> objectBuffer{};
			//objectBuffer.reset(1024 * 1024);
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

		for (size_t x = 0ull; x < 100; ++x) {
			stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
			simdjson::ondemand::parser parser{};
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




		for (size_t x = 0ull; x < 100; ++x) {	Jsonifier::JsonifierCore parserOld{};
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
	}
	catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
};