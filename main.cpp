#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"
#include <memory>
#include <nlohmann/json.hpp>

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

JsonArray& JsonArray::operator=(const JsonArray& theData) noexcept {
	this->theKey = theData.theKey;
	this->theValue = theData.theValue;
	this->theType = theData.theType;
	for (auto& [key, value]: theData.theValues) {
		this->theValues[key] = std::make_unique<JsonObject>(*value);
	}
	return *this;
}
JsonArray::JsonArray(const JsonArray& theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const JsonValue& other) {
	//std::cout << "THE KEY REAL: " << ( int32_t )other.theType << std::endl;
	//std::cout << "THE VALUE REAL: " << ( int32_t )other.numberUint << std::endl;
	//std::cout << "THE VALUE REAL: " << ( int32_t )other.numberInt << std::endl;
	switch (other.theType) {
		case ValueType::Array: {
			this->array = JsonObject::create<JsonArray>();
			*this->array = *other.array;
			break;
		}
		case ValueType::Object: {
			this->object = JsonObject::create<JsonObject>();
			*this->object = *other.object;
			break;
		}
		case ValueType::String: {
			this->string = JsonObject::create<StringType>();
			*this->string = *other.string;
			break;
		}
	}
	this->boolean = other.boolean;
	this->numberDouble = other.numberDouble;
	this->numberInt = other.numberInt;
	this->numberUint = other.numberUint;
	this->theType = other.theType;
	return *this;
}

JsonObject::JsonValue::JsonValue(const JsonValue& other) {
	*this = other;
}
/*
JsonObject& JsonObject::operator=(EnumConverter theData) noexcept {
	this->theType = ValueType::Uint64;
	EnumConverter theConverter{ theData };
	JsonObject theObject{ theData };
	theObject.theKey = this->theKey;
	theObject.theType = this->theType;
	theObject.theValue = JsonValue{ ValueType::Uint64 };
	*this = theObject;
	this->theValue.numberUint = static_cast<uint64_t>(theConverter);
	return *this;
}

JsonObject::JsonObject(EnumConverter theData) noexcept : theValue(ValueType::Uint64) {
	//this->theValue = new EnumConverter{ DiscordCoreAPI::ChannelType::Dm };
	//static_cast<EnumConverter>(this->theValue.numberUint) = theData;
}
*/
JsonObject& JsonObject::operator=(const JsonObject& theKey) noexcept {
	//std::cout << "THE KEY: " << theKey.theKey << std::endl;
	for (auto& [key, value]: theKey.theValues) {
		this->theValues[key] = std::make_unique<JsonObject>(*value);
		*this->theValues[key] = *std::make_unique<JsonObject>(*value);
	}
	this->theValue = theKey.theValue;
	this->theType = theKey.theType;
	this->theKey = theKey.theKey;
	return *this;
}

size_t JsonObject::size() {
	return this->theValues.size();
}

size_t JsonArray::size() {
	return this->theValues.size();
}

void JsonObject::clear() {
	this->theValues.clear();
}

void JsonArray::clear() {
	this->theValues.clear();
}

JsonObject::JsonObject(const JsonObject& theKey) noexcept : theValue(ValueType::Object) {
	*this = theKey;
}


JsonObject::JsonObject(const char* theKey, const JsonObject& theData) noexcept : theValue(ValueType::Object) {
	*this = theData;
	this->theKey = theKey;
}

JsonObject& JsonObject::operator=(const JsonArray& theData) noexcept {
	this->theType = theData.theType;
	this->theKey = theData.theKey;
	for (auto&  [key,value]: theData.theValues) {
		this->theValues[key] = std::make_unique<JsonObject>(*value);
	}
	*this->theValue.array = theData;
	return *this;
}

JsonObject::JsonObject(const JsonArray& theData) noexcept : theValue(ValueType::Array) {
	*this->theValue.array = theData;
	*this = theData;
}

JsonObject& JsonObject::operator=(const char* theData) noexcept {
	this->theType = ValueType::String;
	this->theValue = JsonValue{ ValueType::String };
	*this->theValue.string = theData;
	return *this;
}

JsonObject::JsonObject(const char* theData) noexcept : theValue(ValueType::String) {
	*this->theValue.string = theData;
}

JsonObject& JsonObject::operator=(const JsonValue& theKey) noexcept {
	this->theValue = theKey;
	return *this;
}

JsonObject::JsonObject(const JsonValue& theKey) noexcept {
	*this = theKey;
}

JsonObject& JsonObject::operator=(const std::string theData) noexcept {
	this->theType = ValueType::String;
	this->theValue = JsonValue{ ValueType::String };
	*this->theValue.string = theData;
	return *this;
}

JsonObject::JsonObject(const std::string theData) noexcept : theValue(ValueType::String) {
	*this->theValue.string = theData;
}

JsonObject& JsonObject::operator=(uint64_t theData) noexcept {
	this->theType = ValueType::Uint64;
	this->theValue = JsonValue{ ValueType::Uint64};
	this->theValue.numberUint = theData;
	return *this;
}

JsonObject::JsonObject(uint64_t theData) noexcept : theValue(ValueType::Uint64) {
	this->theValue.numberUint = theData;
}

JsonObject& JsonObject::operator=(uint32_t theData) noexcept {
	this->theType = ValueType::Uint64;
	this->theValue = JsonValue{ ValueType::Uint64 };
	this->theValue.numberUint = theData;
	return *this;
}

JsonObject::JsonObject(uint32_t theData) noexcept : theValue(ValueType::Uint64) {
	this->theValue.numberUint = theData;
}

JsonObject& JsonObject::operator=(uint16_t theData) noexcept {
	this->theType = ValueType::Uint64;
	this->theValue = JsonValue{ ValueType::Uint64 };
	this->theValue.numberUint = theData;
	return *this;
}

JsonObject::JsonObject(uint16_t theData) noexcept : theValue(ValueType::Uint64) {
	this->theValue.numberUint = theData;
}

JsonObject& JsonObject::operator=(uint8_t theData) noexcept {
	this->theType = ValueType::Uint64;
	this->theValue = JsonValue{ ValueType::Uint64 };
	this->theValue.numberUint = theData;
	return *this;
}

JsonObject::JsonObject(uint8_t theData) noexcept : theValue(ValueType::Uint64) {
	this->theValue.numberUint = theData;
}

JsonObject& JsonObject::operator=(int64_t theData) noexcept {
	this->theType = ValueType::Int64;
	this->theValue = JsonValue{ ValueType::Int64 };
	this->theValue.numberInt = theData;
	return *this;
}

JsonObject::JsonObject(int64_t theData) noexcept : theValue(ValueType::Int64) {
	this->theValue.numberInt = theData;
}

JsonObject& JsonObject::operator=(int32_t theData) noexcept {
	this->theType = ValueType::Int64;
	this->theValue = JsonValue{ ValueType::Int64 };
	this->theValue.numberInt = theData;
	return *this;
}

JsonObject::JsonObject(int32_t theData) noexcept : theValue(ValueType::Int64) {
	this->theValue.numberInt = theData;
}

JsonObject& JsonObject::operator=(int16_t theData) noexcept {
	this->theType = ValueType::Int64;
	this->theValue = JsonValue{ ValueType::Int64 };
	this->theValue.numberInt = theData;
	return *this;
}

JsonObject::JsonObject(int16_t theData) noexcept : theValue(ValueType::Int64) {
	this->theValue.numberInt = theData;
}

JsonObject& JsonObject::operator=(int8_t theData) noexcept {
	this->theType = ValueType::Int64;
	this->theValue = JsonValue{ ValueType::Int64 };
	this->theValue.numberInt = theData;
	return *this;
}

JsonObject::JsonObject(int8_t theData) noexcept : theValue(ValueType::Int64) {
	this->theValue.numberInt = theData;
}

JsonObject& JsonObject::operator=(double theData) noexcept {
	this->theType = ValueType::Float;
	this->theValue = JsonValue{ ValueType::Float };
	this->theValue.numberDouble = theData;
	return *this;
}

JsonObject::JsonObject(double theData) noexcept : theValue(ValueType::Float) {
	this->theValue.numberDouble = theData;
}

JsonObject& JsonObject::operator=(float theData) noexcept {
	this->theType = ValueType::Float;
	this->theValue = JsonValue{ ValueType::Float };
	this->theValue.numberDouble = theData;
	return *this;
}

JsonObject::JsonObject(float theData) noexcept : theValue(ValueType::Float) {
	this->theValue.numberDouble = theData;
}

JsonObject& JsonObject::operator=(bool theData) noexcept {
	this->theType = ValueType::Bool;
	this->theValue = JsonValue{ ValueType::Bool };
	this->theValue.boolean= theData;
	return *this;
}


JsonObject::JsonObject(bool theData) noexcept : theValue(ValueType::Bool) {
	this->theValue.boolean = theData;
}

JsonObject& JsonObject::operator[](const char* theKey) noexcept {
	if (this->theKey == "") {
		this->theKey = theKey;
		this->theType = ValueType::Object;
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theType = ValueType::Object;
		this->theValues[theKey]->theValue = ValueType::Object;
		return *this->theValues[theKey];
	} else if (this->theKey == theKey && this->theType == ValueType::Object) {
		return *this->theValues[theKey];;
	} else if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theKey = theKey;
		this->theValues[theKey]->theType = ValueType::Object;
		this->theValues[theKey]->theValue = ValueType::Object;
		return *this->theValues[theKey];
	} else if (this->theValues.contains(theKey)) {
		return *this->theValues[theKey];
	} else {
		this->theType = ValueType::Object;
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theKey = theKey;
		this->theValues[theKey]->theType = ValueType::Object;
		this->theValues[theKey]->theValue = ValueType::Object;
		return *this->theValues[theKey];
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
				theString += *valueNew;
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
				theString += *valueNew;
				doWeAddComma = true;
			}
			theString += "]";
			break;
		}
		case ValueType::Bool: {
			std::stringstream theStream{};
			theStream << std::boolalpha << static_cast<bool>(this->theValue.boolean);
			theString += theStream.str();
			break;
		}
		case ValueType::String: {
			theString += "\"";
			theString += *static_cast<std::string*>(this->theValue.string);
			theString += "\"";
			break;
		}
		case ValueType::Float: {
			theString += static_cast<double>(this->theValue.numberDouble);
			break;
		}
		case ValueType::Uint64: {
			theString += std::to_string(static_cast<uint64_t>(this->theValue.numberUint));
			break;
		}
		case ValueType::Int64: {
			theString += std::to_string(static_cast<int64_t>(this->theValue.numberInt));
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

void JsonObject::JsonValue::destroy(ValueType t) {
	if (t == ValueType::Array || t == ValueType::Object) {
		/*
		std::unordered_map<std::string, JsonObject> stack{};
		if (t == ValueType::Array) {
			stack.reserve(array->size());
			std::move(array->begin(), array->end(), std::back_inserter(stack));
		} else {
			stack.reserve(object->size());
			for (auto&& it: *object) {
				stack.emplace(std::move(*it.second));
			}
		}

		while (!stack.empty()) {
			JsonObject current_item(std::move(stack.end().operator*().second));
			stack.pop_back();

			if (current_item.theType == ValueType::Array) {
				std::move(current_item.theValue.array->begin(), current_item.theValue.array->end(), std::back_inserter(stack));

				current_item.theValue.array->clear();
			} else if (current_item.theType == ValueType::Object) {
				for (auto&& it: *current_item.theValue.object) {
					stack.push_back(std::move(*it.second));
				}

				current_item.theValue.object->clear();
			}
		}*/
	}

	switch (t) {
		case ValueType::Object: {
			AllocatorType<ObjectType> alloc;
			std::allocator_traits<decltype(alloc)>::destroy(alloc, object);
			std::allocator_traits<decltype(alloc)>::deallocate(alloc, object, 1);
			break;
		}

		case ValueType::Array: {
			AllocatorType<ArrayType> alloc;
			std::allocator_traits<decltype(alloc)>::destroy(alloc, array);
			std::allocator_traits<decltype(alloc)>::deallocate(alloc, array, 1);
			break;
		}

		case ValueType::String: {
			AllocatorType<StringType> alloc;
			std::allocator_traits<decltype(alloc)>::destroy(alloc, string);
			std::allocator_traits<decltype(alloc)>::deallocate(alloc, string, 1);
			break;
		}
		default: {
			break;
		}
	}
}

void JsonObject::pushBack(const char* theKey, std::string other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theType = ValueType::Array;
		this->theValues[theKey]->theKey = theKey;
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, JsonObject other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theType = ValueType::Array;
		this->theValues[theKey]->theKey = theKey;
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	}
};

void JsonObject::pushBack(const char* theKey, uint64_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theType = ValueType::Array;
		this->theValues[theKey]->theKey = theKey;
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint32_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theType = ValueType::Array;
		this->theValues[theKey]->theKey = theKey;
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint16_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theType = ValueType::Array;
		this->theValues[theKey]->theKey = theKey;
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, uint8_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theType = ValueType::Array;
		this->theValues[theKey]->theKey = theKey;
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int64_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theType = ValueType::Array;
		this->theValues[theKey]->theKey = theKey;
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int32_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theType = ValueType::Array;
		this->theValues[theKey]->theKey = theKey;
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int16_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theType = ValueType::Array;
		this->theValues[theKey]->theKey = theKey;
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	}
}

void JsonObject::pushBack(const char* theKey, int8_t other) noexcept {
	if (!this->theValues.contains(theKey)) {
		this->theValues[theKey] = std::make_unique<JsonObject>();
		this->theValues[theKey]->theType = ValueType::Array;
		this->theValues[theKey]->theKey = theKey;
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
	} else {
		size_t theSize = this->theValues[theKey]->theValues.size();
		this->theValues[theKey]->theValues[std::to_string(theSize)] = std::make_unique<JsonObject>();
		*this->theValues[theKey]->theValues[std::to_string(theSize)] = other;
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

	operator const JsonObject();

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

EmbedFieldData::operator const JsonObject() {
	JsonObject theData{ ValueType::Object };
	theData["inline"] = this->Inline;
	theData["value"] = this->value;
	theData["name"] = this->name;
	return theData;
}

EmbedData::operator JsonObject() {
	JsonObject theData{ ValueType::Object };
	for (auto& value2 : this->fields) {
		theData.pushBack("fields", value2);
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
	JsonObject theData{ ValueType::Object };
	theData["type"] = static_cast<uint8_t>(this->type);
	if (this->data.attachments.size() > 0) {
		for (auto& value : this->data.attachments) {
			// theData["data"].pushBack("attachments", value );
		}
	}
	for (auto& value : this->data.components) {
		//theData["data"].pushBack("components",  value );
	}
	theData["data"]["allowed_mentions"] = JsonObject{ this->data.allowedMentions.operator JsonObject() };
	if (this->data.choices.size() > 0) {
		for (auto& value : this->data.choices) {
			JsonObject theValue{ ValueType::Object };
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
		theData["data"].pushBack("embeds", value);
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


WebSocketIdentifyData::operator JsonObject() {
	JsonObject theSerializer{ ValueType::Object };
	std::unordered_map<std::string, std::string> theMap{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);
	
	for (auto& value : this->presence.activities) {
		JsonObject theSerializer02{ ValueType::Object };
		if (value.url != "") {
			theSerializer02["url"] = std::string{ value.url };
		}
		theSerializer02["name"] = std::string{ value.name };
		theSerializer02["type"] = uint32_t{ static_cast<uint32_t>(value.type) };
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
	JsonObject theData{ ValueType::Object };
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
		WebSocketIdentifyData theDataBew{};
		theDataBew.numberOfShards = 0;
		theDataBew.currentShard = 23;
		DiscordCoreAPI::ActivityData theData{};
		theData.name = "TESTING";
		theDataBew.presence.activities.push_back(theData);
		//std::string theString = JsonSerializer{}.getString(theDataBew);
		//std::cout << "THE FINAL STRING: 0101 " << theString << std::endl;
		//std::cout << "THE FINAL STRING (PARSED): " << nlohmann::jsoWebSocketIdentifyData
		std::string theString02 = std::string{ theDataBew.operator JsonObject() };
		std::cout << "THE FINAL STRING: 0101 " << theString02 << std::endl;
		std::cout << "THE FINAL STRING (PARSED): " << nlohmann::json::parse(theString02).dump() << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds{ 3 });

	}
	catch (...) { DiscordCoreAPI::reportException("main()"); };

	return 0;
}