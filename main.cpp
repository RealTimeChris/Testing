#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"
#include "JsonObject.hpp"

EnumConverter& EnumConverter::operator=(EnumConverter&& other) noexcept {
	this->theVector = std::move(other.theVector);
	this->vectorType = other.vectorType;
	this->theUint = other.theUint;
	return *this;
}

EnumConverter::EnumConverter(EnumConverter&& other) noexcept {
	*this = std::move(other);
}

EnumConverter::operator std::vector<Uint64>() const noexcept {
	std::vector<Uint64> theObject{};
	for (auto& value: this->theVector) {
		theObject.emplace_back(value);
	}
	return theObject;
}

EnumConverter::operator std::vector<Uint64>() noexcept {
	std::vector<Uint64> theObject{};
	for (auto& value: this->theVector) {
		theObject.emplace_back(value);
	}
	return theObject;
}

EnumConverter::operator Uint64() const noexcept {
	return this->theUint;
}

EnumConverter::operator Uint64() noexcept {
	return this->theUint;
}

bool EnumConverter::isItAVector() const noexcept {
	return this->vectorType;
}

bool EnumConverter::isItAVector() noexcept {
	return this->vectorType;
}

JsonObject::JsonValue::JsonValue() noexcept {};

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const ArrayType& theData) noexcept {
	*this->array = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(ArrayType&& theData) noexcept {
	*this->array = std::move(theData);
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const ObjectType& theData) noexcept {
	*this->object = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(ObjectType&& theData) noexcept {
	*this->object = std::move(theData);
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const StringType& theData) noexcept {
	*this->string = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(StringType&& theData) noexcept {
	*this->string = std::move(theData);
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const char* theData) noexcept {
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

JsonObject::JsonValue::~JsonValue() noexcept {};

JsonObject& JsonObject::operator=(EnumConverter&& theData) noexcept {
	if (theData.isItAVector()) {
		this->set(std::make_unique<ArrayType>());
		for (auto& value: theData.operator std::vector<Uint64, std::allocator<Uint64>>()) {
			this->theValue.array->push_back(value);
		}
	} else {
		this->theValue.numberUint = Uint64{ theData };
		this->theType = ValueType::Uint64;
	}
	return *this;
}

JsonObject::JsonObject(EnumConverter&& theData) noexcept {
	*this = std::move(theData);
}

JsonObject& JsonObject::operator=(const EnumConverter& theData) noexcept {
	if (theData.isItAVector()) {
		this->set(std::make_unique<ArrayType>());
		for (auto& value: theData.operator std::vector<Uint64, std::allocator<Uint64>>()) {
			this->theValue.array->push_back(value);
		}
	} else {
		this->theValue.numberUint = Uint64{ theData };
		this->theType = ValueType::Uint64;
	}
	return *this;
}

JsonObject::JsonObject(const EnumConverter& theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(JsonObject&& theKey) noexcept {
	switch (theKey.theType) {
		case ValueType::Object: {
			this->set(std::make_unique<ObjectType>());
			this->theValue = std::move(*theKey.theValue.object);
			break;
		}
		case ValueType::Array: {
			this->set(std::make_unique<ArrayType>());
			this->theValue = std::move(*theKey.theValue.array);
			break;
		}
		case ValueType::String: {
			this->set(std::make_unique<StringType>());
			this->theValue = std::move(*theKey.theValue.string);
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
	switch (theKey.theType) {
		case ValueType::Object: {
			this->set(std::make_unique<ObjectType>());
			this->theValue = std::move(*theKey.theValue.object);
			this->theType = ValueType::Object;
			break;
		}
		case ValueType::Array: {
			this->set(std::make_unique<ArrayType>());
			this->theValue = std::move(*theKey.theValue.array);
			this->theType = ValueType::Array;
			break;
		}
		case ValueType::String: {
			this->set(std::make_unique<StringType>());
			this->theValue = std::move(*theKey.theValue.string);
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
	this->set(std::make_unique<StringType>());
	*this->theValue.string = std::move(theData);
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(String&& theData) noexcept {
	*this = std::move(theData);
}

JsonObject& JsonObject::operator=(const String& theData) noexcept {
	this->set(std::make_unique<StringType>());
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(const String& theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(const char* theData) noexcept {
	this->set(std::make_unique<StringType>());
	*this->theValue.string = theData;
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
	this->theType = theType;
	return *this;
}

JsonObject::JsonObject(ValueType theType) noexcept {
	*this = theType;
}

JsonObject& JsonObject::operator[](Uint64 index) const {
	return this->theValue.array->operator[](index);
}

JsonObject& JsonObject::operator[](Uint64 index) {
	if (this->theType == ValueType::Null) {
		this->set(std::make_unique<ArrayType>());
		this->theType = ValueType::Array;
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
		this->set(std::make_unique<ObjectType>());
		this->theType = ValueType::Object;
	}

	if (this->theType == ValueType::Object) {
		auto result = this->theValue.object->emplace(std::move(key), JsonObject{});
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

void JsonObject::pushBack(JsonObject&& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->set(std::make_unique<ArrayType>());
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(std::move(other));
	}
}

void JsonObject::pushBack(JsonObject& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->set(std::make_unique<ArrayType>());
		this->theType = ValueType::Array;
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

void JsonObject::set(std::unique_ptr<String> p) {
	destroy();
	new (&this->theValue.string) std::unique_ptr<String>{ std::move(p) };
	this->theType = ValueType::String;
}

void JsonObject::set(std::unique_ptr<ArrayType> p) {
	destroy();
	new (&this->theValue.string) std::unique_ptr<ArrayType>{ std::move(p) };
	this->theType = ValueType::Array;
}

void JsonObject::set(std::unique_ptr<ObjectType> p) {
	destroy();
	new (&this->theValue.string) std::unique_ptr<ObjectType>{ std::move(p) };
	this->theType = ValueType::Object;
}

void JsonObject::destroy() noexcept {
	switch (this->theType) {
		case ValueType::Array: {
			this->theValue.array.reset(nullptr);
			break;
		}
		case ValueType::Object: {
			this->theValue.object.reset(nullptr);
			break;
		}
		case ValueType::String: {
			this->theValue.string.reset(nullptr);
			break;
		}
	}
}

JsonObject::~JsonObject() noexcept {
	this->destroy();
}

struct UpdatePresenceData {
	String status{};///< Current status.
	Int64 since{ 0 };///< When was the activity started?
	Bool afk{ false };///< Are we afk.

	operator JsonObject();
};

UpdatePresenceData ::operator JsonObject() {
	JsonObject theData{};
	theData["TEST"] = this->afk;
	theData["since"] = this->since;
	return theData;
}

struct WebSocketIdentifyData {
	UpdatePresenceData presence{};
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
	
	JsonObject theSerializer02{};
	theSerializer02 = this->presence;
	theSerializer["d"]["presence"]["activities"].pushBack(std::move(theSerializer02));
	
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
	theSerializer["d"]["shard"].pushBack(static_cast<uint32_t>(this->currentShard));
	theSerializer["d"]["shard"].pushBack(static_cast<uint32_t>(this->numberOfShards));
	theSerializer["d"]["token"] = this->botToken;
	
	theSerializer["op"] = static_cast<uint32_t>(2);
	
	return theSerializer;

}

int32_t main() noexcept {
	try {
		DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::milliseconds{ 1 } };
		WebSocketIdentifyData theDataBewTwo{};
		ErlPacker thePacker{};
		DiscordCoreAPI::ActivityData theData{};
		std::vector<std::string> theResults01{};
		JsonObject theObject{};
		std::vector<DiscordCoreAPI::ChannelType> theVector{};
		
		theVector.push_back(DiscordCoreAPI::ChannelType::Dm);
		theVector.push_back(DiscordCoreAPI::ChannelType::Guild_Category);
		theObject["TESTING"] = theVector;
		theData.name = "TESTING";
		theDataBewTwo.numberOfShards = 0;
		theDataBewTwo.currentShard = 23;
		std::vector<std::string> theResults02{};
		theStopWatch.resetTimer();
		for (int32_t x = 0; x < 128 * 128; ++x) {
			theResults01.push_back(theDataBewTwo.operator JsonObject());
		}
		std::cout << "THE TIME 01: " << theObject.operator DiscordCoreAPI::String() << std::endl;

		WebSocketIdentifyData theDataBew{};
		theDataBew.numberOfShards = 0;
		theDataBew.currentShard = 23;
		theStopWatch.resetTimer();
		std::this_thread::sleep_for(std::chrono::milliseconds{ 5000 });
		auto theResult = thePacker.parseJsonToEtf(theDataBew.operator JsonObject());
		std::cout << "THE RESULT: " << thePacker.parseEtfToJson(theResult) << std::endl;


	} catch (...) {
		DiscordCoreAPI::reportException("main()");
	};

	return 0;
}
