#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"


JsonRecord& JsonRecord::operator=(bool theData) noexcept {
	std::string theString{};
	if (theData) {
		theString = "true";
	}
	else {
		theString = "false";
	}
	this->theValue = theString;
	this->theEvent = JsonParseEvent::Boolean;
	std::cout << "THE KEY: BOOL: REAL " << this->theKey << "THE VALUE: REAL " << this->theValue << std::endl;
	return *this;
}

JsonRecord& JsonRecord::operator=(const char* theData) noexcept {
	this->theEvent = JsonParseEvent::String;
	if (!theData) {
		this->theEvent = JsonParseEvent::Null_Value;
		this->theValue = "null";
	}
	else {
		this->theValue = theData;
	}
	std::cout << "THE KEY: STRING: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
	return *this;
}

JsonRecord& JsonRecord::operator=(std::string& theData) noexcept {
	this->theEvent = JsonParseEvent::String;
	if (theData.empty()) {
		this->theEvent = JsonParseEvent::Null_Value;
		this->theValue = "null";
	}
	else {
		this->theValue = theData;
	}
	std::cout << "THE KEY: STRING: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
	return *this;
}

JsonRecord& JsonRecord::operator=(float theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Float;
	this->theValue = std::to_string(theData);
	std::cout << "THE KEY: FLOAT: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
	return *this;
}

JsonRecord& JsonRecord::operator=(double theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Double;
	this->theValue = std::to_string(theData);
	std::cout << "THE KEY: DOUBLE: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
	return *this;
}

JsonRecord& JsonRecord::operator=(int64_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	this->theValue = std::to_string(theData);
	std::cout << "THE KEY: INTEGER: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
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
	std::cout << "THE KEY: INTEGER: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
	return *this;
}

JsonRecord& JsonRecord::operator=(int8_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	this->theValue = std::to_string(theData);
	std::cout << "THE KEY: INTEGER: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
	return *this;
}

JsonRecord& JsonRecord::operator=(uint64_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	this->theValue = std::to_string(theData);
	std::cout << "THE KEY: INTEGER: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
	return *this;
}

JsonRecord& JsonRecord::operator=(uint32_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	std::cout << "THE KEY: INTEGER: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
	return *this;
}

JsonRecord& JsonRecord::operator=(uint16_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
	std::cout << "THE KEY: INTEGER: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
	return *this;
}

JsonRecord& JsonRecord::operator=(uint8_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	this->theValue = std::to_string(theData);
	std::cout << "THE KEY: INTEGER: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
	return *this;
}

JsonRecord::JsonRecord(bool theData) noexcept {
	this->theEvent = JsonParseEvent::Boolean;
	std::string theString{};
	if (theData) {
		theString = "true";
	}
	else {
		theString = "false";
	}
	this->theValue = theString;
}

JsonRecord::JsonRecord(const char* theData) noexcept {
	this->theEvent = JsonParseEvent::String;
	if (theData == nullptr) {
		this->theEvent = JsonParseEvent::Null_Value;
		this->theValue = "null";
	}
	else {
		this->theValue = theData;
	}
}

JsonRecord::JsonRecord(std::string& theData) noexcept {
	this->theEvent = JsonParseEvent::String;
	if (theData.empty()) {
		this->theEvent = JsonParseEvent::Null_Value;
		this->theValue = "null";
	}
	else {
		this->theValue = theData;
	}
}

JsonRecord::JsonRecord(float theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Float;
	this->theValue = std::to_string(theData);
}

JsonRecord::JsonRecord(double theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Double;
	this->theValue = std::to_string(theData);
}

JsonRecord::JsonRecord(int64_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	this->theValue = std::to_string(theData);
}

JsonRecord::JsonRecord(int32_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
}

JsonRecord::JsonRecord(int16_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
}

JsonRecord::JsonRecord(int8_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	this->theValue = std::to_string(theData);
}

JsonRecord::JsonRecord(uint64_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	this->theValue = std::to_string(theData);
}

JsonRecord::JsonRecord(uint32_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
}

JsonRecord::JsonRecord(uint16_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer;
	this->theValue = std::to_string(theData);
}

JsonRecord::JsonRecord(uint8_t theData) noexcept {
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	this->theValue = std::to_string(theData);
}

JsonSerializer::operator std::string() noexcept {
	std::string theString{ "{" };
	theString += this->getString();
	theString += "}";
	return theString;
}

JsonRecord::operator std::string() noexcept {
	std::string theString{};
	if (this->theKey != "") {
		theString += "\"" + this->theKey + "\":";
	}
	if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
		theString += ",";
	}
	if (this->theKey != "") {
		theString += "\"" + this->theKey + "\":";
	}
	switch (this->theEvent) {
	case JsonParseEvent::Object_Start: {
		this->theState = JsonParserState::Starting_Object;
		this->currentObjectOrArrayStartIndex++;
		theString += "{";
		break;
	}
	case JsonParseEvent::Object_End: {
		theString += "}";
		this->currentObjectOrArrayStartIndex--;
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
		this->theState = JsonParserState::Adding_Object_Elements;
		theString += "]";
		if (theString[theString.size() - 2] == ',') {
			theString.erase(theString.begin() + theString.size() - 2);
		}
		break;
	}
	case JsonParseEvent::Boolean: {
		theString += this->theValue;
		if (this->theState == JsonParserState::Starting_Object) {
			this->theState = JsonParserState::Adding_Object_Elements;
		}
		if (this->theState == JsonParserState::Starting_Array) {
			this->theState = JsonParserState::Adding_Array_Elements;
		}
		break;
	}
	case JsonParseEvent::Null_Value: {
		theString += this->theValue;
		if (this->theState == JsonParserState::Starting_Object) {
			this->theState = JsonParserState::Adding_Object_Elements;
		}
		if (this->theState == JsonParserState::Starting_Array) {
			this->theState = JsonParserState::Adding_Array_Elements;
		}
		break;
	}
	case JsonParseEvent::Number_Double: {
		theString += this->theValue;
		if (this->theState == JsonParserState::Starting_Object) {
			this->theState = JsonParserState::Adding_Object_Elements;
		}
		if (this->theState == JsonParserState::Starting_Array) {
			this->theState = JsonParserState::Adding_Array_Elements;
		}
		break;
	}
	case JsonParseEvent::Number_Float: {
		theString += this->theValue;
		if (this->theState == JsonParserState::Starting_Object) {
			this->theState = JsonParserState::Adding_Object_Elements;
		}
		if (this->theState == JsonParserState::Starting_Array) {
			this->theState = JsonParserState::Adding_Array_Elements;
		}
		break;
	}
	case JsonParseEvent::Number_Integer: {
		theString += this->theValue;
		if (this->theState == JsonParserState::Starting_Object) {
			this->theState = JsonParserState::Adding_Object_Elements;
		}
		if (this->theState == JsonParserState::Starting_Array) {
			this->theState = JsonParserState::Adding_Array_Elements;
		}
		break;
	}
	case JsonParseEvent::Number_Integer_Large: {
		theString += this->theValue;
		if (this->theState == JsonParserState::Starting_Object) {
			this->theState = JsonParserState::Adding_Object_Elements;
		}
		if (this->theState == JsonParserState::Starting_Array) {
			this->theState = JsonParserState::Adding_Array_Elements;
		}
		break;
	}
	case JsonParseEvent::Number_Integer_Small: {
		theString += this->theValue;
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
		theString += this->theValue;
		theString += "\"";
		if (this->theState == JsonParserState::Starting_Object) {
			this->theState = JsonParserState::Adding_Object_Elements;
		}
		if (this->theState == JsonParserState::Starting_Array) {
			this->theState = JsonParserState::Adding_Array_Elements;
		}
		break;
	}
		
	}
	for (uint32_t x = 0; x < this->currentObjectOrArrayStartIndex; ++x) {
		theString += "}";
	}
	return theString;
}

void JsonSerializer::addNewStructure(const char* keyName) {
	JsonRecord theRecord{};
	theRecord.theKey = keyName;
	theRecord.theEvent = JsonParseEvent::Object_Start;
	this->theJsonData.push_back(theRecord);
}
void JsonSerializer::appendStructElement(const char* keyName, JsonRecord&&theRecord) {
	theRecord.theKey = keyName;
	this->theJsonData.push_back(std::move(theRecord));

}
void JsonSerializer::appendStructElement(const char* keyName, JsonRecord&theRecord) {
	theRecord.theKey = keyName;
	this->theJsonData.push_back(theRecord);
}
void JsonSerializer::endStructure() {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Object_End;
	this->theJsonData.push_back(theRecord);
}
void JsonSerializer::addNewArray(const char* keyName) {
	JsonRecord theRecord{};
	theRecord.theKey = keyName;
	theRecord.theEvent = JsonParseEvent::Array_Start;
	this->theJsonData.push_back(theRecord);

}

void JsonSerializer::appendArrayElement(JsonSerializer&&theRecord) {
	this->addNewStructure("");
	for (auto& value : theRecord.theJsonData) {
		this->theJsonData.push_back(std::move(value));
	}
	this->endStructure();
}

void JsonSerializer::appendArrayElement(JsonSerializer& theRecord) {
	this->addNewStructure("");
	for (auto& value : theRecord.theJsonData) {
		this->theJsonData.push_back(value);
	}
	this->endStructure();
}

void JsonSerializer::endArray() {
	JsonRecord theRecord{};
	theRecord.theEvent = JsonParseEvent::Array_End;
	this->theJsonData.push_back(theRecord);
}

std::string JsonSerializer::getString() {
	std::string theString{};
	for (auto iterator = this->theJsonData.begin(); iterator != this->theJsonData.end(); ++iterator) {
		if (this->theState != JsonParserState::Starting_Object && this->theState != JsonParserState::Starting_Array) {
			theString += ",";
		}
		if ((*iterator).theKey != "") {
			theString += "\"" + (*iterator).theKey + "\":";
		}
		switch ((*iterator).theEvent) {
		case JsonParseEvent::Object_Start: {
			this->theState = JsonParserState::Starting_Object;
			this->currentObjectOrArrayStartIndex++;
			theString += "{";
			break;
		}
		case JsonParseEvent::Object_End: {
			theString += "}";
			this->currentObjectOrArrayStartIndex--;
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
		}
	}
	for (uint32_t x = 0; x < this->currentObjectOrArrayStartIndex; ++x) {
		theString += "}";
	}
	
	return theString;
}

JsonRecord& JsonRecord::operator=(EnumConverter other) {
	JsonRecord theRecord{};
	if (other.vectorType) {
		theRecord.theEvent = JsonParseEvent::Array_Start;
		for (auto& value : static_cast<std::vector<uint64_t>>(other)) {
			theRecord.theEvent = JsonParseEvent::Number_Integer_Large;
			theRecord.theValue = std::to_string(static_cast<uint64_t>(value));
		}
		theRecord.theEvent = JsonParseEvent::Array_End;
	}
	return *this;
}

JsonRecord& JsonRecord::operator[](const char* keyName) noexcept {
	this->theKey = keyName;
	return *this;
}

    struct WebSocketIdentifyData {
		DiscordCoreInternal::UpdatePresenceData presence{};
		std::int32_t largeThreshold{ 250 };
		std::int32_t numberOfShards{};
		std::int32_t currentShard{};
		bool compress{ false };
		std::string botToken{};
		std::unordered_map<std::string, std::string> theVector{};
		int64_t intents{};

        operator JsonSerializer();
    };
	 
	WebSocketIdentifyData::operator JsonSerializer() {
		JsonSerializer theSerializer{};
		theSerializer.addNewStructure("d");
		theSerializer.appendStructElement("intents", static_cast<uint32_t>(this->intents));
		theSerializer.appendStructElement("large_threshold", static_cast<uint32_t>(250));
		theSerializer.addNewArray("activities");
		for (auto& value : this->presence.activities) {
			JsonSerializer theSerializer02{};
			std::string theString{};
			if (value.url != "") {
				theString = value.url;
				theSerializer02.appendStructElement("url", theString);
			}
			theString = std::string{ value.name };
			theSerializer02.appendStructElement("name", theString);
			theSerializer02.appendStructElement("type", uint32_t{ static_cast<uint32_t>(value.type) });
			theSerializer.appendArrayElement(theSerializer02);
		}
		theSerializer.endArray();
		theSerializer.appendStructElement("afk", this->presence.afk);
		if (this->presence.since != 0) {
			theSerializer.appendStructElement("since", this->presence.since);
		}/*

		theSerializer["d"]["status"] = this->presence.status;
		theSerializer["d"]["properties"] = JsonParseEvent::Object_Start;
		theSerializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
		theSerializer["d"]["properties"]["device"] = "DiscordCoreAPI";
#ifdef _WIN32
		theSerializer["d"]["properties"]["os"] = "Windows";
#else
		theSerializer["d"]["properties"]["os"] = "Linux";
#endif
		theSerializer["d"]["shard"].pushBack(static_cast<uint32_t>(this->currentShard));
		theSerializer["d"]["shard"].pushBack(static_cast<uint32_t>(this->numberOfShards));
		theSerializer["d"]["token"] = this->botToken;
		theSerializer["op"] = static_cast<uint8_t>(2);
		*/
		return theSerializer;
		}


    int32_t main() noexcept {
		try {
			//EnumConverter  theEnum{ DiscordCoreAPI::ChannelType::Dm };
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
			std::cout << "THE TYPE: " << typeid(std::enable_if<std::is_enum<DiscordCoreAPI::ChannelType>::value, DiscordCoreAPI::ChannelType>::type).name() << std::endl;
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
