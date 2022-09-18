#include <discordcoreapi/Index.hpp>



#include <simdjson.h>
#include "ErlPacker.hpp"



#include <stdint.h>
#include <set>
#include <nlohmann/json.hpp>

JsonScalarValue& JsonScalarValue::operator=(const JsonScalarValue& other){
	this->theKey = other.theKey;
	this->theValue = other.theValue;
	return *this;
}
JsonScalarValue::JsonScalarValue(const JsonScalarValue&other) {
	*this = other;
}

JsonScalarValue::operator std::string() {
	return this->theValue;

}

JsonScalarValue& JsonScalarValue::operator=(bool theData) {
	std::string theString{};
	if (theData) {
		theString = "true";
	}
	else {
		theString = "false";
	}
	JsonScalarValue theValue{};
	theValue.theValue = theString;
	*this = theValue;
	std::cout << "THE BOOLEAN VALUE: " << this->theValue << std::endl;
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(const char* theData) {
	this->theValue = theData;
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(std::string theData) {
	this->theValue = theData;
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(float theData) {
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(double theData) {
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(int64_t theData) {
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(int32_t theData) {
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(int16_t theData) {
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(int8_t theData) {
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(uint64_t theData) {
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(uint32_t theData){
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(uint16_t theData) {
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(uint8_t theData) {
	this->theValue = std::to_string(theData);
	return *this;
}
template<typename JsonObjectType>
JsonObject& JsonObject::operator=(std::vector<JsonObjectType>& theData){
	return *this;
}
JsonObject& JsonObject::operator=(int8_t theData) {
	return *this;
}
JsonObject& JsonObject::operator=(int16_t theData) {
	return *this;
}
JsonObject& JsonObject::operator=(int32_t theData) {
	return *this;
}
JsonObject& JsonObject::operator=(int64_t theData) {
	return *this;
}
JsonObject& JsonObject::operator=(uint8_t theData) {
	return *this;
}
JsonObject& JsonObject::operator=(uint16_t theData) {
	return *this;
}
JsonObject& JsonObject::operator=(uint32_t theData) {
	return *this;
}
JsonObject& JsonObject::operator=(uint64_t theData) {
	return *this;
}
JsonObject& JsonObject::operator=(bool theData) {
	return *this;
}
JsonObject& JsonObject::operator=(double theData) {
	return *this;
}
JsonObject& JsonObject::operator=(float theData) {
	return *this;
}
JsonObject& JsonObject::operator=(std::string theData) {
	return *this;
}
JsonObject& JsonObject::operator=(const char* theData) {
	return *this;
}

void JsonSerializer::pushBack(float theData){
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Float;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(double theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Double;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(bool theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Boolean;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(int8_t theData){
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Small;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(int16_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(int32_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(int64_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Large;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(uint8_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Small;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(uint16_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(uint32_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(uint64_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Large;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(std::string theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::String;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

void JsonSerializer::pushBack(const char* theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::String;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
}

JsonSerializer& JsonSerializer::operator=(int8_t theData){
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Small;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (N-INTEGER-SMALL): " << this->indentationLevel << std::endl;
	return *this;
}
JsonSerializer& JsonSerializer::operator=(int16_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (N-INTEGER): " << this->indentationLevel << std::endl;
	return *this;
}
JsonSerializer& JsonSerializer::operator=(int32_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (N-INTEGER): " << this->indentationLevel << std::endl;
	return *this;
}
JsonSerializer& JsonSerializer::operator=(int64_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Large;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (N-INTEGER-LARGE): " << this->indentationLevel << std::endl;
	return *this;
}
JsonSerializer& JsonSerializer::operator=(uint8_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Small;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (N-INTEGER-SMALL): " << this->indentationLevel << std::endl;
	return *this;
}
JsonSerializer& JsonSerializer::operator=(uint16_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (N-INTEGER): " << this->indentationLevel << std::endl;
	return *this;
}
JsonSerializer& JsonSerializer::operator=(uint32_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (N-INTEGER): " << this->indentationLevel << std::endl;
	return *this;
}
JsonSerializer& JsonSerializer::operator=(uint64_t theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Large;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (N-INTEGER-LARGE): " << this->indentationLevel << std::endl;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(bool theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Boolean;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (BOOL): " << this->indentationLevel << std::endl;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(double theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Double;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (N-DOUBLE): " << this->indentationLevel << std::endl;
	return *this;
}
JsonSerializer& JsonSerializer::operator=(float theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Double;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (N-FLOAT): " << this->indentationLevel << std::endl;
	return *this;
}
JsonSerializer& JsonSerializer::operator=(std::string theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::String;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (STRING): " << this->indentationLevel << std::endl;
	return *this;
}
JsonSerializer& JsonSerializer::operator=(const char* theData) {
	JsonScalarValue theValue{};
	theValue = theData;
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::String;
	theRecord.theKey = this->theMostRecentKey;
	theRecord.theIndentationLevel = this->indentationLevel;
	theRecord.theScalar = theValue;
	this->theJsonData.push_back(theRecord);
	this->indentationLevel--;
	std::cout << "INDENTATION LEVEL: (STRING): " << this->indentationLevel << std::endl;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(const JsonSerializer& other) {
	this->theMostRecentKey = other.theMostRecentKey;
	this->indentationLevel = other.indentationLevel;
	this->theJsonData = other.theJsonData;
	return *this;
}

JsonSerializer::JsonSerializer(const JsonSerializer& other) {
	*this = other;
}



JsonSerializer::operator std::string() {
	auto theString = this->getString().begin().operator*();
	return theString.substr(0, theString.size() - 1);
}

bool JsonSerializer::doesItExist(const char* keyName, JsonObject& theRecords) {
	for (auto& [key, value] : theRecords.theValue) {
		this->doesItExist(keyName, value);
		if (key == keyName||keyName==value.theKey||keyName==value.theScalarValue.theKey) {
			return true;
		}
	}
	return false;
}

bool JsonSerializer::doesItExist(const char*keyName,std::vector<JsonRecord>& theRecords) {
	for (uint32_t x = 0; x < theRecords.size(); ++x) {
		if (keyName == theRecords[x].theKey || keyName == theRecords[x].theObject.theKey || keyName == theRecords[x].theScalar.theKey) {
			return true;
		}
		this->doesItExist(keyName, theRecords[x].theObject);
	}
	return false;
}

 JsonSerializer& JsonSerializer::operator[](const char* keyName) {
	 this->indentationLevel++;
	 this->theMostRecentKey = keyName;
	 bool doesItExist{ this->doesItExist(keyName,this->theJsonData) };
	 for (uint32_t x = 0; x < this->theJsonData.size(); ++x) {
		 if (keyName == this->theJsonData[x].theKey||keyName==this->theJsonData[x].theObject.theKey|| keyName == this->theJsonData[x].theScalar.theKey) {
			 doesItExist = true;
		 }
	 }
	 std::cout << "INDENTATION LEVEL: " << this->indentationLevel << ", KEY NAME: " << keyName << std::endl;
	 if (doesItExist) {
		 std::cout << "IT DOES EXIST INDENTATION LEVEL: " << this->indentationLevel << ", KEY NAME: " << keyName << std::endl;
		 this->indentationLevel--;
	 }
	return *this;
}

recursive_generator<std::string> JsonSerializer::getString(const JsonObject& theValueNew) {
	std::string theString{};
	JsonObject theValue{};
	for (auto& value : this->theJsonData) {
		if (value.theEvent == JsonParseEvent::Unset) {
			theValue = this->theJsonData.front().theObject;
		}
		else {
			theValue = theValueNew;
		}
		bool addComma{ false };
		switch (value.theEvent) {
		case JsonParseEvent::Array_Start: {
			theString += "[";
			for (auto [key, value] : theValue.theValue) {
				if (addComma) {
					theString += ",";
				}
				theString += "\"" + key + "\":";
				co_yield getString(std::move(value)).begin().operator*();
				addComma = true;
			}
			theString += "]";
			break;
		}

		case JsonParseEvent::Object_Start: {
			theString += "{";
			for (auto& [key, value] : theValue.theValue) {
				if (addComma) {
					theString += ",";
				}
				theString += "\"" + key + "\":";
				co_yield getString(std::move(value)).begin().operator*();
				addComma = true;
			}
			theString += "}";
			break;
		}

		case static_cast<JsonParseEvent>(static_cast<int32_t>(JsonParseEvent::Number_Double) | static_cast<int32_t>(JsonParseEvent::Number_Float) | static_cast<int32_t>(JsonParseEvent::Number_Integer) | static_cast<int32_t>(JsonParseEvent::Number_Integer_Small) | static_cast<int32_t>(JsonParseEvent::Number_Integer_Large)): {
			// assume it fits in a double
			theString += theValue.theScalarValue;
			break;
		}
		case JsonParseEvent::Null_Value: {
			theString += "null";
			break;
		}
		case JsonParseEvent::String: {
			// get_string() would return escaped string, but
			// we are happy with unescaped string.
			theString += "\"" + theValue.theScalarValue.theValue + "\"";
			break;
		}

		case JsonParseEvent::Boolean: {
			theString += theValue.theScalarValue;
			break;
		}
		}
		std::cout << "THE STRING: " << theString << std::endl;
	}
	
	co_yield theString;
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
		theSerializer["d"]["compress"] = this->compress;
		theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
		theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);
		
		

		for (auto& value : this->presence.activities) {
			JsonSerializer theDataNew{};
			if (value.url != "") {
				theDataNew["url"] = value.url;
			}
			theDataNew["name"] = value.name;
			theDataNew["type"] = static_cast<uint8_t>(value.type);
			theSerializer["d"]["presence"]["activities"].pushBack(theDataNew);
		}
		/*
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
		*/
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

			nlohmann::json::parse(static_cast<std::string>(theSerializer));
			DiscordCoreAPI::DiscordCoreClient theClient{ DiscordCoreAPI::DiscordCoreClientConfig {} };
			std::unique_ptr<DiscordCoreInternal::WebSocketSSLShard> theShard{ std::make_unique<DiscordCoreInternal::WebSocketSSLShard>(&theClient,nullptr,0,nullptr) };
			theShard->connect("gateway.discord.gg", "443", true, true);
			auto theJsonData = nlohmann::json{ theData.operator JsonSerializer().getString().begin().operator*() };
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
