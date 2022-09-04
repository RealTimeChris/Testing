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

void parseObject(std::string& theString, TestClass& theDat) {
	simdjson::ondemand::parser theParser{};
	theString.reserve(theString.size() + simdjson::SIMDJSON_PADDING);
	auto theDocument = theParser.iterate(theString);
	theDat.theInt = theDocument.find_field("theInt").get_int64().take_value();
	theDat.theString = theDocument.find_field("theString").get_string().take_value();
}

int32_t main() {
	 try {
		 nlohmann::json theValue{};
		 theValue["theInt"] = int32_t{ 254423243 };
		 theValue["theString"] = "TESTING TESTING FOR REAL";
		 theValue["theFloat"] = 34.234f;
		 simdjson::ondemand::parser parser{};
		 ErlPacker thePacker{};
		 std::cout << "THE RESULT 00: " << theValue.dump() << std::endl;
		 auto theResult = thePacker.parseJsonToEtf(theValue);
		 std::cout << "THE RESULT 01: " << theResult << std::endl;
		 auto theResult02 = thePacker.parseEtfToJson(theResult);
		 TestClass theObject{};
		 parseObject(theResult02, theObject);
		 std::cout << "THE RESULT 02: " << theResult02 << std::endl;
		 nlohmann::json theStringNewest{ nlohmann::json::parse(theResult02) };
		 std::cout << theObject.theInt << ", TEST VALUE 02: " << theObject.theString << std::endl;
		 std::cout << "THE RESULT 03: " << theStringNewest << std::endl;
		 std::this_thread::sleep_for(std::chrono::seconds{ 3 });

	}
	 catch (...) { DiscordCoreAPI::reportException("main()"); };
	
	 return 0;
}

