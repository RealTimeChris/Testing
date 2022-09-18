#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include "ErlPacker.hpp"
#include <stdint.h>
#include <set>

JsonValue::operator std::string() {
	std::cout << "THE EVENT: " << static_cast<uint32_t>(this->theEvent) << ", THE VALUE: " << this->theValue << std::endl;
	std::string theString{ };
	for (auto& [key, value] : this->theValues) {
		std::cout << "THE EVENT: " << static_cast<uint32_t>(value.theEvent) << ", THE VALUE: " << value.theValue << std::endl;
		switch (value.theEvent) {
		case JsonParseEvent::Object_Start: {
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			value.theState = JsonParserState::Starting_Object;
			theString += "{";
			theString += static_cast<std::string>(value);
			break;
		}
		case JsonParseEvent::Object_End: {
			theString += "}";
			if (theString.size() > 2 && theString[theString.size() - 2] == ',') {
				theString.erase(theString.begin() + theString.size() - 2);
			}
			break;
		}
		case JsonParseEvent::Array_Start: {
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			value.theState = JsonParserState::Starting_Array;
			theString += "[";
			theString += static_cast<std::string>(value);
			break;

		}case JsonParseEvent::Array_End: {
			if (value.theState != JsonParserState::Starting_Object) {
				theString += ",";
			}
			value.theState = JsonParserState::Adding_Object_Elements;
			theString += "]";
			if (theString[theString.size() - 2] == ',') {
				theString.erase(theString.begin() + theString.size() - 2);
			}
			break;
		}
		case JsonParseEvent::Boolean: {
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += static_cast<std::string>(value.theValue);
			if (value.theState == JsonParserState::Starting_Object) {
				value.theState = JsonParserState::Adding_Object_Elements;
			}
			if (value.theState == JsonParserState::Starting_Array) {
				value.theState = JsonParserState::Adding_Array_Elements;
			}
			return theString;
		}
		case JsonParseEvent::Null_Value: {
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += static_cast<std::string>(value.theValue);
			if (value.theState == JsonParserState::Starting_Object) {
				value.theState = JsonParserState::Adding_Object_Elements;
			}
			if (value.theState == JsonParserState::Starting_Array) {
				value.theState = JsonParserState::Adding_Array_Elements;
			}
			return theString;
		}
		case JsonParseEvent::Number_Double: {
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += static_cast<std::string>(value.theValue);
			if (value.theState == JsonParserState::Starting_Object) {
				value.theState = JsonParserState::Adding_Object_Elements;
			}
			if (value.theState == JsonParserState::Starting_Array) {
				value.theState = JsonParserState::Adding_Array_Elements;
			}
			return theString;
		}
		case JsonParseEvent::Number_Float: {
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += static_cast<std::string>(value.theValue);
			if (value.theState == JsonParserState::Starting_Object) {
				value.theState = JsonParserState::Adding_Object_Elements;
			}
			if (value.theState == JsonParserState::Starting_Array) {
				value.theState = JsonParserState::Adding_Array_Elements;
			}
			return theString;
		}
		case JsonParseEvent::Number_Integer: {
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += static_cast<std::string>(value.theValue);
			if (value.theState == JsonParserState::Starting_Object) {
				value.theState = JsonParserState::Adding_Object_Elements;
			}
			if (value.theState == JsonParserState::Starting_Array) {
				value.theState = JsonParserState::Adding_Array_Elements;
			}
			return theString;
		}
		case JsonParseEvent::Number_Integer_Large: {
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += static_cast<std::string>(value.theValue);
			if (value.theState == JsonParserState::Starting_Object) {
				value.theState = JsonParserState::Adding_Object_Elements;
			}
			if (value.theState == JsonParserState::Starting_Array) {
				value.theState = JsonParserState::Adding_Array_Elements;
			}
			return theString;
		}
		case JsonParseEvent::Number_Integer_Small: {
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += static_cast<std::string>(value.theValue);
			if (value.theState == JsonParserState::Starting_Object) {
				value.theState = JsonParserState::Adding_Object_Elements;
			}
			if (value.theState == JsonParserState::Starting_Array) {
				value.theState = JsonParserState::Adding_Array_Elements;
			}
			return theString;
		}
		case JsonParseEvent::String: {
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += "\"";
			theString += static_cast<std::string>(value.theValue);
			theString += "\"";
			if (value.theState == JsonParserState::Starting_Object) {
				value.theState = JsonParserState::Adding_Object_Elements;
			}
			if (value.theState == JsonParserState::Starting_Array) {
				value.theState = JsonParserState::Adding_Array_Elements;
			}
			return theString;
		}
		case JsonParseEvent::Key: {
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += "\"";
			theString += static_cast<std::string>(value.theValue);
			theString += "\":";
			value.theState = JsonParserState::Starting_Object;
			return theString;
		}
		}

	}
	if (theString[theString.size() - 1] == '0') {
		theString = theString.substr(0, theString.size() - 1);
	}
	return theString;
}

JsonValue& JsonValue::operator=(const JsonValue& theData){
	this->theEvent = theData.theEvent;
	this->theState = theData.theState;
	this->theValue = theData.theValue;
	for (auto& [key, value] : theData.theValues) {
		this->theValues[key].theEvent = value.theEvent;
		this->theValues[key].theState = value.theState;
		this->theValues[key].theValue = value.theValue;
		this->theValues[key].theValues = value.theValues;
	}
	return *this;
}

JsonValue::JsonValue(const JsonValue& theData) {
	*this = theData;
}

JsonValue& JsonValue::operator=(JsonValue&& theData){
	this->theEvent = std::move(theData.theEvent);
	this->theState = std::move(theData.theState);
	this->theValue = std::move(theData.theValue);
	for (auto& [key, value] : theData.theValues) {
		this->theValues[key] = std::move(value);
	}
	return *this;
}

JsonValue& JsonValue::operator=(JsonParseEvent theData) {
	this->theEvent = theData;
	std::cout << "THE EVENT: " << ", THE VALUE: " << this->theValue << std::endl;
	if (static_cast<uint16_t>(theData) & static_cast<uint16_t>(JsonParseEvent::Null_Value)) {
		this->theValue = "null";
	}
	return *this;
}

JsonValue& JsonValue::operator=(bool theData) {
	this->theEvent = JsonParseEvent::Boolean;
	if (theData) {
		this->theValue = "true";
	}
	else {
		this->theValue = "false";
	}
	return *this;
}

JsonValue& JsonValue::operator=(const char* theData) {
	this->theEvent = JsonParseEvent::String;
	this->theValue = theData;
	return *this;
}

JsonValue& JsonValue::operator=(const std::string&& theData) {
	this->theEvent = JsonParseEvent::String;
	this->theValue = theData;
	return *this;
}

JsonValue& JsonValue::operator=(const std::string& theData) {
	this->theEvent = JsonParseEvent::String;
	this->theValue = theData;
	return *this;
}

JsonValue& JsonValue::operator=(std::string&& theData) {
	this->theEvent = JsonParseEvent::String;
	this->theValue = std::move(theData);
	return *this;
}

JsonValue& JsonValue::operator=(std::string& theData) {
	this->theEvent = JsonParseEvent::String;
	this->theValue = theData;
	return *this;
}

JsonValue& JsonValue::operator=(float theData) {
	this->theEvent = JsonParseEvent::Number_Float;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonValue& JsonValue::operator=(double theData) {
	this->theEvent = JsonParseEvent::Number_Double;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonValue& JsonValue::operator=(int64_t theData) {
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonValue& JsonValue::operator=(int32_t theData) {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonValue& JsonValue::operator=(int16_t theData) {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonValue& JsonValue::operator=(int8_t theData) {
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonValue& JsonValue::operator=(uint64_t theData) {
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonValue& JsonValue::operator=(uint32_t theData) {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonValue& JsonValue::operator=(uint16_t theData) {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonValue& JsonValue::operator=(uint8_t theData) {
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonValue& JsonValue::operator[](const char* keyName) {
	if (this->theValues.contains(keyName)) {
		return this->theValues[keyName];
	}
	else {
		this->theValues[keyName] = JsonParseEvent::Key;
	}
	return this->theValues[keyName];
}

JsonSerializer& JsonSerializer::operator=(const JsonSerializer& other) noexcept {
	this->currentPosition = other.currentPosition;
	this->isItFound = other.isItFound;
	this->theState = other.theState;
	for (auto& [key, value] : other.theValues) {
		this->theValues[key] = value;
	}
	return *this;
}

JsonSerializer::JsonSerializer(const JsonSerializer& other) noexcept {
	*this = other;
}

JsonSerializer& JsonSerializer::operator=(JsonSerializer&& other) noexcept {
	this->currentPosition = std::move(other.currentPosition);
	this->isItFound = std::move(other.isItFound);
	this->theState = std::move(other.theState);
	for (auto& [key, value] : other.theValues) {
		this->theValues[key] = std::move(value);
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(JsonParseEvent theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theNewData;
	theNewData.theValues[this->latestKey].theEvent = theData;
	this->theValues[this->latestKey + ""] = theNewData;
	std::cout << "THE EVENT: " << ", THE VALUE: " << theNewData.theValue << std::endl;
	this->theValues[this->latestKey + ""].theEvent = theData;
	this->theValues[this->latestKey] = theNewData;
	this->theValues[this->latestKey].theEvent = theData;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint8_t theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theData;
	this->theValues[this->latestKey] = theNewData;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint16_t theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theData;
	this->theValues[this->latestKey] = theNewData;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint32_t theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theData;
	this->theValues[this->latestKey] = theNewData;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint64_t theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theData;
	this->theValues[this->latestKey] = theNewData;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int8_t theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theData;
	this->theValues[this->latestKey] = theNewData;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int16_t theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theData;
	this->theValues[this->latestKey] = theNewData;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int32_t theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theData;
	this->theValues[this->latestKey] = theNewData;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int64_t theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theData;
	this->theValues[this->latestKey] = theNewData;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(float theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theNewData;
	theNewData.theValues[this->latestKey].theEvent = JsonParseEvent::Number_Float;
	this->theValues[this->latestKey] = theNewData;
	this->theValues[this->latestKey].theEvent = JsonParseEvent::Number_Float;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(double theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theNewData;
	theNewData.theValues[this->latestKey].theEvent = JsonParseEvent::Number_Double;
	this->theValues[this->latestKey] = theNewData;
	this->theValues[this->latestKey].theEvent = JsonParseEvent::Number_Double;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(std::string&& theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theNewData;
	theNewData.theValues[this->latestKey].theEvent = JsonParseEvent::String;
	this->theValues[this->latestKey] = theNewData;
	this->theValues[this->latestKey].theEvent = JsonParseEvent::String;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(std::string& theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theNewData;
	theNewData.theValues[this->latestKey].theEvent = JsonParseEvent::String;
	this->theValues[this->latestKey] = theNewData;
	this->theValues[this->latestKey].theEvent = JsonParseEvent::String;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(const char* theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theNewData;
	theNewData.theValues[this->latestKey].theEvent = JsonParseEvent::String;
	this->theValues[this->latestKey] = theNewData;
	this->theValues[this->latestKey].theEvent = JsonParseEvent::String;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(bool theData) noexcept {
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Key;
	theNewData.theValue = this->latestKey;
	theNewData.theValues[this->latestKey] = theNewData;
	theNewData.theValues[this->latestKey].theEvent = JsonParseEvent::Boolean;
	this->theValues[this->latestKey] = theNewData;
	this->theValues[this->latestKey].theEvent = JsonParseEvent::Boolean;
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator[](const char* keyName) {
	if (static_cast<uint32_t>(keyName[0]) != 0) {
		this->latestKey = keyName;
		for (auto& [key, value] : this->theValues) {
			if (value.theValue == keyName) {
				return *this;
			}
			else if (value.theValues.contains(keyName)) {
				return *this;
			}
		}
	}
	if (!this->isItFound) {
		if (static_cast<uint32_t>(keyName[0]) != 0) {
			JsonValue theName{};
			theName.theEvent = JsonParseEvent::Key;
			theName.theValue = keyName;
			this->theValues[keyName] = theName;
		}
	}
	return *this;
}

std::string JsonSerializer::toString() {
	std::string theString{};
	for (auto& [key, value] : this->theValues) {
		theString.append(static_cast<std::string>(value));
	}
	return theString;
}

    struct WebSocketIdentifyData {
		DiscordCoreInternal::UpdatePresenceData presence{};
		std::int32_t largeThreshold{ 250 };
		std::int32_t numberOfShards{};
		std::int32_t currentShard{};
		bool compress{ false };
		std::string botToken{};
		int64_t intents{};

        operator JsonSerializer();
    };

	WebSocketIdentifyData::operator JsonSerializer() {
		JsonSerializer theSerializer{};
		theSerializer["d"] = JsonParseEvent::Object_Start;
		
		theSerializer["compress"] = this->compress;
		theSerializer["compress"] = true;
		theSerializer["intents"] = static_cast<uint32_t>(this->intents);
		theSerializer["large_threshold"] = static_cast<uint32_t>(250);
		theSerializer["d"] = JsonParseEvent::Object_End;
		theSerializer["presence"] = JsonParseEvent::Object_Start;
		theSerializer["activities"] = JsonParseEvent::Array_Start;
		/*

		for (auto& value : this->presence.activities) {
			theSerializer[""] = JsonParseEvent::Object_Start;
			if (value.url != "") {
				theSerializer["url"] = std::string{ value.url };
			}
			theSerializer["name"] = std::string{ value.name };
			theSerializer["type"] = static_cast<uint8_t>(value.type);
			theSerializer[""] = JsonParseEvent::Object_End;
		}
		theSerializer[""] = JsonParseEvent::Array_End;
		theSerializer["afk"] = this->presence.afk;
		if (this->presence.since == 0) { 
			theSerializer["since"] = JsonParseEvent::Null_Value;
		}
		else {
			theSerializer["since"] = this->presence.since;
		}
		
		theSerializer["status"] = this->presence.status;

		theSerializer[""] = JsonParseEvent::Object_End;
		theSerializer["properties"] = JsonParseEvent::Object_Start;
		theSerializer["browser"] = "DiscordCoreAPI";
		theSerializer["device"] = "DiscordCoreAPI";
#ifdef _WIN32
		theSerializer["os"] = "Windows";
#else
		theSerializer["os"] = "Linux";
#endif
		theSerializer[""] = JsonParseEvent::Object_End;
		theSerializer["shard"] = JsonParseEvent::Array_Start;
		theSerializer[""] = static_cast<uint8_t>(this->currentShard);
		theSerializer[""] = static_cast<uint8_t>(this->numberOfShards);
		theSerializer[""] = JsonParseEvent::Array_End;
		theSerializer["token"] = this->botToken;
		theSerializer[""] = JsonParseEvent::Object_End;
		theSerializer["op"] = static_cast<uint8_t>(2);
		theSerializer[""] = JsonParseEvent::Object_End;
		*/
		return theSerializer;
	}


    int32_t main() {
		try {
			WebSocketIdentifyData theData{};
			theData.largeThreshold = 250;
			theData.intents = (int64_t)DiscordCoreAPI::GatewayIntents::All_Intents;
			theData.botToken = "MTAwODE0Mzc1OTk0NDQ1MDE5MA.Gms8fI.UG7Xlvs0NcAk4IzoA6Gfb-XH4OCR5R6coCAS2E";
			theData.numberOfShards = 3;
			ErlPacker thePacker{};
			std::vector<DiscordCoreAPI::ActivityData> activities{};
			DiscordCoreAPI::ActivityData activity{};
			activity.name = "/help for my commands!";
			activity.type = DiscordCoreAPI::ActivityType::Game;
			activities.push_back(activity);
			theData.presence.activities = activities;
			theData.presence.afk = false;
			theData.presence.since = 0;
			theData.presence.status = "online";
			auto theSerializer = theData.operator JsonSerializer();
			
			std::string theString = theSerializer.toString();
			theData.compress = true;
			auto theSerializer02 = theData.operator JsonSerializer();

			std::string theString02 = theSerializer02.toString();
			std::cout << "THE FINAL STRING: 0101 " << theString02 << std::endl;
			auto theResult = nlohmann::json::parse(static_cast<std::string>(theSerializer.toString()));
			DiscordCoreAPI::DiscordCoreClient theClient{ DiscordCoreAPI::DiscordCoreClientConfig {} };
			std::unique_ptr<DiscordCoreInternal::WebSocketSSLShard> theShard{ std::make_unique<DiscordCoreInternal::WebSocketSSLShard>(&theClient,nullptr,0,nullptr) };
			theShard->connect("gateway.discord.gg", "443", true, true);
			auto theJsonData = nlohmann::json{ static_cast<std::string>(theData.operator JsonSerializer().toString()) };
			std::string theString03{};
			theShard->stringifyJsonData(theJsonData, theString03, DiscordCoreInternal::WebSocketOpCode::Op_Text);
			theShard->sendMessage(theString03, false);
			theShard->processIO(100000);
			theShard->processIO(100000);
			theShard->processIO(100000);
			theShard->processIO(100000);
			theShard->handleBuffer(theShard.get());
			

            std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        }
        catch (...) { DiscordCoreAPI::reportException("main()"); };

        return 0;
    }
