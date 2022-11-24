#include "Jsonifier.hpp"



int32_t main() noexcept {
	try {
	
		std::string string64{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
		std::string string256{ "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }"
							   "{ \"\\\\\\\"Nam[{\": [ 116,\"\\\\\\\\\" , 234, \"true\", false ], \"t\":\"\\\\\\\"\" }" };
		std::string stringNew{ "{\"d\":{\"activities\":[],\"client_status\":{\"mobile\":\"online\"},\"guild_id\":\"815087249556373516\",\"status\":"
							   "\"online\",\"user\":{\"id\":\"381531043334717440\"}}}" };
		Jsonifier::StopWatch<std::chrono::nanoseconds> stopWatch{ std::chrono::nanoseconds{ 25 } };
		size_t totalTime{};
		size_t totalSize{};
		Jsonifier::SimdStringScanner stringScanner{ stringNew };
		Jsonifier::Jsonifier theData{};
		auto newJsonData = stringScanner.getJsonData();
		newJsonData.refreshString(Jsonifier::JsonifierSerializeType::Json);
		std::cout << "THE DATA: " << newJsonData.operator std::string&&() << std::endl;
		std::cout << "THE STRING: " << stringNew << std::endl;
		stopWatch.resetTimer();
		for (size_t x = 0; x < 256 * 16384 / 4; ++x) {
			Jsonifier::SimdStringScanner simd8Test{ string256 };
			totalSize += string256.size();
		}
		totalTime += stopWatch.totalTimePassed().count();

		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;

		totalSize = 0;
		totalTime = 0;
		stopWatch.resetTimer();
		for (size_t x = 0; x < 256 * 16384; ++x) {
			//SimdBase64 simd8Test{ string64 };
			totalSize += string64.size();
		}
		totalTime += stopWatch.totalTimePassed().count();
		std::cout << "IT TOOK: " << totalTime << "ns TO PARSE THROUGH IT: " << totalSize << " BYTES!" << std::endl;


	} catch (std::exception& e) {
		std::cout << "THE ERROR: " << e.what() << std::endl;
	}


	return 0;
};