#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"
/*
JsonObject& JsonObject::operator=(JsonSerializer&&other)noexcept{
	this->indentationLevel = other.indentationLevel;
	for (auto& [key, value] : other.theStructData) {
		this->theStructData[key] = std::move(value);
	}
	for (auto& value : other.theArrayData) {
		this->theArrayData.emplace_back(std::move(value));
	}
	this->theEvent = other.theEvent;
	this->theState = other.theState;
	this->theValue = other.theValue;
	return *this;
}

JsonObject& JsonObject::operator=(JsonSerializer& other)noexcept{
	this->indentationLevel = other.indentationLevel;
	for (auto& [key, value] : other.theStructData) {
		this->theStructData[key] = value;
	}
	for (auto& value : other.theArrayData) {
		this->theArrayData.emplace_back(value);
	}
	this->theEvent = other.theEvent;
	this->theState = other.theState;
	this->theValue = other.theValue;
	return *this;
}

JsonObject::JsonObject(JsonSerializer&& other)noexcept{
	*this = other;
}

JsonObject::JsonObject(JsonSerializer& other)noexcept{
	*this = other;
}

JsonSerializer& JsonSerializer::operator[](const char*keyName) noexcept {
	bool isItFound{ false };
	if (this->theStructData.empty()) {
		this->indentationLevel++;
		this->theKey = keyName;
		this->theEvent = JsonParseEvent::Object_Start;
		this->theStructData[keyName] = *this;
		return *this;
	}
	this->indentationLevel++;
	this->theKey = keyName;
	this->theEvent = JsonParseEvent::Unset;
	this->theStructData[keyName] = *this;
	return *this;
}

JsonObject& JsonObject::operator=(bool theData) noexcept {
	std::string theString{};
	if (theData) {
		theString = "true";
	}
	else {
		theString = "false";
	}
	this->theValue = theString;
	this->theEvent = JsonParseEvent::Boolean;
	return *this;
}

JsonObject& JsonObject::operator=(const char* theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::String;
	return *this;
}

JsonObject& JsonObject::operator=(std::nullptr_t theData) noexcept {
	this->theEvent = JsonParseEvent::Null_Value;
	return *this;
}

JsonObject& JsonObject::operator=(std::string&& theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::String;
	return *this;
}

JsonObject& JsonObject::operator=(std::string& theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::String;
	return *this;
}

JsonObject& JsonObject::operator=(float theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::Number_Float;
	return *this;
}

JsonObject& JsonObject::operator=(double theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::Number_Float;
	return *this;
}

JsonObject& JsonObject::operator=(int64_t theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	return *this;
}

JsonObject& JsonObject::operator=(int32_t theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::Number_Integer;
	return *this;
}

JsonObject& JsonObject::operator=(int16_t theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::Number_Integer;
	return *this;
}

JsonObject& JsonObject::operator=(int8_t theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	return *this;
}

JsonObject& JsonObject::operator=(uint64_t theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::Number_Integer_Large;
	return *this;
}

JsonObject& JsonObject::operator=(uint32_t theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::Number_Integer;
	return *this;
}

JsonObject& JsonObject::operator=(uint16_t theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::Number_Integer;
	return *this;
}

JsonObject& JsonObject::operator=(uint8_t theData) noexcept {
	this->theValue = theData;
	this->theEvent = JsonParseEvent::Number_Integer_Small;
	return *this;
}

JsonObject::JsonObject(const char* keyName, JsonParseEvent theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, const char* theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, std::nullptr_t theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, std::string&& theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, bool theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, std::string& theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, float theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, double theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, int64_t theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, int32_t theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, int16_t theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, int8_t theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, uint64_t theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, uint32_t theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, uint16_t theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonObject::JsonObject(const char* keyName, uint8_t theData) noexcept {
	*this = theData;
	this->theKey = keyName;
}

JsonSerializer& JsonSerializer::operator=(const char* theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int64_t theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int32_t theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int16_t theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(int8_t theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint64_t theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint32_t theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(double theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(float theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(nullptr_t theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint16_t theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(uint8_t theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(bool theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(std::string& theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer& JsonSerializer::operator=(std::string&& theData) noexcept {
	JsonObject theObject{};
	theObject = theData;
	this->theStructData = theObject.theStructData;
	this->theArrayData = theObject.theArrayData;
	this->theEvent = theObject.theEvent;
	this->theState = theObject.theState;
	this->theValue = theObject.theValue;
	if (this->theEvent == JsonParseEvent::Object_Start) {
		this->theStructData[this->theKey] = *this;
	}
	return *this;
}

JsonSerializer::JsonSerializer(const char* other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(int8_t other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(int16_t other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(int32_t other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(int64_t other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(uint8_t other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(uint16_t other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(uint32_t other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(uint64_t other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(std::nullptr_t other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(bool other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(double other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(float other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(std::string&& other) noexcept {
	*this = other;
}
JsonSerializer::JsonSerializer(std::string& other) noexcept {
	*this = other;
}
/*
JsonObject::operator std::string() noexcept {
	std::string theString{ "{" };
	this->theState = JsonParserState::Starting_Object;
	std::cout << "WERE HERE THIS IS ITS!" << std::endl;
	if (this->theState == JsonParserState::Adding_Object_Elements || this->theState == JsonParserState::Adding_Array_Elements) {
		theString += ",";
	}
	if (this->theKey != "") {
		theString += "\"" + this->theKey + "\":";
	}
	switch (this->theEvent) {
	case JsonParseEvent::Object_Start: {
		this->theState = JsonParserState::Starting_Object;
		this->indentationLevel++;
		theString += "{";
		for (auto& [key, value] : this->theStructData) {
			theString += value;
		}
		break;
	}
	case JsonParseEvent::Object_End: {
		this->theState = JsonParserState::Adding_Object_Elements;
		theString += "}";
		this->indentationLevel--;
		if (theString[theString.size() - 2] == ',') {
			theString.erase(theString.begin() + theString.size() - 2);
		}
		break;
	}
	case JsonParseEvent::Array_Start: {
		this->theState = JsonParserState::Starting_Array;
		theString += "[";
		for (auto& value : this->theArrayData) {
			theString += value;
		}
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
	theString += "}";
	std::cout << "THE STRING FINAL 0101: " << theString << std::endl;
	return theString;
}

JsonObject& JsonObject::operator=(JsonParseEvent other)noexcept{
	this->theEvent = other;
	return *this;
}

void JsonSerializer::pushBack(JsonSerializer&& theRecord) {
	for (auto& [key, value] : theRecord.theStructData) {
		this->theStructData[key] = value;
	}
}

void JsonSerializer::pushBack(JsonSerializer& theRecord) {
	for (auto& [key, value] : theRecord.theStructData) {
		this->theStructData[key] = value;
	}
}
*/

JsonValue::JsonValue(ValueType theType){
	switch (theType) {
	case ValueType::Bool:{
		this->theBool = false;
	} 
	case ValueType::Double: {
		this->theDouble = 0.0f;
	}
	case ValueType::Float : {
		this->theDouble = 0.0f;
	}
	case ValueType::Uint64: {
		this->theDouble = 1ull;
	}
	case ValueType::Int64: {
		this->theDouble = 1ll;
	}
	case ValueType::String: {
		this->theString = std::make_unique<std::string>().release();
	}
	case ValueType::Null: {
		this->theNull = nullptr;
	}
	case ValueType::Array: {
		this->theArray = std::make_unique<JsonArray>().release();
	}
	case ValueType::Object: {
		this->theObject = std::make_unique<JsonObject>().release();
	}
	}
}

std::string JsonSerializer::getString(JsonObject theObject) {
	std::string theString{ "{" };
	return theString;
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
		/*
		theSerializer["d"];
		theSerializer["intents"] = "testing";
		theSerializer["large_threshold"] = static_cast<uint32_t>(250);
		for (auto& value : this->presence.activities) {
			JsonSerializer theSerializer02{};
			std::string theString{};
			if (value.url != "") {
				theString = value.url;
				theSerializer02["url"] = theString;
			}
			theString = std::string{ value.name };
			theSerializer02["name"] = theString;
			theSerializer02["type"] = uint32_t{ static_cast<uint32_t>(value.type) };
		}
		theSerializer["afk"] = this->presence.afk;
		if (this->presence.since != 0) {
			theSerializer["since"] = this->presence.since;
		}

		theSerializer["status"] = this->presence.status;
		//theSerializer.appendStructElement("browser", "DiscordCoreAPI");
		//theSerializer.appendStructElement("device", "DiscordCoreAPI");
#ifdef _WIN32
		//theSerializer.appendStructElement("os", "Windows");
#else
		theSerializer["d"]["properties"]["os"] = "Linux";
#endif
		auto theInt = static_cast<uint32_t>(this->currentShard);
		theSerializer.pushBack(theInt);
		theSerializer.pushBack(static_cast<uint32_t>(this->numberOfShards));
		*/
		return theSerializer;
		}

	/// For editing the permissions of a single Guild ApplicationCommand. \brief For editing the permissions of a single Guild ApplicationCommand.
	struct EditGuildApplicationCommandPermissionsData {
		std::vector<DiscordCoreAPI::ApplicationCommandPermissionData> permissions{};///< A vector of ApplicationCommand permissions.
		std::string commandName{};///< The command name which you would like to edit the permissions of.
		DiscordCoreAPI::Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		uint64_t commandId{};///< The command id which you would like to edit the permissions of.
		DiscordCoreAPI::Snowflake guildId{};///< The Guild id of the Guild for which you would like to edit the command permissions.

		operator JsonSerializer();
	};

	EditGuildApplicationCommandPermissionsData::operator JsonSerializer() {
		JsonSerializer theData{};
		/*
		for (auto& value : this->permissions) {
			JsonSerializer newData{};
			
			newData["d"]["permission"] = value.permission;
			newData["d"]["type"] = static_cast<uint8_t>(value.type);
			auto theString = std::to_string(value.id);
			newData["d"]["id"] = theString;
		}
		std::cout << "THE STRING: " << theData.getString() << std::endl;
		*/
		return theData;
	}


    int32_t main() noexcept {
		try {
			//EnumConverter  theEnum{ DiscordCoreAPI::ChannelType::Dm };
			EditGuildApplicationCommandPermissionsData theDataBew{};
			theDataBew.applicationId = 12312312323;
			DiscordCoreAPI::ApplicationCommandPermissionData theDataNew{};
			theDataNew.id = 123123123;
			theDataNew.permission = true;
			theDataNew.type = DiscordCoreAPI::ApplicationCommandPermissionType::Channel;
			theDataBew.commandName = "TESTING";
			theDataBew.permissions.push_back(theDataNew);
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
			std::string theString = static_cast<JsonSerializer>(theDataBew).getString(JsonObject{});
			std::cout << "THE FINAL STRING: 0101 " << theString << std::endl;
			theData.compress = true;

			nlohmann::json::parse(theString);
			DiscordCoreAPI::DiscordCoreClient theClient{ DiscordCoreAPI::DiscordCoreClientConfig {} };
			std::unique_ptr<DiscordCoreInternal::WebSocketSSLShard> theShard{ std::make_unique<DiscordCoreInternal::WebSocketSSLShard>(&theClient,nullptr,0,nullptr) };
			theShard->connect("gateway.discord.gg", "443", true, true);
			auto theJsonData = nlohmann::json{ static_cast<std::string>(theData.operator JsonSerializer().getString(JsonObject{})) };
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
