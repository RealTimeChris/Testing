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
/*
int64_t getInt64(Jsonifier::Array&& jsonData, const char* key) {
	int64_t value{}; /*
	if (jsonData.get<int64_t>().get(value) == Jsonifier::ErrorCode::Success) {
		return int64_t{ value };
	} else {
		return 0;
	}
	return 0;
}

int32_t getInt32(Jsonifier::Object jsonData, const char* key) {
	int64_t value{};
	if (jsonData[key].get(value) == Jsonifier::ErrorCode::Success) {
		return static_cast<int32_t>(value);
	} else {
		return 0;
	}
}

int16_t getInt16(Jsonifier::Object jsonData, const char* key) {
	int64_t value{};
	if (jsonData[key].get(value) == Jsonifier::ErrorCode::Success) {
		return static_cast<int16_t>(value);
	} else {
		return 0;
	}
}

int8_t getInt8(Jsonifier::Object jsonData, const char* key) {
	int64_t value{};
	jsonData[key].get<int64_t>(value);
	return static_cast<int8_t>(value);
}

uint64_t getUint64(Jsonifier::Object jsonData, const char* key) {
	uint64_t value{};
	if (jsonData[key].get(value) == Jsonifier::ErrorCode::Success) {
		return uint64_t{ value };
	} else {
		return 0;
	}
}

uint32_t getUint32(Jsonifier::Object jsonData, const char* key) {
	uint64_t value{};
	if (jsonData[key].get(value) == Jsonifier::ErrorCode::Success) {
		return static_cast<uint32_t>(value);
	} else {
		return 0;
	}
}

uint16_t getUint16(Jsonifier::Object jsonData, const char* key) {
	uint64_t value{};
	if (jsonData[key].get(value) == Jsonifier::ErrorCode::Success) {
		return static_cast<uint16_t>(value);
	} else {
		return 0;
	}
}

uint8_t getUint8(Jsonifier::Object jsonData, const char* key) {
	uint64_t value{};
	if (jsonData[key].get(value) == Jsonifier::ErrorCode::Success) {
		return static_cast<uint8_t>(value);
	} else {
		return 0;
	}
}

float getFloat(Jsonifier::Object jsonData, const char* key) {
	double value{};
	if (jsonData[key].get(value) == Jsonifier::ErrorCode::Success) {
		return double{ value };
	} else {
		return 0.0f;
	}
}

bool getBool(Jsonifier::Object jsonData, const char* key) {
	bool value{};
	if (jsonData[key].get(value) == Jsonifier::ErrorCode::Success) {
		return bool{ value };
	} else {
		return false;
	}
}

std::string getString(Jsonifier::Object jsonData, const char* key) {
	std::string_view value{};
	if (jsonData[key].get(value) == Jsonifier::ErrorCode::Success) {
		return std::string{ value.data(), value.size() };
	} else {
		return "";
	}
}

bool getObject(Jsonifier::Object& object, const char* key, Jsonifier::Object jsonObjectData) {
	if (jsonObjectData[key].get(object) == Jsonifier::ErrorCode::Success) {
		return true;
	} else {
		return false;
	}
}

bool getArray(Jsonifier::Array& array, const char* key, Jsonifier::Object jsonObjectData) {
	if (jsonObjectData[key].get(array) == Jsonifier::ErrorCode::Success) {
		return true;
	} else {
		return false;
	}
}

bool getArray(Jsonifier::Array& array, Jsonifier::Object jsonObjectData) {
	if (jsonObjectData.get(array) == Jsonifier::ErrorCode::Success) {
		return true;
	} else {
		return false;
	}
}

std::string getString(Jsonifier::ObjectReturnDataJson jsonData, const char* key) {
	std::string_view value{};
	if (jsonData.didItSucceed && jsonData.object[key].get(value) == Jsonifier::ErrorCode::Success) {
		return static_cast<std::string>(value);
	}
	return static_cast<std::string>(value);
}

std::string getString(Jsonifier::Object jsonData) {
	std::string_view value{};
	if (jsonData.get(value) == Jsonifier::ErrorCode::Success) {
		return std::string{ value.data(), value.size() };
	} else {
		return "";
	}
}

Jsonifier::ObjectReturnDataJson getObject(Jsonifier::Object jsonData, const char* objectName) {
	Jsonifier::ObjectReturnDataJson value{};
	if (jsonData[objectName].get(value.object) == Jsonifier::ErrorCode::Success) {
		value.didItSucceed = true;
	}
	return value;
}

Jsonifier::ObjectReturnDataJson getObject(Jsonifier::ObjectReturnDataJson jsonData, const char* objectName) {
	Jsonifier::ObjectReturnDataJson value{};
	if (jsonData.didItSucceed && jsonData.object[objectName].get(value.object) == Jsonifier::ErrorCode::Success) {
		value.didItSucceed = true;
	}
	return value;
}
/*
Jsonifier::ObjectReturnDataJson getObject(Jsonifier::ArrayReturnDataJson jsonData, uint64_t objectIndex) {
	Jsonifier::ObjectReturnDataJson value{};
	if (jsonData.didItSucceed && jsonData.arrayValue.at(objectIndex).get(value.object) == Jsonifier::ErrorCode::Success) {
		value.didItSucceed = true;
	}
	return value;
}

Jsonifier::ArrayReturnDataJson getArray(Jsonifier::Object jsonData, const char* arrayName) {
	Jsonifier::ArrayReturnDataJson value{};
	if (jsonData[arrayName].get(value.arrayValue) == Jsonifier::ErrorCode::Success) {
		value.didItSucceed = true;
	}
	return value;
}

Jsonifier::ArrayReturnDataJson getArray(Jsonifier::ObjectReturnDataJson jsonData, const char* arrayName) {
	Jsonifier::ArrayReturnDataJson value{};
	if (jsonData.didItSucceed && jsonData.object[arrayName].get(value.arrayValue) == Jsonifier::ErrorCode::Success) {
		value.didItSucceed = true;
	}
	return value;
}
*/
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
		Jsonifier::Object objectNew{};
		auto fieldNew = value.get<Jsonifier::Object>(objectNew);

		std::cout << "CURRENT KEY: " << objectNew.field_key() << std::endl;
		int32_t index{};

		//for (auto iter = arrayNewer.begin(); iter != arrayNewer.end(); ++iter) {
			index++;

		//	std::string_view newValueDouble = iter->get<std::string_view>().getValue();
			//std::cout << "NEW INDEX: " << newValueDouble << std::endl;
			//}
		//std::cout << "NEW INDEX: WERE DONE" << index << std::endl;
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
		for (size_t x = 0; x < 30; ++x) {
			serializer["TEST_VALUE_11"].emplaceBack(std::string{ "222003323.0097898" });
		}
		std::cout << "CURRENT SIZE: " << serializer.size() << std::endl;
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::string stringNew{ serializer.operator std::string&&() };
		size_t totalTime{};
		size_t totalSize{};
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		{
			Jsonifier::ObjectBuffer<bool> objectBuffer{};
			objectBuffer.allocate(512 * 512 * 128);
			objectBuffer.deallocate();
		}

		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		{
			std::unique_ptr<bool[]> objectBuffer{};
			objectBuffer.reset(new (std::nothrow) bool[512 * 512 * 128]);
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
		for (size_t x = 0ull; x < 2048ull * 128ull; ++x) {
			Jsonifier::JsonifierCore parser{};
			auto jsonData = parser.parseJson(stringNew);
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
		for (size_t x = 0ull; x < 2048ull * 128ull; ++x) {
			stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
			simdjson::ondemand::parser parser{};
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