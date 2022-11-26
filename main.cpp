#include "Jsonifier.hpp"
#include "DataParsingFunctionc.hpp"
#include <simdjson.h>

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
		Jsonifier::SimdJsonValue stringScanner{ stringNew };
		auto newJsonData = stringScanner.getJsonData();
		newJsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::cout << "THE DATA: " << newJsonData.operator std::string&&() << std::endl;
		std::cout << "THE STRING: " << stringNew << std::endl;
		stopWatch.resetTimer();
		for (size_t x = 0; x < 256 * 16384 / 4; ++x) {
			Jsonifier::SimdJsonValue stringScanner{ stringNew };
			auto newJsonData = stringScanner.getJsonData();
			totalSize += stringNew.size();
		}
		totalTime += stopWatch.totalTimePassed().count();

		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;

		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		simdjson::ondemand::parser parser{};
		auto newDocument = parser.iterate(stringNew);
		for (size_t x = 0; x < 256 * 16384; ++x) {
			DiscordCoreAPI::getInt64(newDocument, "thevalue");
			totalSize += string64.size();
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;


	} catch (std::exception& e) {
		std::cout << "THE ERROR: " << e.what() << std::endl;
	}


	return 0;
};