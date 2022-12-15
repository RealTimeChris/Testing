#include "Jsonifier.hpp"
#include "DataParsingFunctionc.hpp"
#include <simdjson.h>
//#include "src/generic/stage2/tape_builder.h"

struct ClientStatusJson {
	ClientStatusJson() noexcept = default;
	ClientStatusJson(Jsonifier::Jsonifier& value) {
		this->web = value["web"].getValue<std::string>();
	};
	std::string web{};
};

struct ActivitiesJson {
	ActivitiesJson(Jsonifier::Jsonifier& value) {
		this->createdAt = value["created_at"].getValue<std::string>();
		this->id= value["id"].getValue<std::string>();
		this->name= value["name"].getValue<std::string>();
		this->type = value["type"].getValue<int32_t>();
	};
	std::string createdAt{};
	std::string name{};
	std::string id{};
	int32_t type{};
};

struct TheDJson {
	TheDJson() noexcept = default;
	TheDJson(Jsonifier::Jsonifier& value) {
		auto theArray = value["d"]["activitiess"].getValue<std::vector<ActivitiesJson>>();
		for (auto value: theArray) {
			activities.push_back(value);
		}
	}
	std::vector<ActivitiesJson> activities{};
};

namespace Jsonifier {
	template<> TheDJson Jsonifier::getValue() {
		return TheDJson{ *this };
	}

}

struct TheValueJson {
	TheValueJson(Jsonifier::Jsonifier& value) {
		this->theD = value.getValue<TheDJson>();
	}
	TheDJson theD{};
};

struct ClientStatus {
	ClientStatus() noexcept = default; 
	ClientStatus(simdjson::ondemand::value value) {
		this->web= DiscordCoreAPI::getString(value, "web");
	};
	std::string web{};
};

struct Activities {
	Activities(simdjson::ondemand::value value) {
		this->createdAt = DiscordCoreAPI::getString(value, "created_at");
		this->id = DiscordCoreAPI::getString(value, "id");
		this->name = DiscordCoreAPI::getString(value, "name");
		this->type = DiscordCoreAPI::getInt32(value, "type");
	};
	std::string createdAt{};
	std::string name{};
	std::string id{};
	int32_t type{};
};

struct TheD {
	TheD() noexcept = default;
	TheD(simdjson::ondemand::value value) {
		simdjson::ondemand::value valueNew{};
		value["d"]["activitiess"].get(valueNew);
		auto theArray = DiscordCoreAPI::getArray(value["d"], "activitiess");
		if (theArray.didItSucceed) {
			for (auto value: theArray.arrayValue) {
				activities.push_back(value.value());
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

template<typename OTy>
void prepStringForParsing(std::basic_string<OTy>&string) {
	string.resize(string.size() + 256 - string.size() % 256);
}

int32_t main() noexcept {
	try {  
		std::string stringNew{ "{\"d\":{\"activitiess\":[{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|"
							   "bitbot.tools\",\"type\":3,\"ANOTHER_VALUE\":3434,\"ANOTHER_TEST_VALUE\":\"TESTING-TESTING\"}]}}" };
		
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		size_t oldSize = stringNew.size();
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		std::cout << "THE STRING: " << stringNew << std::endl;
		prepStringForParsing(stringNew);
		std::cout << "THE STRING SIZE: " << stringNew.size() << std::endl;
		stopWatch.resetTimer();
		Jsonifier::Jsonifier jsonData{}; 
		for (size_t x = 0; x < 2048; ++x) {			
			Jsonifier::SimdJsonValue stringScanner{ stringNew.data(), stringNew.size() };
			jsonData = stringScanner.getJsonData();
			TheValueJson theValue{ jsonData };
			totalSize += oldSize;
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		jsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::cout << "THE DATA" << jsonData.operator std::basic_string_view<char, std::char_traits<char>>() << std::endl;
		

		stringNew.reserve(stringNew.size() + simdjson::SIMDJSON_PADDING);
		std::cout << "THE STRING: " << stringNew << std::endl;
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		for (size_t x = 0; x < 2048; ++x) {
			simdjson::ondemand::parser parser{};
			auto newDocument = parser.iterate(stringNew.data(), stringNew.size(), stringNew.capacity());
			TheValue theValue{ newDocument };
			totalSize += oldSize;
			
		}
		
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;

		


	} catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}


	return 0;
};