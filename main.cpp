#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"

JsonScalarValue& JsonScalarValue::operator=(const JsonScalarValue& other) noexcept {
	this->theValue = other.theValue;
	return *this;
}

JsonScalarValue::JsonScalarValue(const JsonScalarValue&other) noexcept {
	*this = other;
}

JsonScalarValue::operator std::string() noexcept {
	return this->theValue;
}

JsonScalarValue& JsonScalarValue::operator=(bool theData) noexcept {
	this->theEvent = JsonParseEvent::Boolean;
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
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(const char* theData) noexcept {
	this->theEvent = JsonParseEvent::String;
	this->theValue = theData;
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(std::string theData) noexcept {
	this->theEvent = JsonParseEvent::String;
	this->theValue = theData;
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(float theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Float;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(double theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Double;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(int64_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(int32_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(int16_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(int8_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(uint64_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(uint32_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(uint16_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue& JsonScalarValue::operator=(uint8_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonScalarValue::JsonScalarValue(int8_t theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(int16_t theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(int32_t theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(int64_t theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(uint8_t theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(uint16_t theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(uint32_t theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(uint64_t theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(bool theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(double theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(float theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(std::string theData) noexcept {
	*this = theData;
}

JsonScalarValue::JsonScalarValue(const char* theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(const JsonScalarValue& theData) noexcept {
	this->theEvent = theData.theEvent;
	this->theScalarValue = theData;
	return *this;
}

JsonObject::JsonObject(const JsonScalarValue& theData) noexcept {
	*this = theData;
}

JsonSerializer::JsonSerializer(const char* theKeyName) noexcept {
	JsonObject theRecord{};
	theRecord.theEvent = JsonParseEvent::Object_Start;
	this->theIndentationLevel++;
	theRecord.theKey = theKeyName;
	if (this->theJsonData.theKey == "") {
		this->theJsonData.theKey = theKeyName;
	}
	else {
		this->theJsonData.theValues.emplace(theKeyName, theRecord);
	}
}

JsonSerializer::operator std::string() noexcept {
	auto theString = this->getString(this->theJsonData);
	return theString.substr(0, theString.size() - 1);
}

bool JsonSerializer::doesItExist(const char* keyName, JsonObject& theRecords) noexcept {
	for (auto& [key, value] : theRecords.theValues) {
		if (key == keyName || keyName == value.theKey) {
			return true;
		}
		return this->doesItExist(keyName, value);
	}
	return false;
}


JsonObject& JsonObject::operator[](const char*keyName) noexcept {
	if (this->theKey == "" || this->theKey == keyName) {
		this->theKey = keyName;
		return *this;
	}
	else {
		if (!this->theValues.contains(keyName)) {
			this->theValues[keyName] = JsonObject{};
			this->theValues[keyName].theKey = keyName;
			this->theIndentationLevel++;
		}
	}
	return this->theValues[keyName];
}

void printValues() {

}

 JsonObject& JsonSerializer::operator[](const char* keyName) noexcept {
	 this->theIndentationLevel++;
	 this->theMostRecentKey = keyName;
	 bool doesItExist{ this->doesItExist(keyName,this->theJsonData) };
	 if (this->theJsonData.theValues.contains(keyName)) {
		 doesItExist = true;
	 }
	 if (doesItExist) {
		 this->theIndentationLevel--;
	 }
	 return this->theJsonData;
}

 std::string JsonSerializer::getScalarObject(JsonObject& theObject)noexcept {
	 std::string theString{};
	 switch(theObject.theEvent){
	 case static_cast<JsonParseEvent>(static_cast<int32_t>(JsonParseEvent::Number_Double) | static_cast<int32_t>(JsonParseEvent::Number_Float) | static_cast<int32_t>(JsonParseEvent::Number_Integer) | static_cast<int32_t>(JsonParseEvent::Number_Integer_Small) | static_cast<int32_t>(JsonParseEvent::Number_Integer_Large)): {
		 theString += theObject.theScalarValue.theValue;
		 break;
	 }
	 case JsonParseEvent::Null_Value: {
		 theString += "null";
		 break;
	 }
	 case JsonParseEvent::String: {
		 theString += "\"" + theObject.theScalarValue.theValue + "\"";
		 break;
	 }
	 case JsonParseEvent::Boolean: {
		 theString += theObject.theScalarValue.theValue;
		 
		 break;
	 }
 }
	 std::cout << "THE STRING: " << theObject.theScalarValue.theValue << std::endl;
	 return theString;
 }

 std::string JsonSerializer::getString( JsonObject &theValueNew) noexcept {
	 std::string theString{ "{" };

	 for (auto& [key, value]:theValueNew.theValues) {
		 std::cout << "THE KEY 01: " << key << std::endl;
		 std::cout << "THE KEY 02: " << value.theKey << std::endl;
		 std::cout << "THE EVENT: " << (int32_t)value.theEvent << std::endl;
		 bool addComma{ false };
		 switch (value.theEvent) {
		 case JsonParseEvent::Array_Start: {
			 theString += "\"" + value.theKey + "\":" + "[";
			 std::cout << "THE NEW OBJECT: " << value.theScalarValue.theValue << std::endl;
			 if (addComma) {
				 theString += ",";
			 }
			 theString += getString(value);
			 addComma = true;
			 theString += "]";
			 break;
		 }

		 case JsonParseEvent::Object_Start: {
			 theString += "\"" + value.theKey + "\":" + "{";
			 std::cout << "THE NEW OBJECT: " << value.theScalarValue.theValue << std::endl;
			 if (addComma) {
				 theString += ",";
			 }
				 
			 theString += getScalarObject(value);
			 addComma = true;
			 
			 theString += "}";
			 break;
		 }

		 case static_cast<JsonParseEvent>(static_cast<int32_t>(JsonParseEvent::Number_Double) | static_cast<int32_t>(JsonParseEvent::Number_Float) | static_cast<int32_t>(JsonParseEvent::Number_Integer) | static_cast<int32_t>(JsonParseEvent::Number_Integer_Small) | static_cast<int32_t>(JsonParseEvent::Number_Integer_Large)): {
			 theString += value.theScalarValue.theValue;
			 break;
		 }
		 case JsonParseEvent::Null_Value: {
			 theString += "null";
			 break;
		 }
		 case JsonParseEvent::String: {
			 theString += "\"" + value.theScalarValue.theValue + "\"";
			 break;
		 }
		 case JsonParseEvent::Boolean: {
			 theString += value.theScalarValue.theValue;
			 std::cout << "THE STRING: " << value.theScalarValue.theValue << std::endl;
			 break;
		 }
		 }
		 getString(value);
	 }	 
	 theString += "}";
	 std::cout << theString << std::endl;
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
		JsonSerializer theSerializer{ "d" };
		std::vector<uint32_t> theVector{};
		theVector.push_back(244);
		theVector.push_back(243);
		theSerializer["d"]["compress"] = this->compress;
		theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
		theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);
		
		
		theSerializer["d"]["presence"]["activities"];
		for (auto& value : this->presence.activities) {
			JsonObject theDataNew{ "" };
			if (value.url != "") {
				theDataNew["url"] = std::string{ value.url };
			}
			theDataNew["name"] = std::string{ value.name };
			theDataNew["type"] = static_cast<uint8_t>(value.type);
			theSerializer["d"]["presence"]["activities"] = theDataNew;
		}
	
		theSerializer["d"]["afk"] = this->presence.afk;
		if (this->presence.since == 0) { 
			theSerializer["d"]["since"];
		}
		else {
			theSerializer["d"]["since"] = this->presence.since;
		}
		
		theSerializer["d"]["status"] = this->presence.status;
		theSerializer["d"]["properties"] = 22;
		theSerializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
		theSerializer["d"]["properties"]["device"] = "DiscordCoreAPI";
#ifdef _WIN32
		theSerializer["d"]["properties"]["os"] = "Windows";
#else
		theSerializer["d"]["properties"]["os"] = "Linux";
#endif
		theSerializer["d"]["shard"];
		theSerializer["d"]["shard"] = static_cast<uint8_t>(this->currentShard);
		theSerializer["d"]["shard"] = static_cast<uint8_t>(this->numberOfShards);
		theSerializer["d"]["token"] = this->botToken;
		theSerializer["op"] = static_cast<uint8_t>(2);
		return theSerializer;
		}


    int32_t main() noexcept {
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
			auto theJsonData = nlohmann::json{ static_cast<std::string>(theData.operator JsonSerializer()) };
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
