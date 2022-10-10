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
		this->set(std::make_unique<ArrayType>());
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
		this->set(std::make_unique<ArrayType>());
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
			this->set(std::make_unique<ObjectType>());
			*this->theValue.object = *theKey.theValue.object;
			break;
		}
		case ValueType::Array: {
			this->set(std::make_unique<ArrayType>());
			*this->theValue.array = *theKey.theValue.array;
			break;
		}
		case ValueType::String: {
			this->set(std::make_unique<StringType>());
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
			this->set(std::make_unique<ObjectType>());
			for (auto& [key, value]: *theKey.theValue.object) {
				this->theValue.object->emplace(key, std::move(value));
			}
			this->theType = ValueType::Object;
			break;
		}
		case ValueType::Array: {
			this->set(std::make_unique<ArrayType>());
			for (auto& value: *theKey.theValue.array) {
				this->theValue.array->emplace_back(std::move(value));
			}
			this->theType = ValueType::Array;
			break;
		}
		case ValueType::String: {
			this->set(std::make_unique<StringType>());
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
	this->set(std::make_unique<StringType>());
	*this->theValue.string = theData;
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

JsonObject& JsonSerializer::operator[](const typename ObjectType::key_type& key) const {
	if (this->theType == ValueType::Object) {
		auto result = this->theValue.theValue.object->emplace(key, JsonObject{ &this->bufferString });
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

JsonObject& JsonObject::operator[](const typename ObjectType::key_type& key) const {
	if (this->theType == ValueType::Object) {
		auto result = this->theValue.object->emplace(key, JsonObject{ this->theString });
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

JsonObject& JsonSerializer::operator[](typename ObjectType::key_type key) {
	if (this->theType == ValueType::Null) {
		this->theValue.set(std::make_unique<JsonObject::ObjectType>());
		this->theType = ValueType::Object;
	}

	if (this->theType == ValueType::Object) {
		auto result = this->theValue.theValue.object->emplace(std::move(key), JsonObject{ this->bufferString });
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
		auto result = this->theValue.object->emplace(std::move(key), JsonObject{ this->theString });
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

Void JsonObject::pushBack(JsonObject&& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->set(std::make_unique<ArrayType>());
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(std::move(other));
	}
}

Void JsonObject::pushBack(JsonObject& other) noexcept {
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

Void JsonObject::set(UniquePtr<String> p) {
	destroy();
	std::pmr::polymorphic_allocator<StringType> theAllocator{};
	UniquePtrD<StringType, Deleter<StringType>> thePtr{ theAllocator.allocate(1), Deleter<StringType>{} };
	theAllocator.construct(thePtr.get());
	this->theValue.string = std::move(thePtr);
	this->theType = ValueType::String;
}

Void JsonObject::set(UniquePtr<ArrayType> p) {
	destroy();
	std::pmr::polymorphic_allocator<ArrayType> theAllocator{};
	UniquePtrD<ArrayType, Deleter<ArrayType>> thePtr{ theAllocator.allocate(1), Deleter<ArrayType>{} };
	theAllocator.construct(thePtr.get());
	this->theValue.array = std::move(thePtr);
	this->theType = ValueType::Array;
}

Void JsonObject::set(UniquePtr<ObjectType> p) {
	destroy();
	std::pmr::polymorphic_allocator<ObjectType> theAllocator{};
	UniquePtrD<ObjectType, Deleter<ObjectType>> thePtr{ theAllocator.allocate(1), Deleter<ObjectType>{} };
	theAllocator.construct(thePtr.get());
	this->theValue.object = std::move(thePtr);
	this->theType = ValueType::Object;
}

Void JsonObject::destroy() noexcept {
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
/*
int main() {
	constexpr int iterations{ 1 };
	constexpr int total_nodes{ 2'0000 };

	auto default_std_alloc = [total_nodes] {
		std::list<int> list;
		for (int i{}; i != total_nodes; ++i) {
			list.push_back(i);
		}
	};

	auto default_pmr_alloc = [total_nodes] {
		std::pmr::list<int> list;
		for (int i{}; i != total_nodes; ++i) {
			list.push_back(i);
		}
	};

	auto pmr_alloc_no_buf = [total_nodes] {
		std::pmr::monotonic_buffer_resource mbr;
		std::pmr::polymorphic_allocator<int> pa{ &mbr };
		std::pmr::list<int> list{ pa };
		for (int i{}; i != total_nodes; ++i) {
			list.push_back(i);
		}
	};

	auto pmr_alloc_and_buf = [total_nodes] {
		std::array<std::byte, total_nodes> buffer;// enough to fit in all nodes
		std::pmr::monotonic_buffer_resource mbr{ buffer.data(), buffer.size() };
		std::cout << "v.data() @ " << buffer.data() << '\n';
		std::pmr::polymorphic_allocator<int> pa{ &mbr };
		std::pmr::list<int> list{ pa };
		
		for (int i{}; i != total_nodes; ++i) {
			list.push_back(i);
			
		}
		std::cout << "v.data() @ " << &(list.front()) - 14 << '\n';
	};

	const double t1 = benchmark(default_std_alloc, iterations);
	const double t2 = benchmark(default_pmr_alloc, iterations);
	const double t3 = benchmark(pmr_alloc_no_buf, iterations);
	const double t4 = benchmark(pmr_alloc_and_buf, iterations);

	std::cout << std::fixed << std::setprecision(3) << "t1 (default std alloc): " << t1 << " sec; t1/t1: " << t1 / t1 << '\n'
			  << "t2 (default pmr alloc): " << t2 << " sec; t1/t2: " << t1 / t2 << '\n'
			  << "t3 (pmr alloc  no buf): " << t3 << " sec; t1/t3: " << t1 / t3 << '\n'
			  << "t4 (pmr alloc and buf): " << t4 << " sec; t1/t4: " << t1 / t4 << '\n';
}
/*

*/

template<class ObjectType> class StackAllocator {
  public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef ObjectType* pointer;
	typedef const ObjectType* const_pointer;
	typedef ObjectType& reference;
	typedef const ObjectType& const_reference;
	typedef ObjectType value_type;
	using pointer = ObjectType*;
	using ValueType = ObjectType;
	StackAllocator() {}

	StackAllocator(const StackAllocator&) {}

	pointer allocate(size_t count) {
		const auto new_offset = offset_ + sizeof(ObjectType);
		const auto place = buffer_ + new_offset - sizeof(ObjectType);
		offset_ = new_offset;
		return new (place) ValueType{};
	}
	void deallocate(pointer,size_t theAmount) {
		//this->theArray.modifyReadOrWritePosition(RingBufferAccessType::Read, theAmount * sizeof(ObjectType));
	}

	char buffer_[1024 * 256]{};
	std::size_t offset_{0};

	//RingBufferInterface theArray{};

	pointer address(reference x) const {
		return &x;
	}
	const_pointer address(const_reference x) const {
		return &x;
	}
	StackAllocator<ObjectType>& operator=(const StackAllocator&) {
		return nullptr;
	}
	void construct(pointer p, const ObjectType& val) {
		new (( ObjectType* )p) ObjectType(val);
	}
	void destroy(pointer p) {
		p->~ObjectType();
	}

	size_type max_size() const {
		return size_t(-1);
	}

	template<class U> struct rebind { typedef StackAllocator<U> other; };

	template<class U> StackAllocator(const StackAllocator<U>&) {
	}

	template<class U> StackAllocator& operator=(const StackAllocator<U>&) {
		return *this;
	}
};

int doit() {
	char ac[80];
	if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
		std::cerr << "Error " << WSAGetLastError() << " when getting local host name." << std::endl;
		return 1;
	}
	std::cout << "Host name is " << ac << "." << std::endl;

	struct hostent* phe = gethostbyname(ac);
	if (phe == 0) {
		std::cerr << "Yow! Bad host lookup." << std::endl;
		return 1;
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		std::cout << "Address " << i << ": " << inet_ntoa(addr) << std::endl;
	}

	return 0;
}

Bool connect(const String& baseUrlNew, const String& portNew) noexcept {
	sockaddr_in theStreamTargetAddress{};
	theStreamTargetAddress.sin_addr.s_addr = inet_addr(baseUrlNew.c_str());
	theStreamTargetAddress.sin_port = DiscordCoreAPI::reverseByteOrder<Uint16>(static_cast<unsigned short>(stoi(portNew)));
	theStreamTargetAddress.sin_family = AF_INET;
	SOCKET theSocket{};
	DiscordCoreInternal::addrinfoWrapper hints{}, address{};
	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_DGRAM;
	hints->ai_protocol = IPPROTO_UDP;
	DiscordCoreAPI::StreamType streamType{ DiscordCoreAPI::StreamType::Client };
	//std::cout << "THE ADDRESS: " << theStreamTargetAddress.sin_addr.s_addr << std::endl;
	if (getaddrinfo(baseUrlNew.c_str(), portNew.c_str(), hints, address)) {
		//std::cout << "THE ADDRESS: " << address->ai_addr->sa_data << std::endl;
		return false;
	}
	std::cout << "THE ADDRESS: " << address->ai_addr->sa_data << std::endl;
	std::string clienthost{};
	clienthost.resize(NI_MAXHOST);
	std::string clientservice{};
	clientservice.resize(NI_MAXSERV);
	int theErrorCode = getnameinfo(address->ai_addr, sizeof(*address->ai_addr), clienthost.data(), sizeof(clienthost), clientservice.data(), sizeof(clientservice),
		NI_NUMERICHOST | NI_NUMERICSERV);
	std::cout << "The ip address is = " << clienthost << std::endl;
	std::cout << "The clientservice = " << clientservice << std::endl;
	if (theSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol); theSocket == SOCKET_ERROR) {
		return false;
	}
	std::cout << "The ip address is = " << clienthost << std::endl;
	std::cout << "The clientservice = " << clientservice << std::endl;

	if (streamType == DiscordCoreAPI::StreamType::None || streamType == DiscordCoreAPI::StreamType::Client) {
		if (::connect(theSocket, address->ai_addr, static_cast<Int32>(address->ai_addrlen))) {
			return false;
		}
		String clientToServerString{};
		clientToServerString = "test string";
		Int32 intSize{ sizeof(theStreamTargetAddress) };
		String serverToClientBuffer{};
		serverToClientBuffer.resize(11);
		auto writtenBytes{ sendto(static_cast<Int32>(theSocket), clientToServerString.data(), static_cast<Int32>(clientToServerString.size()), 0,
			( sockaddr* )&theStreamTargetAddress, static_cast<Int32>(sizeof(theStreamTargetAddress))) };
		auto readBytes{ recvfrom(static_cast<Int32>(theSocket), serverToClientBuffer.data(), static_cast<Int32>(serverToClientBuffer.size()), 0,
			reinterpret_cast<sockaddr*>(&theStreamTargetAddress), &intSize) };
		int theErrorCode = getnameinfo(reinterpret_cast<sockaddr*>(&theStreamTargetAddress), sizeof(*address->ai_addr), clienthost.data(), sizeof(clienthost), clientservice.data(),
			sizeof(clientservice), NI_NUMERICHOST | NI_NUMERICSERV);
		std::cout << "The ip address is 0303 = " << clienthost << std::endl;
		std::cout << "The clientservice 0303 = " << clientservice << std::endl;
		std::cout << "The clientservice 0303 = " << serverToClientBuffer << std::endl;

	} else {
		DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::seconds{ 300 } };
		while (!theStopWatch.hasTimePassed()) {
			
			std::cout << "The ip address is = " << clienthost << std::endl;
			std::cout << "The clientservice = " << clientservice << std::endl;


			if (auto theResult = bind(theSocket, ( sockaddr* )&theStreamTargetAddress, sizeof(sockaddr)); theResult != 0) {
				return false;
			}
			std::cout << "The ip address is 0202 = " << clienthost << std::endl;
			std::cout << "The clientservice 0202 = " << clientservice << std::endl;

			
#ifdef _WIN32

#else
			socklen_t intSize{ sizeof(theStreamTargetAddress) };
#endif
		}
	}
#ifdef _WIN32
	u_long value02{ 1 };
	if (ioctlsocket(theSocket, FIONBIO, &value02)) {
		return false;
	}
#else
	if (fcntl(theSocket, F_SETFL, fcntl(theSocket, F_GETFL, 0) | O_NONBLOCK)) {
		return false;
	}
#endif
	//	return true;
	//}}
}
			
int main() {
	
	{
		std::string address{ "192.168.0.28" };
		std::string port{ "40010" };
		
		WSAData wsaData;
		if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
			return 255;
		}
		connect(address, port);
		doit();
		std::vector<int32_t, StackAllocator<int32_t>> theVector01{};
		std::vector<int32_t, StackAllocator<int32_t>> theVector02{};
		//std::vector<int32_t, StackAllocator<int32_t>> theVector03{};
		for (uint32_t x = 0; x < (1024 * 128 /4)-8; ++x) {
			//theVector03.push_back(x);
			//std::cout << "THE VALUE: " << theVector03[x];
		}
		std::this_thread::sleep_for(std::chrono::seconds{ 4 });
	}
	std::this_thread::sleep_for(std::chrono::seconds{ 4 });
	
	std::this_thread::sleep_for(std::chrono::seconds{ 2 });
	//theTraits.construct(,);

}
/*
int32_t main() noexcept {
	try {

		{
			DiscordCoreAPI::ObjectCacheReal<DiscordCoreAPI::GuildData> theCache{};
			for (int32_t x = 0; x < 1024*10; ++x) {
				if (x % 10000 == 0) {
					std::cout << "WERE HERE THIS IS IT!" << std::endl;
				}
				DiscordCoreAPI::GuildData theData{};
				theData.id = x;
				theCache.emplace(theData);
			}
			
			//theCache.erase(DiscordCoreAPI::GuildData{});
			//for (auto iterator = theCache.begin(); iterator != theCache.end(); ++iterator) {
				//iterator = theCache.erase(*iterator->get());
			//}
			//theCache.erase(DiscordCoreAPI::GuildData{});
			std::this_thread::sleep_for(std::chrono::milliseconds{ 5000 });
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
			JsonSerializer theSerializer{ theDataBewTwo.operator JsonObject() };
			auto theString = theSerializer.getString(DiscordCoreAPI::TextFormat::Etf);
			
			std::cout << "THE LINES: " << theString << std::endl;
			DiscordCoreInternal::ErlPacker thePacker{};
			std::cout << "THE JSON DATA:" << thePacker.parseEtfToJson(theString) << std::endl;
			std::string theResults02{};
			size_t theSize{};
			for (int32_t x = 0; x < 1024 * 256; ++x) {
				theSerializer["d"]["intents"] = x;
				theResults02 = theSerializer.getString(DiscordCoreAPI::TextFormat::Etf);
				theSize += theResults02.size();
				//std::cout << "THE JSON DATA:" << thePacker.parseEtfToJson(theResults02) << std::endl;
				//std::cout << "THE STRING: " << theResults02 << std::endl;
			}
			std::cout << theSize << std::endl;
			std::cout << "THE TIME PASSED: " << theStopWatch.totalTimePassed() << std::endl;
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
*/