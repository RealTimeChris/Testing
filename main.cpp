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
		std::string string64{ "{\"\\\\\\\"Nam[{\":[116,\"\\\\\\\\\",234,\"true\",false],\"t\":\"\\\\\\\"\"}" };
		std::string string256{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
		std::string stringNew{ "{\"d\":{\"activities\":[],\"client_status\":{\"mobile\":\"online\"},\"guild_id\":\"815087249556373516\",\"status\":"
							   "\"online\",\"user\":{\"id\":\"381531043334717440\",\"new_value\":24.56f}}}" };
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		stringNew.reserve(stringNew.size() + 256);
		//auto newJsonData = stringScanner.getJsonData();
		stopWatch.resetTimer();
		std::cout << "GETJSONDATA()'s TIME: " << stopWatch.totalTimePassed() << std::endl;

		//newJsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
		//std::cout << "THE DATA: " << newJsonData.operator std::string&&() << std::endl;
		std::cout << "THE STRING: " << stringNew << std::endl;
		stopWatch.resetTimer();


		//auto newJsonData = stringScanner.getJsonData();

		Jsonifier::SimdJsonValue stringScanner{ stringNew };

		stringScanner.generateTapeRecord();
		for (size_t x = 0; x < 256 * 16384; ++x) {
			Jsonifier::SimdJsonValue stringScannerNew{ stringNew };
			//stopWatch.resetTimer();
			auto newJsonData = stringScannerNew.getJsonData();
			//std::cout << "GETJSONDATA()'s TIME: " << stopWatch.totalTimePassed() << std::endl;
			newJsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
			//std::cout << "THE DATA: " << newJsonData.operator std::string() << std::endl;
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