#include "../Include/Jsonifier.hpp"

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
		case JsonType::Object: {
			this->setValue(JsonType::Object);
			*this->theValue.object = std::move(*theData.theValue.object);
			break;
		}
		case JsonType::Array: {
			this->setValue(JsonType::Array);
			*this->theValue.array = std::move(*theData.theValue.array);
			break;
		}
		case JsonType::String: {
			this->setValue(JsonType::String);
			*this->theValue.string = std::move(*theData.theValue.string);
			break;
		}
		case JsonType::Float: {
			this->theValue.numberDouble = theData.theValue.numberDouble;
			break;
		}
		case JsonType::Uint64: {
			this->theValue.numberUint = theData.theValue.numberUint;
			break;
		}
		case JsonType::Int64: {
			this->theValue.numberInt = theData.theValue.numberInt;
			break;
		}
		case JsonType::Bool: {
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
		case JsonType::Object: {
			this->setValue(JsonType::Object);
			*this->theValue.object = *theData.theValue.object;
			break;
		}
		case JsonType::Array: {
			this->setValue(JsonType::Array);
			*this->theValue.array = *theData.theValue.array;
			break;
		}
		case JsonType::String: {
			this->setValue(JsonType::String);
			*this->theValue.string = *theData.theValue.string;
			break;
		}
		case JsonType::Float: {
			this->theValue.numberDouble = theData.theValue.numberDouble;
			break;
		}
		case JsonType::Int64: {
			this->theValue.numberInt = theData.theValue.numberInt;
			break;
		}
		case JsonType::Uint64: {
			this->theValue.numberUint = theData.theValue.numberUint;
			break;
		}
		case JsonType::Bool: {
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

JsonType Jsonifier::type() noexcept {
	return this->theType;
}

Void Jsonifier::refreshString(JsonifierSerializeType theOpCode) {
	this->theString.clear();
	if (theOpCode == JsonifierSerializeType::Etf) {
		this->appendVersion();
		this->serializeJsonToEtfString(this);
	} else {
		this->serializeJsonToJsonString(this);
	}
}

Jsonifier& Jsonifier::operator=(EnumConverter&& theData) noexcept {
	if (theData.isItAVector()) {
		this->setValue(JsonType::Array);
		for (auto& value: theData.operator Vector<Uint64>()) {
			this->theValue.array->emplace_back(std::move(value));
		}
	} else {
		this->theValue.numberUint = Uint64{ theData };
		this->theType = JsonType::Uint64;
	}
	return *this;
}

Jsonifier::Jsonifier(EnumConverter&& theData) noexcept {
	*this = std::move(theData);
}

Jsonifier& Jsonifier::operator=(const EnumConverter& theData) noexcept {
	if (theData.isItAVector()) {
		this->setValue(JsonType::Array);
		for (auto& value: theData.operator Vector<Uint64>()) {
			this->theValue.array->emplace_back(value);
		}
	} else {
		this->theValue.numberUint = Uint64{ theData };
		this->theType = JsonType::Uint64;
	}
	return *this;
}

Jsonifier::Jsonifier(const EnumConverter& theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(String&& theData) noexcept {
	this->setValue(JsonType::String);
	*this->theValue.string = std::move(theData);
	this->theType = JsonType::String;
	return *this;
}

Jsonifier::Jsonifier(String&& theData) noexcept {
	*this = std::move(theData);
}

Jsonifier& Jsonifier::operator=(const String& theData) noexcept {
	this->setValue(JsonType::String);
	*this->theValue.string = theData;
	this->theType = JsonType::String;
	return *this;
}

Jsonifier::Jsonifier(const String& theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(const char* theData) noexcept {
	this->setValue(JsonType::String);
	*this->theValue.string = theData;
	this->theType = JsonType::String;
	return *this;
}

Jsonifier::Jsonifier(const char* theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Double theData) noexcept {
	this->theValue.numberDouble = theData;
	this->theType = JsonType::Float;
	return *this;
}

Jsonifier::Jsonifier(Double theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Float theData) noexcept {
	this->theValue.numberDouble = theData;
	this->theType = JsonType::Float;
	return *this;
}

Jsonifier::Jsonifier(Float theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Uint64 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = JsonType::Uint64;
	return *this;
}

Jsonifier::Jsonifier(Uint64 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Uint32 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = JsonType::Uint64;
	return *this;
}

Jsonifier::Jsonifier(Uint32 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Uint16 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = JsonType::Uint64;
	return *this;
}

Jsonifier::Jsonifier(Uint16 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Uint8 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = JsonType::Uint64;
	return *this;
}

Jsonifier::Jsonifier(Uint8 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Int64 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = JsonType::Int64;
	return *this;
}

Jsonifier::Jsonifier(Int64 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Int32 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = JsonType::Int64;
	return *this;
}

Jsonifier::Jsonifier(Int32 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Int16 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = JsonType::Int64;
	return *this;
}

Jsonifier::Jsonifier(Int16 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Int8 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = JsonType::Int64;
	return *this;
}

Jsonifier::Jsonifier(Int8 theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(std::nullptr_t) noexcept {
	this->theType = JsonType::Null;
	return *this;
}

Jsonifier::Jsonifier(std::nullptr_t theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(Bool theData) noexcept {
	this->theValue.boolean = theData;
	this->theType = JsonType::Bool;
	return *this;
}

Jsonifier::Jsonifier(Bool theData) noexcept {
	*this = theData;
}

Jsonifier& Jsonifier::operator=(JsonType theTypeNew) noexcept {
	this->theType = theTypeNew;
	this->setValue(this->theType);
	return *this;
}

Jsonifier::Jsonifier(JsonType theType) noexcept {
	*this = theType;
}

Jsonifier& Jsonifier::operator[](typename ObjectType::key_type key) {
	if (this->theType == JsonType::Null) {
		this->setValue(JsonType::Object);
		this->theType = JsonType::Object;
	}

	if (this->theType == JsonType::Object) {
		auto result = this->theValue.object->emplace(std::move(key), Jsonifier{});
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

Jsonifier& Jsonifier::operator[](Uint64 index) {
	if (this->theType == JsonType::Null) {
		this->setValue(JsonType::Array);
		this->theType = JsonType::Array;
	}

	if (this->theType == JsonType::Array) {
		if (index >= this->theValue.array->size()) {
			this->theValue.array->resize(index + 1);
		}

		return this->theValue.array->operator[](index);
	}
	throw std::runtime_error{ "Sorry, but that index could not be produced/accessed." };
}

template<> Jsonifier::ObjectType& Jsonifier::get() {
	return *this->theValue.object;
}

template<> Jsonifier::ArrayType& Jsonifier::get() {
	return *this->theValue.array;
}

template<> Jsonifier::StringType& Jsonifier::get() {
	return *this->theValue.string;
}

template<> Jsonifier::FloatType& Jsonifier::get() {
	return this->theValue.numberDouble;
}

template<> Jsonifier::UintType& Jsonifier::get() {
	return this->theValue.numberUint;
}

template<> Jsonifier::IntType& Jsonifier::get() {
	return this->theValue.numberInt;
}

template<> Jsonifier::BoolType& Jsonifier::get() {
	return this->theValue.boolean;
}

Void Jsonifier::emplaceBack(Jsonifier&& other) noexcept {
	if (this->theType == JsonType::Null) {
		this->setValue(JsonType::Array);
		this->theType = JsonType::Array;
	}

	if (this->theType == JsonType::Array) {
		this->theValue.array->emplace_back(std::move(other));
	}
}

Void Jsonifier::emplaceBack(Jsonifier& other) noexcept {
	if (this->theType == JsonType::Null) {
		this->setValue(JsonType::Array);
		this->theType = JsonType::Array;
	}

	if (this->theType == JsonType::Array) {
		this->theValue.array->emplace_back(other);
	}
}

Void Jsonifier::serializeJsonToEtfString(const Jsonifier* dataToParse) {
	switch (dataToParse->theType) {
		case JsonType::Object: {
			return this->writeEtfObject(*dataToParse->theValue.object);
		}
		case JsonType::Array: {
			return this->writeEtfArray(*dataToParse->theValue.array);
		}
		case JsonType::String: {
			return this->writeEtfString(*dataToParse->theValue.string);
		}
		case JsonType::Float: {
			return this->writeEtfFloat(dataToParse->theValue.numberDouble);
		}
		case JsonType::Uint64: {
			return this->writeEtfUint(dataToParse->theValue.numberUint);
		}
		case JsonType::Int64: {
			return this->writeEtfInt(dataToParse->theValue.numberInt);
		}
		case JsonType::Bool: {
			return this->writeEtfBool(dataToParse->theValue.boolean);
		}
		case JsonType::Null: {
			return this->writeEtfNull();
		}
	}
}

Void Jsonifier::serializeJsonToJsonString(const Jsonifier* dataToParse) {
	switch (dataToParse->theType) {
		case JsonType::Object: {
			return this->writeJsonObject(*dataToParse->theValue.object);
		}
		case JsonType::Array: {
			return this->writeJsonArray(*dataToParse->theValue.array);
		}
		case JsonType::String: {
			return this->writeJsonString(*dataToParse->theValue.string);
		}
		case JsonType::Float: {
			return this->writeJsonFloat(dataToParse->theValue.numberDouble);
		}
		case JsonType::Uint64: {
			return this->writeJsonInt(dataToParse->theValue.numberUint);
		}
		case JsonType::Int64: {
			return this->writeJsonInt(dataToParse->theValue.numberInt);
		}
		case JsonType::Bool: {
			return this->writeJsonBool(dataToParse->theValue.boolean);
		}
		case JsonType::Null: {
			return this->writeJsonNull();
		}
	}
}

Void Jsonifier::writeJsonObject(const Jsonifier::ObjectType& theObjectNew) {
	if (theObjectNew.empty()) {
		this->writeString("{}", 2);
		return;
	}
	this->writeCharacter('{');

	Int32 theIndex{};
	for (auto& [key, value]: theObjectNew) {
		this->writeJsonString(key);
		this->writeCharacter(':');
		this->serializeJsonToJsonString(&value);

		if (theIndex != theObjectNew.size() - 1) {
			this->writeCharacter(',');
		}
		theIndex++;
	}

	this->writeCharacter('}');
}

Void Jsonifier::writeJsonArray(const Jsonifier::ArrayType& theArray) {
	if (theArray.empty()) {
		this->writeString("[]", 2);
		return;
	}

	this->writeCharacter('[');

	Int32 theIndex{};
	for (auto& value: theArray) {
		this->serializeJsonToJsonString(&value);
		if (theIndex != theArray.size() - 1) {
			this->writeCharacter(',');
		}
		theIndex++;
	}

	this->writeCharacter(']');
}

Void Jsonifier::writeJsonString(const Jsonifier::StringType& theStringNew) {
	this->writeCharacter('\"');
	for (auto& value: theStringNew) {
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

Void Jsonifier::writeJsonNull() {
	this->writeString("null", 4);
}

Void Jsonifier::writeEtfObject(const ObjectType& jsonData) {
	this->appendMapHeader(static_cast<Uint32>(jsonData.size()));
	for (auto& [key, value]: jsonData) {
		this->appendBinaryExt(key, static_cast<Uint32>(key.size()));
		this->serializeJsonToEtfString(&value);
	}
}

Void Jsonifier::writeEtfArray(const ArrayType& jsonData) {
	this->appendListHeader(static_cast<Uint32>(jsonData.size()));
	for (auto& value: jsonData) {
		this->serializeJsonToEtfString(&value);
	}
	this->appendNilExt();
}

Void Jsonifier::writeEtfString(const StringType& jsonData) {
	this->appendBinaryExt(jsonData, static_cast<Uint32>(jsonData.size()));
}

Void Jsonifier::writeEtfUint(const UintType jsonData) {
	if (jsonData <= 255) {
		this->appendSmallIntegerExt(static_cast<Uint8>(jsonData));
	} else if (jsonData <= std::numeric_limits<Uint32>::max()) {
		this->appendIntegerExt(static_cast<Uint32>(jsonData));
	} else {
		this->appendUnsignedLongLong(jsonData);
	}
}

Void Jsonifier::writeEtfInt(const IntType jsonData) {
	if (jsonData <= 127 && jsonData >= -127) {
		this->appendSmallIntegerExt(static_cast<Uint8>(jsonData));
	} else if (jsonData <= std::numeric_limits<Int32>::max() && jsonData >= std::numeric_limits<Int32>::min()) {
		this->appendIntegerExt(static_cast<Uint32>(jsonData));
	} else {
		this->appendUnsignedLongLong(static_cast<Uint64>(jsonData));
	}
}

Void Jsonifier::writeEtfFloat(const FloatType jsonData) {
	this->appendNewFloatExt(jsonData);
}

Void Jsonifier::writeEtfBool(const BoolType jsonData) {
	this->appendBool(jsonData);
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
		case JsonType::Object: {
			if (*lhs.theValue.object != *rhs.theValue.object) {
				return false;
			}
			break;
		}
		case JsonType::Array: {
			if (*lhs.theValue.array != *rhs.theValue.array) {
				return false;
			}
			break;
		}
		case JsonType::String: {
			if (*lhs.theValue.string != *rhs.theValue.string) {
				return false;
			}
			break;
		}
		case JsonType::Float: {
			if (lhs.theValue.numberDouble != rhs.theValue.numberDouble) {
				return false;
			}
			break;
		}
		case JsonType::Uint64: {
			if (lhs.theValue.numberUint != rhs.theValue.numberUint) {
				return false;
			}
			break;
		}
		case JsonType::Int64: {
			if (lhs.theValue.numberInt != rhs.theValue.numberInt) {
				return false;
			}
			break;
		}
		case JsonType::Bool: {
			if (lhs.theValue.boolean != rhs.theValue.boolean) {
				return false;
			}
			break;
		}
	}
	return true;
}

Void Jsonifier::appendBinaryExt(const String& bytes, Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(EtfType::Binary_Ext) };
	storeBits(bufferNew, sizeNew);
	this->writeString(bufferNew.data(), bufferNew.size());
	this->writeString(bytes.data(), bytes.size());
}

Void Jsonifier::appendUnsignedLongLong(const Uint64 value) {
	String bufferNew{};
	Uint64 theValueNew = value;
	bufferNew.resize(static_cast<Uint64>(1) + 2 + sizeof(Uint64));
	bufferNew[0] = static_cast<Uint8>(EtfType::Small_Big_Ext);
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
	String bufferNew{ static_cast<unsigned char>(EtfType::New_Float_Ext) };
	const Void* punner{ &FloatValue };
	storeBits(bufferNew, *static_cast<const Uint64*>(punner));
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendSmallIntegerExt(const Uint8 value) {
	String bufferNew{ static_cast<Uint8>(EtfType::Small_Integer_Ext), static_cast<char>(value) };
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendIntegerExt(const Uint32 value) {
	String bufferNew{ static_cast<Uint8>(EtfType::Integer_Ext) };
	storeBits(bufferNew, value);
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendListHeader(const Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(EtfType::List_Ext) };
	storeBits(bufferNew, sizeNew);
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendMapHeader(const Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(EtfType::Map_Ext) };
	storeBits(bufferNew, sizeNew);
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendBool(Bool theData) {
	if (theData) {
		this->writeCharacter(static_cast<Uint8>(EtfType::Small_Atom_Ext));
		this->writeCharacter(4);
		this->writeString("true", 4);

	} else {
		this->writeCharacter(static_cast<Uint8>(EtfType::Small_Atom_Ext));
		this->writeCharacter(5);
		this->writeString("false", 5);
	}
}

Void Jsonifier::appendVersion() {
	String bufferNew{ static_cast<int8_t>(formatVersion) };
	this->writeString(bufferNew.data(), bufferNew.size());
}

Void Jsonifier::appendNilExt() {
	this->writeCharacter(static_cast<Uint8>(EtfType::Nil_Ext));
}

Void Jsonifier::appendNil() {
	this->writeCharacter(static_cast<Uint8>(EtfType::Small_Atom_Ext));
	this->writeCharacter(3);
	this->writeString("nil", 3);
}

Void Jsonifier::setValue(JsonType theTypeNew) {
	this->destroy();
	this->theType = theTypeNew;
	switch (this->theType) {
		case JsonType::Object: {
			AllocatorType<ObjectType> allocator{};
			this->theValue.object = AllocatorTraits<ObjectType>::allocate(allocator, 1);
			AllocatorTraits<ObjectType>::construct(allocator, this->theValue.object);
			break;
		}
		case JsonType::Array: {
			AllocatorType<ArrayType> allocator{};
			this->theValue.array = AllocatorTraits<ArrayType>::allocate(allocator, 1);
			AllocatorTraits<ArrayType>::construct(allocator, this->theValue.array);
			break;
		}
		case JsonType::String: {
			AllocatorType<StringType> allocator{};
			this->theValue.string = AllocatorTraits<StringType>::allocate(allocator, 1);
			AllocatorTraits<StringType>::construct(allocator, this->theValue.string);
			break;
		}
	}
}

Void Jsonifier::destroy() noexcept {
	switch (this->theType) {
		case JsonType::Object: {
			AllocatorType<ObjectType> allocator{};
			AllocatorTraits<ObjectType>::destroy(allocator, this->theValue.object);
			AllocatorTraits<ObjectType>::deallocate(allocator, this->theValue.object, 1);
			break;
		}
		case JsonType::Array: {
			AllocatorType<ArrayType> allocator{};
			AllocatorTraits<ArrayType>::destroy(allocator, this->theValue.array);
			AllocatorTraits<ArrayType>::deallocate(allocator, this->theValue.array, 1);
			break;
		}
		case JsonType::String: {
			AllocatorType<StringType> allocator{};
			AllocatorTraits<StringType>::destroy(allocator, this->theValue.string);
			AllocatorTraits<StringType>::deallocate(allocator, this->theValue.string, 1);
			break;
		}
	}
}

Jsonifier::~Jsonifier() noexcept {
	this->destroy();
}