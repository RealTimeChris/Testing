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

EnumConverter::operator std::vector<Uint64>() {
	std::vector<Uint64> theObject{};
	for (auto& value: *static_cast<std::vector<Uint64>*>(this->thePtr)) {
		theObject.emplace_back(value);
	}
	return theObject;
}

EnumConverter::operator Uint64() {
	return Uint64{ *static_cast<Uint64*>(this->thePtr) };
}

EnumConverter::~EnumConverter() {
	if (this->thePtr) {
		if (this->vectorType) {
			delete static_cast<std::vector<Uint64>*>(this->thePtr);
		} else {
			delete static_cast<Uint64*>(this->thePtr);
		}
	}
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const StringType& theData) noexcept {
	this->string = new StringType{};
	*this->string = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(StringType&& theData) noexcept {
	this->string = new StringType{};
	*this->string = std::move(theData);
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const char* theData) noexcept {
	*this = ValueType::String;
	*this->string = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(Uint64 theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(Uint32 theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(Uint16 theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(Uint8 theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(Int64 theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(Int32 theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(Int16 theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(Int8 theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(Double theData) noexcept {
	this->numberDouble = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(Float theData) noexcept {
	this->numberDouble = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(Bool theData) noexcept {
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
		default: {
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
	this->theValue = Uint64{ theData };
	this->theValue.numberUint = Uint64{ theData };
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(EnumConverter theData) noexcept {
	*this = std::move(theData);
}

JsonObject& JsonObject::operator=(JsonObject&& theKey) noexcept {
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
			*this->theValue.string = std::move(*theKey.theValue.string);
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
	this->theType = theKey.theType;
	return *this;
}

JsonObject::JsonObject(JsonObject&& theKey) noexcept {
	*this = std::move(theKey);
}

JsonObject& JsonObject::operator=(const JsonObject& theKey) noexcept {
	this->theValue.destroy(this->theType);
	switch (theKey.theType) {
		case ValueType::Object: {
			this->theValue = ValueType::Object;
			for (auto& [key, value]: *theKey.theValue.object) {
				this->theValue.object->emplace(key, std::move(value));
			}
			this->theType = ValueType::Object;
			break;
		}
		case ValueType::Array: {
			this->theValue = ValueType::Array;
			for (auto& value: *theKey.theValue.array) {
				this->theValue.array->emplace_back(std::move(value));
			}
			this->theType = ValueType::Array;
			break;
		}
		case ValueType::String: {
			this->theValue = ValueType::String;
			*this->theValue.string = *theKey.theValue.string;
			this->theType = ValueType::String;
			break;
		}
		case ValueType::Bool: {
			this->theValue.boolean = theKey.theValue.boolean;
			this->theType = ValueType::Bool;
			break;
		}
		case ValueType::Int64: {
			this->theValue.numberInt = theKey.theValue.numberInt;
			this->theType = ValueType::Int64;
			break;
		}
		case ValueType::Uint64: {
			this->theValue.numberUint = theKey.theValue.numberUint;
			this->theType = ValueType::Uint64;
			break;
		}
		case ValueType::Float: {
			this->theValue.numberDouble = theKey.theValue.numberDouble;
			this->theType = ValueType::Float;
			break;
		}
		case ValueType::Null: {
			this->theType = ValueType::Null;
			break;
		}
		default: {
			break;
		}
	}
	this->theType = theKey.theType;
	return *this;
}

JsonObject::JsonObject(const JsonObject& theKey) noexcept {
	*this = theKey;
}

JsonObject& JsonObject::operator=(String&& theData) noexcept {
	this->theValue = std::move(theData);
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(String&& theData) noexcept {
	*this = std::move(theData);
}

JsonObject& JsonObject::operator=(const String& theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(const String& theData) noexcept {
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

JsonObject& JsonObject::operator=(Uint64 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint64 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Uint32 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint32 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Uint16 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint16 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Uint8 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint8 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int64 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int64 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int32 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int32 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int16 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int16 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int8 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int8 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Double theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Float;
	return *this;
}

JsonObject::JsonObject(Double theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Float theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Float;
	return *this;
}

JsonObject::JsonObject(Float theData) noexcept {
	this->theValue = theData;
}

JsonObject& JsonObject::operator=(Bool theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Bool;
	return *this;
}

JsonObject::JsonObject(Bool theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(ValueType theType) noexcept {
	this->theValue = theType;
	this->theType = theType;
	return *this;
}

JsonObject& JsonObject::operator[](Uint64 index) const {
	return this->theValue.array->operator[](index);
}

JsonObject& JsonObject::operator[](Uint64 index) {
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

void JsonObject::pushBack(JsonObject&& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->theType = ValueType::Array;
		this->theValue = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(std::move(other));
	}
}

void JsonObject::pushBack(JsonObject& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->theType = ValueType::Array;
		this->theValue = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(std::move(other));
	}
}

JsonObject::operator String() const noexcept {
	String theString{};
	switch (this->theType) {
		case ValueType::Object: {
			if (this->theValue.object->empty()) {
				theString += "{}";
			}

			theString += '{';

			Uint64 theIndex{};
			for (auto iterator = this->theValue.object->cbegin(); iterator != this->theValue.object->cend(); ++iterator) {
				theString += '\"';
				theString += iterator->first;
				theString += "\":";
				theString += iterator->second;
				if (theIndex < this->theValue.object->size() - 1) {
					theString += ',';
				}
				theIndex++;
			}
			theString += '}';
			break;
		}
		case ValueType::Array: {
			if (this->theValue.array->empty()) {
				theString += "[]";
				break;
			}

			theString += '[';

			for (auto iterator = this->theValue.array->cbegin(); iterator != this->theValue.array->cend() - 1; ++iterator) {
				theString += *iterator;
				theString += ',';
			}

			theString += this->theValue.array->back();

			theString += ']';
			break;
		}

		case ValueType::String: {
			theString += '\"';
			theString += std::move(*this->theValue.string);
			theString += '\"';
			break;
		}
		case ValueType::Bool: {
			std::stringstream theStream{};
			theStream << std::boolalpha << this->theValue.boolean;
			theString += theStream.str();
			break;
		}
		case ValueType::Float: {
			theString += std::to_string(this->theValue.numberDouble);
			break;
		}
		case ValueType::Uint64: {
			theString += std::to_string(this->theValue.numberUint);
			break;
		}
		case ValueType::Int64: {
			theString += std::to_string(this->theValue.numberInt);
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

JsonObject::operator String() noexcept {
	String theString{};
	switch (this->theType) {
		case ValueType::Object: {
			if (this->theValue.object->empty()) {
				theString += "{}";
			}

			theString += '{';

			Uint64 theIndex{};
			for (auto iterator = this->theValue.object->cbegin(); iterator != this->theValue.object->cend(); ++iterator) {
				theString += '\"';
				theString += iterator->first;
				theString += "\":";
				theString += iterator->second;
				if (theIndex < this->theValue.object->size() - 1) {
					theString += ',';
				}
				theIndex++;
			}
			theString += '}';
			break;
		}
		case ValueType::Array: {
			if (this->theValue.array->empty()) {
				theString += "[]";
				break;
			}

			theString += '[';

			for (auto iterator = this->theValue.array->cbegin(); iterator != this->theValue.array->cend() - 1; ++iterator) {
				theString += *iterator;
				theString += ',';
			}

			theString += this->theValue.array->back();

			theString += ']';
			break;
		}

		case ValueType::String: {
			theString += '\"';
			theString += std::move(*this->theValue.string);
			theString += '\"';
			break;
		}
		case ValueType::Bool: {
			std::stringstream theStream{};
			theStream << std::boolalpha << this->theValue.boolean;
			theString += theStream.str();
			break;
		}
		case ValueType::Float: {
			theString += std::to_string(this->theValue.numberDouble);
			break;
		}
		case ValueType::Uint64: {
			theString += std::to_string(this->theValue.numberUint);
			break;
		}
		case ValueType::Int64: {
			theString += std::to_string(this->theValue.numberInt);
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

JsonObject::~JsonObject() noexcept {
	this->theValue.destroy(this->theType);
}

struct WebSocketIdentifyData {
	DiscordCoreInternal::UpdatePresenceData presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	int64_t intents{};

	operator String();
};

WebSocketIdentifyData::operator String() {
	std::unordered_map<std::string, std::string> theMap{};
	JsonObject theSerializer{};
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
		theSerializer["d"]["presence"]["activities"].pushBack(std::move(theSerializer02));
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
	
	return static_cast<String>(theSerializer);

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
		WebSocketIdentifyData theDataBewTwo{};
		ErlPacker thePacker{};
		DiscordCoreAPI::ActivityData theData{};
		std::vector<std::string> theResults01{};
		theData.name = "TESTING";
		theDataBewTwo.numberOfShards = 0;
		theDataBewTwo.currentShard = 23;
		theDataBewTwo.presence.activities.push_back(theData);
		std::vector<std::string> theResults02{};
		theStopWatch.resetTimer();
		for (int32_t x = 0; x < 128 * 128; ++x) {
			//theResults01.push_back(theDataBewTwo);
		}
		std::cout << "THE TIME 01: " << theStopWatch.totalTimePassed() << std::endl;
		
		WebSocketIdentifyData theDataBew{};
		theDataBew.numberOfShards = 0;
		theDataBew.currentShard = 23;
		//auto theResult = thePacker.parseJsonToEtf(theDataBew.operator DiscordCoreAPI::String());
		std::cout << "THE RESULT: " << theDataBewTwo.operator DiscordCoreAPI::String() << std::endl;
		//std::cout << "THE RESULT: " << thePacker.parseEtfToJson(theResult) << std::endl;

		theDataBew.presence.activities.push_back(theData);


		theStopWatch.resetTimer();
		for (int32_t x = 0; x < 128 * 128; ++x) {
			//theResults01.push_back(static_cast<std::string>(theDataBew));
		}
		std::cout << "THE TIME 01: " << theStopWatch.totalTimePassed() << std::endl;


		//std::string this->theString = JsonSerializer{}.getString(theDataBew);
		//std::cout << "THE FINAL STRING: 0101 " << this->theString << std::endl;
		//std::cout << "THE FINAL STRING (PARSED): " << nlohmann::jsoWebSocketIdentifyData
		//std::string theString02 = static_cast<std::string>(theDataBew);
		//std::cout << "THE FINAL STRING: 0101 " << theString02 << std::endl;
		//std::cout << "THE FINAL STRING (PARSED): " << nlohmann::json::parse(theString02).dump() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds{ 3 });

	} catch (...) {
		DiscordCoreAPI::reportException("main()");
	};

	return 0;
}
