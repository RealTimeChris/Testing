#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"

JsonObject& JsonObject::operator=(const ValueType& theType) {
	switch (theType) {
	case ValueType::Bool: {
		this->theValue = new bool{};
		break;
	}
	case ValueType::Double: {
		this->theValue = new double{};
		break;
	}
	case ValueType::Float: {
		this->theValue = new float{};
		break;
	}
	case ValueType::Uint64: {
		this->theValue = new uint64_t{};
		break;
	}
	case ValueType::Int64: {
		this->theValue = new int64_t{};
		break;
	}
	case ValueType::String: {
		this->theValue = new std::string{};
		break;
	}
	case ValueType::Null: {
		this->theValue = new nullptr_t{};
		break;
	}
	case ValueType::Array: {
		this->theValue = new JsonArray{};
		break;
	}
	case ValueType::Object: {
		this->theValue = new JsonObject{};
		break;
	}
	case ValueType::Unset: {
		break;
	}
	}
	return *this;
}

JsonObject::JsonObject(const ValueType& theType){
	*this = theType;
}

JsonObject::JsonObject(const char* theData) noexcept {
	this->theValue = new std::string{};
	*static_cast<std::string*>(this->theValue) = theData;
}

JsonObject::JsonObject(std::string theData) noexcept {
	this->theValue = new std::string{};
	*static_cast<std::string*>(this->theValue) = theData;
}

JsonObject::JsonObject(uint64_t theData) noexcept {
	this->theValue = new uint64_t{};
	*static_cast<uint64_t*>(this->theValue) = theData;
}

JsonObject::JsonObject(int64_t theData) noexcept {
	this->theValue = new uint64_t{};
	*static_cast<int64_t*>(this->theValue) = theData;
}

JsonObject::JsonObject(uint32_t theData) noexcept {
	this->theValue = new uint32_t{};
	*static_cast<uint32_t*>(this->theValue) = theData;
}

JsonObject::JsonObject(int32_t theData) noexcept {
	this->theValue = new uint32_t{};
	*static_cast<int32_t*>(this->theValue) = theData;
}

JsonObject::JsonObject(uint16_t theData) noexcept {
	this->theValue = new uint16_t{};
	*static_cast<uint16_t*>(this->theValue) = theData;
}

JsonObject::JsonObject(int16_t theData) noexcept {
	this->theValue = new uint16_t{};
	*static_cast<int16_t*>(this->theValue) = theData;
}

JsonObject::JsonObject(uint8_t theData) noexcept {
	this->theValue = new uint8_t{};
	*static_cast<uint8_t*>(this->theValue) = theData;
}

JsonObject::JsonObject(int8_t theData) noexcept {
	this->theValue = new uint8_t{};
	*static_cast<int8_t*>(this->theValue) = theData;
}

JsonObject::JsonObject(double theData) noexcept {
	this->theValue = new double{};
	*static_cast<double*>(this->theValue) = theData;
}

JsonObject::JsonObject(float theData) noexcept {
	this->theValue = new float{};
	*static_cast<float*> (this->theValue) = theData;
}

JsonObject::JsonObject(bool theData) noexcept {
	this->theValue = new bool{};
	*static_cast<bool*> (this->theValue) = theData;
}

JsonObject& JsonObject::operator=(const JsonArray& theData){
	this->theKey = theData.theKey;
	this->theType = theData.theType;
	this->theValue = theData.theValue;
	for (auto& [key, value] : theData.theValues) {
		this->theValues[key] = value;
	}
	return *this;
}

JsonObject::JsonObject(const JsonArray& theData){
	*this = theData;
}

JsonObject& JsonObject::operator=(const JsonObject& theKey) {
	for (auto& [key, value] : theKey.theValues) {
		this->theValues[key] = value;
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
	this->theType = ValueType::String;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<std::string*>(this->theValue) = theData;
	return *this;
}

JsonObject& JsonObject::operator=(std::string theData) {
	this->theType = ValueType::String;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<std::string*>(this->theValue) = theData;
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
	this->theType = ValueType::Bool;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<bool*>(this->theValue) = theData;
	return *this;
}

JsonObject& JsonObject::operator=(double theData) {
	this->theType = ValueType::Double;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<double*>(this->theValue) = theData;
	return *this;
}

JsonObject& JsonObject::operator=(float theData) {
	this->theType = ValueType::Float;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<float*>(this->theValue) = theData;
	return *this;
}

JsonObject& JsonObject::operator=(uint64_t theData) {
	this->theType = ValueType::Uint64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject& JsonObject::operator=(uint32_t theData) {
	this->theType = ValueType::Uint64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject& JsonObject::operator=(uint16_t theData) {
	this->theType = ValueType::Uint64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject& JsonObject::operator=(uint8_t theData) {
	this->theType = ValueType::Uint64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject& JsonObject::operator=(int64_t theData) {
	this->theType = ValueType::Int64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<int64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject& JsonObject::operator=(int32_t theData) {
	this->theType = ValueType::Int64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<int64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject& JsonObject::operator=(int16_t theData) {
	this->theType = ValueType::Int64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<int64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject& JsonObject::operator=(int8_t theData) {
	this->theType = ValueType::Int64;
	*this = this->theType;
	*static_cast<int64_t*>(this->theValue) = theData;
	return *this;
}

void JsonObject::pushBack(const char* theKey, JsonObject other) {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = std::move(theKey);
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = std::move(other);
	}
	else {
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = std::move(other);
	}
};

void JsonObject::pushBack(const char* theKey, std::string other) {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else{
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint8_t other) {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint16_t other) {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint32_t other) {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint64_t other){
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

JsonObject::operator std::string() {
	std::string theString{};
	if (this->theKey != "") {
		theString += "\"" + this->theKey + "\":";
	}
	switch (this->theType) {
	case ValueType::Object: {
		bool doWeAddComma{ false };
		theString += "{";
		for (auto& [key, valueNew] : this->theValues) {
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
		bool doWeAddComma{ false };
		theString += "[";
		for (auto& [key, valueNew] : this->theValues) {
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
		std::stringstream theStream{};
		theStream << std::boolalpha << *static_cast<bool*>(this->theValue);
		theString += theStream.str();
		break;
	}
	case ValueType::String: {
		theString += "\"";
		theString += *static_cast<std::string*>(this->theValue);
		theString += "\"";
		break;
	}case ValueType::Double: {
		theString += *static_cast<double*>(this->theValue);
		break;
	}case ValueType::Uint64: {
		theString += std::to_string(*static_cast<uint64_t*>(this->theValue));
		break;
	}case ValueType::Int64: {
		theString += std::to_string(*static_cast<int64_t*>(this->theValue));
		break;
	}
	}
	return theString;
}

std::string JsonSerializer::getString(JsonObject theObject) {
	return theObject;
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
			theSerializer["d"].pushBack("activities", theSerializer02);
			theSerializer["d"].pushBack("activities", theSerializer02);
		}
		theSerializer["d"]["afk"] = this->presence.afk;
		if (this->presence.since != 0) {
			theSerializer["since"] = this->presence.since;
		}

		theSerializer["d"]["status"] = this->presence.status;
		theSerializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
		theSerializer["d"]["properties"]["device"] = "DiscordCoreAPI";
#ifdef _WIN32
		theSerializer["d"]["properties"]["os"] = "Windows";
#else
		theSerializer["d"]["properties"]["os"] = "Linux";
#endif
		theSerializer["d"].pushBack("shard", static_cast<uint32_t>(this->currentShard));
		theSerializer["d"].pushBack("shard", static_cast<uint32_t>(this->numberOfShards));
		theSerializer["d"]["token"] = this->botToken;
		theSerializer["op"] = static_cast<uint32_t>(2);
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
