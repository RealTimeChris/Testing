#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include <unordered_set>
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

template<typename ObjectType>
class ObjectCache {
public:
	ObjectCache() noexcept {};

	void emplace(ObjectType&& theData) noexcept {
		std::unique_lock theLock{ this->theMutex };
		this->theMap.emplace(std::move(theData));
	}

	void emplace(ObjectType& theData) noexcept {
		std::unique_lock theLock{ this->theMutex };
		this->theMap.emplace(theData);
	}

	const ObjectType& readOnly(ObjectType& theKey) noexcept {
		std::shared_lock theLock{ this->theMutex };
		return *this->theMap.find(theKey);
	}

	ObjectType& at(ObjectType&& theKey) noexcept {
		std::shared_lock theLock{ this->theMutex };
		return (ObjectType&)*this->theMap.find(theKey);
	}

	ObjectType& at(ObjectType& theKey) noexcept {
		std::shared_lock theLock{ this->theMutex };
		return (ObjectType&)*this->theMap.find(theKey);
	}

	auto begin() {
		std::unique_lock theLock{ this->theMutex };
		return this->theMap.begin();
	}

	auto end() {
		std::unique_lock theLock{ this->theMutex };
		return this->theMap.end();
	}

	bool contains(ObjectType& theKey) noexcept {
		std::unique_lock theLock{ this->theMutex };
		return this->theMap.contains(theKey);
	}

	void erase(ObjectType& theKey) {
		std::unique_lock theLock{ this->theMutex };
		if (this->theMap.contains(theKey)) {
			this->theMap.erase(theKey);
		}
	}

	size_t size() noexcept {
		std::unique_lock theLock{ this->theMutex };
		return this->theMap.size();
	}

protected:
	std::unordered_set<ObjectType> theMap{};
	std::shared_mutex theMutex{};
};


namespace DiscordCoreAPI {
	inline bool operator==(const DiscordCoreAPI::UserData& lhs, const DiscordCoreAPI::UserData& rhs) {
		if (lhs.id == rhs.id) {
			return true;
		}
		else {
			return false;
		}
	}
}
struct UserHash {
	std::size_t operator()(DiscordCoreAPI::UserData const& object) const noexcept {
		return object.id;
	}
};

template<> struct std::hash<DiscordCoreAPI::UserData> {
	std::size_t operator()(DiscordCoreAPI::UserData const& object) const noexcept {
		return object.id;
	}
};
namespace Test {

	void escapeCharacters(std::string& theString) {
		auto theSize = theString.size();
		for (int32_t x = 0; x < theSize; x++) {
			switch (static_cast<char>(theString[x])) {
			case 0x08: {
				theString[x] = '\\';
				theString.insert(theString.begin() + x + 1, 'b');
				theSize++;
				x++;
				break;
			}
			case 0x09: {
				theString[x] = '\\';
				theString.insert(theString.begin() + x + 1, 't');
				theSize++;
				x++;
				break;
			}
			case 0x0A: {
				theString[x] = '\\';
				theString.insert(theString.begin() + x + 1, 'n');
				theSize++;
				x++;
				break;
			}
			case 0x0B: {
				theString[x] = '\\';
				theString.insert(theString.begin() + x + 1, 'v');
				theSize++;
				x++;
				break;
			}
			case 0x0C: {
				theString[x] = '\\';
				theString.insert(theString.begin() + x + 1, 'f');
				theSize++;
				x++;
				break;
			}
			case 0x0D: {
				theString[x] = '\\';
				theString.insert(theString.begin() + x + 1, 'r');
				theSize++;
				x++;
				break;
			}
			case 0x22: {
				theString[x] = '\\';
				theString.insert(theString.begin() + x + 1, '"');
				theSize++;
				x++;
				break;
			}
			case 0x5C: {
				theString[x] = '\\';
				theString.insert(theString.begin() + x + 1, '\\');
				theSize++;
				x++;
				break;
			}
			case 0x27: {
				theString[x] = '\\';
				theString.insert(theString.begin() + x + 1, '\'');
				theSize++;
				x++;
				break;
			}
			default: {
				theString.insert(theString.begin() + x, theString[x]);
			}
			}
		}
	}
	
}



int32_t main() {
	try {
		ObjectCache<DiscordCoreAPI::UserData> theCache{};
		DiscordCoreAPI::UserData theData{};
		theData.id = 2312312312312;
		theCache.emplace(theData);
		if (theCache.contains(theData)) {
			std::cout << "WERE HERE THIS IS IT!" << std::endl;
		}
		auto theDataNew = theCache.readOnly(theData);
		std::string theStringNew{ "\n\n\n" };
		std::cout << theStringNew << std::endl;
		Test::escapeCharacters(theStringNew);
		std::cout << theStringNew << "THIS WAS THE STRING" << std::endl;
		 
		 
		 std::this_thread::sleep_for(std::chrono::seconds{ 3 });

	}
	 catch (...) { DiscordCoreAPI::reportException("main()"); };
	
	 return 0;
}

