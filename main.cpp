#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include "ErlPacker.hpp"
#include <stdint.h>
#include <set>

JsonValue::operator std::string() {
	return this->theValue;

}

JsonValue& JsonValue::operator=(JsonParseEvent theData) {
	this->theEvent = theData;
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

JsonValue& JsonValue::operator=(std::string theData) {
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

JsonValue& JsonValue::operator=(uint32_t theData){
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

template<typename JsonObjectType>
JsonValue& JsonValue::operator=(std::vector<JsonObjectType>theData) {
	for (auto& value : theData) {
		JsonValue theValue{ value };
		this->theValues.push_back(theValue);
	}
	return *this;
}

template<typename JsonObjectType>
JsonSerializer& JsonSerializer::operator=(std::vector<JsonObjectType>theData) {
	JsonValue theValue{};
	theValue.theEvent = JsonParseEvent::Array_Start;
	this->theValues.push_back(theValue);
	for (auto& value : theData) {
		theValue = value;
		this->theValues.push_back(theValue);
	}
	theValue.theEvent = JsonParseEvent::Array_End;
	this->theValues.push_back(theValue);
	return *this;
}

JsonSerializer& JsonSerializer::operator=(JsonParseEvent theData) {
	JsonValue theValue{};
	theValue.theEvent = theData;
	this->theValues.push_back(theValue);
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint8_t theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint16_t theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint32_t theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint64_t theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int8_t theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int16_t theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int32_t theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int64_t theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(float theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(double theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(std::string theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(const char* theData) {
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(bool theData){
	JsonValue theNewData{};
	theNewData = theData;
	if (this->isItFound) {
		this->theValues[this->currentPosition + 1] = theNewData;
	}
	else {
		this->theValues.push_back(theNewData);
	}
	this->isItFound = false;
	return *this;
}


JsonSerializer::operator std::string() {
	auto theString = this->getString();
	return theString.substr(0, theString.size() - 1);
}


JsonSerializer& JsonSerializer::operator[](const char* keyName) {
	for (size_t x = 0; x < this->theValues.size();++x) {
		if (this->theValues[x].theValue == keyName) {
			this->currentPosition = x;
			this->isItFound = true;
			break;
		}
	}
	if (!this->isItFound) {
		JsonValue theName{};
		theName.theEvent = JsonParseEvent::Key;
		theName.theValue = keyName;
		this->theValues.push_back(theName);
	}
	return *this;
}

std::string JsonSerializer::getString() {
	this->theState = JsonParserState::Starting_Object;
	std::string theString{ "{" };
	for (auto& value : this->theValues) {
		switch (value.theEvent) {
		case JsonParseEvent::Object_Start: {
			if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			this->theState = JsonParserState::Starting_Object;
			theString += "{";
			break;
		}
		case JsonParseEvent::Object_End: {
			theString += "}";
			if (theString[theString.size() - 2] == ',') {
				theString.erase(theString.begin() + theString.size() - 2);
			}
			break;
		}
		case JsonParseEvent::Array_Start: {
			if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			this->theState = JsonParserState::Starting_Array;
			theString += "[";
			break;
		}
		case JsonParseEvent::Array_End: {
			if (this->theState != JsonParserState::Starting_Object) {
				theString += ",";
			}
			this->theState = JsonParserState::Adding_Object_Elements;
			theString += "]";
			if (theString[theString.size() - 2] == ',') {
				theString.erase(theString.begin() + theString.size() - 2);
			}
			break;
		}
		case JsonParseEvent::Boolean: {
			if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += value;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Null_Value: {
			if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += value;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Number_Double: {
			if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += value;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Number_Float: {
			if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += value;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Number_Integer: {
			if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += value;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Number_Integer_Large: {
			if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += value;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Number_Integer_Small: {
			if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += value;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::String: {
			if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += "\"";
			theString += value;
			theString += "\"";
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Key: {
			if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
			theString += "\"";
			theString += value;
			theString += "\":"; 
			this->theState = JsonParserState::Starting_Object;
			break;
		}
		}
	}
	std::cout << "THE FINAL STRING REAL: " << theString << std::endl;
	theString += "}";
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
		std::vector<uint32_t> theVector{};
		theVector.push_back(244);
		theVector.push_back(243);
		
		theSerializer["TEST_VECTOR"] = theVector;
		theSerializer["d"] = JsonParseEvent::Object_Start;
		theSerializer["compress"] = this->compress;
		theSerializer["compress"] = true;
		theSerializer["intents"] = static_cast<uint32_t>(this->intents);
		theSerializer["large_threshold"] = static_cast<uint32_t>(250);
		theSerializer["presence"] = JsonParseEvent::Object_Start;
		theSerializer.addEvent(JsonParseEvent::Array_Start, "activities");

		for (auto& value : this->presence.activities) {
			theSerializer.addEvent(JsonParseEvent::Object_Start);
			if (value.url != "") {
				theSerializer.addEvent(std::string{ value.url }, "url");
			}
			theSerializer.addEvent(std::string{ value.name }, "name");
			theSerializer.addEvent(static_cast<uint64_t>(value.type), "type");
			theSerializer.addEvent(JsonParseEvent::Object_End);
		}
		theSerializer.addEvent(JsonParseEvent::Array_End);
		theSerializer.addEvent(this->presence.afk, "afk");
		if (this->presence.since == 0) { 
			theSerializer.addEvent(JsonParseEvent::Null_Value, "since");
		}
		else {
			theSerializer.addEvent(this->presence.since, "since");
		}
		
		theSerializer.addEvent(this->presence.status, "status");

		theSerializer.addEvent(JsonParseEvent::Object_End);
		theSerializer.addEvent(JsonParseEvent::Object_Start, "properties");
		theSerializer.addEvent("DiscordCoreAPI", "browser");
		theSerializer.addEvent("DiscordCoreAPI", "device");
#ifdef _WIN32
		theSerializer.addEvent("Windows", "os");
#else
		theSerializer.addEvent("Linux", "os");
#endif
		theSerializer.addEvent(JsonParseEvent::Object_End);
		theSerializer.addEvent(JsonParseEvent::Array_Start, "shard");
		theSerializer.addEvent(static_cast<uint8_t>(this->currentShard));
		theSerializer.addEvent(static_cast<uint8_t>(this->numberOfShards));
		theSerializer.addEvent(JsonParseEvent::Array_End);
		theSerializer.addEvent(this->botToken, "token");
		theSerializer.addEvent(JsonParseEvent::Object_End);
		theSerializer.addEvent(static_cast<uint8_t>(2), "op");
		theSerializer.addEvent(JsonParseEvent::Object_End);
		return theSerializer;
		}


    int32_t main() {
		try {
			WebSocketIdentifyData theData{};
			theData.largeThreshold = 250;
			theData.intents = (int64_t)DiscordCoreAPI::GatewayIntents::All_Intents;
			theData.botToken = "MTAwODE0Mzc1OTk0NDQ1MDE5MA.GwLNGD.PzfOojwke9KBovcbk4yV9hrjxmcMSNt7bbhnd8";
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
			
			std::string theString = theSerializer;
			std::cout << "THE FINAL STRING: 0101 " << theString << std::endl;
			theData.compress = true;
			auto theSerializer02 = theData.operator JsonSerializer();

			std::string theString02 = theSerializer02;
			std::cout << "THE FINAL STRING: 0101 " << theString02 << std::endl;
			nlohmann::json::parse(static_cast<std::string>(theSerializer));
			DiscordCoreAPI::DiscordCoreClient theClient{ DiscordCoreAPI::DiscordCoreClientConfig {} };
			std::unique_ptr<DiscordCoreInternal::WebSocketSSLShard> theShard{ std::make_unique<DiscordCoreInternal::WebSocketSSLShard>(&theClient,nullptr,0,nullptr) };
			theShard->connect("gateway.discord.gg", "443", true, true);
			auto theJsonData = nlohmann::json{ theData.operator JsonSerializer().getString() };
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
