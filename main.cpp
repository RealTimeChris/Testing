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
	return this->theVector;
}

EnumConverter::operator Uint64() const noexcept {
	return this->theUint;
}

bool EnumConverter::isItAVector() const noexcept {
	return this->vectorType;
}

Jsonifier& Jsonifier::operator=(Jsonifier&& theData) noexcept {
	switch (theData.theType) {
		case ValueType::Object: {
			this->setValue(ValueType::Object);
			*this->theValue.object = std::move(*theData.theValue.object);
			break;
		}
		case ValueType::Array: {
			this->setValue(ValueType::Array);
			*this->theValue.array = std::move(*theData.theValue.array);
			break;
		}
		case ValueType::String: {
			this->setValue(ValueType::String);
			*this->theValue.string = std::move(*theData.theValue.string);
			break;
		}
		case ValueType::Float: {
			this->theValue.numberDouble = theData.theValue.numberDouble;
			break;
		}
		case ValueType::Uint64: {
			this->theValue.numberUint = theData.theValue.numberUint;
			break;
		}
		case ValueType::Int64: {
			this->theValue.numberInt = theData.theValue.numberInt;
			break;
		}
		case ValueType::Bool: {
			this->theValue.boolean = theData.theValue.boolean;
			break;
		}
	}
	this->theString = std::move(theData.theString);
	this->theType = theData.theType;
	return *this;
}

Jsonifier::Jsonifier(Jsonifier&& theData) noexcept {
	*this = std::move(theData);
}

Jsonifier& Jsonifier::operator=(const Jsonifier& theData) noexcept {
	switch (theData.theType) {
		case ValueType::Object: {
			this->setValue(ValueType::Object);
			*this->theValue.object = *theData.theValue.object;
			break;
		}
		case ValueType::Array: {
			this->setValue(ValueType::Array);
			*this->theValue.array = *theData.theValue.array;
			break;
		}
		case ValueType::String: {
			this->setValue(ValueType::String);
			*this->theValue.string = *theData.theValue.string;
			break;
		}
		case ValueType::Float: {
			this->theValue.numberDouble = theData.theValue.numberDouble;
			break;
		}
		case ValueType::Int64: {
			this->theValue.numberInt = theData.theValue.numberInt;
			break;
		}
		case ValueType::Uint64: {
			this->theValue.numberUint = theData.theValue.numberUint;
			break;
		}
		case ValueType::Bool: {
			this->theValue.boolean = theData.theValue.boolean;
			break;
		}
	}
	this->theString = theData.theString;
	this->theType = theData.theType;
	return *this;
}

Jsonifier::Jsonifier(const Jsonifier& theData) noexcept {
	*this = theData;
}

Jsonifier::operator String&&() noexcept {
	return std::move(this->theString);
}

Jsonifier::operator String() noexcept {
	return this->theString;
}

Void Jsonifier::refreshString(WebSocketOpCode theOpCode) {
	this->theString.clear();
	if (theOpCode == WebSocketOpCode::Op_Binary) {
		this->appendVersion();
		this->parseJsonToEtf(this);
	} else {
		this->parseJsonToJson(this);
	}
}

Jsonifier& Jsonifier::operator=(EnumConverter&& theData) noexcept {
	if (theData.isItAVector()) {
		this->setValue(ValueType::Array);
		for (auto& value: theData.operator Vector<Uint64>()) {
			this->theValue.array->emplace_back(std::move(value));
		}
	} else {
		this->theValue.numberUint = Uint64{ theData };
		this->theType = ValueType::Uint64;
	}
	return *this;
}

Jsonifier::Jsonifier(EnumConverter&& theData) noexcept {
	*this = std::move(theData);
}

Jsonifier& Jsonifier::operator=(const EnumConverter& theData) noexcept {
	if (theData.isItAVector()) {
		this->setValue(ValueType::Array);
		for (auto& value: theData.operator Vector<Uint64>()) {
			this->theValue.array->emplace_back(value);
		}
	} else {
		this->theValue.numberUint = Uint64{ theData };
		this->theType = ValueType::Uint64;
	}
	return *this;
}

Jsonifier::Jsonifier(const EnumConverter& theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(String&& theData) noexcept {
	this->setValue(ValueType::String);
	*this->theValue.string = std::move(theData);
	this->theType = ValueType::String;
	return *this;
}

Jsonifier::Jsonifier(String&& theData) noexcept {
	*this = std::move(theData);
}

Jsonifier& Jsonifier::operator=(const String& theData) noexcept {
	this->setValue(ValueType::String);
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	return *this;
}

Jsonifier::Jsonifier(const String& theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(const char* theData) noexcept {
	this->setValue(ValueType::String);
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	return *this;
}

Jsonifier::Jsonifier(const char* theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Double theData) noexcept {
	this->theValue.numberDouble = theData;
	this->theType = ValueType::Float;
	return *this;
}

Jsonifier::Jsonifier(Double theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Float theData) noexcept {
	this->theValue.numberDouble = theData;
	this->theType = ValueType::Float;
	return *this;
}

Jsonifier::Jsonifier(Float theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Uint64 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

Jsonifier::Jsonifier(Uint64 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Uint32 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

Jsonifier::Jsonifier(Uint32 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Uint16 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

Jsonifier::Jsonifier(Uint16 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Uint8 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

Jsonifier::Jsonifier(Uint8 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Int64 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

Jsonifier::Jsonifier(Int64 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Int32 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

Jsonifier::Jsonifier(Int32 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Int16 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

Jsonifier::Jsonifier(Int16 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Int8 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

Jsonifier::Jsonifier(Int8 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Bool theData) noexcept {
	this->theValue.boolean = theData;
	this->theType = ValueType::Bool;
	return *this;
}

Jsonifier::Jsonifier(Bool theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(ValueType theTypeNew) noexcept {
	this->theType = theTypeNew;
	return *this;
}

Jsonifier::Jsonifier(ValueType theType) noexcept {
	*this = theType;
}

Jsonifier& Jsonifier::operator[](typename ObjectType::key_type key) {
	if (this->theType == ValueType::Null) {
		this->setValue(ValueType::Object);
		this->theType = ValueType::Object;
	}

	if (this->theType == ValueType::Object) {
		auto result = this->theValue.object->emplace(std::move(key), Jsonifier{});
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

Jsonifier& Jsonifier::operator[](Uint64 index) {
	if (this->theType == ValueType::Null) {
		this->setValue(ValueType::Array);
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

Void Jsonifier::pushBack(Jsonifier&& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->setValue(ValueType::Array);
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(std::move(other));
	}
}

Void Jsonifier::pushBack(Jsonifier& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->setValue(ValueType::Array);
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(other);
	}
}

Void Jsonifier::parseJsonToEtf(const Jsonifier* dataToParse) {
	switch (dataToParse->theType) {
		case ValueType::Object: {
			return this->writeEtfObject(dataToParse->theValue.object);
		}
		case ValueType::Array: {
			return this->writeEtfArray(dataToParse->theValue.array);
		}
		case ValueType::String: {
			return this->writeEtfString(dataToParse->theValue.string);
		}
		case ValueType::Float: {
			return this->writeEtfFloat(dataToParse->theValue.numberDouble);
		}
		case ValueType::Uint64: {
			return this->writeEtfUint(dataToParse->theValue.numberUint);
		}
		case ValueType::Int64: {
			return this->writeEtfInt(dataToParse->theValue.numberInt);
		}
		case ValueType::Bool: {
			return this->writeEtfBool(dataToParse->theValue.boolean);
		}
		case ValueType::Null_Ext: {
			return this->writeEtfNullExt();
		}
		case ValueType::Null: {
			return this->writeEtfNull();
		}
	}
}

Void Jsonifier::parseJsonToJson(const Jsonifier* dataToParse) {
	switch (dataToParse->theType) {
		case ValueType::Object: {
			return this->writeJsonObject(dataToParse->theValue.object);
		}
		case ValueType::Array: {
			return this->writeJsonArray(dataToParse->theValue.array);
		}
		case ValueType::String: {
			return this->writeJsonString(dataToParse->theValue.string);
		}
		case ValueType::Float: {
			return this->writeJsonFloat(dataToParse->theValue.numberDouble);
		}
		case ValueType::Uint64: {
			return this->writeJsonInt(dataToParse->theValue.numberUint);
		}
		case ValueType::Int64: {
			return this->writeJsonInt(dataToParse->theValue.numberInt);
		}
		case ValueType::Bool: {
			return this->writeJsonBool(dataToParse->theValue.boolean);
		}
		case ValueType::Null_Ext: {
			return this->writeJsonNullExt();
		}
		case ValueType::Null: {
			return this->writeJsonNull();
		}
	}
}

Void Jsonifier::writeJsonObject(const Jsonifier::ObjectType* theObjectNew) {
	if (theObjectNew->empty()) {
		this->writeString("{}", 2);
		return;
	}
	this->writeCharacter('{');

	Int32 theIndex{};
	for (auto& [key, value]: *theObjectNew) {
		this->writeJsonString(&key);
		this->writeCharacter(':');
		this->parseJsonToJson(&value);

		if (theIndex != theObjectNew->size() - 1) {
			this->writeCharacter(',');
		}
		theIndex++;
	}

	this->writeCharacter('}');
}

Void Jsonifier::writeJsonArray(const Jsonifier::ArrayType* theArray) {
	if (theArray->empty()) {
		this->writeString("[]", 2);
		return;
	}

	this->writeCharacter('[');

	Int32 theIndex{};
	for (auto& value: *theArray) {
		this->parseJsonToJson(&value);
		if (theIndex != theArray->size() - 1) {
			this->writeCharacter(',');
		}
		theIndex++;
	}

	this->writeCharacter(']');
}

Void Jsonifier::writeJsonString(const Jsonifier::StringType* theStringNew) {
	this->writeCharacter('\"');
	for (auto& value: *theStringNew) {
		switch (static_cast<std::uint8_t>(value)) {
			case 0x08: {
				this->writeCharacter('b');
				break;
			}
			case 0x09: {
				this->writeCharacter('t');
				break;
			}
			case 0x0A: {
				this->writeCharacter('n');
				break;
			}
			case 0x0C: {
				this->writeCharacter('f');
				break;
			}
			case 0x0D: {
				this->writeCharacter('r');
				break;
			}
			case 0x22: {
				this->writeCharacter('\"');
				break;
			}
			case 0x5C: {
				this->writeCharacter('\\');
				break;
			}
			default: {
				this->writeCharacter(value);
				break;
			}
		}
	}
	this->writeCharacter('\"');
}

Void Jsonifier::writeJsonFloat(const Jsonifier::FloatType x) {
	auto theFloat = std::to_string(x);
	this->writeString(theFloat.data(), theFloat.size());
}

Void Jsonifier::writeJsonBool(const Jsonifier::BoolType theValueNew) {
	if (theValueNew) {
		this->writeString("true", 4);
	} else {
		this->writeString("false", 5);
	}
}

Void Jsonifier::writeJsonNullExt() {
	this->writeString("[]", 2);
}

Void Jsonifier::writeJsonNull() {
	this->writeString("null", 4);
}

Void Jsonifier::writeEtfObject(const Jsonifier::ObjectType* jsonData) {
	this->appendMapHeader(static_cast<Uint32>(jsonData->size()));
	for (auto& [key, value]: *jsonData) {
		this->appendBinaryExt(key, static_cast<Uint32>(key.size()));
		this->parseJsonToEtf(&value);
	}
}

Void Jsonifier::writeEtfArray(const Jsonifier::ArrayType* jsonData) {
	this->appendListHeader(static_cast<Uint32>(jsonData->size()));
	for (auto& value: *jsonData) {
		this->parseJsonToEtf(&value);
	}
	this->appendNilExt();
}

Void Jsonifier::writeEtfString(const Jsonifier::StringType* jsonData) {
	this->appendBinaryExt(*jsonData, static_cast<Uint32>(jsonData->size()));
}

Void Jsonifier::writeEtfUint(const Jsonifier::UintType jsonData) {
	if (jsonData <= 255 && jsonData >= 0) {
		this->appendSmallIntegerExt(static_cast<Uint8>(jsonData));
	} else if (jsonData <= std::numeric_limits<Uint32>::max() && jsonData >= 0) {
		this->appendIntegerExt(static_cast<Uint32>(jsonData));
	} else {
		this->appendUnsignedLongLong(jsonData);
	}
}

Void Jsonifier::writeEtfInt(const Jsonifier::IntType jsonData) {
	if (jsonData <= 127 && jsonData >= -127) {
		this->appendSmallIntegerExt(static_cast<Uint8>(jsonData));
	} else if (jsonData <= std::numeric_limits<Int32>::max() && jsonData >= std::numeric_limits<Int32>::min()) {
		this->appendIntegerExt(static_cast<Uint32>(jsonData));
	} else {
		this->appendUnsignedLongLong(static_cast<Uint64>(jsonData));
	}
}

Void Jsonifier::writeEtfFloat(const Jsonifier::FloatType jsonData) {
	this->appendNewFloatExt(jsonData);
}

Void Jsonifier::writeEtfBool(const Jsonifier::BoolType jsonData) {
	if (jsonData) {
		this->appendTrue();
	} else {
		this->appendFalse();
	}
}

Void Jsonifier::writeEtfNullExt() {
	this->appendNilExt();
}

Void Jsonifier::writeEtfNull() {
	this->appendNil();
}

Void Jsonifier::writeString(const char* theData, std::size_t length) {
	this->theString.append(theData, length);
}

Void Jsonifier::writeCharacter(const char theChar) {
	this->theString.push_back(theChar);
}

bool operator==(const Jsonifier& lhs, const Jsonifier& rhs) {
	if (lhs.theType != rhs.theType) {
		return false;
	}
	switch (rhs.theType) {
		case ValueType::Object: {
			if (*lhs.theValue.object != *rhs.theValue.object) {
				return false;
			}
			break;
		}
		case ValueType::Array: {
			if (*lhs.theValue.array != *rhs.theValue.array) {
				return false;
			}
			break;
		}
		case ValueType::String: {
			if (*lhs.theValue.string != *rhs.theValue.string) {
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
		case ValueType::Uint64: {
			if (lhs.theValue.numberUint != rhs.theValue.numberUint) {
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
		case ValueType::Bool: {
			if (lhs.theValue.boolean != rhs.theValue.boolean) {
				return false;
			}
			break;
		}
	}
	return true;
}

Void Jsonifier::appendBinaryExt(const String& bytes, Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Binary_Ext) };
	storeBits(bufferNew, sizeNew);
	this->writeString(bufferNew.data(), bufferNew.size());
	this->writeString(bytes.data(), bytes.size());
}

Void Jsonifier::appendUnsignedLongLong(const Uint64 value) {
	String bufferNew{};
	Uint64 theValueNew = value;
	bufferNew.resize(static_cast<Uint64>(1) + 2 + sizeof(Uint64));
	bufferNew[0] = static_cast<Uint8>(ETFTokenType::Small_Big_Ext);
	StopWatch theStopWatch{ std::chrono::milliseconds{ 1500 } };
	Uint8 bytesToEncode = 0;
	while (theValueNew > 0) {
		if (theStopWatch.hasTimePassed()) {
			break;
		}
		bufferNew[static_cast<Uint64>(3) + bytesToEncode] = theValueNew & 0xF;
		theValueNew >>= 8;
		bytesToEncode++;
	}
	bufferNew[1] = bytesToEncode;
	bufferNew[2] = 0;
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendNewFloatExt(const Double FloatValue) {
	String bufferNew{ static_cast<unsigned char>(ETFTokenType::New_Float_Ext) };
	const Void* punner{ &FloatValue };
	storeBits(bufferNew, *static_cast<const Uint64*>(punner));
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendSmallIntegerExt(const Uint8 value) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Integer_Ext), static_cast<char>(value) };
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendIntegerExt(const Uint32 value) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Integer_Ext) };
	storeBits(bufferNew, value);
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendListHeader(const Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::List_Ext) };
	storeBits(bufferNew, sizeNew);
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendMapHeader(const Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Map_Ext) };
	storeBits(bufferNew, sizeNew);
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendVersion() {
	String bufferNew{ static_cast<int8_t>(formatVersion) };
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendNilExt() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Nil_Ext) };
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendFalse() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 5, static_cast<Uint8>('f'), static_cast<Uint8>('a'), static_cast<Uint8>('l'), static_cast<Uint8>('s'),
		static_cast<Uint8>('e') };
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendTrue() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 4, static_cast<Uint8>('t'), static_cast<Uint8>('r'), static_cast<Uint8>('u'), static_cast<Uint8>('e') };
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendNil() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 3, static_cast<Uint8>('n'), static_cast<Uint8>('i'), static_cast<Uint8>('l') };
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::setValue(ValueType theTypeNew) {
	this->destroy();
	switch (theTypeNew) {
		case ValueType::Object: {
			AllocatorType<ObjectType> allocator{};
			auto alloc = AllocatorTraits<ObjectType>{};
			this->theValue.object = alloc.allocate(allocator, 1);
			alloc.construct(allocator, this->theValue.object);
			this->theType = theTypeNew;
			break;
		}
		case ValueType::Array: {
			AllocatorType<ArrayType> allocator{};
			auto alloc = AllocatorTraits<ArrayType>{};
			this->theValue.array = alloc.allocate(allocator, 1);
			alloc.construct(allocator, this->theValue.array);
			this->theType = theTypeNew;
			break;
		}
		case ValueType::String: {
			AllocatorType<StringType> allocator{};
			auto alloc = AllocatorTraits<StringType>{};
			this->theValue.string = alloc.allocate(allocator, 1);
			alloc.construct(allocator, this->theValue.string);
			this->theType = theTypeNew;
			break;
		}
	}
}

Void Jsonifier::destroy() noexcept {
	switch (this->theType) {
		case ValueType::Object: {
			AllocatorType<ObjectType> allocator{};
			auto alloc = AllocatorTraits<ObjectType>{};
			alloc.destroy(allocator, this->theValue.object);
			alloc.deallocate(allocator, this->theValue.object, 1);
			break;
		}
		case ValueType::Array: {
			AllocatorType<ArrayType> allocator{};
			auto alloc = AllocatorTraits<ArrayType>{};
			alloc.destroy(allocator, this->theValue.array);
			alloc.deallocate(allocator, this->theValue.array, 1);
			break;
		}
		case ValueType::String: {
			AllocatorType<StringType> allocator{};
			auto alloc = AllocatorTraits<StringType>{};
			alloc.destroy(allocator, this->theValue.string);
			alloc.deallocate(allocator, this->theValue.string, 1);
			break;
		}
	}
}

Jsonifier::~Jsonifier() noexcept {
	this->destroy();
}

struct UpdatePresenceData {
	String status{};///< Current status.
	Int64 since{ 0 };///< When was the activity started?
	Bool afk{ false };///< Are we afk.
	String theString{};
	operator Jsonifier();
};

struct UpdatePresenceDataTwo {
	String status{};///< Current status.
	Int64 since{ 0 };///< When was the activity started?
	Bool afk{ false };///< Are we afk.
	String theString{};
	operator nlohmann::json();
};



UpdatePresenceDataTwo ::operator nlohmann::json() {
	nlohmann::json theData{};
	theData["status"] = this->status;
	theData["since"] = this->since;
	theData["afk"] = this->afk;
	return theData;
}

UpdatePresenceData ::operator Jsonifier() {
	Jsonifier theData{};
	theData["status"] = this->status;
	theData["since"] = this->since;
	theData["afk"] = this->afk;
	return theData;
}

struct WebSocketIdentifyDataTwo {
	UpdatePresenceData presence{}; 
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	int64_t intents{};
	String theString{};
	operator nlohmann::json();
};

struct WebSocketIdentifyData {
	UpdatePresenceData presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	int64_t intents{};
	String theString{};
	operator Jsonifier();
};

WebSocketIdentifyDataTwo::operator nlohmann::json(){
	nlohmann::json theSerializer{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	std::vector<DiscordCoreAPI::ChannelType> theMap{};
	theMap.push_back(DiscordCoreAPI::ChannelType ::Dm);
	theMap.push_back(DiscordCoreAPI::ChannelType ::Dm);
	theSerializer["d"]["large_threshold"] = theMap;

	UpdatePresenceDataTwo theSerializer02{};
	theSerializer["d"]["presence"]["activities"].push_back(std::move(theSerializer02));
	theSerializer["d"]["presence"]["activities"].push_back(std::move(theSerializer02));
	theSerializer["d"]["presence"]["activities"].push_back(std::move(theSerializer02));
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
	//Jsonifier<char> theSerializerTwo{};
	return theSerializer;
}

WebSocketIdentifyData::operator Jsonifier() {
	Jsonifier theSerializer{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	std::vector<DiscordCoreAPI::ChannelType> theMap{};
	theMap.push_back(DiscordCoreAPI::ChannelType ::Dm);
	theMap.push_back(DiscordCoreAPI::ChannelType ::Dm);
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
	//Jsonifier<char> theSerializerTwo{};
	return theSerializer;

}



int32_t main() noexcept {
	try {
		DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::milliseconds{} };
		WebSocketIdentifyData theDataBewTwo{};
		theDataBewTwo.botToken = "TEST_TOKEN";
		theDataBewTwo.numberOfShards = 0;
		theDataBewTwo.currentShard = 23;
		
		Vector<String> theVector{};
		auto theReference = theDataBewTwo.operator Jsonifier();
		
		size_t theSize{};
		Jsonifier theSerializer{ theDataBewTwo.operator Jsonifier() };
		theSerializer.refreshString(WebSocketOpCode::Op_Text);
		theStopWatch.resetTimer();
		for (uint32_t x = 0; x < 1024 * 256; ++x) {
			theSerializer["d"]["intents"] = x;
			theSerializer.refreshString(WebSocketOpCode::Op_Text);
			if (x % 1000 == 0) {
				//std::cout << theSerializer.operator DiscordCoreAPI::String()<< std::endl;
			}
			theVector.push_back(theSerializer.operator String());
			theSize += theVector.back().size();
		}
		std::cout << "THE SIZE: " << theSize << std::endl;
		std::cout << "THE TIME: " << theStopWatch.totalTimePassed() << std::endl;


		WebSocketIdentifyDataTwo theDataBewTwoReal{};
		theDataBewTwoReal.botToken = "TEST_TOKEN";
		theDataBewTwoReal.numberOfShards = 0;
		theDataBewTwoReal.currentShard = 23;

		theVector.clear();
		auto theReferenceTwo = theDataBewTwoReal.operator nlohmann::json_abi_v3_11_2::json();
		theSize = 0;
		theStopWatch.resetTimer();
		for (uint32_t x = 0; x < 1024 * 256; ++x) {
			theReferenceTwo["d"]["intents"] = x;
			if (x % 1000 == 0) {
				//std::cout << theString << std::endl;
			}
			theVector.push_back(theReferenceTwo.dump());
			theSize += theVector.back().size();
		}
		std::cout << "THE SIZE: " << theSize << std::endl;
		std::cout << "THE TIME: " << theStopWatch.totalTimePassed() << std::endl;



		
		
		std::this_thread::sleep_for(std::chrono::milliseconds{ 2000 });


	} catch (...) {
		DiscordCoreAPI::reportException("main()");
	};

	return 0;
}

