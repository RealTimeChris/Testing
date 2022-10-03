#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"
#include <memory>
#include <nlohmann/json.hpp>

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

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const StringType theData) noexcept {
	*this = ValueType::String;
	*this->string = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const char* theData) noexcept {
	*this = ValueType::String;
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

JsonObject::JsonValue& JsonObject::JsonValue::operator=(ValueType theType) noexcept {
	switch (theType) {
		case ValueType::Object: {
			this->object = new ObjectType{};
			break;
		}
		case ValueType::Array: {
			this->array = new ArrayType{};
			break;
		}
		case ValueType::String: {
			this->string = new StringType{};
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

JsonObject::JsonValue::JsonValue(ValueType theType) noexcept {
	*this = theType;
}

void JsonObject::JsonValue::destroy(ValueType theType) {
	switch (theType) {
		case ValueType::Array: {
			delete this->array;
			break;
		}
		case ValueType::Object: {
			delete this->object;
			break;
		}
		case ValueType::String: {
			delete this->string;
			break;
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
	*this = std::move(theData);
}

JsonObject& JsonObject::operator=(const JsonObject& theKey) noexcept {
	if (this->theType != ValueType::Null) {
		this->theValue.destroy(this->theType);
	}
	switch (theKey.theType) {
		case ValueType::Object: {
			this->theValue = ValueType::Object;
			for (auto& [key, value]: *theKey.theValue.object) {
				this->theValue.object->emplace(key, std::move(value));
			}
			break;
		}
		case ValueType::Array: {
			this->theValue = ValueType::Array;
			for (auto& value: *theKey.theValue.array) {
				this->theValue.array->emplace_back(std::move(value));
			}
			break;
		}
		case ValueType::String: {
			this->theValue = ValueType::String;
			*this->theValue.string = *theKey.theValue.string;
			break;
		}
		case ValueType::Bool: {
			this->theValue.boolean = theKey.theValue.boolean;
			break;
		}
		case ValueType::Int64: {
			this->theValue.numberInt = theKey.theValue.numberInt;
			break;
		}
		case ValueType::Uint64: {
			this->theValue.numberUint = theKey.theValue.numberUint;
			break;
		}
		case ValueType::Float: {
			this->theValue.numberDouble = theKey.theValue.numberDouble;
			break;
		}
		case ValueType::Null: {
			break;
		}
	}
	this->theKey = theKey.theKey;
	this->theString = theKey.theString;
	this->theType = theKey.theType;
	return *this;
}

JsonObject::JsonObject(const JsonObject& theKey) noexcept {
	*this = theKey;
}

JsonObject& JsonObject::operator=(const std::string theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(const std::string theData) noexcept {
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

JsonObject::JsonObject(const char* theKey, ValueType theType) noexcept {
	this->theKey = theKey;
	this->theType = theType;
	this->theValue = this->theType;
}

JsonObject& JsonObject::operator=(ValueType theType) noexcept {
	this->theValue = theType;
	this->theType = theType;
	return *this;
}

JsonObject& JsonObject::operator[](size_t index) const {
	return this->theValue.array->operator[](index);
}

JsonObject& JsonObject::operator[](size_t index) {
	if (this->theType == ValueType::Null) {
		this->theType = ValueType::Array;
		this->theValue = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		if (index >= this->theValue.array->size()) {
			this->theValue.array->resize(index + 1);
		}

		return this->theValue.array->operator[](index);
	}
	throw std::runtime_error{ "Sorry, but that index could not be produced/accessed." };
}

JsonObject& JsonObject::operator[](const typename ObjectType::key_type& key) const {
	if (this->theType == ValueType::Object) {
		auto result = this->theValue.object->emplace(key, nullptr);
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

JsonObject& JsonObject::operator[](typename ObjectType::key_type key) {
	if (this->theType == ValueType::Null) {
		this->theType = ValueType::Object;
		this->theValue = ValueType::Object;
	}

	if (this->theType == ValueType::Object) {
		auto result = this->theValue.object->emplace(std::move(key), JsonObject{});
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

void JsonObject::pushBack(JsonObject other) noexcept {
	if (this->theType == ValueType::Null) {
		this->theType = ValueType::Array;
		this->theValue = ValueType::Array;
		this->theKey = theKey;
	} else if (this->theType == ValueType::Object) {
		this->theValue.object->emplace(theKey, ValueType::Array);
		this->theValue.object->at(theKey).theValue.array->emplace_back(other);
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(other);
	}
};

JsonObject::~JsonObject() noexcept {
	this->theValue.destroy(this->theType);
}

void writeToString(const char* theData, size_t& currentlyUsedSpace, size_t theLength, std::string& theString) {
	if (theString.size() < currentlyUsedSpace + theLength) {
		if (theString.size() == 0) {
			theString.resize(512);
		}
		theString.resize(theString.size() * 2);
	}
	memcpy(theString.data() + currentlyUsedSpace, theData, theLength);
	currentlyUsedSpace += theLength;
}

JsonObject::operator std::string_view() noexcept {
	switch (this->theType) {
		case ValueType::Object: {
			if (this->theValue.object->empty()) {
				writeToString("{}", this->currentlyUsedSpace, 2, this->theString);
			}
			writeToString("{", this->currentlyUsedSpace, 1, this->theString);
			size_t theIndex{};
			for (auto iterator = this->theValue.object->cbegin(); iterator != this->theValue.object->cend(); ++iterator) {
				writeToString("\"", this->currentlyUsedSpace, 1, this->theString);
				std::string_view theView = iterator->first;
				writeToString(theView.data(), this->currentlyUsedSpace, theView.size(), this->theString);
				writeToString("\":", this->currentlyUsedSpace, 2, this->theString);
				auto theViewNew = static_cast<std::string_view>(iterator->second);
				writeToString(theViewNew.data(), this->currentlyUsedSpace, theViewNew.size(), this->theString);
				if (theIndex < this->theValue.object->size() - 1) {
					writeToString(",", this->currentlyUsedSpace, 1, this->theString);
				}
				theIndex++;
			}
			writeToString("}", this->currentlyUsedSpace, 1, this->theString);
			break;
		}
		case ValueType::Array: {
			if (this->theValue.array->empty()) {
				writeToString("[]", this->currentlyUsedSpace, 2, this->theString);
				break;
			}

			writeToString("[", this->currentlyUsedSpace, 1, this->theString);

			for (auto iterator = this->theValue.array->cbegin(); iterator != this->theValue.array->cend() - 1; ++iterator) {
				auto theString = std::string_view{ *iterator };
				writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
				writeToString(",", this->currentlyUsedSpace, 1, this->theString);
			}

			auto theString = std::string_view{ this->theValue.array->back() };
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);

			writeToString("]", this->currentlyUsedSpace, 1, this->theString);
			break;
		}

		case ValueType::String: {
			writeToString("\"", this->currentlyUsedSpace, 1, this->theString);
			auto theString = *this->theValue.string;
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			writeToString("\"", this->currentlyUsedSpace, 1, this->theString);
			break;
		}
		case ValueType::Bool: {
			std::stringstream theStream{};
			theStream << std::boolalpha << this->theValue.boolean;
			theString += theStream.str();
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
		case ValueType::Float: {
			theString += std::to_string(this->theValue.numberDouble);
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
		case ValueType::Uint64: {
			theString += std::to_string(this->theValue.numberUint);
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
		case ValueType::Int64: {
			theString += std::to_string(this->theValue.numberInt);
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
		case ValueType::Null: {
			theString += "null";
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
		case ValueType::Null_Ext: {
			theString += "[]";
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
	}
	return std::string_view{ this->theString.data(), this->currentlyUsedSpace };
}

JsonObject::operator std::string_view() const noexcept {
	switch (this->theType) {
		case ValueType::Object: {
			if (this->theValue.object->empty()) {
				writeToString("{}", this->currentlyUsedSpace, 2, this->theString);
			}
			writeToString("{", this->currentlyUsedSpace, 1, this->theString);
			size_t theIndex{};
			for (auto iterator = this->theValue.object->cbegin(); iterator != this->theValue.object->cend(); ++iterator) {
				writeToString("\"", this->currentlyUsedSpace, 1, this->theString);
				std::string_view theView{ iterator->first };
				writeToString(theView.data(), this->currentlyUsedSpace, theView.size(), this->theString);
				writeToString("\":", this->currentlyUsedSpace, 2, this->theString);
				auto theViewNew = static_cast<std::string_view>(iterator->second);
				writeToString(theViewNew.data(), this->currentlyUsedSpace, theViewNew.size(), this->theString);
				if (theIndex < this->theValue.object->size() - 1) {
					writeToString(",", this->currentlyUsedSpace, 1, this->theString);
				}
				theIndex++;
			}
			writeToString("}", this->currentlyUsedSpace, 1, this->theString);
			break;
		}
		case ValueType::Array: {
			if (this->theValue.array->empty()) {
				writeToString("[]", this->currentlyUsedSpace, 2, this->theString);
				break;
			}

			writeToString("[", this->currentlyUsedSpace, 1, this->theString);

			for (auto iterator = this->theValue.array->cbegin(); iterator != this->theValue.array->cend() - 1; ++iterator) {
				auto theString = std::string_view{ *iterator };
				writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
				writeToString(",", this->currentlyUsedSpace, 1, this->theString);
			}

			auto theString = std::string_view{ this->theValue.array->back() };
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);

			writeToString("]", this->currentlyUsedSpace, 1, this->theString);
			break;
		}

		case ValueType::String: {
			writeToString("\"", this->currentlyUsedSpace, 1, this->theString);
			auto theString = *this->theValue.string;
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			writeToString("\"", this->currentlyUsedSpace, 1, this->theString);
			break;
		}
		case ValueType::Bool: {
			std::stringstream theStream{};
			theStream << std::boolalpha << this->theValue.boolean;
			theString += theStream.str();
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
		case ValueType::Float: {
			theString += std::to_string(this->theValue.numberDouble);
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
		case ValueType::Uint64: {
			theString += std::to_string(this->theValue.numberUint);
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
		case ValueType::Int64: {
			theString += std::to_string(this->theValue.numberInt);
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
		case ValueType::Null: {
			theString += "null";
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
		case ValueType::Null_Ext: {
			theString += "[]";
			writeToString(theString.data(), this->currentlyUsedSpace, theString.size(), this->theString);
			break;
		}
	}
	return std::string_view{ this->theString.data(), this->currentlyUsedSpace };
}

struct WebSocketIdentifyData {
	DiscordCoreInternal::UpdatePresenceData presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	int64_t intents{};

	operator std::string();
};

WebSocketIdentifyData::operator std::string() {
	JsonObject theSerializer{ "d", ValueType::Object };
	std::unordered_map<std::string, std::string> theMap{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);
	
	for (auto& value : this->presence.activities) {
		JsonObject theSerializer02{ };
		if (value.url != "") {
			theSerializer02["url"] = std::string{ value.url };
		}
		theSerializer02["theType"] = uint32_t{ static_cast<uint32_t>(value.type) };
		theSerializer02["name"] = "TESTING";
		theSerializer02["test02"] = uint32_t{ static_cast<uint32_t>(value.type) };
		theSerializer["d"]["presence"]["activities"].pushBack(theSerializer02);
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
	//theSerializer["d"].pushBack("shard", static_cast<uint32_t>(this->currentShard));
	//theSerializer["d"].pushBack("shard", static_cast<uint32_t>(this->numberOfShards));
	theSerializer["d"]["token"] = this->botToken;
	
	theSerializer["op"] = static_cast<uint32_t>(2);
	return std::string{ theSerializer };

}

struct WebSocketIdentifyDataTwo {
	DiscordCoreInternal::UpdatePresenceData presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	int64_t intents{};

	operator std::string();
};

WebSocketIdentifyDataTwo::operator std::string() {
	nlohmann::json theSerializer{};
	std::unordered_map<std::string, std::string> theMap{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);

	for (auto& value: this->presence.activities) {
		nlohmann::json theSerializer02{};
		if (value.url != "") {
			theSerializer02["url"] = std::string{ value.url };
		}
		theSerializer02["theType"] = uint32_t{ static_cast<uint32_t>(value.type) };
		theSerializer02["name"] = "TESTING";
		theSerializer02["test02"] = uint32_t{ static_cast<uint32_t>(value.type) };
		theSerializer["d"]["presence"]["activities"].push_back(theSerializer02);
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
	//theSerializer["d"].pushBack("shard", static_cast<uint32_t>(this->currentShard));
	//theSerializer["d"].pushBack("shard", static_cast<uint32_t>(this->numberOfShards));
	theSerializer["d"]["token"] = this->botToken;

	theSerializer["op"] = static_cast<uint32_t>(2);

	return theSerializer.dump();
}

int32_t main() noexcept {
	try {
		DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::milliseconds{ 1 } };
		WebSocketIdentifyDataTwo theDataBewTwo{};
		DiscordCoreAPI::ActivityData theData{};
		std::vector<std::string> theResults01{};
		theData.name = "TESTING";
		theDataBewTwo.numberOfShards = 0;
		theDataBewTwo.currentShard = 23;
		theDataBewTwo.presence.activities.push_back(theData);
		std::vector<std::string> theResults02{};
		theStopWatch.resetTimer();
		for (int32_t x = 0; x < 64 * 64; ++x) {
			theResults01.push_back(theDataBewTwo);
		}
		std::cout << "THE TIME 01: " << theStopWatch.totalTimePassed() << std::endl;

		WebSocketIdentifyData theDataBew{};
		theDataBew.numberOfShards = 0;
		theDataBew.currentShard = 23;

		theDataBew.presence.activities.push_back(theData);


		theStopWatch.resetTimer();
		for (int32_t x = 0; x < 64 * 64; ++x) {
			theResults01.push_back(theDataBew);
		}
		std::cout << "THE TIME 01: " << theStopWatch.totalTimePassed() << std::endl;


		//std::string this->theString = JsonSerializer{}.getString(theDataBew);
		//std::cout << "THE FINAL STRING: 0101 " << this->theString << std::endl;
		//std::cout << "THE FINAL STRING (PARSED): " << nlohmann::jsoWebSocketIdentifyData
		std::string theString02 = std::string{ theDataBew };
		std::cout << "THE FINAL STRING: 0101 " << theString02 << std::endl;
		std::cout << "THE FINAL STRING (PARSED): " << nlohmann::json::parse(theString02).dump() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds{ 3 });

	} catch (...) {
		DiscordCoreAPI::reportException("main()");
	};

	return 0;
}