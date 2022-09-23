#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"

JsonValue& JsonValue::operator=(const JsonValue&other){
	switch (other.theType) {
	case ValueType::Array: {
		this->theArray = std::make_unique<JsonArray>();
		for (auto& value : other.theArray->theArrayValues) {
			this->theArray->theArrayValues.push_back(value);
		}
		break;
	}
	case ValueType::Object: {
		this->theObject = std::make_unique<JsonObject>();
		*this->theObject = *other.theObject;
		break;
	}
	case ValueType::Uint64: {
		this->theUint = other.theUint;
		break;
	}
	case ValueType::String: {
		this->theString = std::make_unique<std::string>();
		*this->theString = *other.theString;
		break;
	}
	case ValueType::Int64: {
		this->theInt = other.theInt;
		break;
	}
	case ValueType::Double: {
		this->theDouble = other.theDouble;
		break;
	}
	case ValueType::Float: {
		this->theFloat = other.theFloat;
		break;
	}
	case ValueType::Bool: {
		this->theBool = other.theBool;
		break;
	}
	case ValueType::Null: {
		this->theNull = other.theNull;
		break;
	}
	}
	this->theType = other.theType;
	return *this;
}

JsonValue::JsonValue(const JsonValue&other) {
	*this = other;
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

JsonValue& JsonValue::operator=(ValueType theType) {
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
	}
	return *this;
}

JsonValue::JsonValue(ValueType theType) {
	*this = theType;
}

JsonValue::~JsonValue() {}

JsonObject::JsonObject(ValueType theType) noexcept {
	this->theValue = theType;
	this->theType = theType;
}

JsonObject::JsonObject(const JsonObject& theKey) {
	*this = theKey;
}

JsonObject& JsonObject::operator=(const JsonObject& theKey) {
	for (auto& [key, value] : theKey.theValues) {
		this->theValues[key] = value;
	}
	this->theValue = theKey.theValue;
	this->theType = theKey.theType;
	this->theValue = this->theType;
	this->theKey = theKey.theKey;
	std::cout << "THE KEY NAME: 234234: " << theKey.theKey << std::endl;
	std::cout << "THE KEY NAME: 234234: (BOOL) " << theKey.theValue.theBool << std::endl;
	if (theKey.theValue.theString) {
		std::cout << "THE KEY NAME: 234234 (STRING): " << theKey.theValue.theString << std::endl;
	}
	return *this;
}

JsonObject& JsonObject::operator[](const char* theKey) {
	if (this->theKey == "") {
		JsonObject theObject{};
		theObject.theKey = theKey;
		theObject.theType = ValueType::Object;
		std::cout << "THE KEY NAME: 0303: " << theKey << std::endl;
		this->theValues[theKey] = theObject;
		return this->theValues[theKey];
	}
	else if (this->theKey == theKey && this->theType == ValueType::Object) {
		std::cout << "THE KEY NAME: 0202: " << theKey << std::endl;
		return *this;
	}
	else if (!this->theValues.contains(theKey)) {
		JsonObject theObject{};
		theObject.theKey = theKey;
		theObject.theType = ValueType::Object;
		std::cout << "THE KEY NAME: 0303: " << theKey << std::endl;
		this->theValues[theKey] = theObject;
		return this->theValues[theKey];
	}
	else if (this->theValues.contains(theKey)) {
		std::cout << "THE KEY NAME: 0303: " << theKey << std::endl;
		return this->theValues[theKey];
	}
	else {
		std::cout << "THE KEY NAME: 030404: " << theKey << std::endl;
		JsonObject theObject{};
		theObject.theType = ValueType::Object;
		theObject.theKey = theKey;
		this->theValues[theKey] = theObject;
		return this->theValues[theKey];
	}
}

JsonObject& JsonObject::operator=(const char* theData) {
	if (this->theType == ValueType::Object) {
		this->theType = ValueType::String;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		*this->theValue.theString = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << *this->theValue.theString<< std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::String;
		this->theValue = this->theType;
		*this->theValue.theString = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << *this->theValue.theString << std::endl;
		return *this;
	}
}

JsonObject& JsonObject::operator=(std::string theData) {
	if (this->theType == ValueType::Object) {
		this->theType = ValueType::String;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		*this->theValue.theString = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << *this->theValue.theString << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Bool;
		this->theValue = this->theType;
		*this->theValue.theString = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << *this->theValue.theString << std::endl;
		return *this;
	}
}

JsonObject& JsonObject::operator=(bool theData) {
	
	if (this->theType == ValueType::Object) {
		JsonObject theObject{};
		this->theType = ValueType::Bool;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		this->theValue.theBool = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << theObject.theValue.theBool << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Bool;
		this->theValue = this->theType;
		this->theValue.theBool = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << this->theValue.theBool << std::endl;
		return *this;
	}

}

JsonObject& JsonObject::operator=(double theData) {

	if (this->theType == ValueType::Object) {
		JsonObject theObject{};
		this->theType = ValueType::Double;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		this->theValue.theDouble = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << theObject.theValue.theDouble << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Double;
		this->theValue = this->theType;
		this->theValue.theDouble = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << this->theValue.theDouble << std::endl;
		return *this;
	}

}

JsonObject& JsonObject::operator=(float theData) {

	if (this->theType == ValueType::Object) {
		JsonObject theObject{};
		this->theType = ValueType::Float;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		this->theValue.theFloat = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << theObject.theValue.theFloat << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Float;
		this->theValue = this->theType;
		this->theValue.theFloat = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << this->theValue.theFloat << std::endl;
		return *this;
	}

}

JsonObject& JsonObject::operator=(uint64_t theData) {

	if (this->theType == ValueType::Object) {
		JsonObject theObject{};
		this->theType = ValueType::Uint64;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << theObject.theValue.theUint << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Uint64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << this->theValue.theUint << std::endl;
		return *this;
	}

}

JsonObject& JsonObject::operator=(uint32_t theData) {

	if (this->theType == ValueType::Object) {
		JsonObject theObject{};
		this->theType = ValueType::Uint64;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << theObject.theValue.theUint << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Uint64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << this->theValue.theUint << std::endl;
		return *this;
	}

}

JsonObject& JsonObject::operator=(uint16_t theData) {

	if (this->theType == ValueType::Object) {
		JsonObject theObject{};
		this->theType = ValueType::Uint64;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int16_t)this->theType << "THE VALUE : " << theObject.theValue.theUint << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Uint64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int16_t)this->theType << "THE VALUE : " << this->theValue.theUint << std::endl;
		return *this;
	}

}

JsonObject& JsonObject::operator=(uint8_t theData) {

	if (this->theType == ValueType::Object) {
		JsonObject theObject{};
		this->theType = ValueType::Uint64;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int8_t)this->theType << "THE VALUE : " << theObject.theValue.theUint << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Uint64;
		this->theValue = this->theType;
		this->theValue.theUint = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int8_t)this->theType << "THE VALUE : " << this->theValue.theUint << std::endl;
		return *this;
	}

}

JsonObject& JsonObject::operator=(int64_t theData) {

	if (this->theType == ValueType::Object) {
		JsonObject theObject{};
		this->theType = ValueType::Int64;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		this->theValue.theInt = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << theObject.theValue.theInt << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Int64;
		this->theValue = this->theType;
		this->theValue.theInt = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << this->theValue.theInt << std::endl;
		return *this;
	}

}

JsonObject& JsonObject::operator=(int32_t theData) {

	if (this->theType == ValueType::Object) {
		JsonObject theObject{};
		this->theType = ValueType::Int64;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		this->theValue.theInt = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << theObject.theValue.theInt << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Int64;
		this->theValue = this->theType;
		this->theValue.theInt = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int32_t)this->theType << "THE VALUE : " << this->theValue.theInt << std::endl;
		return *this;
	}

}

JsonObject& JsonObject::operator=(int16_t theData) {

	if (this->theType == ValueType::Object) {
		JsonObject theObject{};
		this->theType = ValueType::Int64;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		this->theValue.theInt = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int16_t)this->theType << "THE VALUE : " << theObject.theValue.theInt << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Int64;
		this->theValue = this->theType;
		this->theValue.theInt = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int16_t)this->theType << "THE VALUE : " << this->theValue.theInt << std::endl;
		return *this;
	}

}

JsonObject& JsonObject::operator=(int8_t theData) {

	if (this->theType == ValueType::Object) {
		JsonObject theObject{};
		this->theType = ValueType::Int64;
		this->theKey = this->theKey;
		this->theValue = this->theType;
		this->theValue.theInt = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE EVENT: " << (int8_t)this->theType << "THE VALUE : " << theObject.theValue.theInt << std::endl;
		return *this;
	}
	else {

		this->theType = ValueType::Int64;
		this->theValue = this->theType;
		this->theValue.theInt = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE EVENT: " << (int8_t)this->theType << "THE VALUE : " << this->theValue.theInt << std::endl;
		return *this;
	}

}


void JsonObject::pushBack(JsonArray&& theData) {
	this->theValue = ValueType::Array;
	*this->theValue.theArray = std::move(theData);
}
void JsonObject::pushBack(JsonArray& theData) {
	this->theValue = ValueType::Array;
	*this->theValue.theArray = std::move(theData);
}

JsonObject::operator std::string() {
	std::string theString{};
	theString += "{";
	bool doWeAddComma{ false };
	for (auto& [key, value] : this->theValues) {
		if (doWeAddComma) {
			theString += ",";
		}

		switch (value.theType) {
		case ValueType::Object: {
			if (value.theKey != "") {
				theString += "\"" + value.theKey + "\":";
			}
			theString += JsonSerializer{}.getString(value);
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
		}
		}
		doWeAddComma = true;
	}
	return theString;
}
std::string JsonSerializer::getString(JsonObject theObject) {
	std::string theString = theObject;/*
	bool doWeAddComma{ false };
	for (auto& [key, value] : theObject.theValues) {
		if (doWeAddComma) {
			theString += ",";
		}
		
		switch (value.theType) {
		case ValueType::Object: {
			if (value.theKey != "") {
				theString += "\"" + value.theKey + "\":";
			}
			theString += JsonSerializer{}.getString(value);
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
		}
		}
		if (theObject.theType == ValueType::Object) {
			theString += "}";
		}		
		
		doWeAddComma = true;
	}*/
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

	EditGuildApplicationCommandPermissionsData::operator JsonObject() {
		JsonObject theData{};
		theData["d"] = true;
		theData["23"]["TESTty"]["TEST_TWO"] = true;
		theData["23"]["TEST"] = true;
		theData["Teetertytot"] = "TESTING";
		theData["test"] = "TESTING VALUES";
		theData["TEST"] = false;
		theData["tee"]["TESTerasds"]["TESTINGiners"] = double{ 4.423f };
		//std::cout << "THE STRING: " << theData.getString() << std::endl;
		
		return theData;
	}


    int32_t main() noexcept {
		try {
			EditGuildApplicationCommandPermissionsData theDataBew{};
			theDataBew.applicationId = 12312312323;
			std::string theString = JsonSerializer{}.getString(theDataBew);
			std::cout << "THE FINAL STRING: 0101 " << theString << std::endl;
			std::cout << "THE FINAL STRING (PARSED): " << nlohmann::json::parse(theString).dump() << std::endl;

			std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        }
        catch (...) { DiscordCoreAPI::reportException("main()"); };

        return 0;
    }
