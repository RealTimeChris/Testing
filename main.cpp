#ifndef _TESTING
#define _TESTING
#endif

#include "Jsonifier.hpp"
#include <simdjson.h>
#include <fstream>

struct ObjectReturnData {
	simdjson::ondemand::value object{};
	bool didItSucceed{ false };
};

struct ArrayReturnData {
	simdjson::ondemand::array arrayValue{};
	bool didItSucceed{ false };
};

int64_t getInt64(simdjson::ondemand::value jsonData, const char* key);

int32_t getInt32(simdjson::ondemand::value jsonData, const char* key);

int16_t getInt16(simdjson::ondemand::value jsonData, const char* key);

int8_t getInt8(simdjson::ondemand::value jsonData, const char* key);

uint64_t getUint64(simdjson::ondemand::value jsonData, const char* key);

uint32_t getUint32(simdjson::ondemand::value jsonData, const char* key);

uint16_t getUint16(simdjson::ondemand::value jsonData, const char* key);

uint8_t getUint8(simdjson::ondemand::value jsonData, const char* key);

float getFloat(simdjson::ondemand::value jsonData, const char* key);

bool getBool(simdjson::ondemand::value jsonData, const char* key);

std::string getString(simdjson::ondemand::value jsonData, const char* key);

std::string getString(ObjectReturnData jsonData, const char* key);

bool getObject(simdjson::ondemand::value& object, const char* key, simdjson::ondemand::value jsonObjectData);

bool getArray(simdjson::ondemand::array& array, const char* key, simdjson::ondemand::value jsonObjectData);

bool getArray(simdjson::ondemand::array& array, simdjson::ondemand::value jsonObjectData);

std::string getString(simdjson::ondemand::value jsonData);

ObjectReturnData getObject(simdjson::ondemand::value jsonObjectData, const char* objectName);

ObjectReturnData getObject(ObjectReturnData jsonObjectData, const char* objectName);

ObjectReturnData getObject(ArrayReturnData jsonObjectData, uint64_t objectIndex);

ArrayReturnData getArray(simdjson::ondemand::value jsonObjectData, const char* arrayName);

ArrayReturnData getArray(ObjectReturnData jsonObjectData, const char* arrayName);

	int64_t getInt64(simdjson::ondemand::value jsonData, const char* key) {
	int64_t value{};
	if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
		return int64_t{ value };
	} else {
		return 0;
	}
}

int32_t getInt32(simdjson::ondemand::value jsonData, const char* key) {
	int64_t value{};
	if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
		return static_cast<int32_t>(value);
	} else {
		return 0;
	}
}

int16_t getInt16(simdjson::ondemand::value jsonData, const char* key) {
	int64_t value{};
	if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
		return static_cast<int16_t>(value);
	} else {
		return 0;
	}
}

int8_t getInt8(simdjson::ondemand::value jsonData, const char* key) {
	int64_t value{};
	if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
		return static_cast<int8_t>(value);
	} else {
		return 0;
	}
}

uint64_t getUint64(simdjson::ondemand::value jsonData, const char* key) {
	uint64_t value{};
	if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
		return uint64_t{ value };
	} else {
		return 0;
	}
}

uint32_t getUint32(simdjson::ondemand::value jsonData, const char* key) {
	uint64_t value{};
	if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
		return static_cast<uint32_t>(value);
	} else {
		return 0;
	}
}

uint16_t getUint16(simdjson::ondemand::value jsonData, const char* key) {
	uint64_t value{};
	if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
		return static_cast<uint16_t>(value);
	} else {
		return 0;
	}
}

uint8_t getUint8(simdjson::ondemand::value jsonData, const char* key) {
	uint64_t value{};
	if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
		return static_cast<uint8_t>(value);
	} else {
		return 0;
	}
}

float getFloat(simdjson::ondemand::value jsonData, const char* key) {
	double value{};
	if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
		return double{ value };
	} else {
		return 0.0f;
	}
}

bool getBool(simdjson::ondemand::value jsonData, const char* key) {
	bool value{};
	if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
		return bool{ value };
	} else {
		return false;
	}
}

std::string getString(simdjson::ondemand::value jsonData, const char* key) {
	std::string_view value{};
	if (jsonData[key].get(value) == simdjson::error_code::SUCCESS) {
		return std::string{ value.data(), value.size() };
	} else {
		return "";
	}
}

bool getObject(simdjson::ondemand::value& object, const char* key, simdjson::ondemand::value jsonObjectData) {
	if (jsonObjectData[key].get(object) == simdjson::error_code::SUCCESS) {
		return true;
	} else {
		return false;
	}
}

bool getArray(simdjson::ondemand::array& array, const char* key, simdjson::ondemand::value jsonObjectData) {
	if (jsonObjectData[key].get(array) == simdjson::error_code::SUCCESS) {
		return true;
	} else {
		return false;
	}
}

bool getArray(simdjson::ondemand::array& array, simdjson::ondemand::value jsonObjectData) {
	if (jsonObjectData.get(array) == simdjson::error_code::SUCCESS) {
		return true;
	} else {
		return false;
	}
}

std::string getString(ObjectReturnData jsonData, const char* key) {
	std::string_view value{};
	if (jsonData.didItSucceed && jsonData.object[key].get(value) == simdjson::error_code::SUCCESS) {
		return static_cast<std::string>(value);
	}
	return static_cast<std::string>(value);
}

std::string getString(simdjson::ondemand::value jsonData) {
	std::string_view value{};
	if (jsonData.get(value) == simdjson::error_code::SUCCESS) {
		return std::string{ value.data(), value.size() };
	} else {
		return "";
	}
}

ObjectReturnData getObject(simdjson::ondemand::value jsonData, const char* objectName) {
	ObjectReturnData value{};
	if (jsonData[objectName].get(value.object) == simdjson::error_code::SUCCESS) {
		value.didItSucceed = true;
	}
	return value;
}

ObjectReturnData getObject(ObjectReturnData jsonData, const char* objectName) {
	ObjectReturnData value{};
	if (jsonData.didItSucceed && jsonData.object[objectName].get(value.object) == simdjson::error_code::SUCCESS) {
		value.didItSucceed = true;
	}
	return value;
}

ObjectReturnData getObject(ArrayReturnData jsonData, uint64_t objectIndex) {
	ObjectReturnData value{};
	if (jsonData.didItSucceed && jsonData.arrayValue.at(objectIndex).get(value.object) == simdjson::error_code::SUCCESS) {
		value.didItSucceed = true;
	}
	return value;
}

ArrayReturnData getArray(simdjson::ondemand::value jsonData, const char* arrayName) {
	ArrayReturnData value{};
	if (jsonData[arrayName].get(value.arrayValue) == simdjson::error_code::SUCCESS) {
		value.didItSucceed = true;
	}
	return value;
}

ArrayReturnData getArray(ObjectReturnData jsonData, const char* arrayName) {
	ArrayReturnData value{};
	if (jsonData.didItSucceed && jsonData.object[arrayName].get(value.arrayValue) == simdjson::error_code::SUCCESS) {
		value.didItSucceed = true;
	}
	return value;
}


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
		this->theD = value.getValue<TheDJson>();
	}
	TheDJson theD{};
};

struct Activities {
	Activities(simdjson::ondemand::value value) {
		this->createdAt = getString(value, "created_at");
		this->name = getString(value, "name");
		//std::cout << "CURRENT NAME: " << this->name << std::endl;
		this->anotherValue = getInt32(value, "ANOTHER_VALUE");
		this->anotherTestValue = getString(value, "ANOTHER_TEST_VALUE");
		this->anotherValue02 = getInt32(value, "ANOTHER_VALUE_02");
		this->anotherValue02w = getInt32(value, "ANOTHER_VALUE_02w");
		this->id = getString(value, "id");
		this->type = getInt8(value, "type");
		this->testDouble = getFloat(value, "test_double");
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

struct TheD {
	TheD() noexcept = default;
	TheD(simdjson::ondemand::value value) {
		simdjson::ondemand::value valueNew{};
		value["d"].get(valueNew);
		auto theArray = getArray(valueNew, "activitiess");
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
		std::string stringNew{
			"{\"d\":{\"activitiess\":[{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|\",\"test_double\":334.4545,"
			"\"type\":3,\"ANOTHER_VALUE\":3434,\"ANOTHER_TEST_VALUE\":\"TESTING-TESTING\",\"ANOTHER_VALUE_02\":3434,\"ANOTHER_TEST_"
			"VALUE_03\":\"TESTING-TESTING_031\",\"ANOTHER_VALUE_02w\":3434,\"ANOTHER_TEST_VALUE_03d\":\"TESTING-TESTING_031d\",\"TRUTH_TEST\":true},{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|\",\"test_double\":334.4545,"
			"\"type\":3,\"ANOTHER_VALUE\":3434,\"ANOTHER_TEST_VALUE\":\"TESTING-TESTING\",\"ANOTHER_VALUE_02\":3434,\"ANOTHER_TEST_"
			"VALUE_03\":\"TESTING-TESTING_031\",\"ANOTHER_VALUE_02w\":3434,\"ANOTHER_TEST_VALUE_03d\":\"TESTING-TESTING_031d\",\"TRUTH_TEST\":true},{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|\",\"test_double\":334.4545,"
			"\"type\":3,\"ANOTHER_VALUE\":3434,\"ANOTHER_TEST_VALUE\":\"TESTING-TESTING\",\"ANOTHER_VALUE_02\":3434,\"ANOTHER_TEST_"
			"VALUE_03\":\"TESTING-TESTING_031\",\"ANOTHER_VALUE_02w\":3434,\"ANOTHER_TEST_VALUE_03d\":\"TESTING-TESTING_031d\",\"TRUTH_TEST\":true},{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|\",\"test_double\":334.4545,"
			"\"type\":3,\"ANOTHER_VALUE\":3434,\"ANOTHER_TEST_VALUE\":\"TESTING-TESTING\",\"ANOTHER_VALUE_02\":3434,\"ANOTHER_TEST_"
			"VALUE_03\":\"TESTING-TESTING_031\",\"ANOTHER_VALUE_02w\":3434,\"ANOTHER_TEST_VALUE_03d\":\"TESTING-TESTING_031d\",\"TRUTH_TEST\":true},{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|\",\"test_double\":334.4545,"
			"\"type\":3,\"ANOTHER_VALUE\":3434,\"ANOTHER_TEST_VALUE\":\"TESTING-TESTING\",\"ANOTHER_VALUE_02\":3434,\"ANOTHER_TEST_"
			"VALUE_03\":\"TESTING-TESTING_031\",\"ANOTHER_VALUE_02w\":3434,\"ANOTHER_TEST_VALUE_03d\":\"TESTING-TESTING_031d\",\"TRUTH_TEST\":true},{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|\",\"test_double\":334.4545,"
			"\"type\":3,\"ANOTHER_VALUE\":3434,\"ANOTHER_TEST_VALUE\":\"TESTING-TESTING\",\"ANOTHER_VALUE_02\":3434,\"ANOTHER_TEST_"
			"VALUE_03\":\"TESTING-TESTING_031\",\"ANOTHER_VALUE_02w\":3434,\"ANOTHER_TEST_VALUE_03d\":\"TESTING-TESTING_031d\",\"TRUTH_TEST\":true}]}}"
		};

		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		size_t oldSize = stringNew.size();
		

		std::cout << "THE STRING: " << stringNew << std::endl;
		std::string stringNewer = stringNew;
		stopWatch.resetTimer();
		std::cout << "THE STRING SIZE BEFORE: " << stringNew.size() << std::endl;
		stringNew.resize(stringNew.size() + 256 - stringNew.size() % 256);
		std::cout << "THE STRING SIZE AFTER: " << stringNew.size() << std::endl;

		
		for (size_t x = 0ull; x < 2048ull * 64ull; ++x) {
			Jsonifier::SimdJsonValue theParser{};
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

		
			
		for (size_t x = 0ull; x < 2048ull * 64ull; ++x) {
			stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
			simdjson::ondemand::parser parser{};
			auto newDocument = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheValue value{ newDocument };
			//std::cout << "THE VALUE: " << value.theD.activities.back().name << std::endl;
			totalSize += oldSize;
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		

	} catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}


	return 0;
};
