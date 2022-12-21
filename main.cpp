#ifndef _TESTING
	#define _TESTING
#endif

#include "Jsonifier.hpp"
#include <simdjson.h>
#include "DataParsingFunctionc.hpp"
#include <fstream>

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
		auto theArray = value["d"]["activities"].getValue<std::vector<Jsonifier::JsonParser>>();
		for (auto& value: theArray) {
			std::cout << "THE CURRENT SIZE: " << theArray.size() << std::endl;
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
		this->createdAt = DiscordCoreAPI::getString(value, "created_at");
		this->name = DiscordCoreAPI::getString(value, "name");
		//std::cout << "CURRENT NAME: " << this->name << std::endl;
		this->anotherValue = DiscordCoreAPI::getInt32(value, "ANOTHER_VALUE");
		this->anotherTestValue = DiscordCoreAPI::getString(value, "ANOTHER_TEST_VALUE");
		this->anotherValue02 = DiscordCoreAPI::getInt32(value, "ANOTHER_VALUE_02");
		this->anotherValue02w = DiscordCoreAPI::getInt32(value, "ANOTHER_VALUE_02w");
		this->id = DiscordCoreAPI::getString(value, "id");
		this->type = DiscordCoreAPI::getInt8(value, "type");
		this->testDouble = DiscordCoreAPI::getFloat(value, "test_double");
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
		auto theArray = DiscordCoreAPI::getArray(valueNew, "activities");
		//std::string_view guildId = value["d"]["guild_id"].get<std::string_view>();
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
		Jsonifier::Jsonifier serializer{};
		Jsonifier::Jsonifier arrayValue{};
		arrayValue["TEST_VALUE_01"] = 0.00333423f;
		arrayValue["TEST_VALUE_02"] = false;
		arrayValue["TEST_VALUE_03"] = "TESTING_VALUE0101";
		arrayValue["TEST_VALUE_04"] = "4325454";
		for (size_t x = 0; x < 10; ++x) {
			serializer["d"]["activitiess"].emplaceBack(arrayValue);
		}
		serializer.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::string stringNew{
			"{\"d\":{\"activities\":[{\"created_at\":\"1671633694558\",\"id\":\"custom\",\"name\":\"Custom Status\",\"state\":\"Destroyer of "
			"ethereal garbage, Jewish speed runner, mematic warfare expert, hates midwits,  why do you need to be "
			"needed?\",\"type\":4},{\"created_at\":\"1671633694558\",\"id\":\"custom\",\"name\":\"Custom Status\",\"state\":\"Destroyer of "
			"ethereal garbage, Jewish speed runner, mematic warfare expert, hates midwits,  why do you need to be "
			"needed?\",\"type\":4},{\"created_at\":\"1671633694558\",\"id\":\"custom\",\"name\":\"Custom Status\",\"state\":\"Destroyer of "
			"ethereal garbage, Jewish speed runner, mematic warfare expert, hates midwits,  why do you need to be "
			"needed?\",\"type\":4},{\"created_at\":\"1671633694558\",\"id\":\"custom\",\"name\":\"Custom Status\",\"state\":\"Destroyer of "
			"ethereal garbage, Jewish speed runner, mematic warfare expert, hates midwits,  why do you need to be "
			"needed?\",\"type\":4}],\"TEST_VALUE\":0.0444,\"client_status\":{\"mobile\":\"online\"},\"guild_id\":\"931640556814237706\",\"status\":"
			"\"online\",\"user\":{"
			"\"id\":\"849261763013640233\"}},\"op\":0,\"s\":3053,\"t\":\"PRESENCE_UPDATE\"}"
		};

		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		size_t oldSize = stringNew.size();


		std::cout << "THE STRING: " << stringNew << std::endl;
		std::string stringNewer = stringNew;
		stopWatch.resetTimer();

		
			Jsonifier::SimdJsonValue theParser{};
		for (size_t x = 0ull; x < 2048ull * 64ull; ++x) {
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



		stringNew.resize(stringNew.size() + 256 - stringNew.size() % 256);
		totalSize = 0;
		totalTime = 0;

		stopWatch.resetTimer();

		
			stringNewer.reserve(oldSize + simdjson::SIMDJSON_PADDING);
		simdjson::ondemand::parser parser{};
		for (size_t x = 0ull; x < 2048ull * 64ull; ++x) {
			auto newDocument = parser.iterate(stringNewer.data(), stringNewer.size(), stringNewer.capacity());
			TheValue value{ newDocument };
			//std::cout << "THE VALUE: " << value.theD.activities.back().name << std::endl;
			totalSize += oldSize;
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;


	} catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }


	return 0;
};
