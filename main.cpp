#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"
#include "JsonSerializer.hpp"
//#include <nlohmann/json.hpp>

String& JsonObject::parseJsonToEtf(JsonObject&& dataToParse) {
	this->bufferString.clear();
	this->offSet = 0;
	this->size = 0;
	this->buffer = {};
	this->appendVersion();
	this->singleValueJsonToETF(std::move(dataToParse));
	return this->bufferString;
}

void JsonObject::singleValueJsonToETF(JsonObject&& jsonData) {
	switch (jsonData.theType) {
		case ValueType::Array: {
			this->writeArray(std::move(*jsonData.theValue.array));
			break;
		}
		case ValueType::Object: {
			this->writeObject(std::move(*jsonData.theValue.object));
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
			this->writeString(std::move(*jsonData.theValue.string));
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

void JsonObject::writeNullExt() {
	this->appendNilExt();
}

void JsonObject::writeNull() {
	this->appendNil();
}

void JsonObject::writeObject(JsonObject::ObjectType&& jsonData) {
	Bool add_comma{ false };
	this->appendMapHeader(static_cast<Uint32>(jsonData.size()));
	for (auto field: jsonData) {
		if (add_comma) {
		}
		StringStream theStream{};
		theStream << field.first;
		String theKey = theStream.str();

		auto theSize = theKey.size();
		this->appendBinaryExt(std::move(theKey), static_cast<Uint32>(theSize));
		this->singleValueJsonToETF(std::move(field.second));
		add_comma = true;
	}
}

void JsonObject::writeString(JsonObject::StringType&& jsonData) {
	StringStream theStream{};
	theStream << jsonData;
	auto theSize = static_cast<Uint32>(theStream.str().size());
	this->appendBinaryExt(theStream.str(), theSize);
}

void JsonObject::writeFloat(JsonObject::FloatType jsonData) {
	auto theFloat = jsonData;
	this->appendNewFloatExt(theFloat);
}

void JsonObject::writeUint(JsonObject::UintType jsonData) {
	auto theInt = jsonData;
	if (theInt <= 255 && theInt >= 0) {
		this->appendSmallIntegerExt(static_cast<Uint8>(theInt));
	} else if (theInt <= std::numeric_limits<Uint32>::max() && theInt >= 0) {
		this->appendIntegerExt(static_cast<Uint32>(theInt));
	} else {
		this->appendUnsignedLongLong(theInt);
	}
}

void JsonObject::writeInt(JsonObject::IntType jsonData) {
	auto theInt = jsonData;
	if (theInt <= 127 && theInt >= -127) {
		this->appendSmallIntegerExt(static_cast<Uint8>(theInt));
	} else if (theInt <= std::numeric_limits<Int32>::max() && theInt >= std::numeric_limits<Int32>::min()) {
		this->appendIntegerExt(static_cast<Uint32>(theInt));
	} else {
		this->appendUnsignedLongLong(static_cast<Uint64>(theInt));
	}
}

void JsonObject::writeArray(JsonObject::ArrayType&& jsonData) {
	Bool add_comma{ false };
	this->appendListHeader(static_cast<Uint32>(jsonData.size()));
	for (auto element: jsonData) {
		this->singleValueJsonToETF(std::move(element));
		add_comma = true;
	}
	this->appendNilExt();
}

void JsonObject::writeBool(JsonObject::BoolType jsonData) {
	auto theBool = jsonData;
	if (theBool) {
		this->appendTrue();
	} else {
		this->appendFalse();
	}
}

void JsonObject::writeToBuffer(String&& bytes) {
	this->bufferString.insert(this->bufferString.end(), bytes.begin(), bytes.end());
	this->offSet += bytes.size();
}

void JsonObject::appendBinaryExt(String&& bytes, Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Binary_Ext) };
	DiscordCoreAPI::storeBits(bufferNew, sizeNew);
	this->writeToBuffer(std::move(bufferNew));
	this->writeToBuffer(std::move(bytes));
}

void JsonObject::appendUnsignedLongLong(Uint64 value) {
	String bufferNew{};
	bufferNew.resize(static_cast<Uint64>(1) + 2 + sizeof(Uint64));
	bufferNew[0] = static_cast<Uint8>(ETFTokenType::Small_Big_Ext);
	DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::milliseconds{ 1500 } };
	Uint8 bytesToEncode = 0;
	while (value > 0) {
		if (theStopWatch.hasTimePassed()) {
			break;
		}
		bufferNew[static_cast<Uint64>(3) + bytesToEncode] = value & 0xF;
		value >>= 8;
		bytesToEncode++;
	}
	bufferNew[1] = bytesToEncode;
	bufferNew[2] = 0;
	this->writeToBuffer(std::move(bufferNew));
}

void JsonObject::appendSmallIntegerExt(Uint8 value) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Integer_Ext), static_cast<char>(value) };
	this->writeToBuffer(std::move(bufferNew));
}

void JsonObject::appendIntegerExt(Uint32 value) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Integer_Ext) };
	DiscordCoreAPI::storeBits(bufferNew, value);
	this->writeToBuffer(std::move(bufferNew));
}

void JsonObject::appendListHeader(Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::List_Ext) };
	DiscordCoreAPI::storeBits(bufferNew, sizeNew);
	this->writeToBuffer(std::move(bufferNew));
}

void JsonObject::appendMapHeader(Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Map_Ext) };
	DiscordCoreAPI::storeBits(bufferNew, sizeNew);
	this->writeToBuffer(std::move(bufferNew));
}

void JsonObject::appendNewFloatExt(Double FloatValue) {
	String bufferNew{};
	bufferNew.push_back(static_cast<unsigned char>(ETFTokenType::New_Float_Ext));

	void* punner{ &FloatValue };
	DiscordCoreAPI::storeBits(bufferNew, *static_cast<Uint64*>(punner));
	this->writeToBuffer(std::move(bufferNew));
}

void JsonObject::appendVersion() {
	String bufferNew{};
	bufferNew.push_back(static_cast<char>(formatVersion));
	this->writeToBuffer(std::move(bufferNew));
}

void JsonObject::appendNilExt() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Nil_Ext) };
	this->writeToBuffer(std::move(bufferNew));
}

void JsonObject::appendFalse() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 5, static_cast<Uint8>('f'), static_cast<Uint8>('a'), static_cast<Uint8>('l'), static_cast<Uint8>('s'),
		static_cast<Uint8>('e') };
	this->writeToBuffer(std::move(bufferNew));
}

void JsonObject::appendTrue() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 4, static_cast<Uint8>('t'), static_cast<Uint8>('r'), static_cast<Uint8>('u'), static_cast<Uint8>('e') };
	this->writeToBuffer(std::move(bufferNew));
}

void JsonObject::appendNil() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 3, static_cast<Uint8>('n'), static_cast<Uint8>('i'), static_cast<Uint8>('l') };
	this->writeToBuffer(std::move(bufferNew));
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
	this->appendUnsignedLongLong(theData);
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
		auto result = this->theValue.object->emplace(key, ValueType::Null);
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

JsonObject& JsonSerializer::operator[](Uint64 index) const {
	return this->theValue.theValue.array->operator[](index);
}

JsonObject& JsonSerializer::operator[](Uint64 index) {
	if (this->theType == ValueType::Null) {
		this->theValue.set(std::make_unique<JsonObject::ArrayType>());
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		if (index >= this->theValue.theValue.array->size()) {
			this->theValue.theValue.array->resize(index + 1);
		}

		return this->theValue.theValue.array->operator[](index);
	}
	throw std::runtime_error{ "Sorry, but that index could not be produced/accessed." };
}

JsonObject& JsonSerializer::operator[](const typename ObjectType::key_type& key) const {
	if (this->theType == ValueType::Object) {
		auto result = this->theValue.theValue.object->emplace(key, ValueType::Null);
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
		auto result = this->theValue.theValue.object->emplace(std::move(key), JsonObject{});
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

void JsonObject::set(std::unique_ptr<String> pointer) {
	destroy();
	new (&this->theValue.string) std::unique_ptr<String>{ std::move(pointer) };
	this->theType = ValueType::String;
}

void JsonObject::set(std::unique_ptr<ArrayType> pointer) {
	destroy();
	new (&this->theValue.string) std::unique_ptr<ArrayType>{ std::move(pointer) };
	this->theType = ValueType::Array;
}

void JsonObject::set(std::unique_ptr<ObjectType> pointer) {
	destroy();
	new (&this->theValue.string) std::unique_ptr<ObjectType>{ std::move(pointer) };
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

void JsonSerializer::writeString(const JsonObject& theObject, String& theString) const noexcept {
	switch (theObject.theType) {
		case ValueType::Object: {
			//theObject.currentOffsetIntoStringStart = theString.size();
			if (theObject.theValue.object->empty()) {
				theString += "{}";
			}

			theString += '{';

			Uint64 theIndex{};
			for (auto iterator = theObject.theValue.object->cbegin(); iterator != theObject.theValue.object->cend(); ++iterator) {
				theString += '\"';
				theString += iterator->first;
				theString += "\":";
				writeString(iterator->second, theString);
				if (theIndex < theObject.theValue.object->size() - 1) {
					theString += ',';
				}
				theIndex++;
			}
			theString += '}';
			//theObject.currentOffsetIntoStringEnd = theString.size();
			//theObject.theString =
			//StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			//std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Array: {
			//theObject.currentOffsetIntoStringStart = theString.size();
			if (theObject.theValue.array->empty()) {
				theString += "[]";
				break;
			}

			theString += '[';

			for (auto iterator = theObject.theValue.array->cbegin(); iterator != theObject.theValue.array->cend() - 1; ++iterator) {
				writeString(*iterator, theString);
				theString += ',';
			}

			theString += theObject.theValue.array->back();

			theString += ']';
			//theObject.currentOffsetIntoStringEnd = theString.size();
			//theObject.theString =
			//				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			//std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}

		case ValueType::String: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += '\"';
			theString += std::move(*theObject.theValue.string);
			theString += '\"';
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			//std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Bool: {
			theObject.currentOffsetIntoStringStart = theString.size();
			std::stringstream theStream{};
			theStream << std::boolalpha << theObject.theValue.boolean;
			theString += theStream.str();
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			//std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Float: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += std::to_string(theObject.theValue.numberDouble);
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			//std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Uint64: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += std::to_string(theObject.theValue.numberUint);
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			//std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Int64: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += std::to_string(theObject.theValue.numberInt);
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			//std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Null: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += "null";
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			//std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Null_Ext: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += "[]";
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			//std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
	}
	return;
}

void JsonSerializer::writeString(const JsonObject& theObject, String& theString) noexcept {
	
	switch (theObject.theType) {
		case ValueType::Object: {
			//theObject.currentOffsetIntoStringStart = theString.size();
			if (theObject.theValue.object->empty()) {
				theString += "{}";
			}

			theString += '{';

			Uint64 theIndex{};
			for (auto iterator = theObject.theValue.object->cbegin(); iterator != theObject.theValue.object->cend(); ++iterator) {
				theString += '\"';
				theString += iterator->first;
				theString += "\":";
				writeString(iterator->second, theString);
				if (theIndex < theObject.theValue.object->size() - 1) {
					theString += ',';
				}
				theIndex++;
			}
			theString += '}';
			//theObject.currentOffsetIntoStringEnd = theString.size();
			//theObject.theString =
			//StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Array: {
			//theObject.currentOffsetIntoStringStart = theString.size();
			if (theObject.theValue.array->empty()) {
				theString += "[]";
				break;
			}

			theString += '[';

			for (auto iterator = theObject.theValue.array->cbegin(); iterator != theObject.theValue.array->cend() - 1; ++iterator) {
				writeString(*iterator, theString);
				theString += ',';
			}

			theString += theObject.theValue.array->back();

			theString += ']';
			//theObject.currentOffsetIntoStringEnd = theString.size();
			//theObject.theString =
			//				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}

		case ValueType::String: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += '\"';
			theString += std::move(*theObject.theValue.string);
			theString += '\"';
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Bool: {
			theObject.currentOffsetIntoStringStart = theString.size();
			std::stringstream theStream{};
			theStream << std::boolalpha << theObject.theValue.boolean;
			theString += theStream.str();
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Float: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += std::to_string(theObject.theValue.numberDouble);
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Uint64: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += std::to_string(theObject.theValue.numberUint);
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Int64: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += std::to_string(theObject.theValue.numberInt);
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Null: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += "null";
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
		case ValueType::Null_Ext: {
			theObject.currentOffsetIntoStringStart = theString.size();
			theString += "[]";
			theObject.currentOffsetIntoStringEnd = theString.size();
			theObject.theString =
				StringView{ theString.data() + theObject.currentOffsetIntoStringStart, theObject.currentOffsetIntoStringEnd - theObject.currentOffsetIntoStringStart };
			std::cout << "THE STRING: " << theObject.theString << std::endl;
			break;
		}
	}
	return;
}

JsonSerializer::operator String&() const {
	this->writeString(this->theValue, *this->theString);
	return *this->theString;
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

	operator JsonObject();
};

WebSocketIdentifyData::operator JsonObject() {
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
	int32_t theIndex{};
	for (auto& value: *theSerializer["d"]["presence"]["activities"].theValue.array) {
		theIndex++;
		JsonObject theObject{};
		theObject = JsonObject{ value };
		theObject["status"] = theIndex;
		value = theObject;
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
	std::cout << "THE LINES: " << static_cast<String>(theSerializer) << std::endl;
	return theSerializer;

}

int32_t main() noexcept {
	try {
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
			
			std::string theResults02{};
			size_t theSize{};
			for (int32_t x = 0; x < 1; ++x) {
				theSerializer["d"]["intents"] = x;
				theResults02 = theSerializer;
				std::cout << "THE STRING: " << theResults02 << std::endl;
				theSize += theResults02.size();
				
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

