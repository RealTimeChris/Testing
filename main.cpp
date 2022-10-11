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

bool EnumConverter::isItAVector() noexcept {
	return this->vectorType;
}

JsonObject::JsonValue::JsonValue() noexcept {};

JsonObject::JsonValue& JsonObject::JsonValue::operator=(const StringType& theData) noexcept {
	*this->string = theData;
	return *this;
}

JsonObject::JsonValue& JsonObject::JsonValue::operator=(StringType&& theData) noexcept {
	*this->string = theData;
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
		this->set(ValueType::Array);
		for (auto& value: theData.operator Vector<Uint64>()) {
			this->theValue.array->push_back(value);
		}
	} else {
		this->theValue.numberUint = Uint64{ theData };
		*this = ValueType::Uint64;
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
			this->theValue.array->push_back(value);
		}
	} else {
		this->theValue.numberUint = Uint64{ theData };
		this->set(ValueType::Uint64);
	}
	return *this;
}

JsonObject::JsonObject(const EnumConverter& theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(JsonObject&& theKey) noexcept {
	switch (theKey.theType) {
		case ValueType::Object: {
			this->set(ValueType::Object);
			*this->theValue.object = *theKey.theValue.object;
			break;
		}
		case ValueType::Array: {
			this->set(ValueType::Array);
			*this->theValue.array = *theKey.theValue.array;
			break;
		}
		case ValueType::String: {
			this->set(ValueType::String);
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
	this->theType = theKey.theType;
	return *this;
}

JsonObject::JsonObject(JsonObject&& theKey) noexcept {
	*this = std::move(theKey);
}

JsonObject& JsonObject::operator=(const JsonObject& theKey) noexcept {
	switch (theKey.theType) {
		case ValueType::Object: {
			this->set(ValueType::Object);
			for (auto& [key, value]: *theKey.theValue.object) {
				this->theValue.object->emplace(key, std::move(value));
			}
			break;
		}
		case ValueType::Array: {
			this->set(ValueType::Array);
			for (auto& value: *theKey.theValue.array) {
				this->theValue.array->emplace_back(std::move(value));
			}
			break;
		}
		case ValueType::String: {
			this->set(ValueType::String);
			*this->theValue.string = *theKey.theValue.string;
			break;
		}
		case ValueType::Bool: {
			this->theValue.boolean = theKey.theValue.boolean;
			this->set(ValueType::Bool);
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
	this->set(ValueType::String);
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(String&& theData) noexcept {
	*this = std::move(theData);
}

JsonObject& JsonObject::operator=(const String& theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	return *this;
}

JsonObject::JsonObject(const String& theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(const char* theData) noexcept {
	this->set(ValueType::String);
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
	this->set(this->theType);
	return *this;
}

JsonObject::JsonObject(ValueType theValue) noexcept {
	*this = theValue;
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

JsonObject& JsonSerializer::operator[](typename ObjectType::key_type key) {
	if (this->theType == ValueType::Null) {
		this->theValue->set(ValueType::Object);
		this->theType = ValueType::Object;
	}

	if (this->theType == ValueType::Object) {
		auto result = this->theValue->theValue.object->emplace(std::move(key), JsonObject{ &this->theString, ValueType::Object });
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
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
			StringStream theStream{};
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
			StringStream theStream{};
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

String JsonSerializer::getString(DiscordCoreAPI::TextFormat theFormatNew) {
	return this->theString;
}

Void JsonObject::set(ValueType theTypeNew) {
	destroy();
	
	switch (theTypeNew) {
		case ValueType::Object: {
			std::pmr::polymorphic_allocator<ObjectType> theAllocator{};
			this->theValue.object = theAllocator.new_object<ObjectType>();
			break;
		}
		case ValueType::Array: {
			std::pmr::polymorphic_allocator<ArrayType> theAllocator{};
			this->theValue.array = theAllocator.new_object<ArrayType>();
			break;
		}
		case ValueType::String: {
			std::pmr::polymorphic_allocator<StringType> theAllocator{};
			this->theValue.string = theAllocator.new_object<StringType>();
			break;
		}
	}
	this->theType = theTypeNew;
}

Void JsonObject::destroy() noexcept {
	switch (this->theType) {
		case ValueType::Array: {
			std::pmr::polymorphic_allocator<ArrayType> theAllocator{};
			theAllocator.delete_object(this->theValue.array);
			break;
		}
		case ValueType::Object: {
			std::pmr::polymorphic_allocator<ObjectType> theAllocator{};
			theAllocator.delete_object(this->theValue.object);
			break;
		}
		case ValueType::String: {
			std::pmr::polymorphic_allocator<StringType> theAllocator{};
			theAllocator.delete_object(this->theValue.string);
			break;
		}
	}
	this->theType = ValueType::Null;
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
	JsonSerializer theData{};
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

	operator JsonObject();
};

WebSocketIdentifyData::operator JsonObject() {
	JsonSerializer theSerializer{};
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

std::atomic_int64_t theAtomic{};


#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <memory_resource>
#include <vector>

enum class RingBufferAccessType { Read = 0, Write = 1 };

class RingBufferInterface {
  public:
	void modifyReadOrWritePosition(RingBufferAccessType theType, Uint64 theSize) noexcept {
		if (theType == RingBufferAccessType::Read) {
			this->tail += theSize;
			this->areWeFull = false;
		} else {
			this->head += theSize;
			if (this->head == this->tail) {
				this->areWeFull = true;
			}
			if (this->head != this->tail) {
				this->areWeFull = false;
			}
		}
	}

	Uint64 getUsedSpace() noexcept {
		if (this->areWeFull) {
			return 1024 * 256;
		}
		if ((this->head % 1024 * 256) >= (this->tail % (1024 * 256))) {
			Uint64 freeSpace = (1024 * 256) - ((this->head % (1024 * 256)) - (this->tail % (1024 * 256)));
			return (1024 * 256) - freeSpace;
		} else {
			Uint64 freeSpace = (this->tail % (1024 * 256)) - (this->head % (1024 * 256));
			return (1024 * 256) - freeSpace;
		}
	}

	Uint64 getFreeSpace() noexcept {
		return (1024 * 256) - this->getUsedSpace();
	}

	void* getCurrentTail() noexcept {
		return (this->theArray + (this->tail % ((1024 * 256))));
	}

	void* getCurrentHead() noexcept {
		return (this->theArray + (this->head % ((1024 * 256))));
	}

	Bool isItFull() noexcept {
		return this->areWeFull;
	}

	virtual void clear() noexcept {
		this->areWeFull = false;
		this->tail = 0;
		this->head = 0;
	}

  protected:
	std::byte theArray[1024 * 256]{};
	 Bool areWeFull{ false };
	Uint64 tail{};
	Uint64 head{};
};

template<typename Func> auto benchmark(Func test_func, int iterations) {
	const auto start = std::chrono::system_clock::now();
	while (iterations-- > 0) {
		test_func();
	}
	const auto stop = std::chrono::system_clock::now();
	const auto secs = std::chrono::duration<double>(stop - start);
	return secs.count();
}

int32_t main() noexcept {
	try {
		JsonObject* theObject;
		{
			
			std::pmr::polymorphic_allocator<JsonObject> theAllocator{};
			theObject = theAllocator.new_object<JsonObject>();
			
		}
		{
			std::pmr::polymorphic_allocator<JsonObject> theAllocator{};
			theAllocator.delete_object(theObject);
		}
		std::cout << "THE FINAL COUNT: " << theAtomic.load() << std::endl;
		
		WebSocketIdentifyData theDataBewTwo{};
		DiscordCoreAPI::ActivityData theData{};
		std::vector<DiscordCoreAPI::ChannelType> theVector{};
		theVector.push_back(DiscordCoreAPI::ChannelType::Dm);
		theVector.push_back(DiscordCoreAPI::ChannelType::Guild_Category);
		theData.name = "TESTING";
		theDataBewTwo.numberOfShards = 0;
		theDataBewTwo.currentShard = 23;
		DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::milliseconds{} };
		theStopWatch.resetTimer();
		{
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds{ 5000 });
		

		WebSocketIdentifyData theDataBew{};
		theDataBew.numberOfShards = 0;
		theDataBew.currentShard = 23;
		std::this_thread::sleep_for(std::chrono::milliseconds{ 5000 });


	} catch (...) {
		DiscordCoreAPI::reportException("main()");
	};

	return 0;
}
