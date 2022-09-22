#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"

JsonValue::JsonValue(std::string theData) noexcept {
	this->theString = std::make_unique<std::string>().release();
	*this->theString = theData;
}

JsonValue::JsonValue(JsonObject theData) noexcept {
	this->theObject = std::make_unique<JsonObject>().release();
	*this->theObject = theData;
}

JsonValue::JsonValue(JsonArray theData) noexcept {
	this->theArray = std::make_unique<JsonArray>().release();
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
	}
	case ValueType::Double: {
		this->theDouble = 0.0f;
	}
	case ValueType::Float: {
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
	return *this;
}

JsonValue::~JsonValue() {
	if (this->theArray) {
		delete this->theArray;
	}
	else if (this->theObject) {
		delete this->theObject;
	}
	else if (this->theString) {
		delete this->theString;
	}
}

JsonValue::JsonValue(ValueType theType){
	*this = theType;
}

JsonObject::JsonObject(const char* theKey)noexcept {
	this->theKey = theKey;
}

JsonObject& JsonObject::operator[](const char* theKey) {
	std::cout << "THE KEY NAME: 0202: " << theKey << std::endl;
	if (this->theKey == "") {
		this->theKey = theKey;
		this->theType = ValueType::Object;
	}
	return *this;
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


JsonObject& JsonObject::operator=(bool theData) {
	//this->theType = ValueType::Bool;
	//this->theValue.theBool = theData;
	//std::cout << "THE KEY NAME: 0303: " << this->theKey << std::endl;
	return *this;
}
*/
	std::string JsonSerializer::getString(JsonObject theObject) {
		std::string theString{};
		bool doWeAddComma{ false };

		if (theObject.theKey != "") {
			theString += "{";
			theString += "\"" + theObject.theKey + "\":";
			theString += "{";
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
		theData["d"] = "TEST";
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
			

			std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        }
        catch (...) { DiscordCoreAPI::reportException("main()"); };

        return 0;
    }
