#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"

JsonValue& JsonValue::operator=(const JsonValue&other){
	if (other.theArray) {
		this->theArray = std::make_unique<JsonArray>();
		*this->theArray = *other.theArray;
	} 
	else if (other.theObject) {
		this->theObject= std::make_unique<JsonObject>();
		*this->theObject = *other.theObject;
	}
	else if (other.theString) {
		this->theString = std::make_unique<std::string>();
		*this->theString = *other.theString;
	}
	this->theBool = other.theBool;
	this->theDouble = other.theDouble;
	this->theFloat = other.theFloat;
	this->theInt = other.theInt;
	this->theUint = other.theUint;
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

JsonValue::JsonValue(ValueType theType) {
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
}

JsonValue::~JsonValue() {}

JsonObject::JsonObject(ValueType theType) noexcept {
	this->theValue = theType;
	this->theType = theType;
}

JsonObject::JsonObject(const JsonObject& theKey) {
	*this = theKey;
}

JsonObject& JsonObject::operator=(const char* theData) {
	if (this->theType == ValueType::Unset) {
		this->theType = ValueType::String;
		this->theValue.theBool = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE VALUE: " << this->theValue.theBool << std::endl;
		return *this;
	}
	else {
		JsonObject theObject{ ValueType::String };
		*theObject.theValue.theString = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE VALUE: " << theObject.theValue.theBool << std::endl;
		this->theValues[this->theKey] = theObject;
		return this->theValues[this->theKey];
	}
}

JsonObject& JsonObject::operator=(const JsonObject& theKey) {
	for (auto& [key, value] : theKey.theValues) {
		this->theValues[key] = value;
	}
	this->theKey = theKey.theKey;
	this->theValue = theKey.theValue;
	this->theType = theKey.theType;
	return *this;
}

JsonObject& JsonObject::operator[](const char* theKey) {
	if (this->theKey == "") {
		std::cout << "THE KEY NAME: 0101: " << theKey << std::endl;
		this->theKey = theKey;
		this->theType = ValueType::Object;
		return *this;
	} else if (this->theKey == theKey) {
		std::cout << "THE KEY NAME: 0202: " << theKey << std::endl;
		return *this;
	} else if (this->theValues.contains(theKey)){
		std::cout << "THE KEY NAME: 0303: " << theKey << std::endl;
		return this->theValues[theKey];
	} else if (!this->theValues.contains(theKey)) {
		JsonObject theObject{};
		theObject.theKey = theKey;
		theObject.theType = ValueType::Object;
		std::cout << "THE KEY NAME: 0303: " << theKey << std::endl;
		this->theValues[theKey] = theObject;
		return this->theValues[theKey];
	}
	else {
		std::cout << "THE KEY NAME: 030404: " << theKey << std::endl;
		JsonObject theObject{};
		theObject.theType = ValueType::Unset;
		theObject.theKey = theKey;
		this->theValues[theKey] = theObject;
		return this->theValues[theKey];
	}
}
/*
JsonObject& JsonObject::operator=(JsonObject theData) {
	//this->theValues.emplace(theData.theKey, theData);
	//std::cout << "THE KEY NAME: 0101: " << this->theKey << std::endl;
	return *this;
}
JsonObject::JsonObject(bool theData) {
	std::cout << "THE DATA: 0101: " << theData << ", THE KEY: " << this->theKey << std::endl;
	*this = theData;
}


JsonObject& JsonObject::operator=(const char* theData) {
	std::cout << "THE KEY NAME: 0101: " << this->theKey << std::endl;
	if (this->theType == ValueType::Object) {
		JsonObject theObject{ ValueType::String };
		*theObject.theValue.theString = theData;
		this->theValues[this->theKey] = theObject;
		return this->theValues[this->theKey];
	}
	return *this;
}
*/
JsonObject& JsonObject::operator=(bool theData) {
	
	if (this->theType == ValueType::Unset) {
		this->theType = ValueType::Bool;
		this->theValue.theBool = theData;
		std::cout << "THE KEY NAME: 233434: " << this->theKey << "THE VALUE: " << this->theValue.theBool << std::endl;
		return *this;
	}
	else {
		JsonObject theObject{ ValueType::Bool };
		theObject.theValue.theBool = theData;
		std::cout << "THE KEY NAME: 344545: " << this->theKey << "THE VALUE: " << theObject.theValue.theBool << std::endl;
		this->theValues[this->theKey] = theObject;
		return this->theValues[this->theKey];
	}
}

	std::string JsonSerializer::getString(JsonObject theObject) {
		std::string theString{};
		bool doWeAddComma{ false };

		if (theObject.theKey != "") {
			theString += "{";
			theString += "\"" + theObject.theKey + "\":";
		}
		for (auto& [key, value] : theObject.theValues) {
			switch (value.theType) {
			case ValueType::Object: {
			
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
			}

			if (theObject.theKey != "") {
				theString += "}";
			}
		}
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
		theData["d"]["test"]["RESULTS"] = true;
		theData["d"]["test"]["RESULTS"] = false;
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
			std::string theString = JsonSerializer{}.getString(theDataBew);
			std::cout << "THE FINAL STRING: 0101 " << theString << std::endl;
			std::cout << "THE FINAL STRING (PARSED): " << nlohmann::json::parse(theString).dump() << std::endl;

			std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        }
        catch (...) { DiscordCoreAPI::reportException("main()"); };

        return 0;
    }
