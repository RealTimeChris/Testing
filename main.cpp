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

EnumConverter::operator std::vector<uint64_t>() {
	std::vector<uint64_t> theObject{};
	for (auto& value: *static_cast<std::vector<uint64_t>*>(this->thePtr)) {
		theObject.emplace_back(value);
	}

	return theObject;
}

EnumConverter::operator uint64_t() {
	uint64_t theObject{};
	theObject = *static_cast<uint64_t*>(this->thePtr);
	return theObject;
}

EnumConverter::~EnumConverter() {
	if (this->thePtr) {
		if (this->vectorType) {
			delete static_cast<std::vector<uint64_t>*>(this->thePtr);
		} else {
			delete static_cast<uint64_t*>(this->thePtr);
		}
	}
}

JsonObject& JsonObject::operator=(EnumConverter theData) noexcept {
	this->theType = ValueType::Uint64;
	EnumConverter theConverter{ theData };
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue) = static_cast<uint64_t>(theConverter);
	return *this;
}

JsonObject::JsonObject(EnumConverter theData) noexcept {
	this->theValue = new EnumConverter{ DiscordCoreAPI::ChannelType::Dm };
	*static_cast<EnumConverter*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(const JsonObject& theKey) noexcept {
	for (auto& [key, value]: theKey.theValues) {
		this->theValues[key] = value;
	}
	this->theValue = theKey.theValue;
	this->theType = theKey.theType;
	this->theKey = theKey.theKey;
	return *this;
}

JsonObject::JsonObject(const JsonObject& theKey) noexcept {
	*this = theKey;
}

JsonObject& JsonObject::operator=(ValueType theType) noexcept {
	switch (theType) {
		case ValueType::Bool: {
			this->theType = ValueType::Bool;
			this->theValue = new bool{};
			break;
		}
		case ValueType::Double: {
			this->theType = ValueType::Double;
			this->theValue = new double{};
			break;
		}
		case ValueType::Float: {
			this->theType = ValueType::Float;
			this->theValue = new float{};
			break;
		}
		case ValueType::Uint64: {
			this->theType = ValueType::Uint64;
			this->theValue = new uint64_t{};
			break;
		}
		case ValueType::Int64: {
			this->theType = ValueType::Int64;
			this->theValue = new int64_t{};
			break;
		}
		case ValueType::String: {
			this->theType = ValueType::String;
			this->theValue = new std::string{};
			break;
		}
		case ValueType::Null_Ext: {
			this->theType = ValueType::Null_Ext;
			this->theValue = new uint64_t{};
			*static_cast<uint64_t*>(this->theValue) = static_cast<uint64_t>(theType);
			break;
		}
		case ValueType::Null: {
			this->theType = ValueType::Null;
			this->theValue = new uint64_t{};
			*static_cast<uint64_t*>(this->theValue) = static_cast<uint64_t>(theType);
			break;
		}
		case ValueType::Array: {
			this->theType = ValueType::Array;
			this->theValue = new JsonArray{};
			break;
		}
		case ValueType::Object: {
			this->theType = ValueType::Object;
			this->theValue = new JsonObject{};
			break;
		}
		case ValueType::Unset: {
			this->theType = ValueType::Unset;
			break;
		}
	}
	return *this;
}

JsonObject::JsonObject(const char*theKey, const JsonObject& theData) noexcept {
	*this = theData;
	this->theKey = theKey;
}

JsonObject& JsonObject::operator=(const JsonArray& theData) noexcept {
	this->theKey = theData.theKey;
	this->theType = theData.theType;
	this->theValue = theData.theValue;
	for (auto& [key, value]: theData.theValues) {
		this->theValues[key] = value;
	}
	return *this;
}

JsonObject::JsonObject(const JsonArray& theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(const char* theData) noexcept {
	this->theType = ValueType::String;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<std::string*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(const char* theData) noexcept {
	this->theValue = new std::string{};
	*static_cast<std::string*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(std::string theData) noexcept {
	this->theType = ValueType::String;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<std::string*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(std::string theData) noexcept {
	this->theValue = new std::string{};
	*static_cast<std::string*>(this->theValue) = theData;
}

JsonObject::JsonObject(ValueType theData) noexcept {
	this->theValue = new uint64_t{};
	*this = theData;
}

JsonObject& JsonObject::operator=(uint64_t theData) noexcept {
	this->theType = ValueType::Uint64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(uint64_t theData) noexcept {
	this->theValue = new uint64_t{};
	*static_cast<uint64_t*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(uint32_t theData) noexcept {
	this->theType = ValueType::Uint64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(uint32_t theData) noexcept {
	this->theValue = new uint32_t{};
	*static_cast<uint32_t*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(uint16_t theData) noexcept {
	this->theType = ValueType::Uint64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(uint16_t theData) noexcept {
	this->theValue = new uint16_t{};
	*static_cast<uint16_t*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(uint8_t theData) noexcept {
	this->theType = ValueType::Uint64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<uint64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(uint8_t theData) noexcept {
	this->theValue = new uint8_t{};
	*static_cast<uint8_t*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(int64_t theData) noexcept {
	this->theType = ValueType::Int64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<int64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(int64_t theData) noexcept {
	this->theValue = new uint64_t{};
	*static_cast<int64_t*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(int32_t theData) noexcept {
	this->theType = ValueType::Int64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<int64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(int32_t theData) noexcept {
	this->theValue = new uint32_t{};
	*static_cast<int32_t*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(int16_t theData) noexcept {
	this->theType = ValueType::Int64;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<int64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(int16_t theData) noexcept {
	this->theValue = new uint16_t{};
	*static_cast<int16_t*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(int8_t theData) noexcept {
	this->theType = ValueType::Int64;
	*this = this->theType;
	*static_cast<int64_t*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(int8_t theData) noexcept {
	this->theValue = new uint8_t{};
	*static_cast<int8_t*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(double theData) noexcept {
	this->theType = ValueType::Double;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<double*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(double theData) noexcept {
	this->theValue = new double{};
	*static_cast<double*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(float theData) noexcept {
	this->theType = ValueType::Float;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<float*>(this->theValue) = theData;
	return *this;
}

JsonObject::JsonObject(float theData) noexcept {
	this->theValue = new float{};
	*static_cast<float*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator=(bool theData) noexcept {
	this->theType = ValueType::Bool;
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	*this = theObject;
	*static_cast<bool*>(this->theValue) = theData;
	return *this;
}


JsonObject::JsonObject(bool theData) noexcept {
	this->theValue = new bool{};
	*static_cast<bool*>(this->theValue) = theData;
}

JsonObject& JsonObject::operator[](const char* theKey) noexcept {
	if (this->theKey == "") {
		JsonObject theObject{};
		theObject.theKey = theKey;
		theObject.theType = ValueType::Object;
		this->theValues[theKey] = theObject;
		return this->theValues[theKey];
	} else if (this->theKey == theKey && this->theType == ValueType::Object) {
		//return *this;
	} else if (!this->theValues.contains(theKey)) {
		JsonObject theObject{};
		theObject.theKey = theKey;
		theObject.theType = ValueType::Object;
		this->theValues[theKey] = theObject;
		return this->theValues[theKey];
	} else if (this->theValues.contains(theKey)) {
		return this->theValues[theKey];
	} else {
		JsonObject theObject{};
		theObject.theType = ValueType::Object;
		theObject.theKey = theKey;
		this->theValues[theKey] = theObject;
		return this->theValues[theKey];
	}
}

JsonObject::operator std::string() noexcept {
	std::string theString{};
	if (this->theKey != "") {
		theString += "\"" + this->theKey + "\":";
	}
	switch (this->theType) {
		case ValueType::Object: {
			bool doWeAddComma{ false };
			theString += "{";
			for (auto& [key, valueNew]: this->theValues) {
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
			for (auto& [key, valueNew]: this->theValues) {
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
		}
		case ValueType::Double: {
			theString += *static_cast<double*>(this->theValue);
			break;
		}
		case ValueType::Float: {
			theString += *static_cast<double*>(this->theValue);
			break;
		}
		case ValueType::Uint64: {
			theString += std::to_string(*static_cast<uint64_t*>(this->theValue));
			break;
		}
		case ValueType::Int64: {
			theString += std::to_string(*static_cast<int64_t*>(this->theValue));
			break;
		}
		case ValueType::Null: {
			theString += "null";
			break;
		}
		case ValueType::Null_Ext: {
			theString += "[]";
			break;
		}
	}
	return theString;
}

void JsonObject::pushBack(const char* theKey, std::string other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, JsonObject other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = std::move(theKey);
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = std::move(other);
	} else {
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = std::move(other);
	}
};

void JsonObject::pushBack(const char* theKey, uint64_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint32_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint16_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint8_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int64_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int32_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int16_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int8_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = JsonObject{};
		this->theValues[theKey].theType = ValueType::Array;
		this->theValues[theKey].theKey = theKey;
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey].theValues.size();
		this->theValues[theKey].theValues[std::to_string(theSize)] = other;
	}
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

AllowedMentionsData::operator JsonObject() {
	JsonObject theData{ "allowed_mentions" };
	for (auto& value : this->parse) {
		theData.pushBack("parse", value);
	}
	for (auto& value : this->roles) {
		theData.pushBack("roles", value);
	}
	for (auto& value : this->users) {
		theData.pushBack("users", value);
	}
	return theData;
}

/// Embed field data. \brief Embed field data.
struct EmbedFieldData {
	bool Inline{ false };///< Is the field inline with the rest of them?
	std::string value{};///< The text on the field.
	std::string name{};///< The title of the field.
	EmbedFieldData() noexcept = default;

	operator JsonObject();

	virtual ~EmbedFieldData() noexcept = default;
};

/// Embed data. \brief Embed data.
class EmbedData {
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
	std::vector<std::string> theObjectVector{ "TESTING", "TESTING02" };
	std ::unordered_map<std::string, std::string> theRoles{ { "TESTING", "TESTING02" }, { "TESTING23", "TESTING34" } };
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

EmbedFieldData::operator JsonObject() {
	JsonObject theData{};
	theData["inline"] = this->Inline;
	theData["value"] = this->value;
	theData["name"] = this->name;
	return theData;
}

EmbedData::operator JsonObject() {
	JsonObject theData{};
	for (auto& value2 : this->fields) {
		theData.pushBack("fields", JsonObject{ value2 });
	}
	std::string realColorVal = std::to_string(this->hexColorValue.getIntColorValue());
	theData["footer"]["proxy_icon_url"] = this->footer.proxyIconUrl;
	theData["footer"]["icon_url"] = this->footer.iconUrl;
	theData["roles"] = this->theRoles;
	theData["channels"] = this->theObjectVector;
	theData["footer"]["text"] = this->footer.text;
	theData["author"]["proxy_icon_url"] = this->author.proxyIconUrl;
	theData["author"]["icon_url"] = this->author.iconUrl;
	theData["author"]["name"] = this->author.name;
	theData["author"]["url"] = this->author.url;
	theData["thumbnail"]["proxy_url"] = this->thumbnail.proxyUrl;
	theData["thumbnail"]["height"] = this->thumbnail.height;
	theData["thumbnail"]["width"] = this->thumbnail.width;
	theData["thumbnail"]["url"] = this->thumbnail.url;
	theData["image"]["proxy_url"] = this->image.proxyUrl;
	theData["image"]["height"] = this->image.height;
	theData["image"]["width"] = this->image.width;
	theData["image"]["url"] = this->image.url;
	theData["video"]["proxy_url"] = this->video.proxyUrl;
	theData["video"]["height"] = this->video.height;
	theData["video"]["url"] = this->video.url;
	theData["video"]["width"] = this->video.width;
	theData["provider"]["name"] = this->provider.name;
	theData["provider"]["url"] = this->provider.url;
	theData["description"] = this->description;
	theData["timestamp"] = this->timestamp;
	theData["title"] = this->title;
	theData["color"] = realColorVal;
	theData["type"] = this->type;
	theData["url"] = this->url;
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
	JsonObject theData{};
	theData["type"] = static_cast<uint8_t>(this->type);
	if (this->data.attachments.size() > 0) {
		for (auto& value : this->data.attachments) {
			// theData["data"].pushBack("attachments", value );
		}
	}
	for (auto& value : this->data.components) {
		//theData["data"].pushBack("components",  value );
	}
	theData["data"]["allowed_mentions"] = JsonObject{ this->data.allowedMentions };
	if (this->data.choices.size() > 0) {
		for (auto& value : this->data.choices) {
			JsonObject theValue{};
			theValue["name"] = value.name;
			theValue["name_localizations"] = value.nameLocalizations;
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
			theData["data"].pushBack("choices", theValue);
		}
	}
	for (auto& value : this->data.embeds) {
		theData["data"].pushBack("embeds", JsonObject{ value });
	}
	if (this->data.customId != "") {
		theData["data"]["custom_id"] = this->data.customId;
	}
	if (this->data.content != "") {
		theData["data"]["content"] = this->data.content;
	}
	if (this->data.title != "") {
		theData["data"]["title"] = this->data.title;
	}
	theData["data"]["flags"] = this->data.flags;
	theData["data"]["tts"] = this->data.tts;
	return theData;
}

/*
WebSocketIdentifyData::operator JsonObject() {
	JsonObject theSerializer{};
	std::unordered_map<std::string, std::string> theMap{};
	std::vector<std::string> theMapTwo{};
	theMapTwo.push_back("THE_VALUE");
	theMap["TEST"] = "TESTvalue";
	theMap["TEST_TWO"] = "TESTvalue";
	theSerializer["test_Map"] = theMapTwo;
	theSerializer["test_Map_Two"] = theMap;
	theSerializer["test_Map_Three"] = theMap;
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
		theSerializer["TEST_ENUM"] = DiscordCoreAPI::ChannelType::Guild_Directory;
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
*/

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
		EmbedFieldData theDataNew{};
		theDataNew.Inline = false;
		theDataNew.name = "TESTY";
		theDataNew.value = "TESTING HERE";
		theData.fields.push_back(theDataNew);
		theDataReal.data.embeds.push_back(theData);
		std::string theString02 = theDataReal.operator JsonObject();
		std::cout << "THE FINAL STRING: 0101 " << theString02 << std::endl;
		std::cout << "THE FINAL STRING (PARSED): " << nlohmann::json::parse(theString02).dump() << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds{ 3 });

	}
	catch (...) { DiscordCoreAPI::reportException("main()"); };

	return 0;
}