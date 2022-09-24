#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"

JsonValue& JsonValue::operator=(const ValueType& theType) {
	switch (theType) {
	case ValueType::Bool: {
		this->theBool = false;
		break;
	}
	case ValueType::Double: {
		this->theDouble = 0.0f;
		break;
	}
	case ValueType::Float: {
		this->theDouble = 0.0f;
		break;
	}
	case ValueType::Uint64: {
		this->theDouble = 1ull;
		break;
	}
	case ValueType::Int64: {
		this->theDouble = 1ll;
		break;
	}
	case ValueType::String: {
		this->theString = std::make_unique<std::string>();
		break;
	}
	case ValueType::Null: {
		this->theNull = nullptr;
		break;
	}
	case ValueType::Array: {
		this->theArray = std::make_unique<JsonArray>();
		break;
	}
	case ValueType::Object: {
		this->theObject = std::make_unique<JsonObject>();
		break;
	}
	case ValueType::Unset: {
		break;
	}
	}
	return *this;
}

JsonValue::JsonValue(const ValueType& theType){
	*this = theType;
}

JsonValue& JsonValue::operator=(const JsonValue& other) {
	if (other.theArray) {
		*this->theArray = *other.theArray;
	}
	else if (other.theObject) {
		*this->theObject = *other.theObject;
	}
	else if (other.theString) {
		*this->theString = *other.theString;
	}
	this->theDouble = other.theDouble;
	this->theFloat = other.theFloat;
	this->theBool = other.theBool;
	this->theUint = other.theUint;
	this->theInt = other.theInt;
	return *this;
}

JsonValue::JsonValue(const JsonValue& theType){
	*this = theType;
}

JsonValue::JsonValue(std::string theData) noexcept {
	this->theString = std::make_unique<std::string>();
	*this->theString = theData;
}

JsonValue::JsonValue(JsonObject theData) noexcept {
	this->theObject = std::make_unique<JsonObject>();
	*this->theObject = theData;
}

JsonValue::JsonValue(JsonArray theData) noexcept {
	this->theArray = std::make_unique<JsonArray>();
	*this->theArray = theData;
}

JsonValue::JsonValue(nullptr_t theData) noexcept {
	this->theNull = theData;
}

JsonValue::JsonValue(uint64_t theData) noexcept {
	this->theUint = theData;
}

JsonValue::JsonValue(int64_t theData) noexcept {
	this->theInt = theData;
}

JsonValue::JsonValue(double theData) noexcept {
	this->theDouble = theData;
}

JsonValue::JsonValue(float theData) noexcept {
	this->theFloat = theData;
}

JsonValue::JsonValue(bool theData) noexcept {
	this->theBool = theData;
}

JsonValue::~JsonValue() {}

JsonObject::JsonObject(ValueType theType) noexcept {
	this->theValue = theType;
	this->theType = theType;
}

JsonObject& JsonObject::operator=(const JsonObject& theKey) {
	for (auto& [key, value] : theKey.theValues) {
		this->theValues[key] = value;
	}
	for (auto& value : theKey.theArrayValues) {
		this->theArrayValues.push_back(value);
	}
	this->theValue = theKey.theValue;
	this->theType = theKey.theType;
	this->theKey = theKey.theKey;
	return *this;
}

JsonObject::JsonObject(const JsonObject& theKey) {
	*this = theKey;
}

JsonObject& JsonObject::operator=(const char* theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::String;
		this->theValue = this->theType;
		*this->theValue.theString = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::String;
		this->theValues[this->theKey].theValue = ValueType::String;
		*this->theValues[this->theKey].theValue.theString = theData;
	}
	return *this;
}

JsonObject& JsonObject::operator=(std::string theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::String;
		this->theValue = this->theType;
		*this->theValue.theString = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::String;
		this->theValues[this->theKey].theValue = ValueType::String;;
		*this->theValues[this->theKey].theValue.theString = theData;
	}
	return *this;
}

JsonObject& JsonObject::operator[](const char* theKey) {
	if (this->theKey == "") {
		JsonObject theObject{};
		theObject.theKey = theKey;
		theObject.theType = ValueType::Object;
		this->theValues[theKey] = theObject;
		return this->theValues[theKey];
	}
	else if (this->theKey == theKey && this->theType == ValueType::Object) {
		return *this;
	} else if (!this->theValues.contains(theKey)) {
		JsonObject theObject{};
		theObject.theKey = theKey;
		theObject.theType = ValueType::Object;
		this->theValues[theKey] = theObject;
		return this->theValues[theKey];
	} else if (this->theValues.contains(theKey)){
		return this->theValues[theKey];
	} else {
		JsonObject theObject{};
		theObject.theType = ValueType::Object;
		theObject.theKey = theKey;
		this->theValues[theKey] = theObject;
		return this->theValues[theKey];
	}
}

JsonObject& JsonObject::operator=(bool theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::Bool;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::Bool;
		this->theValues[this->theKey].theValue = ValueType::Bool;
		this->theValues[this->theKey].theValue.theBool = theData;
	}
	return *this;
}

JsonObject& JsonObject::operator=(double theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::Double;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::Double;
		this->theValues[this->theKey].theValue = ValueType::Double;
		this->theValues[this->theKey].theValue.theDouble= theData;
	}
	return *this;
}

JsonObject& JsonObject::operator=(float theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::Float;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::Float;
		this->theValues[this->theKey].theValue = ValueType::Float;
		this->theValues[this->theKey].theValue.theFloat = theData;
	}
	return *this;
}

JsonObject& JsonObject::operator=(uint64_t theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::Uint64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::Uint64;
		this->theValues[this->theKey].theValue = ValueType::Uint64;
		this->theValues[this->theKey].theValue.theUint = theData;
	}
	return *this;
}

JsonObject& JsonObject::operator=(uint32_t theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::Uint64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::Uint64;
		this->theValues[this->theKey].theValue = ValueType::Uint64;;
		this->theValues[this->theKey].theValue.theUint = theData;
	}
	return *this;
}

JsonObject& JsonObject::operator=(uint16_t theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::Uint64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::Uint64;
		this->theValues[this->theKey].theValue = ValueType::Uint64;;
		this->theValues[this->theKey].theValue.theUint = theData;
	}
	return *this;
}

JsonObject& JsonObject::operator=(uint8_t theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::Uint64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::Uint64;
		this->theValues[this->theKey].theValue = ValueType::Uint64;
		this->theValues[this->theKey].theValue.theUint = theData;
	}
	return *this;
}

JsonObject& JsonObject::operator=(int64_t theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::Int64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::Int64;
		this->theValues[this->theKey].theValue = ValueType::Int64;
		this->theValues[this->theKey].theValue.theInt = theData;
	}
	return *this;
}

JsonObject& JsonObject::operator=(int32_t theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::Int64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::Int64;
		this->theValues[this->theKey].theValue = ValueType::Int64;
		this->theValues[this->theKey].theValue.theInt = theData;
	}
	return *this;
}

JsonObject& JsonObject::operator=(int16_t theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::Int64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::Int64;
		this->theValues[this->theKey].theValue = ValueType::Int64;
		this->theValues[this->theKey].theValue.theInt = theData;
	}
	return *this;
}

JsonObject& JsonObject::operator=(int8_t theData) {
	if (this->theType != ValueType::Object) {
		this->theType = ValueType::Int64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
	}
	else {
		this->theValues[this->theKey] = ValueType::Int64;
		this->theValues[this->theKey].theValue = ValueType::Int64;
		this->theValues[this->theKey].theValue.theInt = theData;
	}
	return *this;
}

void JsonObject::pushBack(const char* theKey, JsonObject& other) {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theKey = theKey;
		this->theValues[theKey].theType = ValueType::Array;
		std::cout << "WERE HERE THIS IS IT! 0303" << std::endl;
		if (other.theValues.size() > 0) {
			for (auto& [key,value] : other.theValues) {
				std::cout << "WERE HERE THIS IS IT! 0202" << std::endl;
				this->theValues[theKey] .theArrayValues.emplace_back(value);
			}
		}
		else if (other.theArrayValues.size()>0) {
			for (auto& value: other.theArrayValues) {
				std::cout << "WERE HERE THIS IS IT! 0101" << std::endl;
				this->theValues[theKey].theArrayValues.emplace_back(value);
			}
		}
		else {
			this->theValues[theKey].theValue = other;
		}
	}
	else {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theKey = theKey;
		this->theValues[theKey].theType = ValueType::Array;
		std::cout << "WERE HERE THIS IS IT! 0404" << std::endl;
		if (other.theValues.size() > 0) { 
			for (auto& value : other.theValues) {
				this->theArrayValues.push_back(value.second);
			}
		}
		else {
			this->theValues[theKey].theValue = other;
		}
	}
};

void JsonObject::pushBack(const char*theKey, JsonObject&& other) {
	if (this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theKey = theKey;
		this->theValues[theKey].theType = ValueType::Array;
		if (other.theValues.size() > 0) {
			for (auto& value : other.theValues) {
				this->theArrayValues.push_back(value.second);
			}
		}
		else {
			this->theValues[theKey].theValue = other;
		}
	}
};

JsonObject::operator std::string() {
	std::string theString{};
	bool doWeAddComma{ false };
	std::cout << "THE SIZE: 02 " << "THE KEY (REAL): " << this->theKey << ", THE TYPE: " << (int32_t)this->theType << std::endl;
	if (this->theKey != "") {
		theString += "\"" + this->theKey + "\":";
	}
	for (auto& [key, value] : this->theValues) {
		if (doWeAddComma) {
			theString += ",";
		}
		if (value.theKey != "") {
			theString += "\"" + value.theKey + "\":";
		}
		switch (value.theType) {
		case ValueType::Object: {
			doWeAddComma = false;
			theString += "{";
			for (auto& [key, valueNew] : value.theValues) {
				if (doWeAddComma) {
					theString += ",";
				}
				theString += valueNew;
				doWeAddComma = true;
			}			
			theString += "}";
			break;
		}
		case ValueType::Array: {
			doWeAddComma = false;
			std::cout << "WERE HERE THIS IS IT! WITH THE ARRAY" << std::endl;
			theString += "[";
			std::cout << "WERE HERE THIS IS IT! WITH THE ARRAY 0101: " << value.theArrayValues.size() << std::endl;
			for (auto& valueNew : value.theArrayValues) {
				if (doWeAddComma) {
					theString += ",";
				}
				theString += valueNew;
				doWeAddComma = true;
			}
			theString += "]";
			break;
		}
		case ValueType::Bool: {

			if (value.theKey != "") {
				theString += "\"" + value.theKey + "\":";
			}
			std::stringstream theStream{};
			theStream << std::boolalpha << value.theValue.theBool;
			theString += theStream.str();
			break;
		}
		case ValueType::String: {

			if (value.theKey != "") {
				theString += "\"" + value.theKey + "\":";
			}
			theString += "\"";
			theString += *value.theValue.theString;
			theString += "\"";
			break;
		}case ValueType::Double: {
			if (value.theKey != "") {
				theString += "\"" + value.theKey + "\":";
			}
			theString += std::to_string(value.theValue.theDouble);
			break;
		}case ValueType::Uint64: {
			if (value.theKey != "") {
				theString += "\"" + value.theKey + "\":";
			}
			theString += std::to_string(value.theValue.theUint);
			break;
		}case ValueType::Int64: {
			if (value.theKey != "") {
				theString += "\"" + value.theKey + "\":";
			}
			theString += std::to_string(value.theValue.theInt);
			break;
		}
		}
		doWeAddComma = true;
	}
	return theString;
}

std::string JsonSerializer::getString(JsonObject theObject) {
	std::string theString{};
	theString += "{";
	theString += theObject;
	theString += "}";
	return theString;
}

	/// For editing the permissions of a single Guild ApplicationCommand. \brief For editing the permissions of a single Guild ApplicationCommand.
	struct EditGuildApplicationCommandPermissionsData {
		std::vector<DiscordCoreAPI::ApplicationCommandPermissionData> permissions{};///< A vector of ApplicationCommand permissions.
		std::string commandName{};///< The command name which you would like to edit the permissions of.
		DiscordCoreAPI::Snowflake applicationId{};///< The current application's Id (The Bot's User Id).
		uint64_t commandId{};///< The command id which you would like to edit the permissions of.
		DiscordCoreAPI::Snowflake guildId{};///< The Guild id of the Guild for which you would like to edit the command permissions.

		operator JsonObject();
	};

	struct WebSocketIdentifyData {
		DiscordCoreInternal::UpdatePresenceData presence{};
		int32_t largeThreshold{ 250 };
		int32_t numberOfShards{};
		int32_t currentShard{};
		std::string botToken{};
		int64_t intents{};

		operator JsonObject();
	};

	WebSocketIdentifyData::operator JsonObject() {
		JsonObject theSerializer{};
		theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
		theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);
		
		for (auto& value : this->presence.activities) {
			JsonObject theSerializer02{};
			if (value.url != "") {
				theSerializer02["url"] = std::string{ value.url };
			}
			theSerializer02["name"] = std::string{ value.name };
			theSerializer02["type"] = uint32_t{ static_cast<uint32_t>(value.type) };
			theSerializer.pushBack("activities", theSerializer02);
		}/*
		theSerializer.endArray();
		theSerializer.appendStructElement("afk", this->presence.afk);
		if (this->presence.since != 0) {
			theSerializer.appendStructElement("since", this->presence.since);
		}

		theSerializer.appendStructElement("status", this->presence.status);
		theSerializer.addNewStructure("properties");
		theSerializer.appendStructElement("browser", "DiscordCoreAPI");
		theSerializer.appendStructElement("device", "DiscordCoreAPI");
#ifdef _WIN32
		theSerializer.appendStructElement("os", "Windows");
#else
		theSerializer.appendStructElement("os", "Linux");
#endif
		theSerializer.endStructure();
		theSerializer.addNewArray("shard");
		theSerializer.appendArrayElement(static_cast<uint32_t>(this->currentShard));
		theSerializer.appendArrayElement(static_cast<uint32_t>(this->numberOfShards));
		theSerializer.endArray();
		theSerializer.appendStructElement("token", this->botToken);
		theSerializer.endStructure();
		theSerializer.appendStructElement("op", static_cast<uint32_t>(2));
		*/
		return theSerializer;
	}

	EditGuildApplicationCommandPermissionsData::operator JsonObject() {
		JsonObject theData{};
		theData["d"] = true;
		theData["23"]["TESTty"]["TEST_TWO"] = true;
		theData["23"]["TEST"] = true;
		theData["Teetertytot"] = "TESTING";
		theData["test"] = "TESTING VALUES";
		theData["TEST"] = false;
		theData["tee"]["TESTerasds"]["TESTINGiners"] = double{ 4.423f };
		for (auto& value : this->permissions) {
			
			
			//newData["d"]["type"] = static_cast<uint8_t>(value.type);
			//auto theString = std::to_string(value.id);
			//newData["d"]["id"] = theString;
		}
		//std::cout << "THE STRING: " << theData.getString() << std::endl;
		
		return theData;
	}


    int32_t main() noexcept {
		try {
			EditGuildApplicationCommandPermissionsData theDataBew{};
			theDataBew.applicationId = 12312312323;
			//std::string theString = JsonSerializer{}.getString(theDataBew);
			//std::cout << "THE FINAL STRING: 0101 " << theString << std::endl;
			//std::cout << "THE FINAL STRING (PARSED): " << nlohmann::json::parse(theString).dump() << std::endl;

			WebSocketIdentifyData theDataReal{};
			DiscordCoreAPI::ActivityData theData{};
			theData.name = "TESTING!";
			theDataReal.presence.activities.push_back(theData);
			theDataReal.botToken = "12312312323";
			std::string theString02 = JsonSerializer{}.getString(theDataReal);
			std::cout << "THE FINAL STRING: 0101 " << theString02 << std::endl;
			std::cout << "THE FINAL STRING (PARSED): " << nlohmann::json::parse(theString02).dump() << std::endl;

			std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        }
        catch (...) { DiscordCoreAPI::reportException("main()"); };

        return 0;
    }
