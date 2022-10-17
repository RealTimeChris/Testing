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

JsonSerializer& JsonSerializer::operator=(EnumConverter&& theData) noexcept {
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

JsonSerializer::JsonSerializer(EnumConverter&& theData) noexcept {
	*this = std::move(theData);
}

JsonSerializer& JsonSerializer::operator=(const EnumConverter& theData) noexcept {
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

JsonSerializer::JsonSerializer(const EnumConverter& theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(JsonSerializer&& theKey) noexcept {
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

JsonSerializer::JsonSerializer(JsonSerializer&& theKey) noexcept {
	*this = std::move(theKey);
}

JsonSerializer& JsonSerializer::operator=(const JsonSerializer& theKey) noexcept {
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

JsonSerializer::JsonSerializer(const JsonSerializer& theKey) noexcept {
	*this = theKey;
}

JsonSerializer& JsonSerializer::operator=(String&& theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.string = std::move(theData);
	this->theType = ValueType::String;
	return *this;
}

JsonSerializer::JsonSerializer(String&& theData) noexcept {
	*this = std::move(theData);
}

JsonSerializer& JsonSerializer::operator=(const String& theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	return *this;
}

JsonSerializer::JsonSerializer(const String& theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(const char* theData) noexcept {
	this->set(ValueType::String);
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	return *this;
}

JsonSerializer::JsonSerializer(const char* theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Uint64 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonSerializer::JsonSerializer(Uint64 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Uint32 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonSerializer::JsonSerializer(Uint32 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Uint16 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonSerializer::JsonSerializer(Uint16 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Uint8 theData) noexcept {
	this->theValue.numberUint = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonSerializer::JsonSerializer(Uint8 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Int64 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonSerializer::JsonSerializer(Int64 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Int32 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonSerializer::JsonSerializer(Int32 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Int16 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonSerializer::JsonSerializer(Int16 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Int8 theData) noexcept {
	this->theValue.numberInt = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonSerializer::JsonSerializer(Int8 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Double theData) noexcept {
	this->theValue.numberDouble = theData;
	this->theType = ValueType::Float;
	return *this;
}

JsonSerializer::JsonSerializer(Double theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Float theData) noexcept {
	this->theValue.numberDouble = theData;
	this->theType = ValueType::Float;
	return *this;
}

JsonSerializer::JsonSerializer(Float theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Bool theData) noexcept {
	this->theValue.boolean = theData;
	this->theType = ValueType::Bool;
	return *this;
}

JsonSerializer::JsonSerializer(Bool theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(ValueType theTypeNew) noexcept {
	this->theType = theTypeNew;
	return *this;
}

JsonSerializer::JsonSerializer(ValueType theType) noexcept {
	*this = theType;
}

JsonSerializer& JsonSerializer::operator[](typename ObjectType::key_type key) {
	if (this->theType == ValueType::Null) {
		this->set(ValueType::Object);
		this->theType = ValueType::Object;
	}

	if (this->theType == ValueType::Object) {
		auto result = this->theValue.object->emplace(std::move(key), JsonSerializer{});
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

JsonSerializer& JsonSerializer::operator[](Uint64 index) {
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

Void JsonSerializer::pushBack(JsonSerializer&& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->set(ValueType::Array);
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(std::move(other));
	}
}

Void JsonSerializer::pushBack(JsonSerializer& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->set(ValueType::Array);
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(other);
	}
}

Void JsonSerializer::set(ValueType theTypeNew) {
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

Void JsonSerializer::destroy() noexcept {
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

JsonSerializer::~JsonSerializer() noexcept {
	this->destroy();
}

bool operator==(const JsonSerializer& lhs, const JsonSerializer& rhs) {
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

JsonSerializer::operator String() noexcept {
	return std::move(this->theString);
}

void JsonSerializer::dump(WebSocketOpCode theOpCodeNew) {
	if (theOpCodeNew == WebSocketOpCode::Op_Binary) {
		this->parseJsonToEtf(*this);
	} else {
		this->parseJsonToJson(*this);
	}
	return;
}

void JsonSerializer::singleValueJsonToETF(const JsonSerializer& jsonData) {
	switch (jsonData.theType) {
		case ValueType::Object: {
			this->writeObject(jsonData.theValue.object);
			break;
		}
		case ValueType::Array: {
			this->writeArray(jsonData.theValue.array);
			break;
		}
		case ValueType::Bool: {
			this->writeBool(jsonData.theValue.boolean);
			break;
		}
		case ValueType::Float: {
			this->writeFloat(jsonData.theValue.numberDouble);
			break;
		}
		case ValueType::Int64: {
			this->writeInt(jsonData.theValue.numberInt);
			break;
		}
		case ValueType::Uint64: {
			this->writeUint(jsonData.theValue.numberUint);
			break;
		}
		case ValueType::String: {
			this->writeString(jsonData.theValue.string);
			break;
		}
		case ValueType::Null: {
			this->writeNull();
			break;
		}
		case ValueType::Null_Ext: {
			this->writeNullExt();
			break;
		}
	}
}

void JsonSerializer::parseJsonToJson(const JsonSerializer& dataToParse) {
	switch (dataToParse.theType) {
		case ValueType::Object: {
			this->writeJsonObject(*dataToParse.theValue.object);
			break;
		}
		case ValueType::Array: {
			this->writeJsonArray(*dataToParse.theValue.array);
			break;
		}
		case ValueType::String: {
			this->writeCharacter('\"');
			dumpEscaped(*dataToParse.theValue.string);
			this->writeCharacter('\"');
			break;
		}
		case ValueType::Bool: {
			if (dataToParse.theValue.boolean) {
				this->writeCharacters("true", 4);
			} else {
				this->writeCharacters("false", 5);
			}
			break;
		}
		case ValueType::Int64: {
			dumpInt(dataToParse.theValue.numberInt);
			break;
		}
		case ValueType::Uint64: {
			dumpInt(dataToParse.theValue.numberUint);
			break;
		}
		case ValueType::Float: {
			dumpFloat(dataToParse.theValue.numberDouble);
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
}

void JsonSerializer::parseJsonToEtf(const JsonSerializer& dataToParse) {
	this->appendVersion();
	this->singleValueJsonToETF(dataToParse);
	return;
}

void JsonSerializer::writeJsonObject(const JsonSerializer::ObjectType& theObjectNew) {
	if (theObjectNew.empty()) {
		this->writeCharacters("{}", 2);
		return;
	}
	this->writeCharacter('{');

	Int32 theIndex{};
	for (auto x = theObjectNew.cbegin(); x != theObjectNew.cend(); ++x) {
		this->writeCharacter('\"');
		dumpEscaped(x->first);
		this->writeCharacters("\":", 2);
		dump(x->second);

		if (theIndex != theObjectNew.size() - 1) {
			this->writeCharacter(',');
		}
		theIndex++;
	}

	this->writeCharacter('}');
}

void JsonSerializer::writeJsonArray(const JsonSerializer::ArrayType& theArray) {
	if (theArray.empty()) {
		this->writeCharacters("[]", 2);
		return;
	}

	this->writeCharacter('[');

	Int32 theIndex{};
	for (auto x = theArray.cbegin(); x != theArray.cend(); ++x) {
		dump(*x);
		if (theIndex != theArray.size() - 1) {
			this->writeCharacter(',');
		}
		theIndex++;
	}

	this->writeCharacter(']');
}

void JsonSerializer::writeObject(const JsonSerializer::ObjectType* jsonData) {
	this->appendMapHeader(static_cast<Uint32>(jsonData->size()));
	for (auto& field: *jsonData) {
		this->appendBinaryExt(field.first, static_cast<Uint32>(field.first.size()));
		this->singleValueJsonToETF(field.second);
	}
}

void JsonSerializer::writeString(const JsonSerializer::StringType* jsonData) {
	this->appendBinaryExt(*jsonData, static_cast<Uint32>(jsonData->size()));
}

void JsonSerializer::writeFloat(const JsonSerializer::FloatType jsonData) {
	auto theFloat = jsonData;
	this->appendNewFloatExt(theFloat);
}

void JsonSerializer::writeUint(const JsonSerializer::UintType jsonData) {
	auto theInt = jsonData;
	if (theInt <= 255 && theInt >= 0) {
		this->appendSmallIntegerExt(static_cast<Uint8>(theInt));
	} else if (theInt <= std::numeric_limits<Uint32>::max() && theInt >= 0) {
		this->appendIntegerExt(static_cast<Uint32>(theInt));
	} else {
		this->appendUnsignedLongLong(theInt);
	}
}

void JsonSerializer::writeInt(const JsonSerializer::IntType jsonData) {
	auto theInt = jsonData;
	if (theInt <= 127 && theInt >= -127) {
		this->appendSmallIntegerExt(static_cast<Uint8>(theInt));
	} else if (theInt <= std::numeric_limits<Int32>::max() && theInt >= std::numeric_limits<Int32>::min()) {
		this->appendIntegerExt(static_cast<Uint32>(theInt));
	} else {
		this->appendUnsignedLongLong(static_cast<Uint64>(theInt));
	}
}

void JsonSerializer::writeArray(const JsonSerializer::ArrayType* jsonData) {
	this->appendListHeader(static_cast<Uint32>(jsonData->size()));
	for (auto& element: *jsonData) {
		this->singleValueJsonToETF(element);
	}
	this->appendNilExt();
}

void JsonSerializer::writeBool(const JsonSerializer::BoolType jsonData) {
	auto theBool = jsonData;
	if (theBool) {
		this->appendTrue();
	} else {
		this->appendFalse();
	}
}

void JsonSerializer::writeNullExt() {
	this->appendNilExt();
}

void JsonSerializer::writeNull() {
	this->appendNil();
}

void JsonSerializer::appendBinaryExt(const String& bytes, Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Binary_Ext) };
	storeBits(bufferNew, sizeNew);
	this->writeCharacters(bufferNew.data(), bufferNew.size());
	this->writeCharacters(bytes.data(), bytes.size());
}

void JsonSerializer::appendUnsignedLongLong(const Uint64 value) {
	String bufferNew{};
	Uint64 theValue = value;
	bufferNew.resize(static_cast<Uint64>(1) + 2 + sizeof(Uint64));
	bufferNew[0] = static_cast<Uint8>(ETFTokenType::Small_Big_Ext);
	StopWatch theStopWatch{ std::chrono::milliseconds{ 1500 } };
	Uint8 bytesToEncode = 0;
	while (theValue > 0) {
		if (theStopWatch.hasTimePassed()) {
			break;
		}
		bufferNew[static_cast<Uint64>(3) + bytesToEncode] = theValue & 0xF;
		theValue >>= 8;
		bytesToEncode++;
	}
	bufferNew[1] = bytesToEncode;
	bufferNew[2] = 0;
	this->writeCharacters(bufferNew.data(), bufferNew.size());
}

void JsonSerializer::appendNewFloatExt(const Double FloatValue) {
	String bufferNew{};
	bufferNew.push_back(static_cast<unsigned char>(ETFTokenType::New_Float_Ext));

	const void* punner{ &FloatValue };
	storeBits(bufferNew, *static_cast<const Uint64*>(punner));
	this->writeCharacters(bufferNew.data(), bufferNew.size());
}

void JsonSerializer::appendSmallIntegerExt(const Uint8 value) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Integer_Ext), static_cast<char>(value) };
	this->writeCharacters(bufferNew.data(), bufferNew.size());
}

void JsonSerializer::appendIntegerExt(const Uint32 value) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Integer_Ext) };
	storeBits(bufferNew, value);
	this->writeCharacters(bufferNew.data(), bufferNew.size());
}

void JsonSerializer::appendListHeader(const Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::List_Ext) };
	storeBits(bufferNew, sizeNew);
	this->writeCharacters(bufferNew.data(), bufferNew.size());
}

void JsonSerializer::appendMapHeader(const Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Map_Ext) };
	storeBits(bufferNew, sizeNew);
	this->writeCharacters(bufferNew.data(), bufferNew.size());
}

void JsonSerializer::appendVersion() {
	String bufferNew{ static_cast<int8_t>(formatVersion) };
	this->writeCharacters(bufferNew.data(), bufferNew.size());
}

void JsonSerializer::appendNilExt() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Nil_Ext) };
	this->writeCharacters(bufferNew.data(), bufferNew.size());
}

void JsonSerializer::appendFalse() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 5, static_cast<Uint8>('f'), static_cast<Uint8>('a'), static_cast<Uint8>('l'), static_cast<Uint8>('s'),
		static_cast<Uint8>('e') };
	this->writeCharacters(bufferNew.data(), bufferNew.size());
}

void JsonSerializer::appendTrue() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 4, static_cast<Uint8>('t'), static_cast<Uint8>('r'), static_cast<Uint8>('u'), static_cast<Uint8>('e') };
	this->writeCharacters(bufferNew.data(), bufferNew.size());
}

void JsonSerializer::appendNil() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 3, static_cast<Uint8>('n'), static_cast<Uint8>('i'), static_cast<Uint8>('l') };
	this->writeCharacters(bufferNew.data(), bufferNew.size());
}

void JsonSerializer::writeCharacters(const char* theData, std::size_t length) {
	theString.append(theData, length);
}

void JsonSerializer::writeCharacter(const char theChar) {
	theString.push_back(theChar);
}

void JsonSerializer::dumpEscaped(const String& string) {
	for (std::size_t x = 0; x < string.size(); ++x) {
		switch (static_cast<std::uint8_t>(string[x])) {
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
				this->writeCharacter(string[x]);
				break;
			}
		}
	}
}

void JsonSerializer::dump(const JsonSerializer& val) {
	switch (val.theType) {
		case ValueType::Object: {
			if (val.theValue.object->empty()) {
				this->writeCharacters("{}", 2);
				return;
			}
			this->writeCharacter('{');

			Int32 theIndex{};
			for (auto x = val.theValue.object->cbegin(); x != val.theValue.object->cend(); ++x) {
				this->writeCharacter('\"');
				dumpEscaped(x->first);
				this->writeCharacters("\":", 2);
				dump(x->second);

				if (theIndex != val.theValue.object->size() - 1) {
					this->writeCharacter(',');
				}
				theIndex++;
			}

			this->writeCharacter('}');
			return;
		}
		case ValueType::Array: {
			if (val.theValue.array->empty()) {
				this->writeCharacters("[]", 2);
				return;
			}

			this->writeCharacter('[');

			Int32 theIndex{};
			for (auto x = val.theValue.array->cbegin(); x != val.theValue.array->cend(); ++x) {
				dump(*x);
				if (theIndex != val.theValue.array->size() - 1) {
					this->writeCharacter(',');
				}
				theIndex++;
			}

			this->writeCharacter(']');
			return;
		}

		case ValueType::String: {
			this->writeCharacter('\"');
			dumpEscaped(*val.theValue.string);
			this->writeCharacter('\"');
			return;
		}

		case ValueType::Bool: {
			if (val.theValue.boolean) {
				this->writeCharacters("true", 4);
			} else {
				this->writeCharacters("false", 5);
			}
			return;
		}

		case ValueType::Int64: {
			dumpInt(val.theValue.numberInt);
			return;
		}

		case ValueType::Uint64: {
			dumpInt(val.theValue.numberUint);
			return;
		}

		case ValueType::Float: {
			dumpFloat(val.theValue.numberDouble);
			return;
		}

		case ValueType::Null: {
			this->writeCharacters("null", 4);
			return;
		}
		case ValueType::Null_Ext: {
			this->writeCharacters("[]", 2);
			return;
		}
	}
}

void JsonSerializer::dumpFloat(const Float x) {
	auto theFloat = std::to_string(x);
	this->writeCharacters(theFloat.data(), theFloat.size());
}

struct UpdatePresenceData {
	String status{};///< Current status.
	Int64 since{ 0 };///< When was the activity started?
	Bool afk{ false };///< Are we afk.
	String theString{};
	operator JsonSerializer();
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

UpdatePresenceData ::operator JsonSerializer() {
	JsonSerializer theData{};
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
	operator JsonSerializer();
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
	//JsonSerializer<char> theSerializerTwo{};
	return theSerializer;
}

WebSocketIdentifyData::operator JsonSerializer() {
	JsonSerializer theSerializer{};
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
	//JsonSerializer<char> theSerializerTwo{};
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
		auto theReference = theDataBewTwo.operator JsonSerializer();
		
		size_t theSize{};
		JsonSerializer theSerializer{ theDataBewTwo.operator JsonSerializer() };
		theStopWatch.resetTimer();
		for (uint32_t x = 0; x < 1024 * 256; ++x) {
			theSerializer["d"]["intents"] = x;
			theSerializer.dump(WebSocketOpCode::Op_Text);
			if (x % 1000 == 0) {
				//std::cout << theString << std::endl;
			}
			theVector.push_back(theSerializer);
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

