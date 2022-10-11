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
			this->theType = ValueType::Object;
			break;
		}
		case ValueType::Array: {
			this->set(ValueType::Array);
			for (auto& value: *theKey.theValue.array) {
				this->theValue.array->emplace_back(std::move(value));
			}
			this->theType = ValueType::Array;
			break;
		}
		case ValueType::String: {
			this->set(ValueType::String);
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
	this->set(ValueType::String);
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	auto theIndex = this->theString->size();
	this->theString += '\"';
	*this->theString += *this->theValue.string;
	this->theString += '\"';
	this->theStringReal = StringView{ this->theString->data() + theIndex, this->theString->size() - theIndex };
	return *this;
}

JsonObject::JsonObject(String&& theData) noexcept {
	*this = std::move(theData);
}

JsonObject& JsonObject::operator=(const String& theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	auto theIndex = this->theString->size();
	this->theString += '\"';
	*this->theString += *this->theValue.string;
	this->theString += '\"';
	this->theStringReal = StringView{ this->theString->data() + theIndex, this->theString->size() - theIndex };
	return *this;
}

JsonObject::JsonObject(const String& theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(const char* theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	auto theIndex = this->theString->size();
	this->theString += '\"';
	*this->theString += *this->theValue.string;
	this->theString += '\"';
	this->theStringReal = StringView{ this->theString->data() + theIndex, this->theString->size() - theIndex };
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
	*this->theString += std::to_string(this->theValue.numberInt);
	return *this;
}

JsonObject::JsonObject(Int64 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int32 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	*this->theString += std::to_string(this->theValue.numberInt);
	return *this;
}

JsonObject::JsonObject(Int32 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int16 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	*this->theString += std::to_string(this->theValue.numberInt);
	return *this;
}

JsonObject::JsonObject(Int16 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int8 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	*this->theString += std::to_string(this->theValue.numberInt);
	return *this;
}

JsonObject::JsonObject(Int8 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Double theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Float;
	*this->theString += std::to_string(this->theValue.numberDouble);
	return *this;
}

JsonObject::JsonObject(Double theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Float theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Float;
	*this->theString += std::to_string(this->theValue.numberDouble);
	return *this;
}

JsonObject::JsonObject(Float theData) noexcept {
	this->theValue = theData;
}

JsonObject& JsonObject::operator=(Bool theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Bool;
	StringStream theStream{};
	theStream << std::boolalpha << this->theValue.boolean;
	*this->theString += theStream.str();
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
		auto result = this->theValue.object->emplace(std::move(key), JsonObject{ this->theString });
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

JsonObject& JsonSerializer::operator[](const char* key) {
	if (this->theValue.theType == ValueType::Null) {
		this->theValue.set(ValueType::Object);
		this->theValue.theType = ValueType::Object;
	}

	if (this->theValue.theType == ValueType::Object) {
		auto result = this->theValue.theValue.object->emplace(std::move(key), JsonObject{ &this->theString });
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

JsonObject::operator String() noexcept {
	String theString{};
	switch (this->theType) {
		case ValueType::Object: {
			if (this->theValue.object->empty()) {
				theString += "{}";
			}

			theString += '{';

			Uint64 theIndex{};
			for (auto& [key, value]: *this->theValue.object) {				
				theString += '\"';
				theString += key;
				theString += "\":";
				theString += value;
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

			for (auto& value: *this->theValue.array) {
				theString += value;
				theString += ',';
			}

			theString += this->theValue.array->back();

			theString += ']';
			break;
		}

		case ValueType::String: {
			theString += *this->theString;
			break;
		}
		case ValueType::Bool: {
			theString += *this->theString;
			break;
		}
		case ValueType::Float: {
			theString += *this->theString;
			break;
		}
		case ValueType::Uint64: {
			theString += *this->theString;
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

Void JsonObject::set(ValueType theTypeNew) {
	this->destroy();
	switch (theTypeNew) {
		case ValueType::String: {
			std::pmr::polymorphic_allocator<StringType> allocator{};
			this->theValue.string = allocator.new_object<StringType>();
			this->theType = theTypeNew;
			break;
		}
		case ValueType::Array: {
			std::pmr::polymorphic_allocator<ArrayType> allocator{};
			this->theValue.array = allocator.new_object<ArrayType>();
			this->theType = theTypeNew;
			break;
		}
		case ValueType::Object: {
			std::pmr::polymorphic_allocator<ObjectType> allocator{};
			this->theValue.object = allocator.new_object<ObjectType>();
			this->theType = theTypeNew;
			break;
		}
	}
	
	
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
			if (lhs.theValue.string != rhs.theValue.string) {
				return false;
			}
			break;
		}
		case ValueType::Bool: {
			if (lhs.theValue.boolean != rhs.theValue.boolean) {
				return false;
			}
			break;
		}
		case ValueType::Float: {
			if (lhs.theValue.numberDouble != rhs.theValue.numberDouble) {
				return false;
			}
			break;
		}
		case ValueType::Int64: {
			if (lhs.theValue.numberInt != rhs.theValue.numberInt) {
				return false;
			}
			break;
		}
		case ValueType::Uint64: {
			if (lhs.theValue.numberUint != rhs.theValue.numberUint) {
				return false;
			}
			break;
		}
	}
	return true;
}

Void JsonObject::destroy() noexcept {
	switch (this->theType) {
		case ValueType::Array: {
			std::pmr::polymorphic_allocator<ArrayType> allocator{};
			allocator.delete_object(this->theValue.array);
			break;
		}
		case ValueType::Object: {
			std::pmr::polymorphic_allocator<ObjectType> allocator{};
			allocator.delete_object(this->theValue.object);
			break;
		}
		case ValueType::String: {
			std::pmr::polymorphic_allocator<StringType> allocator{};
			allocator.delete_object(this->theValue.string);
			break;
		}
	}
}

JsonObject::~JsonObject() noexcept {
	this->destroy();
}

String JsonSerializer::getString(DiscordCoreAPI::TextFormat theFormat) {
	return this->theValue;
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

	operator JsonSerializer();
};

WebSocketIdentifyData::operator JsonSerializer() {
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
		std::vector<JsonObject*> theObject;
		{
			std::pmr::polymorphic_allocator<JsonObject> theAllocator{};
			for (uint32_t x = 0; x < 1024 * 1024 * 8; ++x) {
				theObject.emplace_back(theAllocator.new_object<JsonObject>());
			}
			
		}
		{
			std::pmr::polymorphic_allocator<JsonObject> theAllocator{};
			for (uint32_t x = 0; x < 1024 * 1024 * 8; ++x) {
				theAllocator.delete_object(theObject[x]);
			}
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
		std::cout << "THE DATA: " << theDataBewTwo.operator JsonSerializer().getString(DiscordCoreAPI::TextFormat::Etf) << std::endl;
		
		std::this_thread::sleep_for(std::chrono::milliseconds{ 2000 });
		

		WebSocketIdentifyData theDataBew{};
		theDataBew.numberOfShards = 0;
		theDataBew.currentShard = 23;


	} catch (...) {
		DiscordCoreAPI::reportException("main()");
	};

	return 0;
}
