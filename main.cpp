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
	this->theString = theKey.theString;
	switch (theKey.theType) {
		case ValueType::Object: {
			this->set(ValueType::Object);
			*this->theValue.object = std::move(*theKey.theValue.object);
			break;
		}
		case ValueType::Array: {
			this->set(ValueType::Array);
			*this->theValue.array = std::move(*theKey.theValue.array);
			break;
		}
		case ValueType::String: {
			this->set(ValueType::String);
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
	this->theString = theKey.theString;
	switch (theKey.theType) {
		case ValueType::Object: {
			this->set(ValueType::Object);
			*this->theValue.object = *theKey.theValue.object;
			this->theType = ValueType::Object;
			break;
		}
		case ValueType::Array: {
			this->set(ValueType::Array);
			*this->theValue.array = *theKey.theValue.array;
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
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint64 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Uint32 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint32 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Uint16 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint16 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Uint8 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonObject::JsonObject(Uint8 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int64 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int64 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int32 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int32 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int16 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int16 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Int8 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonObject::JsonObject(Int8 theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Double theData) noexcept {
	this->theValue.numberDouble = theData;
	this->theType = ValueType::Float;
	return *this;
}

JsonObject::JsonObject(Double theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Float theData) noexcept {
	this->theValue.numberDouble = theData;
	this->theType = ValueType::Float;
	return *this;
}

JsonObject::JsonObject(Float theData) noexcept {
	*this = theData;
}

JsonObject& JsonObject::operator=(Bool theData) noexcept {
	this->theValue.boolean = theData;
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
			this->theValue.array->resize(index + 1, this->theString);
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

Void JsonObject::pushBack(JsonObject&& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->set(ValueType::Array);
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		other.theString = this->theString;
		this->theValue.array->emplace_back(std::move(other));
	}
}

Void JsonObject::pushBack(JsonObject& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->set(ValueType::Array);
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		other.theString = this->theString;
		this->theValue.array->emplace_back(other);
	}
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
		case ValueType::String: {
			std::allocator<JsonObject::StringType> allocator{};
			using AllocatorTraits = std::allocator_traits<std::allocator<JsonObject::StringType>>;
			AllocatorTraits alloc{};
			this->theValue.string = alloc.allocate(allocator, 1);
			alloc.construct(allocator, this->theValue.string);
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
		case ValueType::Object: {
			AllocatorType<ObjectType> allocator{};
			auto alloc = std::allocator_traits<std::allocator<JsonObject::ObjectType>>{};
			alloc.destroy(allocator, this->theValue.object);
			alloc.deallocate(allocator, this->theValue.object, 1);
			break;
		}
		case ValueType::Array: {
			AllocatorType<ArrayType> allocator{};
			auto alloc = std::allocator_traits<std::allocator<JsonObject::ArrayType>>{};
			alloc.destroy(allocator, this->theValue.array);
			alloc.deallocate(allocator, this->theValue.array, 1);
			break;
		}
		case ValueType::String: {
			AllocatorType<StringType> allocator{};
			auto alloc = std::allocator_traits<std::allocator<JsonObject::StringType>>{};
			alloc.destroy(allocator, this->theValue.string);
			alloc.deallocate(allocator, this->theValue.string, 1);
			break;
		}
	}
}

JsonObject::~JsonObject() noexcept {
	this->destroy();
}

void JsonObject::dump() {
	this->theString = "";
	switch (this->theType) {
		case ValueType::Object: {
			if (this->theValue.object->empty()) {
				this->writeCharacters("{}", 2);
				return;
			}

			this->writeCharacter('{');

			Uint64 theIndex{};
			for (auto& [key, value]: *this->theValue.object) {
				this->writeCharacter('\"');
				this->writeCharacters(key.data(), key.size());
				this->writeCharacters("\":", 3);
				value.dump();
				this->theString += value.theString;
				if (theIndex < this->theValue.object->size() - 1) {
					this->writeCharacter(',');
				}
				theIndex++;
			}
			this->writeCharacter('}');
			break;
		}
		case ValueType::Array: {
			if (this->theValue.array->empty()) {
				this->writeCharacters("[]", 2);
				break;
			}

			this->writeCharacter('[');

			Uint64 theIndex{};
			for (auto& value: *this->theValue.array) {
				value.dump();
				this->theString += value.theString;
				if (theIndex < this->theValue.array->size() - 1) {
					this->writeCharacter(',');
				}
				theIndex++;
			}

			this->writeCharacter(']');
			break;
		}

		case ValueType::String: {
			this->writeCharacter('\"');
			this->writeCharacters(this->theValue.string->data(), this->theValue.string->size());
			this->writeCharacter('\"');
			break;
		}
		case ValueType::Bool: {
			StringStream theStream{};
			theStream << std::boolalpha << this->theValue.boolean;
			auto theStringNew = theStream.str();
			this->writeCharacters(theStringNew.data(), theStringNew.size());
			break;
		}
		case ValueType::Float: {
			auto theStringNew = std::to_string(this->theValue.numberDouble);
			this->writeCharacters(theStringNew.data(), theStringNew.size());
			break;
		}
		case ValueType::Uint64: {
			auto theStringNew = std::to_string(this->theValue.numberUint);
			this->writeCharacters(theStringNew.data(), theStringNew.size());
			break;
		}
		case ValueType::Int64: {
			auto theStringNew = std::to_string(this->theValue.numberInt);
			this->writeCharacters(theStringNew.data(), theStringNew.size());
			break;
		}
		case ValueType::Null: {
			this->writeCharacters("null", 4);
			break;
		}
		case ValueType::Null_Ext: {
			this->writeCharacters("[]", 2);
			break;
		}
	}
	return;
}

void JsonObject::writeCharacter(char C) {
	this->theString.push_back(C);
}

void JsonObject::writeCharacters(const char* C, size_t length) {
	this->theString.append(C, length);
}

JsonObject::operator String() noexcept {
	String theString{};
	switch (this->theType) {
		case ValueType::Object: {
			theString += "{";
			Uint64 theIndex{};
			for (auto& [key, value]: *this->theValue.object) {
				theString += "\"" + key + "\":";
				theString += value.operator String();
				if (theIndex < this->theValue.object->size() - 1) {
					theString += ",";
				}
				theIndex++;
			}
			theString += "}";
			break;
		}
		case ValueType::Array: {
			Uint64 theIndex{};
			theString += "[";
			for (auto& value: *this->theValue.array) {
				theString += value.operator String();
				if (theIndex < this->theValue.array->size() - 1) {
					theString += ",";
				}
				theIndex++;
			}
			theString += "]";
			break;
		}
		case ValueType::String: {
			theString += this->theString;
			break;
		}
		case ValueType::Bool: {
			theString += this->theString;
			break;
		}
		case ValueType::Float: {
			theString += this->theString;
			break;
		}
		case ValueType::Uint64: {
			theString += this->theString;
			break;
		}
		case ValueType::Int64: {
			theString += this->theString;
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

/*

JsonObject& JsonSerializer::operator[](JsonObject::ObjectType::key_type theKey) {
	if (this->theObject.theType == ValueType::Null) {
		this->theObject.set(ValueType::Object);
		this->theObject.theType = ValueType::Object;
	}

	if (this->theObject.theType == ValueType::Object) {
		auto result = this->theObject.theValue.object->emplace(std::move(theKey), JsonObject{ &this->theString });
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

String JsonSerializer::getString() {
	return this->theObject;
}


JsonSerializer::JsonSerializer() noexcept {}

*/
struct UpdatePresenceData {
	String status{};///< Current status.
	Int64 since{ 0 };///< When was the activity started?
	Bool afk{ false };///< Are we afk.
	String theString{};
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
	String theString{};
	operator JsonObject();
};

WebSocketIdentifyData::operator JsonObject() {
	JsonObject theSerializer{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	std::unordered_map<String, String> theMap{};
	theMap["TEST"] = "TESTIONG";
	theMap["TESTTWO"] = "TESTIONG";
	theSerializer["d"]["large_threshold"] = theMap;
	
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
	//theSerializer["d"]["shard"].pushBack(JsonObject{});
	//theSerializer["d"]["shard"].pushBack(JsonObject{theSerializer});
	theSerializer["d"]["token"] = this->botToken;
	theSerializer["op"] = static_cast<uint32_t>(2);
	return theSerializer;

}


int32_t main() noexcept {
	try {
		Uint8 arrayOne[12]{ 0 };
		for (Uint8 x = 0; x < 12; ++x) {
			arrayOne[x] = x;
		}
		Uint8 arrayTwo[12]{ 0 };
		std::copy(arrayOne, arrayOne + 12, arrayTwo);
		for (Uint8 x = 0; x < 12; ++x) {
			std::cout << "THE VALUE: " << +arrayTwo[x] << std::endl;
		}
		
		WebSocketIdentifyData theDataBewTwo{};
		DiscordCoreAPI::ActivityData theData{};
		theDataBewTwo.botToken = "TEST_TOKEN";
		theData.name = "TESTING";
		theDataBewTwo.numberOfShards = 0;
		theDataBewTwo.currentShard = 23;
		DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::milliseconds{} };
		Vector<String> theVector{};
		auto theReferece = theDataBewTwo.operator JsonObject();
		size_t theSize{};
		for (uint32_t x = 0; x < 1024 * 128; ++x) {
			theReferece["d"]["intents"] = x;
			theReferece["d"]["intents"].dump();
			theVector.push_back(theReferece.operator DiscordCoreAPI::String());
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

