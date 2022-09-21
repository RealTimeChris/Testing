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
	std::cout << "THE KEY: INTEGER: " << this->theKey << "THE VALUE: " << this->theValue << std::endl;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		*this = JsonRecord{};
		this->theEvent = JsonParseEvent::Boolean;
		this->theValue = this->theValue;

		std::cout << "THE KEY: BOOL: REAL " << this->theKey << "THE VALUE: REAL " << this->theValue << std::endl;
		return *this;
	}
	else {
		this->theEvent = JsonParseEvent::Number_Integer;
	}
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
	if (this->theEvent == JsonParseEvent::Unset) {
		*this = JsonRecord{};
		this->theEvent = JsonParseEvent::Boolean;
		this->theValue = this->theValue;

		std::cout << "THE KEY: BOOL: REAL " << this->theKey << "THE VALUE: REAL " << this->theValue << std::endl;
		return *this;
	}
	else {
		this->theEvent = JsonParseEvent::Number_Integer;
	}
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

JsonSerializer::JsonSerializer() noexcept {
}

JsonSerializer::operator std::string() noexcept {
	auto theString = this->getString();
	return theString;
}

void JsonRecord::pushBack(const char* keyName, JsonRecord& other) noexcept {
	bool isItFound{ false };
	if (this->theKey == keyName) {
		isItFound = true;
	}
	if (!isItFound) {
		JsonRecord theRecord{};
		theRecord.theEvent = JsonParseEvent::Array_Start;
		theRecord.theKey = keyName;
		*this = theRecord;
	}
}

void JsonRecord::pushBack(const char* keyName, JsonRecord&& other) noexcept {
	bool isItFound{ false };
	if (this->theKey == keyName) {
		isItFound = true;
	}
	if (!isItFound) {
		JsonRecord theRecord{};
		theRecord.theEvent = JsonParseEvent::Array_Start;
		theRecord.theKey = keyName;
		*this = theRecord;
	}
}

JsonRecord::operator std::string() noexcept {
	std::string theString{};
	if (this->theEvent == JsonParseEvent::Object_Start) {
		theString += "{";
		if (this->theKey != "") {
			theString += "\"" + this->theKey + "\":";
		}
	}
	
	this->theState = JsonParserState::Starting_Object;
	switch (this->theEvent) {
	case JsonParseEvent::Object_Start: {

		std::cout << "THE KEY: OBJECT-START: " << this->theKey << "THE VALUE: OBJECT-START: " << static_cast<std::string>(this->theValue) << std::endl;
		this->theState = JsonParserState::Starting_Object;
		this->currentObjectOrArrayStartIndex++;
		theString += "{";
		for (auto& [key, value] : this->theJsonData) {

			theString += "\"" + value.theKey + "\":";
			theString += value;
			if (value.theState != JsonParserState::Starting_Object && value.theState != JsonParserState::Starting_Array) {
				theString += ",";
			}
		}
		theString += "}";
		this->currentObjectOrArrayStartIndex--;
		if (theString.size() > 2) {
			if (theString[theString.size() - 2] == ',') {
				theString.erase(theString.begin() + theString.size() - 2);
			}
		}
		break;
	}
	case JsonParseEvent::Array_Start: {
		this->theState = JsonParserState::Starting_Array;
		theString += "[";
		theString += "\"" + this->theKey + "\":";
		for (auto& [key, value] : this->theJsonData) {
			theString += "\"" + value.theKey + "\":";
			theString += value;
		}
		this->theState = JsonParserState::Adding_Object_Elements;
		theString += "]";
		if (theString[theString.size() - 2] == ',') {
			theString.erase(theString.begin() + theString.size() - 2);
		}
		break;
	}
	case JsonParseEvent::Boolean: {
		std::cout << "THE KEY: BOOL: " << this->theKey << "THE VALUE: BOOL: " << static_cast<std::string>(this->theValue) << std::endl;
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
		std::cout << "THE KEY: NULL: " << this->theKey << "THE VALUE: NULL: " << static_cast<std::string>(this->theValue) << std::endl;
		theString += "null";
		if (this->theState == JsonParserState::Starting_Object) {
			this->theState = JsonParserState::Adding_Object_Elements;
		}
		if (this->theState == JsonParserState::Starting_Array) {
			this->theState = JsonParserState::Adding_Array_Elements;
		}
		break;
	}
	case JsonParseEvent::Number_Double: {
		std::cout << "THE KEY: DOUBLE: " << this->theKey << "THE VALUE: DOUBLE: " << static_cast<std::string>(this->theValue) << std::endl;
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
		std::cout << "THE KEY: FLOAT: " << this->theKey << "THE VALUE: FLOAT: " << static_cast<std::string>(this->theValue) << std::endl;
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
		std::cout << "THE KEY: INTEGER: " << this->theKey << "THE VALUE: INTEGER: " << static_cast<std::string>(this->theValue) << std::endl;
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
		std::cout << "THE KEY: INTEGER_LARGE: " << this->theKey << "THE VALUE: INTEGER_LARGE: " << static_cast<std::string>(this->theValue) << std::endl;
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
		std::cout << "THE KEY: INTEGER_SMALL: " << this->theKey << "THE VALUE: INTEGER_SMALL: " << static_cast<std::string>(this->theValue) << std::endl;
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
		std::cout << "THE KEY: STRING: " << this->theKey << "THE VALUE: STRING: " << static_cast<std::string>(this->theValue) << std::endl;
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
	case JsonParseEvent::Unset: {
		std::cout << "THE KEY: UNSET: " << this->theKey << "THE VALUE: UNSET: " << static_cast<std::string>(this->theValue) << std::endl;
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
	if (this->theEvent == JsonParseEvent::Object_Start) {
		theString += "}";
	}
	return theString;
}

std::string JsonSerializer::getString() {
	this->currentObjectOrArrayStartIndex = 0;
	this->theState = JsonParserState::Starting_Object;
	std::string theString{ static_cast<std::string>(this->theJsonData.begin().operator*().second) };
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
	if (this->theEvent == JsonParseEvent::Object_Start) {
		if (this->theJsonData.contains(keyName)) {
			return this->theJsonData[keyName];
		}
		this->theJsonData[keyName] = JsonRecord{};
		this->theJsonData[keyName].theKey = keyName;
		this->theJsonData[keyName].theEvent = JsonParseEvent::Unset;
		return this->theJsonData[keyName];
	}
	else {
		std::cout << "THE KEY: 123123: " << keyName << std::endl;
		return *this;
	}
}

JsonRecord& JsonSerializer::operator[](const char* keyName) noexcept {
	if (!this->theJsonData.contains(keyName)) {
		std::cout << "THE KEY: 034034: " << keyName << std::endl;
		this->theJsonData[keyName] = JsonRecord{};
		this->theJsonData[keyName].theEvent = JsonParseEvent::Object_Start;
		this->theJsonData[keyName].theKey = keyName;
	}
	return this->theJsonData[keyName];
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
		theSerializer["d"]["compress"] = false;
		theSerializer["d"]["token"] = this->botToken;
		/*
		for (auto& value : this->presence.activities) {
			JsonRecord theSerializer02{};
			std::string theString{};
			if (value.url != "") {
				theString = value.url;
				theSerializer02["url"] = theString;
			}
			theString = std::string{ value.name };
			theSerializer02["name"] = theString;
			theSerializer02["type"] = uint32_t{ static_cast<uint32_t>(value.type) };
			theSerializer["d"]["presences"].pushBack("activities", theSerializer02);
		}

		theSerializer["d"]["afk"] = this->presence.afk;
		if (this->presence.since == 0) {
			theSerializer["d"]["since"] = JsonParseEvent::Null_Value;
		}
		else {
			theSerializer["d"]["since"] = this->presence.since;
		}

		theSerializer["d"]["status"] = this->presence.status;
		theSerializer["d"]["properties"] = JsonParseEvent::Object_Start;
		theSerializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
		theSerializer["d"]["properties"]["device"] = "DiscordCoreAPI";
#ifdef _WIN32
		theSerializer["d"]["properties"]["os"] = "Windows";
#else
		theSerializer["d"]["properties"]["os"] = "Linux";
#endif
		theSerializer[""] = JsonParseEvent::Object_End;
		theSerializer["d"].pushBack("shard", static_cast<uint32_t>(this->currentShard));
		theSerializer["d"].pushBack("shard", static_cast<uint32_t>(this->numberOfShards));
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
