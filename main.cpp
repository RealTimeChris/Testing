#include "Jsonifier.hpp"
#include "DataParsingFunctionc.hpp"
#include <simdjson.h>


struct UserJson {
	UserJson() noexcept = default;
	UserJson(Jsonifier::Jsonifier value) {
		this->id = value["id"].getValue<std::string>();
	}
	std::string id{};
};

namespace Jsonifier {
	template<> UserJson&& Jsonifier::getValue() {
		return UserJson{ *this };
	}

}

struct ClientStatusJson {
	ClientStatusJson() noexcept = default;
	ClientStatusJson(Jsonifier::Jsonifier value) {
		this->mobile = value["mobile"].getValue<std::string>();
	}
	std::string mobile{};
};

struct TheDJson {
	TheDJson() noexcept = default;
	TheDJson(Jsonifier::Jsonifier value) {
		this->user = value["user"].getValue<UserJson>();
		this->status = value["client_status"].getValue<ClientStatusJson>();
		this->guildId = value["guild_id"].getValue<std::string>();
		this->statusString = value["status"].getValue<std::string>();
	}
	ClientStatusJson status{};
	std::string guildId{};
	std::string statusString{};
	UserJson user{};
};

struct TheValueJson {
	TheValueJson(Jsonifier::Jsonifier&& value) {
		this->theD = value["d"].getValue<TheDJson>();
	}
	TheDJson theD{};
};

struct User {
	User() noexcept = default;
	User(simdjson::ondemand::value value) {
		this->id = DiscordCoreAPI::getString(value, "id");
	}
	std::string id{};
};

struct ClientStatus {
	ClientStatus() noexcept = default;
	ClientStatus(simdjson::ondemand::value value) {
		this->mobile = DiscordCoreAPI::getString(value, "mobile");
	}
	std::string mobile{};
};

struct TheD {
	TheD() noexcept = default;
	TheD(simdjson::ondemand::value value) {
		this->user = User{ value["user"] };
		this->status = ClientStatus{ value["client_status"] };
		this->guildId = DiscordCoreAPI::getString(value, "guild_id");
		this->statusString = DiscordCoreAPI::getString(value, "status");
	}
	ClientStatus status{};
	std::string guildId{};
	std::string statusString{};
	User user{};
};

struct TheValue {
	TheValue(simdjson::ondemand::value value) {
		this->theD = TheD{ value["d"] };
	}
	TheD theD{};
};

int32_t main() noexcept {
	try {
		std::string string64{ "{\"Nam\":{\"test\":234,\"true\":false},\"t\":\"testing\\""}" };
		std::string string256{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
		std::string stringNew{ "{\"d\":{\"activitiess\":[{\"created_at\":\"1669495273631\",\"id\":\"ec0b28a579ecb4bd\",\"name\":\"ETH+0.58%|"
							   "bitbot.tools\",\"type\":3}],\"client_status\":{\"web\":\"online\"},\"guild_id\":\"815087249556373516\",\"status\":"
							   "\"online\",\"user\":{\"id\":\"821569769786179584\"}},\"op\":0,\"s\":18349,\"t\":\"PRESENCE_UPDATE\"}" };
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		stringNew.reserve(stringNew.size() + 256);
		string64.reserve(stringNew.size() + 256);
		//auto newJsonData = stringScanner.getJsonData();
		stopWatch.resetTimer();
		std::cout << "GETJSONDATA()'s TIME: " << stopWatch.totalTimePassed() << std::endl; 

		//newJsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
		//std::cout << "THE DATA: " << newJsonData.operator std::string&&() << std::endl;
		std::cout << "THE STRING: " << stringNew << std::endl;
		std::cout << "THE STRING: " << string64 << std::endl;
		stopWatch.resetTimer();


		//auto newJsonData = stringScanner.getJsonData();
		//Jsonifier::SimdJsonValue stringScanner{ string64 };
		Jsonifier::SimdJsonValue stringScanner{ string64 };
		//Jsonifier::SimdJsonValue stringScannerNew{ stringNew };
		//stringScannerNew.generateRawTape();
		for (size_t x = 0; x < 256 * 16384; ++x) {
			auto newJsonData = stringScanner.getJsonData();
			//std::cout << "GETJSONDATA()'s TIME: " << stopWatch.totalTimePassed() << std::endl;
			newJsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
			std::cout << "THE DATA: " << newJsonData.operator std::string() << std::endl;
			TheValueJson theValue{ std::move(newJsonData) };
			totalSize += stringNew.size();
		}
		totalTime += stopWatch.totalTimePassed().count();

		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;

		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();

		for (size_t x = 0; x < 256 * 16384; ++x) {
			simdjson::ondemand::parser parser{};
			auto newDocument = parser.iterate(stringNew);
			TheValue theValue{ newDocument };
			totalSize += stringNew.size();
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;


	} catch (std::exception& e) {
		std::cout << "THE ERROR: " << e.what() << std::endl;
	}


	return 0;
};