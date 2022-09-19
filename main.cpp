#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"

JsonSerializer& JsonSerializer::operator=(bool theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Boolean;
	std::string theString{};
	if (theData) {
		theString = "true";
	}
	else {
		theString = "false";
	}
	theRecord.theValue = theString;
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(const char* theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::String;
	theRecord.theValue = theData;
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(std::string&& theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::String;
	theRecord.theValue = std::move(theData);
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(std::string& theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::String;
	theRecord.theValue = theData;
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(float theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Float;
	theRecord.theValue = std::to_string(theData);
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(double theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Double;
	theRecord.theValue = std::to_string(theData);
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int64_t theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Large;
	theRecord.theValue = std::to_string(theData);
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int32_t theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theValue = std::to_string(theData);
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int16_t theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theValue = std::to_string(theData);
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int8_t theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Small;
	theRecord.theValue = std::to_string(theData);
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint64_t theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Large;
	theRecord.theValue = std::to_string(theData);
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint32_t theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theValue = std::to_string(theData);
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint16_t theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer;
	theRecord.theValue = std::to_string(theData);
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint8_t theData) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Number_Integer_Small;
	theRecord.theValue = std::to_string(theData);
	if (this->theJsonData.back().theEvent == JsonParseEvent::Unset) {
		this->theJsonData.back().theValue = theRecord.theValue;
		this->theJsonData.back().theEvent = theRecord.theEvent;
	}
	else {
		this->theJsonData.push_back(theRecord);
	}
	this->theIndentationLevel--;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(JsonRecord& theData) {
	this->theJsonData.push_back(theData);
	return *this;
}

JsonRecord& JsonRecord::operator=(bool theData) noexcept {
	this->theEvent = JsonParseEvent::Boolean;
	std::string theString{};
	if (theData) {
		theString = "true";
	}
	else {
		theString = "false";
	}
	this->theValue = theString;
	return *this;
}

JsonRecord& JsonRecord::operator=(const char* theData) noexcept {
	this->theEvent = JsonParseEvent::String;
	this->theValue = theData;
	return *this;
}

JsonRecord& JsonRecord::operator=(std::string&& theData) noexcept {
	this->theEvent = JsonParseEvent::String;
	this->theValue = std::move(theData);
	return *this;
}

JsonRecord& JsonRecord::operator=(std::string& theData) noexcept {
	this->theEvent = JsonParseEvent::String;
	this->theValue = theData;
	return *this;
}

JsonRecord& JsonRecord::operator=(float theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Float;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonRecord& JsonRecord::operator=(double theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Double;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonRecord& JsonRecord::operator=(int64_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonRecord& JsonRecord::operator=(int32_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonRecord& JsonRecord::operator=(int16_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonRecord& JsonRecord::operator=(int8_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonRecord& JsonRecord::operator=(uint64_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonRecord& JsonRecord::operator=(uint32_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonRecord& JsonRecord::operator=(uint16_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonRecord& JsonRecord::operator=(uint8_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	this->theValue = std::to_string(theData);
	return *this;
}

JsonRecord::JsonRecord(int8_t theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(int16_t theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(int32_t theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(int64_t theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(uint8_t theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(uint16_t theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(uint32_t theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(uint64_t theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(bool theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(double theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(float theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(std::string&& theData) noexcept {
	*this = std::move(theData);
}

JsonRecord::JsonRecord(std::string& theData) noexcept {
	*this = theData;
}

JsonRecord::JsonRecord(const char* theData) noexcept {
	*this = theData;
}


JsonSerializer::JsonSerializer()noexcept{
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Object_Start;
	this->theIndentationLevel++;
	theRecord.theKey = "";
	this->theJsonData.emplace_back(theRecord);
}

JsonSerializer::JsonSerializer(const char* theKeyName) noexcept {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Object_Start;
	this->theIndentationLevel++;
	theRecord.theKey = theKeyName;
	this->theJsonData.emplace_back(theRecord);
}


JsonSerializer::operator std::string() noexcept {
	auto theString = this->getString();
	return theString;
}

void JsonSerializer::pushBack(const char* keyName, JsonRecord& other) noexcept {
	bool isItFound{ false };
	for (uint32_t x = 0; x < this->theJsonData.size(); ++x) {
		if (this->theJsonData[x].theKey == keyName) {
			isItFound = true;
		}
	}
	this->theState = JsonParserState::Adding_Array_Elements;
	if (!isItFound) {
		JsonRecord theRecord{};
		theRecord.theEvent = JsonParseEvent::Array_Start;
		theRecord.theKey = keyName;
		this->theJsonData.push_back(theRecord);
	}
	*this = other;
}

void JsonSerializer::pushBack(const char*keyName, JsonRecord&&other) noexcept {
	bool isItFound{ false };
	for (uint32_t x = 0; x < this->theJsonData.size(); ++x) {
		if (this->theJsonData[x].theKey == keyName) {
			isItFound = true;
		}
	}
	this->theState = JsonParserState::Adding_Array_Elements;
	if (!isItFound) {
		JsonRecord theRecord{};
		theRecord.theEvent = JsonParseEvent::Array_Start;
		theRecord.theKey = keyName;
		this->theJsonData.push_back(theRecord);
	}
	*this = std::move(other);
}

void JsonSerializer::pushBack(const char* keyName, JsonSerializer& other) noexcept {
	bool isItFound{ false };
	for (uint32_t x = 0; x < this->theJsonData.size(); ++x) {
		if (this->theJsonData[x].theKey == keyName) {
			isItFound = true;
		}
	}
	this->theState = JsonParserState::Adding_Array_Elements;
	if (!isItFound) {
		JsonRecord theRecord{};
		theRecord.theEvent = JsonParseEvent::Array_Start;
		theRecord.theKey = keyName;
		this->theJsonData.push_back(theRecord);
	}
	*this = other;
}

void JsonSerializer::pushBack(const char* keyName, JsonSerializer&& other) noexcept {
	bool isItFound{ false };
	for (uint32_t x = 0; x < this->theJsonData.size(); ++x) {
		if (this->theJsonData[x].theKey == keyName) {
			isItFound = true;
		}
	}
	this->theState = JsonParserState::Adding_Array_Elements;
	if (!isItFound) {
		JsonRecord theRecord{};
		theRecord.theEvent = JsonParseEvent::Array_Start;
		theRecord.theKey = keyName;
		this->theJsonData.push_back(theRecord);
	}
	*this = std::move(other);
};

JsonRecord::operator std::string() noexcept {
	std::string theString{};
	if (this->theKey != "") {
		theString += "\"" + this->theKey + "\":";
	}if (this->theEvent == JsonParseEvent::String) {
		theString += "\"" + this->theValue + "\"";
	}
	else {
		theString += this->theValue;
	}
	return theString;
}

std::string JsonSerializer::getString() {
	this->theState = JsonParserState::Starting_Object;
	std::string theString{ };
	int32_t currentDepth{};
	std::cout << "THE SIZ EOF THE ARRAY: " << this->theJsonData.size() << std::endl;
	for (auto iterator = this->theJsonData.begin(); iterator != this->theJsonData.end();++iterator){
		if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
			theString += ",";
		}
		if ((*iterator).theKey != "") {
			theString += "\"" + (*iterator).theKey + "\":";
		}
		switch ((*iterator).theEvent) {
		case JsonParseEvent::Object_Start: {
			this->theState = JsonParserState::Starting_Object;
			currentDepth++;
			theString += "{";
			break;
		}
		case JsonParseEvent::Object_End: {
			theString += "}";
			currentDepth--;
			if (theString[theString.size() - 2] == ',') {
				theString.erase(theString.begin() + theString.size() - 2);
			}
			break;
		}
		case JsonParseEvent::Array_Start: {
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
			theString += (*iterator).theValue;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Null_Value: {
			theString += (*iterator).theValue;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Number_Double: {
			theString += (*iterator).theValue;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Number_Float: {
			theString += (*iterator).theValue;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Number_Integer: {
			theString += (*iterator).theValue;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Number_Integer_Large: {
			theString += (*iterator).theValue;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::Number_Integer_Small: {
			theString += (*iterator).theValue;
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
		case JsonParseEvent::String: {
			theString += "\"";
			theString += (*iterator).theValue;
			theString += "\"";
			if (this->theState == JsonParserState::Starting_Object) {
				this->theState = JsonParserState::Adding_Object_Elements;
			}
			if (this->theState == JsonParserState::Starting_Array) {
				this->theState = JsonParserState::Adding_Array_Elements;
			}
			break;
		}
								   this->theJsonData.erase(iterator);
		}
		
	}
	for (uint32_t x = 0; x < currentDepth; ++x) {
		theString += "}";
	}
	std::cout << "THE FINAL STRING REAL: " << theString << std::endl;
	return theString;
}

JsonSerializer::JsonSerializer(const JsonSerializer& other) noexcept{
	*this = other;
}

JsonSerializer& JsonSerializer::operator=(const JsonSerializer& other) noexcept{
	this->currentIndentationLevel = other.currentIndentationLevel;
	this->theIndentationLevel = other.theIndentationLevel;
	this->theState = other.theState;
	for (auto& value : other.theJsonData) {
		this->theJsonData.push_back(value);
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator[](const char* keyName) noexcept {
	this->theIndentationLevel++;
	bool doesItExist{ false };
	for (uint32_t x = 0; x < this->theJsonData.size(); ++x) {
		if (this->theJsonData[x].theKey == keyName) {
			doesItExist = true;
		}
	}
	if (doesItExist) {
		this->theIndentationLevel--;
	}
	if (!doesItExist) {
		if (this->theState == JsonParserState::Adding_Array_Elements) {
			JsonRecord theRecord{};
			theRecord.theEvent = JsonParseEvent::Array_End;
			this->theJsonData.push_back(theRecord);
			this->theState = JsonParserState::Adding_Object_Elements;
		}
		this->theJsonData.push_back(JsonRecord{});
		this->theJsonData.back().theEvent = JsonParseEvent::Unset;
		this->theJsonData.back().theKey = keyName;
		std::cout << "THE ARRAY SIZE REAL: " << this->theJsonData.size() << std::endl;
	}
	std::cout << "THE ARRAY SIZE REAL: " << this->theJsonData.size() << std::endl;
	return *this;
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
		std::vector<uint32_t> theVector{}; 
		JsonSerializer theSerializer{};
		theVector.push_back(244);
		theVector.push_back(243);
		theSerializer["d"]["compress"] = this->compress;
		theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
		theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);
		
	
		for (auto& value : this->presence.activities) {
			JsonSerializer theSerializer02{};
			
			if (value.url != "") {
				theSerializer02["url"] = std::string{ value.url };
			}
			theSerializer02["name"] = std::string{ value.name };
			theSerializer02["type"] = uint32_t{ static_cast<uint32_t>(value.type) };
			theSerializer.pushBack("activities", theSerializer02);
		}
	
		theSerializer["d"]["afk"] = this->presence.afk;
		if (this->presence.since == 0) { 
			theSerializer["d"]["since"];
		}
		else {
			theSerializer["d"]["since"] = this->presence.since;
		}
		
		theSerializer["d"]["status"] = this->presence.status;
		theSerializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
		theSerializer["d"]["properties"]["device"] = "DiscordCoreAPI";
#ifdef _WIN32
		theSerializer["d"]["properties"]["os"] = "Windows";
#else
		theSerializer["d"]["properties"]["os"] = "Linux";
#endif
		theSerializer["d"].pushBack("shard", static_cast<uint8_t>(this->currentShard));
		theSerializer["d"].pushBack("shard", static_cast<uint8_t>(this->numberOfShards));
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
			
			std::string theString = static_cast<JsonSerializer>(theData);
			std::cout << "THE FINAL STRING: 0101 " << theString << std::endl;
			theData.compress = true;

			nlohmann::json::parse(theString);
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
