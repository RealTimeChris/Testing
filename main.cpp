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
	std::string id{};
	std::string name{};
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
	bool status{};
	bool guildId{};
	std::vector<int8_t> user{};
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
	std::string id{};
	std::string name{};
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
	bool status{};
	bool guildId{};
	std::vector<int8_t> user{};
};

struct TheValue {
	TheValue(simdjson::ondemand::value value) {
		this->theD = TheD{ value };
	}
	TheD theD{};
};


int32_t main() noexcept {
	try {

		//std::string stringNew{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
		
		//std::string stringNew{ "{\"d\":{\"activitiess\":[{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|"
				//"bitbot.tools\",\"type\":3}],\"client_status\":{\"web\":\"online\"},\"guild_id\":\"815087249556373516\",\"status\":"
				//			   "\"online\",\"user\":{\"id\":\"821569769786179584\"}},\"op\":0,\"s\":18349,\"t\":\"PRESENCE_UPDATE\"}" };
							   
		std::string stringNew{ "{\"d\":{\"activitiess\":[{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|"
							   "bitbot.tools\",\"type\":3}]}}" };
		
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		size_t oldSize = stringNew.size();
		//string64.reserve(string64.size() + 256);
		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		std::cout << "THE STRING: " << stringNew << std::endl;
		stringNew.resize(stringNew.size() + 256 - stringNew.size() % 256);
		std::cout << "THE STRING SIZE: " << stringNew.size() << std::endl;
		stopWatch.resetTimer();
		Jsonifier::Jsonifier jsonData{}; 
		for (size_t x = 0; x < 2048; ++x) {			
			Jsonifier::SimdJsonValue stringScanner{ stringNew.data(), stringNew.size(), stringNew.capacity() };
			jsonData = stringScanner.getJsonData();
			TheValueJson theValue{ jsonData };
			totalSize += oldSize;
		}
		totalTime += stopWatch.totalTimePassed().count();

		//std::cout << "THE FINAL STRING: " << stringNewer << std::endl;
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;
		jsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::cout << "THE DATA" << jsonData.operator std::basic_string_view<char, std::char_traits<char>>() << std::endl;
		//std::cout << "THE SIZE: " << string64.size() << std::endl;
		

		stringNew.reserve(stringNew.size() + simdjson::SIMDJSON_PADDING);
		////string64.reserve(string64.size() + simdjson::SIMDJSON_PADDING);
		std::cout << "SIZE: " << stringNew.size() << ", CAPACITY: " << stringNew.capacity() << std::endl;
		std::cout << "THE STRING: " << stringNew << std::endl;
		////std::cout << "THE STRING: " << string64 << std::endl;
		

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
		std::cout << "THE FINAL STRING: " << stringNew << std::endl;
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;

		


	} catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}


	return 0;
};