#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include "ErlPacker.hpp"

struct testClass {
	void testFunction(std::string&& theVector) {
		std::string newString{};
		newString = std::move(theVector);
	}
	void testFunction02(char* theVector, size_t length) {
		std::unique_ptr<char[]> thePtr{};
		std::string theString{ theVector, length };
		std::cout << "THE STRING: " << theString << "TESTING" << std::endl;
	}
};

struct TestClass {
	float theFloat{ 2.33 };
	int32_t theInt{ 233 };
	std::string theString{ "HELP HELP HELP" };
};

DiscordCoreAPI::CoRoutine<void> parseObject(std::string& theString, TestClass& theDat, simdjson::ondemand::parser*theParser) {
	co_await DiscordCoreAPI::NewThreadAwaitable<void>();
	theString.reserve(theString.size() + simdjson::SIMDJSON_PADDING);
	auto theDocument = theParser->iterate(theString);
	
	theDat.theFloat = theDocument.find_field("theFloat").get_double().take_value();
	theDat.theInt = theDocument.find_field("theInt").get_int64().take_value();
	theDat.theString = theDocument.find_field("theString").get_string().take_value();
}
void function(const std::string& theString) {
	auto newString = (std::string&)theString;
	newString.resize(2000);
}
int32_t main() {
	try {
		std::string theString{ "-2" };
		std::cout << stoull(theString) << std::endl;
		{std::string theValueNew{ R"(\u000D)" };
		std::cout << theValueNew << std::endl;
			DiscordCoreAPI::ModifyGuildData theGuild{ DiscordCoreAPI::Guild{} };
			theValueNew.resize(1024 * 1024*1024);
			DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::microseconds{1} };
			theStopWatch.resetTimer();
			std::string_view theNewStringer{ theValueNew };
			std::cout << "THE TIME PASSED: " << theStopWatch.totalTimePassed() << std::endl;
			nlohmann::json theValue{ };
			theValue["d"] = static_cast<std::string>(theGuild);
			theValue["op"] = 1;
			theValue["s"] = 1344;
			theValue["t"] = "GUILD_CREATE";
			simdjson::ondemand::parser parser{};
			ErlPacker thePacker{};
			auto theString = thePacker.parseJsonToEtf(theValue);
			auto theNewString = thePacker.parseEtfToJson(theString);
			theNewString.reserve(theNewString.size() + simdjson::SIMDJSON_PADDING);
			auto theDocument = parser.iterate(theNewString);
			std::cout << "THE RESULT 03: " << theNewString << std::endl;
			uint64_t theValue01{};
			theValue01 = theDocument["s"].get_uint64();
			std::cout << "THE RESULT 03: " << theValue01 << std::endl;
			uint64_t theValue02{};
			theDocument["op"].get(theValue02);
			std::cout << "THE RESULT 03: " << theValue02 << std::endl;
			theDocument["opss"].get(theValue02);
			std::cout << "THE RESULT 03: " << theValue02 << std::endl;
			theValue01 = theDocument["opss"].get_uint64();
			//std::cout << "THE RESULT 03: " << theDocument.get_object() << std::endl;

		}
		 
		 
		 
		 std::this_thread::sleep_for(std::chrono::seconds{ 3 });

	}
	 catch (...) { DiscordCoreAPI::reportException("main()"); };
	
	 return 0;
}

