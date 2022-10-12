#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"
#include "JsonSerializer.hpp"
#include <scoped_allocator>
//#include <nlohmann/json.hpp>

EnumConverter& EnumConverter::operator=(EnumConverter&& other) noexcept {
	this->theVector = std::move(other.theVector);
	this->vectorType = other.vectorType;
	this->theUint = other.theUint;
	return *this;
}

EnumConverter::EnumConverter(EnumConverter&& other) noexcept {
	*this = std::move(other);
}

EnumConverter::operator Vector<Uint64>() const noexcept {
	Vector<Uint64> theObject{};
	for (auto& value: this->theVector) {
		theObject.emplace_back(value);
	}
	return theObject;
}

EnumConverter::operator Uint64() const noexcept {
	return this->theUint;
}

bool EnumConverter::isItAVector() const noexcept {
	return this->vectorType;
}

JsonObject::JsonValue::JsonValue() noexcept {};

JsonObject::JsonValue::~JsonValue() noexcept {};

JsonObject& JsonObject::operator=(EnumConverter&& theData) noexcept {
	if (theData.isItAVector()) {
		this->set(ValueType::Array);
		for (auto& value: theData.operator Vector<Uint64>()) {
			*this->theValue.theValue = value;
		}
	} else {
		*this->theValue.theValue = Uint64{ theData };
		this->theType = ValueType::Uint64;
	}
	return *this;
}

JsonObject::JsonObject(EnumConverter&& theData) noexcept {
	*this = std::move(theData);
}

JsonObject& JsonObject::operator=(const EnumConverter& theData) noexcept {
	if (theData.isItAVector()) {
		this->set(ValueType::Array);
		for (auto& value: theData.operator Vector<Uint64>()) {
			*this->theValue.theValue = value;
		}
	} else {
		*this->theValue.theValue = Uint64{ theData };
		this->theType = ValueType::Uint64;
	}
	return *this;
}

JsonObject::JsonObject(const EnumConverter& theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(JsonObject&& theKey) noexcept {
	this->set(theKey.theType);
	*this->theValue.theValue = std::move(*theKey.theValue.theValue);
	this->theKey = theKey.theKey;
	this->theType = theKey.theType;
	return *this;
}

JsonObject::JsonObject(JsonObject&& theKey) noexcept {
	*this = std::move(theKey);
}

JsonObject& JsonObject::operator=(const JsonObject& theKey) noexcept {
	this->set(theKey.theType);
	*this->theValue.theValue = *theKey.theValue.theValue;
	this->theKey = theKey.theKey;
	this->theType = theKey.theType;
	return *this;
}

JsonObject::JsonObject(const JsonObject& theKey) noexcept {
	*this = theKey;
}

JsonObject& JsonObject::operator=(String&& theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue += "\"";
	*this->theValue.theValue += std::move(theData);
	*this->theValue.theValue += "\"";
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(String&& theData) noexcept {
	*this = std::move(theData);
}

JsonObject& JsonObject::operator=(const String& theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue += "\"";
	*this->theValue.theValue += std::move(theData);
	*this->theValue.theValue += "\"";
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(const String& theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(const char* theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue += "\"";
	*this->theValue.theValue += std::move(theData);
	*this->theValue.theValue += "\"";
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(const char* theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Uint64 theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue = std::to_string(theData);
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint64 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Uint32 theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue = std::to_string(theData);
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint32 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Uint16 theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue = std::to_string(theData);
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint16 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Uint8 theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue = std::to_string(theData);
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint8 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int64 theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue = std::to_string(theData);
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int64 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int32 theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue = std::to_string(theData);
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int32 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int16 theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue = std::to_string(theData);
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int16 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int8 theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue = std::to_string(theData);
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int8 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Double theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue = std::to_string(theData);
	this->theType = ValueType::Float;
	return *this;
}

JsonObject::JsonObject(Double theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Float theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.theValue = std::to_string(theData);
	this->theType = ValueType::Float;
	return *this;
}

JsonObject::JsonObject(Float theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Bool theData) noexcept {
	StringStream theStream{};
	theStream << std::boolalpha << theData;
	this->set(ValueType::String);
	*this->theValue.theValue = theStream.str();
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

JsonObject& JsonObject::operator[](Uint64 index) {
	if (this->theType == ValueType::Null) {
		this->set(ValueType::Array);
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

JsonObject& JsonObject::operator[](typename ObjectType::key_type key) {
	if (this->theType == ValueType::Null) {
		this->set(ValueType::Object);
		this->theType = ValueType::Object;
	}

	if (this->theType == ValueType::Object) {
		auto result = this->theValue.object->emplace(std::move(key), JsonObject{ ValueType::Object });
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

String& JsonSerializer::operator[](const char* key) {
	if (this->theValue.theType == ValueType::Null) {
		this->theValue.set(ValueType::Object);
		this->theValue.theType = ValueType::Object;
	}

	if (this->theValue.theType == ValueType::Object) {
		auto result = this->theValue.theValue.object->emplace(std::move(key), JsonObject{});
		return *result.first->second.theValue.theValue;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

JsonObject& JsonSerializer::operator[](JsonSerializer::ObjectType::key_type key) {
	if (this->theValue.theType == ValueType::Null) {
		this->theValue.set(ValueType::Object);
		this->theValue.theType = ValueType::Object;
	}

	if (this->theValue.theType == ValueType::Object) {
		auto result = this->theValue.theValue.object->emplace(std::move(key), JsonObject{});
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

JsonObject& JsonSerializer::operator=(const char* key)noexcept {
	if (this->theValue.theType == ValueType::Null) {
		this->theValue.set(ValueType::Object);
		this->theValue.theType = ValueType::Object;
	}

	if (this->theValue.theType == ValueType::Object) {
		auto result = this->theValue.theValue.object->emplace(std::move(key), JsonObject{});
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

Void JsonObject::pushBack(JsonObject&& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->set(ValueType::Array);
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(std::move(other));
	}
}

Void JsonObject::pushBack(JsonObject& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->set(ValueType::Array);
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(other);
	}
}

JsonSerializer::operator String() {
	return this->theValue;
}

JsonObject::operator String() noexcept {
	String theString{};
	switch (this->theType) {
		case ValueType::Object: {
			if (this->theValue.object->empty()) {
				theString = "{}";
			}

			theString = '{';

			Uint64 theIndex{};
			for (auto& [key, value]: *this->theValue.object) {
				theString += '\"';
				theString += std::move(key);
				theString += "\":";
				theString += std::move(value);
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
				theString = "[]";
				break;
			}

			theString = '[';

			Uint64 theIndex{};
			for (auto& value: *this->theValue.array) {
				theString += std::move(value);
				if (theIndex < this->theValue.array->size() - 1) {
					theString += ',';
				}
				theIndex++;
			}

			theString += ']';
			break;
		}
		case ValueType::Null: {
			theString = "null";
			break;
		}
		case ValueType::Null_Ext: {
			theString = "[]";
			break;
		}
		default:{
			theString = *this->theValue.theValue;
			break;
		}
	}
	return theString;
}

Void JsonObject::set(ValueType theTypeNew) {
	this->destroy();
	switch (theTypeNew) {
		case ValueType::Object: {
			std::allocator<JsonObject::ObjectType> allocator{};
			using AllocatorTraits = std::allocator_traits<std::allocator<JsonObject::ObjectType>>;
			AllocatorTraits alloc{};
			this->theValue.object = alloc.allocate(allocator, 1);
			alloc.construct(allocator, this->theValue.object);
			this->theType = theTypeNew;
			break;
		}
		case ValueType::Array: {
			std::allocator<JsonObject::ArrayType> allocator{};
			using AllocatorTraits = std::allocator_traits<std::allocator<JsonObject::ArrayType>>;
			AllocatorTraits alloc{};
			this->theValue.array = alloc.allocate(allocator, 1);
			alloc.construct(allocator, this->theValue.array);
			this->theType = theTypeNew;
			break;
		}
		default:{
			std::allocator<JsonObject::StringType> allocator{};
			using AllocatorTraits = std::allocator_traits<std::allocator<JsonObject::StringType>>;
			AllocatorTraits alloc{};
			this->theValue.theValue = alloc.allocate(allocator, 1);
			alloc.construct(allocator, this->theValue.theValue);
			this->theType = theTypeNew;
			break;
		}
	}
}

JsonSerializer& JsonSerializer::operator=(JsonObject& other) noexcept {
	this->theString = other;
	this->theValue = other;
	return *this;
}

JsonSerializer& JsonSerializer::operator=(JsonObject&&other) noexcept {
	this->theString = other;
	this->theValue = std::move(other);
	return *this;
}

JsonSerializer::JsonSerializer(JsonObject& other) noexcept {
	*this = other;
}

JsonSerializer::JsonSerializer(JsonObject&& other) noexcept {
	*this = std::move(other);
}

bool operator==(const JsonObject& lhs, const JsonObject& rhs) {
	if (lhs.theType != rhs.theType) {
		return false;
	}
	switch (rhs.theType) {
		case ValueType::Array: {
			if (lhs.theValue.array != rhs.theValue.array) {
				return false;
			}
			break;
		}
		case ValueType::Object: {
			if (lhs.theValue.object != rhs.theValue.object) {
				return false;
			}
			break;
		}
		case ValueType::String: {
			if (lhs.theValue.theValue != rhs.theValue.theValue) {
				return false;
			}
			break;
		}
		case ValueType::Bool: {
			if (lhs.theValue.theValue != rhs.theValue.theValue) {
				return false;
			}
			break;
		}
		case ValueType::Float: {
			if (lhs.theValue.theValue != rhs.theValue.theValue) {
				return false;
			}
			break;
		}
		case ValueType::Int64: {
			if (lhs.theValue.theValue != rhs.theValue.theValue) {
				return false;
			}
			break;
		}
		case ValueType::Uint64: {
			if (lhs.theValue.theValue != rhs.theValue.theValue) {
				return false;
			}
			break;
		}
	}
	return true;
}

Void JsonObject::destroy() noexcept {
	switch (this->theType) {
		case ValueType::Object: {
			std::allocator<JsonObject::ObjectType> allocator{};
			using AllocatorTraits = std::allocator_traits<std::allocator<JsonObject::ObjectType>>;
			AllocatorTraits alloc{};
			alloc.destroy(allocator, this->theValue.object);
			alloc.deallocate(allocator, this->theValue.object, 1);
			break;
		}
		case ValueType::Array: {
			std::allocator<JsonObject::ArrayType> allocator{};
			using AllocatorTraits = std::allocator_traits<std::allocator<JsonObject::ArrayType>>;
			AllocatorTraits alloc{};
			alloc.destroy(allocator, this->theValue.array);
			allocator.deallocate(this->theValue.array, 1);
			break;
		}
		case ValueType::String: {
			std::allocator<JsonObject::StringType> allocator{};
			using AllocatorTraits = std::allocator_traits<std::allocator<JsonObject::StringType>>;
			AllocatorTraits alloc{};
			alloc.destroy(allocator, this->theValue.theValue);
			allocator.deallocate(this->theValue.theValue, 1);
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
	theData["status"] = this->status;
	theData["since"] = this->since;
	theData["afk"] = this->afk;
	return theData;
}

struct WebSocketIdentifyData {
	UpdatePresenceData presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	int64_t intents{};

	operator JsonSerializer();
};

WebSocketIdentifyData::operator JsonSerializer() {
	JsonObject theSerializer{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);
	
	UpdatePresenceData theSerializer02{};
	theSerializer["d"]["presence"]["activities"].pushBack(std::move(theSerializer02));
	theSerializer["d"]["presence"]["activities"].pushBack(std::move(theSerializer02));
	theSerializer["d"]["presence"]["activities"].pushBack(std::move(theSerializer02));
	std::cout << "THE ARRAY: " << std::endl;
	for (auto& value: *theSerializer["d"]["presence"]["activities"].theValue.array) {
		std::cout << "THE VALUE: " << value.operator DiscordCoreAPI::String() << std::endl;
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
	theSerializer["d"]["shard"].pushBack(static_cast<uint32_t>(this->currentShard));
	theSerializer["d"]["shard"].pushBack(static_cast<uint32_t>(this->numberOfShards));
	theSerializer["d"]["token"] = this->botToken;
	theSerializer["op"] = static_cast<uint32_t>(2);
	return theSerializer;

}

struct WebSocketIdentifyDataTwo {
	UpdatePresenceData presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	int64_t intents{};

	operator JsonObject();
};

WebSocketIdentifyDataTwo::operator JsonObject() {
	JsonObject theSerializer{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);

	UpdatePresenceData theSerializer02{};
	theSerializer["d"]["presence"]["activities"].pushBack(std::move(theSerializer02));
	theSerializer["d"]["presence"]["activities"].pushBack(std::move(theSerializer02));
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
				
		WebSocketIdentifyDataTwo theDataBewTwo{};
		DiscordCoreAPI::ActivityData theData{};
		std::cout << "THE DATA: " << theDataBewTwo.operator JsonObject().operator DiscordCoreAPI::String() << std::endl;
		theDataBewTwo.botToken = "TEST_TOKEN";
		std::cout << "THE DATA: " << theDataBewTwo.operator JsonObject().operator DiscordCoreAPI::String() << std::endl;
		theData.name = "TESTING";
		theDataBewTwo.numberOfShards = 0;
		theDataBewTwo.currentShard = 23;
		DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::milliseconds{} };
		Vector<String> theVector{};
		theStopWatch.resetTimer();
		size_t theSize{};
		for (uint32_t x = 0; x < 1024 * 32; ++x) {
			theVector.push_back(theDataBewTwo.operator JsonObject());
			theSize += theVector.back().size();
			std::cout << "THE STRING: " << theVector.back() << std::endl;
		}
		

		std::cout << "THE SIZE: " << theSize << std::endl;
		std::cout << "THE TIME: " << theStopWatch.totalTimePassed() << std::endl;
		
		std::this_thread::sleep_for(std::chrono::milliseconds{ 2000 });


	} catch (...) {
		DiscordCoreAPI::reportException("main()");
	};

	return 0;
}
