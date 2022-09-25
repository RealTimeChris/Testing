#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"

EnumConverter& EnumConverter::operator=(EnumConverter& other) noexcept {
	this->thePtr = other.thePtr;
	other.thePtr = nullptr;
	this->vectorType = other.vectorType;
	return *this;
}

EnumConverter::EnumConverter(EnumConverter& other) noexcept {
	*this = other;
}

EnumConverter& EnumConverter::operator=(EnumConverter&& other) noexcept {
	this->thePtr = other.thePtr;
	other.thePtr = nullptr;
	this->vectorType = other.vectorType;
	return *this;
}

EnumConverter::EnumConverter(EnumConverter&& other) noexcept {
	*this = other;
}
/*
JsonObject& JsonObject::operator=(EnumConverter theData) noexcept {
	EnumConverter theConverter{ theData };
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::Uint64;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue.theValue) = static_cast<uint64_t>(theConverter);
	return *this;
}

JsonObject::JsonObject(EnumConverter theData) noexcept {
	this->theValue.theValue = new EnumConverter{ DiscordCoreAPI::ChannelType::Dm };
	*static_cast<EnumConverter*>(this->theValue.theValue) = theData;
}
*/
JsonObject& JsonObject::operator=(const JsonObject& theKey) noexcept {
	
	if (this->theValue.theObject.theType == ValueType::Object) {
		for (auto& [key, value] : theKey.theValue.theObject.theValues) {
			std::cout << "THE MIXED KEY: " << key << ", THE MIXED TYPE: " << (int32_t)value.theValue.theObject.theType << std::endl;
			this->theValue.theObject.theValues.emplace(key, value);
		}
		std::cout << "THE OTHER KEY: " << theKey.theValue.theObject.theKey << ", THE OTHER TYPE: " << (int32_t)theKey.theValue.theObject.theType << std::endl;
		*static_cast<JsonObject*>(this->theValue.theObject.theValue) = *static_cast<JsonObject*>(theKey.theValue.theObject.theValue);
		this->theValue.theObject.theType = theKey.theValue.theObject.theType;
		this->theValue.theObject.theKey = theKey.theValue.theObject.theKey;
		return *this;
	}
	else {
		
		std::cout << "THE OTHER KEY: " << theKey.theValue.theObject.theKey << ", THE OTHER TYPE: " << (int32_t)theKey.theValue.theObject.theType << std::endl;
		this->theValue.theObject.theValue = theKey.theValue.theObject.theValue;
		this->theValue.theObject.theType = theKey.theValue.theObject.theType;
		this->theValue.theObject.theKey = theKey.theValue.theObject.theKey;
		return *this;
	}
	
	
}

JsonObject::JsonObject(std::source_location theLocation)noexcept {
	this->theValue.theObject.theType = ValueType::Object;
	this->theValue.theObject.createPtr(this->theValue.theObject.theType);
	std::cout << "WERE BEING CONSTRUCTED RAW! AT: " << theLocation.line() << ", COLUMN: " << theLocation.column() << ", IN FILE: " << theLocation.file_name() << std::endl;
	this->areWeTopLevel = true;
}

JsonObject::JsonObject(const JsonObject& theKey) noexcept {
	*this = theKey;
}

void ObjectType::createPtr(ValueType theType) noexcept {
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
		this->theValue = new JsonObject::JsonValue{};
		break;
	}
	case ValueType::Unset: {
		break;
	}
	}
}

/*
JsonObject& JsonObject::operator=(const JsonArray& theData) noexcept {
	this->theValue.theKey = theData.theKey;
	this->theValue.theType = theData.theType;
	this->theValue.theValue = theData.theValue;
	for (auto& [key, value] : theData.theValues) {
		this->theValue.theValues[key] = value;
	}
	return *this;
}

JsonObject::JsonObject(const JsonArray& theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(const char* theData) noexcept {
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::String;
	*this = theObject;
	*static_cast<std::string*>(this->theValue.theValue) = theData;
	return *this;
}

JsonObject::JsonObject(const char* theData) noexcept {
	this->theValue.theValue = new std::string{};
	*static_cast<std::string*>(this->theValue.theValue) = theData;
}
*/
void JsonObject::operator=(std::string theData) noexcept {
	this->theValue.theObject.theValue = new std::string{};
	this->theValue.theObject.theType = ValueType::String;
	std::cout << "THE RIGHT KEY: 050505 " << this->theValue.theObject.theKey << std::endl;
	*static_cast<std::string*>(this->theValue.theObject.theValue) = theData;
}

void JsonObject::operator=(std::nullptr_t theData) noexcept{
	this->theValue.theObject.theValue = new nullptr_t{};
	this->theValue.theObject.theType = ValueType::Null;
	std::cout << "THE RIGHT KEY: 050505 " << this->theValue.theObject.theKey << std::endl;
	*static_cast<nullptr_t*>(this->theValue.theObject.theValue) = theData;
}

void JsonObject::operator=(uint64_t theData) noexcept {
	this->theValue.theObject.theValue = new uint64_t{};
	this->theValue.theObject.theType = ValueType::Uint64;
	std::cout << "THE RIGHT KEY: 050505 " << this->theValue.theObject.theKey << std::endl;
	*static_cast<uint64_t*>(this->theValue.theObject.theValue) = theData;
}


/*
JsonObject& JsonObject::operator=(uint32_t theData) noexcept {
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::Uint64;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue.theValue) = theData;
	return *this;
}

JsonObject::JsonObject(uint32_t theData) noexcept {
	this->theValue.theValue = new uint32_t{};
	*static_cast<uint32_t*>(this->theValue.theValue) = theData;
}

JsonObject& JsonObject::operator=(uint16_t theData) noexcept {
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::Uint64;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue.theValue) = theData;
	return *this;
}

JsonObject::JsonObject(uint16_t theData) noexcept {
	this->theValue.theValue = new uint16_t{};
	*static_cast<uint16_t*>(this->theValue.theValue) = theData;
}

JsonObject& JsonObject::operator=(uint8_t theData) noexcept {
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::Uint64;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue.theValue) = theData;
	return *this;
}

JsonObject::JsonObject(uint8_t theData) noexcept {
	this->theValue.theValue = new uint8_t{};
	*static_cast<uint8_t*>(this->theValue.theValue) = theData;
}

JsonObject& JsonObject::operator=(int64_t theData) noexcept {
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::Uint64;
	*this = theObject;
	*static_cast<int64_t*>(this->theValue.theValue) = theData;
	return *this;
}

JsonObject::JsonObject(int64_t theData) noexcept {
	this->theValue.theValue = new uint64_t{};
	*static_cast<int64_t*>(this->theValue.theValue) = theData;
}

JsonObject& JsonObject::operator=(int32_t theData) noexcept {
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::Int64;
	*this = theObject;
	*static_cast<int64_t*>(this->theValue.theValue) = theData;
	return *this;
}

JsonObject::JsonObject(int32_t theData) noexcept {
	this->theValue.theValue = new uint32_t{};
	*static_cast<int32_t*>(this->theValue.theValue) = theData;
}

JsonObject& JsonObject::operator=(int16_t theData) noexcept {
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::Int64;
	*this = theObject;
	*static_cast<int64_t*>(this->theValue.theValue) = theData;
	return *this;
}

JsonObject::JsonObject(int16_t theData) noexcept {
	this->theValue.theValue = new uint16_t{};
	*static_cast<int16_t*>(this->theValue.theValue) = theData;
}

JsonObject& JsonObject::operator=(int8_t theData) noexcept {
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::Int64;
	*this = theObject;
	*static_cast<int64_t*>(this->theValue.theValue) = theData;
	return *this;
}

JsonObject::JsonObject(int8_t theData) noexcept {
	this->theValue.theValue = new uint8_t{};
	*static_cast<int8_t*>(this->theValue.theValue) = theData;
}

JsonObject& JsonObject::operator=(double theData) noexcept {
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::Double;
	*this = theObject;
	*static_cast<double*>(this->theValue.theValue) = theData;
	return *this;
}

JsonObject::JsonObject(double theData) noexcept {
	this->theValue.theValue = new double{};
	*static_cast<double*>(this->theValue.theValue) = theData;
}

JsonObject& JsonObject::operator=(float theData) noexcept {
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::Float;
	*this = theObject;
	*static_cast<float*>(this->theValue.theValue) = theData;
	return *this;
}

JsonObject::JsonObject(float theData) noexcept {
	this->theValue.theValue = new float{};
	*static_cast<float*> (this->theValue.theValue) = theData;
}

JsonObject& JsonObject::operator=(bool theData) noexcept {
	JsonObject theObject{ theData };
	theObject.theKey = this->theValue.theKey;
	theObject.theType = ValueType::Bool;
	*this = theObject;
	*static_cast<bool*>(this->theValue.theValue) = theData;
	return *this;
}


JsonObject::JsonObject(bool theData) noexcept {
	this->theValue.theValue = new bool{};
	*static_cast<bool*> (this->theValue.theValue) = theData;
}
*/

JsonObject::JsonObject(const char* keyName, ValueType theType) {
	this->theValue.theObject.theKey = keyName;
	this->theValue.theObject.theType = theType;
	this->theValue.theObject.createPtr(this->theValue.theObject.theType);
}

JsonObject& JsonObject::operator[](const char* theKey) noexcept {
	if (this->areWeTopLevel) {
		if (this->theValue.theObject.theValues.contains(theKey)) {
			this->theValue.theObject.theValues.emplace(theKey, JsonObject{ theKey,ValueType::Object });
			return this->theValue.theObject.theValues.find(theKey).operator*().second;
		}
		else {
			this->theValue.theObject.theValues.emplace(theKey, JsonObject{ theKey,ValueType::Object });
			return this->theValue.theObject.theValues.find(theKey).operator*().second;
		}
	}
	else if (this->theValue.theObject.theKey == theKey) {
		return *this;
	}
	else {
		this->theValue.theObject.theValues.emplace(theKey, JsonObject{ theKey,ValueType::Object });
		return this->theValue.theObject.theValues.find(theKey).operator*().second;
	}
}

JsonObject::operator std::string() noexcept {
	std::string theString{};
	if (this->theValue.theObject.theType == ValueType::Object && this->areWeTopLevel) {
		theString += "{";
	}
	if (this->theValue.theObject.theKey != "") {
		theString += "\"" + this->theValue.theObject.theKey + "\":";
	}std::cout << "THE STRING KEY: " << this->theValue.theObject.theKey << ", THE TYPE: " << (int32_t)this->theValue.theObject.theType << std::endl;
	switch (this->theValue.theObject.theType) {
	case ValueType::Object: {
		bool doWeAddComma{ false };
		theString += "{";
		for (auto& [key, valueNew] : this->theValue.theObject.theValues) {
			if (doWeAddComma) {
				theString += ",";
			}
			if (valueNew.theValue.theObject.theKey != "") {
				theString += "\"" + valueNew.theValue.theObject.theKey + "\":";
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
		for (auto& [key, valueNew] : this->theValue.theObject.theValues) {
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
		theStream << std::boolalpha << *static_cast<bool*>(this->theValue.theObject.theValue);
		theString += theStream.str();
		break;
	}
	case ValueType::String: {
		theString += "\"";
		theString += *static_cast<std::string*>(this->theValue.theObject.theValue);
		theString += "\"";
		break;
	}case ValueType::Double: {
		theString += *static_cast<double*>(this->theValue.theObject.theValue);
		break;
	}case ValueType::Uint64: {
		theString += std::to_string(*static_cast<uint64_t*>(this->theValue.theObject.theValue));
		break;
	}case ValueType::Int64: {
		theString += std::to_string(*static_cast<int64_t*>(this->theValue.theObject.theValue));
		break;
	}
	}
	if (this->theValue.theObject.theType == ValueType::Object && this->areWeTopLevel) {
		theString += "}";
	}	
	return theString;
}

void JsonObject::pushBack(const char* theKey, JsonObject other) noexcept {
	if (!this->theValue.theObject.theValues.contains(theKey)) {
		std::cout << "WERE HERE THIS IS IT THE KEY: " << theKey << std::endl;
		std::cout << "THESE AREE THE KEYS AND VALUES: " << std::endl;
		for (auto &[key, value]:other.theValue.theObject.theValues) {
			std::cout << key << ", VALUE: " << *static_cast<uint64_t*>(value.theValue.theObject.theValue) << ", TYPE" << (int32_t)value.theValue.theObject.theType << std::endl;
		}
		int32_t theSize = this->theValue.theObject.theValues.size();
		this->theValue.theObject.theValues.emplace(std::to_string(theSize), JsonObject{ std::source_location::current() });
		this->theValue.theObject.theType = ValueType::Array;
		this->theValue.theObject.theKey = std::move(theKey);
		this->theValue.theObject.theValues.find(std::to_string(theSize)).operator*().second.theValue.theObject.theType = ValueType::Array;
	}
	else {
		std::cout << "WERE HERE THIS IS IT!" << std::endl;
		int32_t theSize = this->theValue.theObject.theValues.size();
		this->theValue.theObject.theValues.emplace(std::to_string(theSize), std::move(other));
	}
};

/*
void JsonObject::pushBack(const char* theKey, std::string other) noexcept {
	if (!this->theValue.theValues.contains(theKey)) {
		this->theValue.theValues[theKey] = JsonObject{};
		this->theValue.theValues[theKey].theType = ValueType::Array;
		this->theValue.theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint64_t other) noexcept {
	if (!this->theValue.theValues.contains(theKey)) {
		this->theValue.theValues[theKey] = JsonObject{};
		this->theValue.theValues[theKey].theType = ValueType::Array;
		this->theValue.theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint32_t other) noexcept {
	if (!this->theValue.theValues.contains(theKey)) {
		this->theValue.theValues[theKey] = JsonObject{};
		this->theValue.theValues[theKey].theType = ValueType::Array;
		this->theValue.theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint16_t other) noexcept {
	if (!this->theValue.theValues.contains(theKey)) {
		this->theValue.theValues[theKey] = JsonObject{};
		this->theValue.theValues[theKey].theType = ValueType::Array;
		this->theValue.theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint8_t other) noexcept {
	if (!this->theValue.theValues.contains(theKey)) {
		this->theValue.theValues[theKey] = JsonObject{};
		this->theValue.theValues[theKey].theType = ValueType::Array;
		this->theValue.theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int64_t other) noexcept {
	if (!this->theValue.theValues.contains(theKey)) {
		this->theValue.theValues[theKey] = JsonObject{};
		this->theValue.theValues[theKey].theType = ValueType::Array;
		this->theValue.theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int32_t other) noexcept {
	if (!this->theValue.theValues.contains(theKey)) {
		this->theValue.theValues[theKey] = JsonObject{};
		this->theValue.theValues[theKey].theType = ValueType::Array;
		this->theValue.theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int16_t other) noexcept {
	if (!this->theValue.theValues.contains(theKey)) {
		this->theValue.theValues[theKey] = JsonObject{};
		this->theValue.theValues[theKey].theType = ValueType::Array;
		this->theValue.theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int8_t other) noexcept {
	if (!this->theValue.theValues.contains(theKey)) {
		this->theValue.theValues[theKey] = JsonObject{};
		this->theValue.theValues[theKey].theType = ValueType::Array;
		this->theValue.theValues[theKey].theKey = theKey;
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
	else {
		int32_t theSize = this->theValue.theValues[theKey].theValues.size();
		this->theValue.theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}
*/
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

/// Allowable mentions for a Message. \brief Allowable mentions for a Message.
class AllowedMentionsData {
public:
	std::vector<std::string> parse{};///< A vector of allowed mention types to parse from the content.
	std::vector<std::string> roles{};///< Array of role_ids to mention (Max size of 100)
	std::vector<std::string> users{};///< Array of user_ids to mention (Max size of 100)
	bool repliedUser{ false };///< For replies, whether to mention the author of the Message being replied to (default false).


	operator JsonObject();
};

/// Embed field data. \brief Embed field data.
struct EmbedFieldData {
	bool Inline{ false };///< Is the field inline with the rest of them?
	std::string value{};///< The text on the field.
	std::string name{};///< The title of the field.

	EmbedFieldData() noexcept = default;

	operator JsonObject();

	virtual ~EmbedFieldData() noexcept = default;
};

EmbedFieldData::operator JsonObject() {
	JsonObject theData{ "THE KEY", ValueType::Object };
	theData["inline"] = this->Inline;
	theData["value"] = this->value;
	theData["name"] = this->name;
	return theData;
}

AllowedMentionsData::operator JsonObject() {
	//JsonObject theData{ "THE KEY", ValueType::Object };
	//theData["parse"] = nullptr;
	/*
	for (auto& value : this->parse) {
		//theData.pushBack("parse", value);
	}
	for (auto& value : this->roles) {
		//theData.pushBack("roles", value);
	}
	for (auto& value : this->users) {
		//theData.pushBack("users", value);
	}
	return theData;
	*/return JsonObject{ std::source_location::current() };
}

/// Embed data. \brief Embed data.
class  EmbedData {
public:
	std::vector<EmbedFieldData> fields{};///< Array of embed fields.
	DiscordCoreAPI::EmbedThumbnailData thumbnail{};///< Embed thumbnail data.
	DiscordCoreAPI::ColorValue hexColorValue{ 0 };///< Hex color value of the embed.
	DiscordCoreAPI::EmbedProviderData provider{};///< Embed provider data.
	std::string description{};///< Description of the embed.
	DiscordCoreAPI::EmbedFooterData footer{};///< Embed footer data.
	DiscordCoreAPI::EmbedAuthorData author{};///< Embed author data.
	std::string timestamp{};///< Timestamp to be placed on the embed.
	DiscordCoreAPI::EmbedImageData image{};///< Embed image data.
	DiscordCoreAPI::EmbedVideoData video{};///< Embed video data.
	std::string title{};///< Title of the embed.
	std::string type{};///< Type of the embed.
	std::string url{};///< Url for the embed.

	EmbedData() noexcept = default;

	operator JsonObject();

	/// Sets the author's name and avatar for the embed. \brief Sets the author's name and avatar for the embed.
	/// \param authorName The author's name.
	/// \param authorAvatarUrl The url to their avatar.
	/// \returns EmbedData& A reference to this embed.
	EmbedData& setAuthor(const std::string& authorName, const std::string& authorAvatarUrl = "");

	/// Sets the footer's values for the embed. \brief Sets the footer's values for the embed.
	/// \param footerText The footer's text.
	/// \param footerIconUrlText Url to the footer's icon.
	/// \returns EmbedData& A reference to this embed.
	EmbedData& setFooter(const std::string& footerText, const std::string& footerIconUrlText = "");

	/// Sets the timestamp on the embed. \brief Sets the timestamp on the embed.
	/// \param timeStamp The timestamp to be set.
	/// \returns EmbedData& A reference to this embed.
	EmbedData& setTimeStamp(const std::string& timeStamp);

	/// Adds a field to the embed. \brief Adds a field to the embed.
	/// \param name The title of the embed field.
	/// \param value The contents of the embed field.
	/// \param Inline Is it inline with the rest of the fields on the embed?
	/// \returns EmbedData& A reference to this embed.
	EmbedData& addField(const std::string& name, const std::string& value, bool Inline = true);

	/// Sets the description (the main contents) of the embed. \brief Sets the description (the main contents) of the embed.
	/// \param descriptionNew The contents of the description to set.
	/// \returns EmbedData& A reference to this embed.
	EmbedData& setDescription(const std::string& descriptionNew);

	/// Sets the color of the embed, by applying a hex-color value. \brief Sets the color of the embed, by applying a hex-color value.
	/// \param hexColorValueNew A string containing a hex-number value (Between 0x00 0xFFFFFF).
	/// \returns EmbedData& A reference to this embed.
	EmbedData& setColor(const std::string& hexColorValueNew);

	/// Sets the thumbnail of the embed. \brief Sets the thumbnail of the embed.
	/// \param thumbnailUrl The url to the thumbnail to be used.
	/// \returns EmbedData& A reference to this embed.
	EmbedData& setThumbnail(const std::string& thumbnailUrl);

	/// Sets the title of the embed. \brief Sets the title of the embed.
	/// \param titleNew A string containing the desired title.
	/// \returns EmbedData& A reference to this embed.
	EmbedData& setTitle(const std::string& titleNew);

	/// Sets the image of the embed. \brief Sets the image of the embed.
	/// \param imageUrl The url of the image to be set on the embed.
	/// \returns EmbedData& A reference to this embed.
	EmbedData& setImage(const std::string& imageUrl);

	virtual ~EmbedData() noexcept = default;
};

EmbedData::operator JsonObject() {
	JsonObject theData{ std::source_location::current() };
	for (auto& value2 : this->fields) {
		//theData.pushBack("fields", value2);
	}
	/*
	std::string realColorVal = std::to_string(this->theValue.hexColorValue.getIntColorValue());
	theData["footer"]["proxy_icon_url"] = this->theValue.footer.proxyIconUrl;
	theData["footer"]["icon_url"] = this->theValue.footer.iconUrl;
	theData["footer"]["text"] = this->theValue.footer.text;
	theData["author"]["proxy_icon_url"] = this->theValue.author.proxyIconUrl;
	theData["author"]["icon_url"] = this->theValue.author.iconUrl;
	theData["author"]["name"] = this->theValue.author.name;
	theData["author"]["url"] = this->theValue.author.url;
	theData["thumbnail"]["proxy_url"] = this->theValue.thumbnail.proxyUrl;
	theData["thumbnail"]["height"] = this->theValue.thumbnail.height;
	theData["thumbnail"]["width"] = this->theValue.thumbnail.width;
	theData["thumbnail"]["url"] = this->theValue.thumbnail.url;
	theData["image"]["proxy_url"] = this->theValue.image.proxyUrl;
	theData["image"]["height"] = this->theValue.image.height;
	theData["image"]["width"] = this->theValue.image.width;
	theData["image"]["url"] = this->theValue.image.url;
	theData["video"]["proxy_url"] = this->theValue.video.proxyUrl;
	theData["video"]["height"] = this->theValue.video.height;
	theData["video"]["url"] = this->theValue.video.url;
	theData["video"]["width"] = this->theValue.video.width;
	*/
	theData["provider"]["name"] = this->provider.name;
	theData["provider"]["url"] = this->provider.url;
	theData["description"] = this->description;
	theData["timestamp"] = this->timestamp;
	theData["title"] = this->title;
	//theData["color"] = realColorVal;
	theData["type"] = this->type;
	
	theData["url"] = this->url;;
	return theData;
}

/// Interaction ApplicationCommand callback data. \brief Interaction ApplicationCommand callback data.
struct InteractionCallbackData {
	std::vector<DiscordCoreAPI::ApplicationCommandOptionChoiceData> choices{};///< Autocomplete choices(max of 25 choices).
	std::vector<DiscordCoreAPI::AttachmentData> attachments{};///< Array of partial attachment objects attachment objects with filename and description.
	std::vector<DiscordCoreAPI::ActionRowData> components{};///< Message components.
	AllowedMentionsData allowedMentions{};///< Allowed mentions data.
	std::vector<EmbedData> embeds{};///< Message embeds.
	std::vector<DiscordCoreAPI::File> files{};///< Files for uploading.
	std::string customId{};///< A developer-defined identifier for the component, max 100 characters.
	std::string content{};///< Message content.
	std::string title{};///< The title of the popup modal.
	int32_t flags{ 0 };///< Flags.
	bool tts{ false };///< Is it TTS?
	operator JsonObject();
};

struct InteractionResponseData {

	InteractionResponseData() noexcept = default;

	operator JsonObject();

	InteractionCallbackData data{};///< Interaction ApplicationCommand callback data.
	DiscordCoreAPI::InteractionCallbackType type{};///< Interaction callback type.
};



InteractionResponseData::operator JsonObject() {
	JsonObject theData{ std::source_location::current() };
	
	theData["type"] = static_cast<uint8_t>(this->type);
	if (this->data.attachments.size() > 0) {
		for (auto& value : this->data.attachments) {
			//theData["data"].pushBack("attachments", JsonObject{ value });
		}
	}
	for (auto& value : this->data.components) {
		//theData["data"].pushBack("components", JsonObject{ value });
	}
	theData["data"]["allowed_mentions"] = this->data.allowedMentions.operator JsonObject();
	if (this->data.choices.size() > 0) {
		for (auto& value : this->data.choices) {
			JsonObject theValue{ "THE KEY", ValueType::Object };
			theValue["name"] = value.name;
			//theValue["name_localizations"] = value.nameLocalizations;
			switch (value.type) {
			case DiscordCoreAPI::JsonType::Boolean: {
				theValue["value"] = value.valueBool;
				break;
			}
			case DiscordCoreAPI::JsonType::String: {
				theValue["value"] = value.valueStringReal;
				break;
			}
			case DiscordCoreAPI::JsonType::Float: {
				theValue["value"] = value.valueFloat;
				break;
			}
			case DiscordCoreAPI::JsonType::Integer: {
				theValue["value"] = value.valueInt;
				break;
			}
			}
			//theData["data"].pushBack("choices", theValue);
		}
	}
	for (auto& value : this->data.embeds) {
		theData["data"].pushBack("embeds", JsonObject{ value.operator JsonObject() });
	}
	theData["data"]["custom_id"] = this->data.customId;
	theData["data"]["content"] = this->data.content;
	theData["data"]["title"] = this->data.title;
	theData["data"]["flags"] = this->data.flags;
	theData["data"]["tts"] = this->data.tts;
	
	return theData;
}

WebSocketIdentifyData::operator JsonObject() {
	JsonObject theData{ "THE KEY", ValueType::Object };
	/*
	std::unordered_map<std::string, std::string> theMap{};
	std::vector<std::string> theMapTwo{};
	theMapTwo.push_back("THE_VALUE");
	theMap["TEST"] = "TESTvalue";
	theMap["TEST_TWO"] = "TESTvalue";
	//theSerializer["test_Map"] = theMapTwo;
	//theSerializer["test_Map_Two"] = theMap;
	//theSerializer["test_Map_Three"] = theMap;
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->theValue.intents);
	theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);

	for (auto& value : this->theValue.presence.activities) {
		JsonObject theSerializer02{};
		if (value.url != "") {
			theSerializer02["url"] = std::string{ value.url };
		}
		theSerializer02["name"] = std::string{ value.name };
		theSerializer02["type"] = uint32_t{ static_cast<uint32_t>(value.type) };
		theSerializer["d"].pushBack("activities", theSerializer02);
		theSerializer["d"].pushBack("activities", theSerializer02);
		//theSerializer["TEST_ENUM"] = DiscordCoreAPI::ChannelType::Guild_Directory;
	}
	theSerializer["d"]["afk"] = this->theValue.presence.afk;
	if (this->theValue.presence.since != 0) {
		theSerializer["since"] = this->theValue.presence.since;
	}

	theSerializer["d"]["status"] = this->theValue.presence.status;
	theSerializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
	theSerializer["d"]["properties"]["device"] = "DiscordCoreAPI";
#ifdef _WIN32
	theSerializer["d"]["properties"]["os"] = "Windows";
#else
	theSerializer["d"]["properties"]["os"] = "Linux";
#endif
	//theSerializer["d"].pushBack("shard", static_cast<uint32_t>(this->theValue.currentShard));
	//theSerializer["d"].pushBack("shard", static_cast<uint32_t>(this->theValue.numberOfShards));
	theSerializer["d"]["token"] = this->theValue.botToken;
	theSerializer["op"] = static_cast<uint32_t>(2);
	*/
	return theData;
}

EditGuildApplicationCommandPermissionsData::operator JsonObject() {
	JsonObject theData{ "THE KEY", ValueType::Object };
	theData["d"] = true;
	theData["23"]["TESTty"]["TEST_TWO"] = true;
	theData["23"]["TEST"] = true;
	theData["Teetertytot"] = "TESTING";
	theData["test"] = "TESTING VALUES";
	//theData["TEST"] = false;
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

		InteractionResponseData theDataReal{};
		theDataReal.data.allowedMentions.roles.push_back("TESTING");
		EmbedData theData{};
		theData.description = "TESTING";
		theDataReal.data.embeds.push_back(theData);
		std::string theString02 = JsonSerializer{}.getString(theDataReal);
		std::cout << "THE FINAL STRING: 0101 " << theString02 << std::endl;
		std::cout << "THE FINAL STRING (PARSED): " << nlohmann::json::parse(theString02).dump() << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds{ 3 });

	}
	catch (...) { DiscordCoreAPI::reportException("main()"); };

	return 0;
}
