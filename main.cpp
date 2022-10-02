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
	*this = std::move(other);
}

EnumConverter::operator std::vector<uint64_t>() {
	std::vector<uint64_t> theObject{};
	for (auto& value: *static_cast<std::vector<uint64_t>*>(this->thePtr)) {
		theObject.emplace_back(value);
	}
	return theObject;
}


EnumConverter::operator uint64_t() {
	return uint64_t{ *static_cast<uint64_t*>(this->thePtr) };
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
	this->theValue = uint64_t{ theData };
	this->theValue.numberUint = uint64_t{ theData };
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(EnumConverter theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const char* theData) noexcept {
	this->string = JsonObject::create<StringType>();
	*this->string = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const std::string theData) noexcept {
	this->string = JsonObject::create<StringType>();
	*this->string = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(uint64_t theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(uint32_t theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(uint16_t theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(uint8_t theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(int64_t theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(int32_t theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(int16_t theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(int8_t theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(double theData) noexcept {
	this->numberDouble = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(float theData) noexcept {
	this->numberDouble = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(bool theData) noexcept {
	this->boolean = theData;
	return *this;
}

JsonObject::JsonValue::JsonValue(const char* theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(const std::string theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(uint64_t theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(uint32_t theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(uint16_t theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(uint8_t theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(int64_t theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(int32_t theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(int16_t theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(int8_t theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(double theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(float theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(bool theData) noexcept {
	*this = theData;
}

JsonObject::JsonValue::JsonValue(ValueType theType) noexcept {
	*this = theType;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(ValueType theType) noexcept {
	switch (theType) {
		case ValueType::Object: {
			this->object = create<ObjectType>();
			break;
		}

		case ValueType::Array: {
			this->array = create<ArrayType>();
			break;
		}

		case ValueType::String: {
			this->string = create<StringType>("");
			break;
		}

		case ValueType::Bool: {
			this->boolean = static_cast<BoolType>(false);
			break;
		}

		case ValueType::Int64: {
			this->numberInt = static_cast<IntType>(0);
			break;
		}

		case ValueType::Uint64: {
			this->numberUint = static_cast<UintType>(0);
			break;
		}

		case ValueType::Float: {
			this->numberDouble = static_cast<FloatType>(0.0);
			break;
		}

		case ValueType::Null: {
			break;
		}
	}
	return *this;
}

JsonObject& JsonObject::operator=(const JsonArray& theData) noexcept {
	this->theString = theData.theString;
	this->theValue = theData.theValue;
	this->theType = theData.theType;
	this->theKey = theData.theKey;
	return *this;
}

JsonObject::JsonObject(const JsonArray& theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(const JsonObject& theKey) noexcept {
	this->theString = theKey.theString;
	this->theValue = theKey.theValue;
	this->theType = theKey.theType;
	this->theKey = theKey.theKey;
	return *this;
}

JsonObject::JsonObject(const JsonObject& theKey) noexcept {
	*this = theKey;
}

JsonObject::JsonObject(const JsonObject::JsonValue& other) noexcept {
	this->theValue = other;
}

JsonObject::JsonObject(const char* theKey, const JsonObject& theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(const char* theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(const char* theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(std::string theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(std::string theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(uint64_t theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(uint64_t theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(uint32_t theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(uint32_t theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(uint16_t theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(uint16_t theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(uint8_t theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(uint8_t theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(int64_t theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(int64_t theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(int32_t theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(int32_t theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(int16_t theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(int16_t theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(int8_t theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(int8_t theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(double theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Float;
	return *this;
}

JsonObject::JsonObject(double theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(float theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Float;
	return *this;
}

JsonObject::JsonObject(float theData) noexcept {
	this->theValue = theData;
}

JsonObject& JsonObject::operator=(bool theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Bool;
	return *this;
}

JsonObject::JsonObject(bool theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator[](const char* theKey) noexcept {
	if (this->theKey == "") {
		std::cout << "WERE HERE THIS IS IT!0202: " << theKey << std::endl;
		this->theKey = theKey;
		this->theType = ValueType::Object;
		this->theValue = ValueType::Object;
		return *this;
	} else if (this->theKey == theKey && this->theType == ValueType::Object) {
		std::cout << "WERE HERE THIS IS IT!0404: " << theKey << std::endl;
		return *this;
	} else if (!this->theValue.object->contains(theKey)) {
		std::cout << "WERE HERE THIS IS IT!0505: " << theKey << std::endl;
		JsonObject theObject{};
		theObject.theKey = theKey;
		theObject.theType = ValueType::Object;
		this->theValue.object->emplace(theKey, theObject);
		return this->theValue.object->at(theKey);
	} else {
		std::cout << "WERE HERE THIS IS IT!: " << theKey << std::endl;
		return *this;
	}
}

std::string JsonObject::dump(const JsonObject& theData,std::string& theString, const unsigned int indentationLevel, const unsigned int current_indent) {
	switch (theData.theType) {
		case ValueType::Object: {
			std::cout << "THE OBJECT SIZE: " << theData.theValue.object->size() << std::endl; 
			if (theData.theValue.object->empty()) {
				
				theString += "{}";
				return theString;
			}

				theString += '{';
		
				auto iterator = theData.theValue.object->cbegin();
				for (std::size_t count = 0; count < theData.theValue.object->size() - 1; ++count, ++iterator) {
					theString += '\"';
					theString += iterator->first;
					theString += "\":";
					dump(iterator->second, theString, indentationLevel, current_indent);
					theString += ',';
					std::cout << "THE STRING: " << theString << std::endl;
				}
				std::cout << "THE STRING: " << theString << std::endl;
				assert(iterator != theData.theValue.object->cend());
				assert(std::next(iterator) == theData.theValue.object->cend());
				theString += '\"';
				theString += iterator->first;
				theString += "\":";
				dump(iterator->second, theString, indentationLevel, current_indent);
				std::cout << "THE STRING: " << theString << std::endl;
				theString += '}';
				return theString;
			} case ValueType::Array: {
			if (theData.theValue.array->empty()) {
					theString += "[]";
				return theString;
			}

			theString += '[';

			// first n-1 elements
			for (auto iterator = theData.theValue.array->cbegin(); iterator != theData.theValue.array->cend() - 1; ++iterator) {
				dump(*iterator, theString, current_indent);
				theString += ',';
			}

			// last element
			assert(!theData.theValue.array->empty());
			dump(theData.theValue.array->back(), theString, indentationLevel, current_indent);

			theString += ']';
			return theString;

			
		}

		case ValueType::String:{
			theString += '\"';
			theString += *theData.theValue.string;
			theString += '\"';
			return theString;
		}
		case ValueType::Bool: {
			std::stringstream theStream{};
			theStream << std::boolalpha << theData.theValue.boolean;
			theString += theStream.str();
			return theString;
		}
		case ValueType::Float: {
			theString += std::to_string(theData.theValue.numberDouble);
			return theString;
		}
		case ValueType::Uint64: {
			theString += std::to_string(theData.theValue.numberUint);
			return theString;
		}
		case ValueType::Int64: {
			theString += std::to_string(theData.theValue.numberInt);
			return theString;
		}
		case ValueType::Null: {
			theString += "null";
			return theString;
		}
		case ValueType::Null_Ext: {
			theString += "[]";
			return theString;
		}
	}
}

JsonObject::operator std::string() noexcept {
	std::string theString{};
	theString = this->dump(*this, theString, 0, 0);
	return theString;
}

void JsonObject::pushBack(const char* theKey, std::string other) noexcept {
	if (this->theType == ValueType::Array) {
		this->theValue.array->push_back(other);
	} else {
		this->theValue = ValueType::Array;
		this->theValue.array->push_back(other);
	}
}

void JsonObject::pushBack(const char* theKey, JsonObject other) noexcept {
	if (this->theType == ValueType::Array) {
		this->theValue.array->push_back(other);
	} else {
		this->theValue = ValueType::Array;
		this->theValue.array->push_back(other);
	}
};

void JsonObject::pushBack(const char* theKey, uint64_t other) noexcept {
	if (this->theType == ValueType::Array) {
		this->theValue.array->push_back(other);
	} else {
		this->theValue = ValueType::Array;
		this->theValue.array->push_back(other);
	}
}

void JsonObject::pushBack(const char* theKey, uint32_t other) noexcept {
	if (this->theType == ValueType::Array) {
		this->theValue.array->push_back(other);
	} else {
		this->theValue = ValueType::Array;
		this->theValue.array->push_back(other);
	}
}

void JsonObject::pushBack(const char* theKey, uint16_t other) noexcept {
	if (this->theType == ValueType::Array) {
		this->theValue.array->push_back(other);
	} else {
		this->theValue = ValueType::Array;
		this->theValue.array->push_back(other);
	}
}

void JsonObject::pushBack(const char* theKey, uint8_t other) noexcept {
	if (this->theType == ValueType::Array) {
		this->theValue.array->push_back(other);
	} else {
		this->theValue = ValueType::Array;
		this->theValue.array->push_back(other);
	}
}

void JsonObject::pushBack(const char* theKey, int64_t other) noexcept {
	if (this->theType == ValueType::Array) {
		this->theValue.array->push_back(other);
	} else {
		this->theValue = ValueType::Array;
		this->theValue.array->push_back(other);
	}
}

void JsonObject::pushBack(const char* theKey, int32_t other) noexcept {
	if (this->theType == ValueType::Array) {
		this->theValue.array->push_back(other);
	} else {
		this->theValue = ValueType::Array;
		this->theValue.array->push_back(other);
	}
}

void JsonObject::pushBack(const char* theKey, int16_t other) noexcept {
	if (this->theType == ValueType::Array) {
		this->theValue.array->push_back(other);
	} else {
		this->theValue = ValueType::Array;
		this->theValue.array->push_back(other);
	}
}

void JsonObject::pushBack(const char* theKey, int8_t other) noexcept {
	if (this->theType == ValueType::Array) {
		this->theValue.array->push_back(other);
	} else {
		this->theValue = ValueType::Array;
		this->theValue.array->push_back(other);
	}
}

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
	std::unordered_map<std::string, std::string> theMap{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);
	
	
	for (auto& value : this->presence.activities) {
		JsonObject theSerializer02{ "" };
		if (value.url != "") {
			theSerializer02["url"] = std::string{ value.url };
		}
		//theSerializer02["theType"] = uint32_t{ static_cast<uint32_t>(value.type) };
		//theSerializer02["name"] = "TESTING";
		//theSerializer02["test02"] = uint32_t{ static_cast<uint32_t>(value.type) };
		theSerializer["d"]["presences"].pushBack("activities", theSerializer02);
		theSerializer["d"]["presences"].pushBack("activities", theSerializer02);
	}
	theSerializer["d"]["afk"] = this->presence.afk;
	if (this->presence.since != 0) {
		theSerializer["since"] = this->presence.since;
	}
	
	theSerializer["d"]["status"] = this->presence.status;
	theSerializer["d"]["properties"];
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
	
	//theSerializer["op"] = static_cast<uint32_t>(2);
	
	return theSerializer;

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