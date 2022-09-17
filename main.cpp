#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include "ErlPacker.hpp"
#include <stdint.h>
#include <set>

JsonValue::operator std::string() {
	return this->theValue;

}

JsonValue& JsonValue::operator=(uint32_t theData){
	this->theEvent = JsonParseEvent::Number_Integer;
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

JsonSerializer& JsonSerializer::operator=(bool theData){
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Boolean;
	std::string theString{};
	if (theData) {
		theString = "true";
	}
	else {
		theString = "false";
	}
	theNewData.theValue = theString;
	this->theValues.push_back(theNewData);
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint32_t theData){
	JsonValue theNewData{};
	theNewData.theEvent = JsonParseEvent::Boolean;
	std::string theString{ std::to_string(theData) };
	theNewData.theValue = theString;
	this->theValues.push_back(theNewData);
	return *this;
}

JsonSerializer::operator std::string() {
	auto theString = this->getString();
	return theString.substr(0, theString.size() - 1);
}


JsonSerializer& JsonSerializer::operator[](const char* keyName) {
	JsonValue theName{};
	theName.theEvent = JsonParseEvent::Key;
	theName.theValue = keyName;
	this->theValues.push_back(theName);
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
		theSerializer["compress"] = false;
		theSerializer["intents"] = static_cast<uint32_t>(this->intents);
		theSerializer.addEvent(static_cast<uint32_t>(250), "large_threshold");
		theSerializer.addEvent(JsonParseEvent::Object_Start, "presence");
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
			std::cout << "THE FINAL STRING: " << thePacker.parseJsonToEtf(theString) << std::endl;
			nlohmann::json::parse(static_cast<std::string>(theSerializer));
			DiscordCoreAPI::DiscordCoreClient theClient{ DiscordCoreAPI::DiscordCoreClientConfig {} };
			std::unique_ptr<DiscordCoreInternal::WebSocketSSLShard> theShard{ std::make_unique<DiscordCoreInternal::WebSocketSSLShard>(&theClient,nullptr,0,nullptr) };
			theShard->connect("gateway.discord.gg", "443", true, true);
			auto theJsonData = nlohmann::json{ theData.operator JsonSerializer().getString() };
			std::string theString02{};
			theShard->stringifyJsonData(theJsonData, theString02, DiscordCoreInternal::WebSocketOpCode::Op_Text);
			theShard->sendMessage(theString02, false);
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
