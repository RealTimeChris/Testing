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

void testFunction(const char* theString, int32_t length) {
	std::string theStringNew{};
	theStringNew.resize(25);
	memcpy(theStringNew.data(), theString, length);

}

std::string getString(simdjson::ondemand::value& jsonData, const char* theKey) {
	try {
		std::string_view theStringNew{};
		auto theValue = jsonData[theKey].get(theStringNew);
		if (theValue != simdjson::error_code::SUCCESS) {
			return "0";
		}
		std::string theStringNewer{ theStringNew.data(), theStringNew.size() };
		return theStringNewer;
	}
	catch (...) {
		return "";
	}
}

template<typename ReturnType>void parseObject(simdjson::ondemand::value& jsonObjectData, ReturnType& theData){}
	
template<> void parseObject(simdjson::ondemand::value& jsonObjectData, DiscordCoreAPI::GuildWidgetImageData& theData) {
	theData.url = getString(jsonObjectData, "widget_image");
}

void escapeCharacters(std::string& theString) {
	auto theSize = theString.size();
	for (int32_t x = 0; x < theSize; x++) {
		switch (static_cast<char>(theString[x])) {
		case 0x0008: {
			theString[x] = '\\';
			theString.insert(theString.begin() + x + 1, 'b');
			break;
		}
		case 0x0009: {
			theString[x] = '\\';
			theString.insert(theString.begin() + x + 1, 't');
			break;
		}
		case 0x000A: {
			theString[x] = '\\';
			theString.insert(theString.begin() + x + 1, 'n');
			break;
		}
		case 0x000B: {
			theString[x] = '\\';
			theString.insert(theString.begin() + x + 1, 'v');
			break;
		}
		case 0x000C: {
			theString[x] = '\\';
			theString.insert(theString.begin() + x + 1, 'f');
			break;
		}
		case 0x000D: {
			theString[x] = '\\';
			theString.insert(theString.begin() + x + 1, 'r');
			break;
		}
		case 0x005: {
			theString[x] = '\\';
			theString.insert(theString.begin() + x + 1, '\\');
			break;
		}
		case 0x0022: {
			theString[x] = '\\';
			theString.insert(theString.begin() + x + 1, '"');
			break;
		}
		case 0x0027: {
			theString[x] = '\\';
			theString.insert(theString.begin() + x + 1, '\'');
			break;
		}
		default: {
		}
		}
	}
}

std::string_view theFunction(std::string& theString, DiscordCoreAPI::StopWatch<std::chrono::microseconds>& theStopWatch) {
	std::cout << "TIME PASSED 0303: " << theStopWatch.totalTimePassed() << std::endl;
	theStopWatch.resetTimer();
	std::string_view theStringNew{ theString.data(), theString.size() };
	std::cout << "TIME PASSED 0404: " << theStopWatch.totalTimePassed() << std::endl;
	theStopWatch.resetTimer();
	return theString;
}

int32_t main() {
	try {
		DiscordCoreAPI::GuildWidgetImageData theData{};
		DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::microseconds{1} };
		theStopWatch.resetTimer();
		std::string theValue{};
		std::string& theValue02{ theValue };
		std::cout << "TIME PASSED 0101: " << theStopWatch.totalTimePassed() << std::endl;
		theValue.resize(1024 * 1024 * 1024);
		std::cout << "TIME PASSED 0202: " << theStopWatch.totalTimePassed() << std::endl;

		theStopWatch.resetTimer();
		theValue02 = theFunction(theValue, theStopWatch);
		std::cout << "TIME PASSED 0505: " << theStopWatch.totalTimePassed() << std::endl;

		 
		 
		 std::this_thread::sleep_for(std::chrono::seconds{ 3 });

	}
	 catch (...) { DiscordCoreAPI::reportException("main()"); };
	
	 return 0;
}

