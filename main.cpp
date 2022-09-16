#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include "ErlPacker.hpp"
#include <stdint.h>
#include <set>

enum class JsonParseEvent { Null_Value = 0, Object_Start = 1, Object_End = 2, Array_Start = 3, Array_End = 4, String = 5, Boolean = 6, Number_Integer = 7, Number_Unsigned = 8, Number_Float = 9, Number_Double = 10 };
enum class JsonParserState { Adding_Object_Elements = 0, Adding_Array_Elements = 1 };
struct JsonValue {
	JsonParseEvent theEvent{};
	std::string theValue{};
	std::string theKey{};
};
class JsonSerializer {
public:
	JsonSerializer()noexcept = default;
	std::string getString();

	size_t parseForward(JsonParseEvent theEvent);

	JsonValue getEvent();

	size_t getObjectSize(const char* theName = nullptr);

	size_t getArraySize(const char* theName = nullptr);

	template<std::same_as<uint64_t> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
		std::string theString = std::to_string(theData);
		this->theValues.push_back({ .theEvent = JsonParseEvent::Number_Unsigned,.theValue = theString,.theKey = keyName });
	}

	template<std::same_as<int64_t> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
		std::string theString = std::to_string(theData);
		this->theValues.push_back({ .theEvent = JsonParseEvent::Number_Integer,.theValue = theString,.theKey = keyName });
	}

	template<std::same_as<std::string> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
		this->theValues.push_back({ .theEvent = JsonParseEvent::String,.theValue = theData,.theKey = keyName });
	}

	template<std::same_as<bool> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
		std::stringstream theStream{};
		theStream << std::boolalpha << theData;
		this->theValues.push_back({ .theEvent = JsonParseEvent::String,.theValue = theStream.str(),.theKey = keyName });
	}

	template<std::same_as<float> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
		std::string theString = std::to_string(theData);
		this->theValues.push_back({ .theEvent = JsonParseEvent::Number_Integer,.theValue = theString,.theKey = keyName });
	}

	template<std::same_as<double> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
		std::string theString = std::to_string(theData);
		this->theValues.push_back({ .theEvent = JsonParseEvent::Number_Integer,.theValue = theString,.theKey = keyName });
	}

	template<std::same_as<const char*> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
		this->theValues.push_back({ .theEvent = JsonParseEvent::String,.theValue = theData,.theKey = keyName });
	}

	template<std::same_as<JsonParseEvent> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
		this->theValues.push_back({ .theEvent = theData,.theValue = "",.theKey = keyName });
	}

protected:
	std::vector<JsonValue> theValues{};
	bool isThisTheFirstElement{ true };
	JsonParserState theState{};
	size_t currentPosition{};
};

size_t JsonSerializer::parseForward(JsonParseEvent theEvent) {
	for (uint32_t x = 0; x < this->theValues.size(); ++x) {
		if (this->theValues[x].theEvent == theEvent) {
			return x;
		}
	}
}

JsonValue JsonSerializer::getEvent() {
	JsonValue theResult = this->theValues.front();
	this->theValues.erase(this->theValues.begin());
	return theResult;
}

size_t JsonSerializer::getArraySize(const char* theName) {
	size_t theSize{};
	if (theName != nullptr) {
		for (uint32_t x = 0; x < this->theValues.size(); ++x) {
			if (this->theValues[x].theKey == theName) {
				this->currentPosition = x;
				break;
			}
		}
	}
	bool areWeCounting{ false };
	bool areWeCountingObjects{ false };
	for (uint32_t x = this->currentPosition; x < this->theValues.size(); ++x) {
		if (this->theValues[x].theEvent == JsonParseEvent::Array_Start) {
			areWeCounting = true;
		}
		if (areWeCounting && this->theValues[x].theEvent == JsonParseEvent::Array_End) {
			areWeCounting = false;
			this->currentPosition = x;
			break;
		}
		if (areWeCounting && this->theValues[x].theEvent == JsonParseEvent::Object_Start) {
			areWeCountingObjects = true;
			theSize++;
		}
		if (areWeCounting && !areWeCountingObjects) {
			if (this->theValues[x].theEvent == JsonParseEvent::Boolean || this->theValues[x].theEvent == JsonParseEvent::Number_Double ||
				this->theValues[x].theEvent == JsonParseEvent::Number_Float || this->theValues[x].theEvent == JsonParseEvent::Number_Integer ||
				this->theValues[x].theEvent == JsonParseEvent::Number_Unsigned || this->theValues[x].theEvent == JsonParseEvent::String || this->theValues[x].theEvent == JsonParseEvent::Null_Value) {
				theSize++;
			}
		}
		this->currentPosition = x;
	}
	return theSize;
}

size_t JsonSerializer::getObjectSize(const char* theName) {
	size_t theSize{};
	if (theName != nullptr) {
		for (uint32_t x = 0; x < this->theValues.size(); ++x) {
			if (this->theValues[x].theKey == theName) {
				this->currentPosition = x;
				break;
			}
		}
	}
	int32_t currentDepth{ 0 };
	bool areWeInsideASubObject{ false };
	for (uint32_t x = this->currentPosition; x < this->theValues.size(); ++x) {
		if (this->theValues[x].theEvent == JsonParseEvent::Object_Start) {
			std::cout << "THE CURRENT INDEX: " << x << ", THE EVENT: " << (int)this->theValues[x].theEvent << std::endl;
			if (!areWeInsideASubObject) {
				theSize++;
				areWeInsideASubObject = true;
			}
			else {
				currentDepth++;
			}
		}
		if (this->theValues[x].theEvent == JsonParseEvent::Object_End) {			
			if (areWeInsideASubObject && !currentDepth) {
				areWeInsideASubObject = false;
			}
			else {
				currentDepth--;
			}
			this->currentPosition = x;
		}
		this->currentPosition = x;
	}
	return theSize;
}
std::string JsonSerializer::getString() {
	std::string theString{"{"};
	for (auto& value : this->theValues) {
		switch (value.theEvent) {
		case JsonParseEvent::Object_Start: {
			if (!this->isThisTheFirstElement) {
				theString += ",";
			}
			this->isThisTheFirstElement = true;
			if (this->theState == JsonParserState::Adding_Object_Elements) {
				theString += "\"" + value.theKey + "\":{";
			}
			else {
				this->theState = JsonParserState::Adding_Object_Elements;
				theString += "{";
			}
			break;
		}
		case JsonParseEvent::Object_End: {
			theString += "}";
			if (this->isThisTheFirstElement) {
				this->isThisTheFirstElement = false;
			}
			if (theString[theString.size() - 2] == ',') {
				theString.erase(theString.begin() + theString.size() - 2);
			}
			break;
		}
		case JsonParseEvent::Array_Start: {
			if (!this->isThisTheFirstElement) {
				theString += ",";
			}
			this->isThisTheFirstElement = true;
			this->theState = JsonParserState::Adding_Array_Elements;
			theString += "\"" + value.theKey + "\":[";
			break;
		}
		case JsonParseEvent::Array_End: {
			if (!this->isThisTheFirstElement) {
				theString += ",";
			}
			this->theState = JsonParserState::Adding_Object_Elements;
			theString += "]";
			if (theString[theString.size() - 2] == ',') {
				theString.erase(theString.begin() + theString.size() - 2);
			}
			if (this->isThisTheFirstElement) {
				this->isThisTheFirstElement = false;
			}
			break;
		}
		case JsonParseEvent::Boolean: {
			if (!this->isThisTheFirstElement) {
				theString += ",";
			}
			if (this->theState == JsonParserState::Adding_Object_Elements) {
				theString += "\"" + value.theKey + "\":" + value.theValue;
			}
			else {
				theString += value.theValue;
			}
			if (this->isThisTheFirstElement) {
				this->isThisTheFirstElement = false;
			}
			break;
		}
		case JsonParseEvent::Null_Value: {
			if (!this->isThisTheFirstElement) {
				theString += ",";
			}
			if (this->theState == JsonParserState::Adding_Object_Elements) {
				theString += "\"" + value.theKey + "\":" + value.theValue;
			}
			else {
				theString += value.theValue;
			}
			if (this->isThisTheFirstElement) {
				this->isThisTheFirstElement = false;
			}
			break;
		}
		case JsonParseEvent::Number_Double: {
			if (!this->isThisTheFirstElement) {
				theString += ",";
			}
			if (this->theState == JsonParserState::Adding_Object_Elements) {
				theString += "\"" + value.theKey + "\":" + value.theValue;
			}
			else {
				theString += value.theValue;
			}
			if (this->isThisTheFirstElement) {
				this->isThisTheFirstElement = false;
			}
			break;
		}
		case JsonParseEvent::Number_Float: {
			if (!this->isThisTheFirstElement) {
				theString += ",";
			}
			if (this->theState == JsonParserState::Adding_Object_Elements) {
				theString += "\"" + value.theKey + "\":" + value.theValue;
			}
			else {
				theString += value.theValue;
			}
			if (this->isThisTheFirstElement) {
				this->isThisTheFirstElement = false;
			}
			break;
		}
		case JsonParseEvent::Number_Integer: {
			if (!this->isThisTheFirstElement) {
				theString += ",";
			}
			if (this->theState == JsonParserState::Adding_Object_Elements) {
				theString += "\"" + value.theKey + "\":" + value.theValue;
			}
			else {
				theString += value.theValue;
			}
			if (this->isThisTheFirstElement) {
				this->isThisTheFirstElement = false;
			}
			break;
		}
		case JsonParseEvent::Number_Unsigned: {
			if (!this->isThisTheFirstElement) {
				theString += ",";
			}
			if (this->theState == JsonParserState::Adding_Object_Elements) {
				theString += "\"" + value.theKey + "\":" + value.theValue;
			}
			else {
				theString += value.theValue;
			}
			if (this->isThisTheFirstElement) {
				this->isThisTheFirstElement = false;
			}
			break;
		}
		case JsonParseEvent::String: {
			if (!this->isThisTheFirstElement) {
				theString += ",";
			}
			if (this->theState == JsonParserState::Adding_Object_Elements) {
				theString += "\"" + value.theKey + "\":\"" + value.theValue + "\"";
			}
			else {
				theString += "\"" + value.theValue + "\"";
			}
			
			if (this->isThisTheFirstElement) {
				this->isThisTheFirstElement = false;
			}
			break;
		}
		}
		
	}
	theString += "}";
	return theString;
}
    struct WebSocketIdentifyData {
        DiscordCoreInternal::UpdatePresenceData presence{};
        std::int32_t largeThreshold{};
        std::int32_t numberOfShards{};
        std::int32_t currentShard{};
        std::string botToken{};
        int64_t intents{};

        operator JsonSerializer();
    };

    WebSocketIdentifyData::operator JsonSerializer() {
		JsonSerializer theSerializer{};
		theSerializer.addEvent("op", static_cast<uint64_t>(2));
		theSerializer.addEvent("d", JsonParseEvent::Object_Start);
		theSerializer.addEvent("token", this->botToken);
		
		theSerializer.addEvent("properties", JsonParseEvent::Object_Start);
		theSerializer.addEvent("browser", "DiscordCoreAPI");
		theSerializer.addEvent("device", "DiscordCoreAPI");
#ifdef _WIN32
		theSerializer.addEvent("os", "Windows");
#else
		theSerializer.addEvent("os", "Linux");
#endif
		theSerializer.addEvent("properties", JsonParseEvent::Object_End);
		theSerializer.addEvent("shard", JsonParseEvent::Array_Start);
		theSerializer.addEvent("", static_cast<uint64_t>(this->currentShard));
		theSerializer.addEvent("", static_cast<uint64_t>(this->numberOfShards));
		theSerializer.addEvent("shard", JsonParseEvent::Array_End);
		theSerializer.addEvent("large_threshold", static_cast<uint64_t>(this->largeThreshold));
		theSerializer.addEvent("intents", static_cast<uint64_t>(this->intents));
		theSerializer.addEvent("compress", false);
		theSerializer.addEvent("presence", JsonParseEvent::Object_Start);
		theSerializer.addEvent("status", this->presence.status);
		theSerializer.addEvent("afk", this->presence.afk);
		theSerializer.addEvent("since", this->presence.since);
		theSerializer.addEvent("activities", JsonParseEvent::Array_Start);
		std::vector<uint64_t>theVector{};
		for (uint32_t x = 0; x < 112; ++x) {
			theVector.push_back(x);
		}
		DiscordCoreAPI::ActivityData theDataNew{};
		theDataNew.name = "/help for my commands";
		theDataNew.url = "";
		this->presence.activities.push_back(theDataNew);
		for (auto& value : this->presence.activities) {
			theSerializer.addEvent("", JsonParseEvent::Object_Start);
			theSerializer.addEvent("url", std::string{ value.url });
			theSerializer.addEvent("name", std::string{ value.name });
			theSerializer.addEvent("type", static_cast<uint64_t>(value.type));
			theSerializer.addEvent("", JsonParseEvent::Object_End);
		}
		theSerializer.addEvent("activities", JsonParseEvent::Array_End);
		theSerializer.addEvent("presence", JsonParseEvent::Object_End);
		theSerializer.addEvent("d", JsonParseEvent::Object_End);

		return theSerializer;
    }

    int32_t main() {
        try {
			WebSocketIdentifyData theData{};
			theData.botToken = "TESTING VALUE 23123123123";
			auto serializer = theData.operator JsonSerializer();
			std::cout << "THE STRING FINAL: " << serializer.getObjectSize("shard") << std::endl;
			std::cout << "THE STRING FINAL: " << serializer.getObjectSize("d") << std::endl;
			std::cout << "THE STRING FINAL: " << serializer.getObjectSize("d") << std::endl;
			std::cout << "THE STRING FINAL: " << serializer.getObjectSize("d") << std::endl;
			std::cout << "THE STRING FINAL: " << serializer.getArraySize("theVector") << std::endl;
			std::cout << "THE STRING FINAL: " << serializer.getArraySize("theVector") << std::endl;
			std::cout << "THE STRING FINAL: " << serializer.getArraySize("theVector") << std::endl;
			std::cout << "THE STRING FINAL: " << serializer.getArraySize("theVector") << std::endl;
			
			std::cout << "THE STRING FINAL: " << serializer.getString() << std::endl;
			std::cout << "THE STRING FINAL: " << serializer.getString() << std::endl;
			

            std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        }
        catch (...) { DiscordCoreAPI::reportException("main()"); };

        return 0;
    }
