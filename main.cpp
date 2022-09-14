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


class JsonStringGenerator {
public:
	JsonStringGenerator()noexcept;
	operator std::string();

	void appendInteger(uint64_t theInteger, const char* theName);

	void appendString(std::string theString, const char* theName);

	void appendBool(bool theBool, const char* theName);

	void appendFloat(double theFloat, const char* theName);

	void appendArray(const char* theName);

	void closeArray();

	void closeStruct();

	template<typename ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<std::string> ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<const char*> ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<int8_t>  ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<int16_t>  ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<int32_t>  ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<int64_t>  ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<uint8_t>  ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<uint16_t>  ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<uint32_t>  ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<uint64_t>  ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<float>  ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<double>  ElementType>
	void appendElement(ElementType theElement);

	template<std::same_as<bool>  ElementType>
	void appendElement(ElementType theElement);

	void appendStruct(const char* theName);

protected:
	std::string theString{};
	std::vector<bool> haveWeStarted{ false };
	std::vector<bool> haveWeStartedTheArray{ false };
};

JsonStringGenerator::JsonStringGenerator()noexcept{
	this->theString += "{";
}

JsonStringGenerator::operator std::string() {
	this->theString += "}";
	this->haveWeStarted.push_back(false);
	return this->theString;
}

void JsonStringGenerator::appendInteger(uint64_t theInteger, const char* theName = nullptr) {
	
	if (theName != nullptr) {
		if (*(this->haveWeStarted.end()-1)) {
			this->theString += ",";
		}
		if (!*(this->haveWeStarted.end()-1)) {
			*this->haveWeStarted.end() = true;
		}
		this->theString += "\"" + std::string{ theName } + "\":";
	}
	else {
		if (*(this->haveWeStartedTheArray.end()-1)) {
			this->theString += ",";
		}
		if (!*(this->haveWeStartedTheArray.end()-1)) {
			*(this->haveWeStartedTheArray.end() -1)= true;
		}
	}
	this->theString += std::to_string(theInteger);
}

void JsonStringGenerator::appendString(std::string theString, const char* theName = nullptr) {
	if (theName != nullptr) {
		if (*(this->haveWeStarted.end() - 1)) {
			this->theString += ",";
		}
		if (!*(this->haveWeStarted.end()-1)) {
			*this->haveWeStarted.end() = true;
		}
		this->theString += "\"" + std::string{ theName } + "\":";
	}
	else {
		if (*(this->haveWeStartedTheArray.end()-1)) {
			this->theString += ",";
		}
		if (!*(this->haveWeStartedTheArray.end()-1)) {
			*(this->haveWeStartedTheArray.end() -1)= true;
		}
	}
	this->theString += "\"" + theString + "\"";
}

void JsonStringGenerator::appendBool(bool theBool, const char* theName = nullptr) {
	if (theName != nullptr) {
		if (*(this->haveWeStarted.end() - 1)) {
			this->theString += ",";
		}
		if (!*(this->haveWeStarted.end() - 1)) {
			*this->haveWeStarted.end() = true;
		}
		this->theString += "\"" + std::string{ theName } + "\":";
	}
	else {
		if (*(this->haveWeStartedTheArray.end()-1)) {
			this->theString += ",";
		}
		if (!*(this->haveWeStartedTheArray.end()-1)) {
			*(this->haveWeStartedTheArray.end() -1)= true;
		}
	}
	std::stringstream theStream{};
	theStream << std::boolalpha << theBool;
	this->theString += theStream.str();
}

void JsonStringGenerator::appendFloat(double theFloat, const char* theName = nullptr) {
	if (theName != nullptr) {
		if (*(this->haveWeStarted.end()-1)) {
			this->theString += ",";
		}
		if (!*(this->haveWeStarted.end()-1)) {
			*this->haveWeStarted.end() = true;
		}
		this->theString += "\"" + std::string{ theName } + "\":";
	}
	else {
		if (*(this->haveWeStartedTheArray.end()-1)) {
			this->theString += ",";
		}
		if (!*(this->haveWeStartedTheArray.end()-1)) {
			*(this->haveWeStartedTheArray.end() -1)= true;
		}
	}
	this->theString += std::to_string(theFloat);
}

void JsonStringGenerator::appendArray(const char* theName) {
	if (*(this->haveWeStarted.end()-1)) {
		this->theString += ",";
	}
	if (this->haveWeStarted.size() == 0) {
		this->haveWeStarted.push_back(true);
	}
	this->theString += "\"" + std::string{ theName } + "\":[";
}

template<typename ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	
}

template<std::same_as<const char*>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendString(theElement);
}

template<std::same_as<std::string>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendString(theElement);
}

template<std::same_as<int8_t>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendInteger(theElement);
}

template<std::same_as<int16_t>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendInteger(theElement);
}

template<std::same_as<int32_t>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendInteger(theElement);
}

template<std::same_as<int64_t>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendInteger(theElement);
}

template<std::same_as<uint8_t>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendInteger(theElement);
}

template<std::same_as<uint16_t>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendInteger(theElement);
}

template<std::same_as<uint32_t>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendInteger(theElement);
}

template<std::same_as<uint64_t>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendInteger(theElement);
}

template<std::same_as<float>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendFloat(theElement);
}

template<std::same_as<double>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendFloat(theElement);
}

template<std::same_as<bool>  ElementType>
void JsonStringGenerator::appendElement(ElementType theElement) {
	this->appendBool(theElement);
}

void JsonStringGenerator::closeArray() {
	this->theString += "]";
	this->haveWeStartedTheArray.erase(this->haveWeStartedTheArray.end() - 1);
}

void JsonStringGenerator::appendStruct(const char* theName) {}

void JsonStringGenerator::closeStruct() {}

int32_t main() {
	try {
		JsonStringGenerator theGenerator{};
		theGenerator.appendBool(true, "theValue");
		theGenerator.appendString("THE VALUE", "theValue");
		theGenerator.appendInteger(23232, "theValue");
		theGenerator.appendFloat(23232.02f, "theValue");
		theGenerator.appendArray("TEST ARRAY");
		theGenerator.appendElement<double>(0.00f);
		theGenerator.appendElement(23);
		theGenerator.appendElement("TESTING THE ARRAY");
		theGenerator.closeArray();
		std::cout << static_cast<std::string>(theGenerator) << std::endl;
		 
		 
		 std::this_thread::sleep_for(std::chrono::seconds{ 3 });

	}
	 catch (...) { DiscordCoreAPI::reportException("main()"); };
	
	 return 0;
}

