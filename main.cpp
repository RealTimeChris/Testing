#ifndef _TESTING
	#define _TESTING
#endif

#include "Jsonifier.hpp"
#include <simdjson.h>
#include "DataParsingFunctions.hpp"
#include <fstream>

Jsonifier ::StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
int64_t iterationCount{};
int64_t totalTime{};

struct ActivitiesJson {
	ActivitiesJson() noexcept = default;
	ActivitiesJson(Jsonifier::Array&& value) {
		this->TEST_VALUE_00 = value["TEST_VALUE_00"].get<double>().getValue();
		//std::cout << "CURRENT TYPE: " << ( int32_t )value["TEST_VALUE_01"].getType() << std::endl;
		this->TEST_VALUE_01 = value["TEST_VALUE_01"].get<bool>().getValue();
		this->TEST_VALUE_02 = value["TEST_VALUE_02"].get<std::string>().getValue();
		this->TEST_VALUE_03 = value["TEST_VALUE_03"].get<uint64_t>().getValue();
		this->TEST_VALUE_04 = value["TEST_VALUE_04"].get<double>().getValue();
		//std::cout << "CURRENT TYPE: " << ( int32_t )va["TEST_VALUE_05"].getType() << std::endl;
		this->TEST_VALUE_05 = value["TEST_VALUE_05"].get<bool>().getValue();
		this->TEST_VALUE_06 = value["TEST_VALUE_06"].get<std::string>().getValue();
		this->TEST_VALUE_07 = value["TEST_VALUE_07"].get<uint64_t>().getValue();
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
		auto fieldNew = value.get<Jsonifier::Field>("TEST_VALUE_11");
		auto objectNewer = fieldNew.getValue();
		Jsonifier::Array arrayNewer = objectNewer.get<Jsonifier::Array>().getValue();

		int32_t index{};

		for (auto iter = arrayNewer.begin(); iter != arrayNewer.end(); ++iter) {
			index++;

			std::string_view newValueDouble = iter->get<std::string_view>().getValue();
			std::cout << "NEW INDEX: " << newValueDouble << std::endl;
		}
		std::cout << "NEW INDEX: WERE DONE" << index << std::endl;
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
	TheValueJson(Jsonifier::JsonifierResult<Jsonifier::Document>&& value) {
		Jsonifier::Document documentNew{};
		if (auto result = value.get(documentNew);result != Jsonifier::ErrorCode::Success) {
			throw Jsonifier::JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<Jsonifier::EnumStringConverter>(result) } };
		}
		this->theD = TheDJson{ std::move(documentNew) };
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
		//arrayValueNew["TEST_VALUE_95"] = arrayValue;
		for (size_t x = 0; x < 30; ++x) {
			serializer["TEST_VALUE_11"].emplaceBack(std::string{ "222003323" });
		}
		std::cout << "CURRENT SIZE: " << serializer.size() << std::endl;
		//for (auto& value : serializer) {
		//std::cout << "THE TYPE: " << ( int32_t )value.getType() << std::endl;
		//}
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::string stringNew{ serializer.operator std::string&&() };
		//std::string stringNew{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
		size_t totalTime{};
		size_t totalSize{};
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		{
			Jsonifier::ObjectBuffer<Jsonifier::OpenContainer> objectBuffer{};
			objectBuffer.allocate(512 * 512 * 128);
			objectBuffer.deallocate();
		}

		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		{
			std::unique_ptr<Jsonifier::OpenContainer[]> objectBuffer{};
			objectBuffer.reset(new (std::nothrow) Jsonifier::OpenContainer[512 * 512 * 128]);
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
		for (size_t x = 0ull; x < 2048ull * 1ull; ++x) {
			Jsonifier::JsonifierCore theParser{};
			auto jsonData = theParser.getJsonData(stringNew);
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

		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		
		for (size_t x = 0ull; x < 2048ull * 1ull; ++x) {
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


	} catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }


	return 0;
};