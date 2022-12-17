#include "Jsonifier.hpp"

namespace Jsonifier {

	JsonifierException::JsonifierException(const std::string& error, std::source_location location) noexcept : std::runtime_error(error) {
		std::stringstream stream{};
		stream << "Error Report: \n"
			   << "Caught in File: " << location.file_name() << " (" << std::to_string(location.line()) << ":" << std::to_string(location.column())
			   << ")"
			   << "\nThe Error: \n"
			   << error << std::endl
			   << std::endl;
		*static_cast<std::runtime_error*>(this) = std::runtime_error{ stream.str() };
	}

	EnumConverter::operator std::vector<uint64_t>() const noexcept {
		return this->vector;
	}

	EnumConverter::operator uint64_t() const noexcept {
		return this->integer;
	}

	bool EnumConverter::isItAVector() const noexcept {
		return this->vectorType;
	}

	Jsonifier& Jsonifier::operator=(Jsonifier&& data) noexcept {
		switch (data.type) {
			case JsonType::Object: {
				this->setValue(JsonType::Object);
				*this->jsonValue.object = std::move(*data.jsonValue.object);
				break;
			}
			case JsonType::Array: {
				this->setValue(JsonType::Array);
				*this->jsonValue.array = std::move(*data.jsonValue.array);
				break;
			}
			case JsonType::String: {
				this->setValue(JsonType::String);
				*this->jsonValue.string = std::move(*data.jsonValue.string);
				break;
			}
			case JsonType::Float: {
				this->jsonValue.numberDouble = data.jsonValue.numberDouble;
				break;
			}
			case JsonType::Uint64: {
				this->jsonValue.numberUint = data.jsonValue.numberUint;
				break;
			}
			case JsonType::Int64: {
				this->jsonValue.numberInt = data.jsonValue.numberInt;
				break;
			}
			case JsonType::Bool: {
				this->jsonValue.boolean = data.jsonValue.boolean;
				break;
			}
		}
		this->string = std::move(data.string);
		this->type = data.type;
		return *this;
	}

	Jsonifier& Jsonifier::operator=(ErrorCode data) {
		throw std::runtime_error{ std::string{ "Error Parsing Json Data - it is: " } + std::to_string(( int32_t )data) };
		return *this;
	}

	Jsonifier::Jsonifier(ErrorCode data) {
		*this = std::move(data);
	}

	Jsonifier& Jsonifier::operator=(ObjectType&& data) noexcept {
		this->setValue(JsonType::Object);
		for (auto& [key, value]: data) {
			(*this->jsonValue.object)[key] = std::move(value);
		}
		return *this;
	}

	Jsonifier::Jsonifier(ObjectType&& data) noexcept {
		*this = std::move(data);
	}

	Jsonifier& Jsonifier::operator=(const ObjectType& data) noexcept {
		this->setValue(JsonType::Object);
		for (auto& [key, value]: data) {
			(*this->jsonValue.object)[key] = value;
		}
		return *this;
	}

	Jsonifier::Jsonifier(const ObjectType& data) noexcept {
		*this = data;
	}

	Jsonifier::Jsonifier(Jsonifier&& data) noexcept {
		*this = std::move(data);
	}

	Jsonifier& Jsonifier::operator=(const Jsonifier& data) noexcept {
		switch (data.type) {
			case JsonType::Object: {
				this->setValue(JsonType::Object);
				*this->jsonValue.object = *data.jsonValue.object;
				break;
			}
			case JsonType::Array: {
				this->setValue(JsonType::Array);
				*this->jsonValue.array = *data.jsonValue.array;
				break;
			}
			case JsonType::String: {
				this->setValue(JsonType::String);
				*this->jsonValue.string = *data.jsonValue.string;
				break;
			}
			case JsonType::Float: {
				this->jsonValue.numberDouble = data.jsonValue.numberDouble;
				break;
			}
			case JsonType::Uint64: {
				this->jsonValue.numberUint = data.jsonValue.numberUint;
				break;
			}
			case JsonType::Int64: {
				this->jsonValue.numberInt = data.jsonValue.numberInt;
				break;
			}
			case JsonType::Bool: {
				this->jsonValue.boolean = data.jsonValue.boolean;
				break;
			}
		}
		this->string = data.string;
		this->type = data.type;
		return *this;
	}

	Jsonifier::Jsonifier(const Jsonifier& data) noexcept {
		*this = data;
	}

	Jsonifier::operator std::string_view() noexcept {
		return this->string;
	}

	JsonType Jsonifier::getType() noexcept {
		return this->type;
	}

	void Jsonifier::refreshString(JsonifierSerializeType opCode) {
		this->string.clear();
		if (opCode == JsonifierSerializeType::Etf) {
			this->appendVersion();
			this->serializeJsonToEtfString(this);
		} else {
			this->serializeJsonToJsonString(this);
		}
	}

	Jsonifier& Jsonifier::operator=(EnumConverter&& data) noexcept {
		if (data.isItAVector()) {
			this->setValue(JsonType::Array);
			for (auto& value: data.operator std::vector<uint64_t>()) {
				this->jsonValue.array->emplace_back(std::move(value));
			}
		} else {
			this->jsonValue.numberUint = uint64_t{ data };
			this->type = JsonType::Uint64;
		}
		return *this;
	}

	Jsonifier::Jsonifier(EnumConverter&& data) noexcept {
		*this = std::move(data);
	}

	Jsonifier& Jsonifier::operator=(const EnumConverter& data) noexcept {
		if (data.isItAVector()) {
			this->setValue(JsonType::Array);
			for (auto& value: data.operator std::vector<uint64_t>()) {
				this->jsonValue.array->emplace_back(value);
			}
		} else {
			this->jsonValue.numberUint = uint64_t{ data };
			this->type = JsonType::Uint64;
		}
		return *this;
	}

	Jsonifier::Jsonifier(const EnumConverter& data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(std::string_view&& data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = std::move(data);
		this->type = JsonType::String;
		return *this;
	}

	Jsonifier::Jsonifier(std::string_view&& data) noexcept {
		*this = std::move(data);
	}

	Jsonifier& Jsonifier::operator=(std::string_view& data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = data;
		this->type = JsonType::String;
		return *this;
	}

	Jsonifier::Jsonifier(std::string_view& data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(std::string&& data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = std::move(data);
		this->type = JsonType::String;
		return *this;
	}

	Jsonifier::Jsonifier(std::string&& data) noexcept {
		*this = std::move(data);
	}

	Jsonifier& Jsonifier::operator=(const std::string& data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = data;
		this->type = JsonType::String;
		return *this;
	}

	Jsonifier::Jsonifier(const std::string& data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(const char* data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = data;
		this->type = JsonType::String;
		return *this;
	}

	Jsonifier::Jsonifier(const char* data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(double data) noexcept {
		this->jsonValue.numberDouble = data;
		this->type = JsonType::Float;
		return *this;
	}

	Jsonifier::Jsonifier(double data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(float data) noexcept {
		this->jsonValue.numberDouble = data;
		this->type = JsonType::Float;
		return *this;
	}

	Jsonifier::Jsonifier(float data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(uint64_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	Jsonifier::Jsonifier(uint64_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(uint32_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	Jsonifier::Jsonifier(uint32_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(uint16_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	Jsonifier::Jsonifier(uint16_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(uint8_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	Jsonifier::Jsonifier(uint8_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(int64_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	Jsonifier::Jsonifier(int64_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(int32_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	Jsonifier::Jsonifier(int32_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(int16_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	Jsonifier::Jsonifier(int16_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(int8_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	Jsonifier::Jsonifier(int8_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(std::nullptr_t) noexcept {
		this->type = JsonType::Null;
		return *this;
	}

	Jsonifier::Jsonifier(std::nullptr_t data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(bool data) noexcept {
		this->jsonValue.boolean = data;
		this->type = JsonType::Bool;
		return *this;
	}

	Jsonifier::Jsonifier(bool data) noexcept {
		*this = data;
	}

	Jsonifier& Jsonifier::operator=(JsonType typeNew) noexcept {
		this->setValue(typeNew);
		return *this;
	}

	Jsonifier::Jsonifier(JsonType type) noexcept {
		*this = type;
	}

	bool Jsonifier::contains(std::string& key) {
		if (this->type == JsonType::Object) {
			return this->jsonValue.object->contains(key);
		} else {
			return false;
		}
	}

	Jsonifier& Jsonifier::operator[](typename ObjectType::key_type key) {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Object);
			this->type = JsonType::Object;
		}
		if (this->type == JsonType::Object) {
			auto result = this->jsonValue.object->emplace(std::move(key), Jsonifier{});
			return result.first->second;
		}
		throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
	}

	Jsonifier& Jsonifier::operator[](uint64_t index) {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Array);
			this->type = JsonType::Array;
		}

		if (this->type == JsonType::Array) {
			if (index >= this->jsonValue.array->size()) {
				this->jsonValue.array->resize(index + 1);
			}

			return this->jsonValue.array->operator[](index);
		}
		throw std::runtime_error{ "Sorry, but that index could not be produced/accessed." };
	}

	Jsonifier& Jsonifier::emplaceBack(Jsonifier&& other) noexcept {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Array);
			this->type = JsonType::Array;
		}

		if (this->type == JsonType::Array) {
			this->jsonValue.array->emplace_back(std::move(other));
		}
		return *this;
	}

	Jsonifier& Jsonifier::emplaceBack(Jsonifier& other) noexcept {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Array);
			this->type = JsonType::Array;
		}

		if (this->type == JsonType::Array) {
			this->jsonValue.array->emplace_back(std::move(other));
		}
		return *this;
	}

	void Jsonifier::serializeJsonToEtfString(const Jsonifier* jsonDataNew) {
		switch (jsonDataNew->type) {
			case JsonType::Object: {
				return this->writeEtfObject(*jsonDataNew->jsonValue.object);
			}
			case JsonType::Array: {
				return this->writeEtfArray(*jsonDataNew->jsonValue.array);
			}
			case JsonType::String: {
				return this->writeEtfString(*jsonDataNew->jsonValue.string);
			}
			case JsonType::Float: {
				return this->writeEtfFloat(jsonDataNew->jsonValue.numberDouble);
			}
			case JsonType::Uint64: {
				return this->writeEtfUint(jsonDataNew->jsonValue.numberUint);
			}
			case JsonType::Int64: {
				return this->writeEtfInt(jsonDataNew->jsonValue.numberInt);
			}
			case JsonType::Bool: {
				return this->writeEtfBool(jsonDataNew->jsonValue.boolean);
			}
			case JsonType::Null: {
				return this->writeEtfNull();
			}
		}
	}

	void Jsonifier::serializeJsonToJsonString(const Jsonifier* jsonDataNew) {
		switch (jsonDataNew->type) {
			case JsonType::Object: {
				return this->writeJsonObject(*jsonDataNew->jsonValue.object);
			}
			case JsonType::Array: {
				return this->writeJsonArray(*jsonDataNew->jsonValue.array);
			}
			case JsonType::String: {
				return this->writeJsonString(*jsonDataNew->jsonValue.string);
			}
			case JsonType::Float: {
				return this->writeJsonFloat(jsonDataNew->jsonValue.numberDouble);
			}
			case JsonType::Uint64: {
				return this->writeJsonInt(jsonDataNew->jsonValue.numberUint);
			}
			case JsonType::Int64: {
				return this->writeJsonInt(jsonDataNew->jsonValue.numberInt);
			}
			case JsonType::Bool: {
				return this->writeJsonBool(jsonDataNew->jsonValue.boolean);
			}
			case JsonType::Null: {
				return this->writeJsonNull();
			}
		}
	}

	void Jsonifier::writeJsonObject(const ObjectType& objectNew) {
		if (objectNew.empty()) {
			this->writeString("{}", 2);
			return;
		}
		this->writeCharacter('{');

		int32_t index{};
		for (auto& [key, value]: objectNew) {
			this->writeJsonString(key);
			this->writeCharacter(':');
			this->serializeJsonToJsonString(&value);

			if (index != objectNew.size() - 1) {
				this->writeCharacter(',');
			}
			index++;
		}

		this->writeCharacter('}');
	}

	void Jsonifier::writeJsonArray(const ArrayType& arrayNew) {
		if (arrayNew.empty()) {
			this->writeString("[]", 2);
			return;
		}

		this->writeCharacter('[');

		int32_t index{};
		for (auto& value: arrayNew) {
			this->serializeJsonToJsonString(&value);
			if (index != arrayNew.size() - 1) {
				this->writeCharacter(',');
			}
			index++;
		}

		this->writeCharacter(']');
	}

	void Jsonifier::writeJsonString(const StringType& stringNew) {
		this->writeCharacter('\"');
		for (auto& value: stringNew) {
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

	void Jsonifier::writeJsonFloat(const FloatType x) {
		auto floatValue = std::to_string(x);
		this->writeString(floatValue.data(), floatValue.size());
	}

	void Jsonifier::writeJsonBool(const BoolType jsonValueNew) {
		if (jsonValueNew) {
			this->writeString("true", 4);
		} else {
			this->writeString("false", 5);
		}
	}

	void Jsonifier::writeJsonNull() {
		this->writeString("null", 4);
	}

	void Jsonifier::writeEtfObject(const ObjectType& jsonData) {
		this->appendMapHeader(static_cast<uint32_t>(jsonData.size()));
		for (auto& [key, value]: jsonData) {
			this->appendBinaryExt(key, static_cast<uint32_t>(key.size()));
			this->serializeJsonToEtfString(&value);
		}
	}

	void Jsonifier::writeEtfArray(const ArrayType& jsonData) {
		this->appendListHeader(static_cast<uint32_t>(jsonData.size()));
		for (auto& value: jsonData) {
			this->serializeJsonToEtfString(&value);
		}
		this->appendNilExt();
	}

	void Jsonifier::writeEtfString(const StringType& jsonData) {
		this->appendBinaryExt(jsonData, static_cast<uint32_t>(jsonData.size()));
	}

	void Jsonifier::writeEtfUint(const UintType jsonData) {
		if (jsonData <= 255) {
			this->appendSmallIntegerExt(static_cast<int8_t>(jsonData));
		} else if (jsonData <= std::numeric_limits<uint32_t>::max()) {
			this->appendIntegerExt(static_cast<uint32_t>(jsonData));
		} else {
			this->appendUnsignedLongLong(jsonData);
		}
	}

	void Jsonifier::writeEtfInt(const IntType jsonData) {
		if (jsonData <= 127 && jsonData >= -127) {
			this->appendSmallIntegerExt(static_cast<int8_t>(jsonData));
		} else if (jsonData <= std::numeric_limits<int32_t>::max() && jsonData >= std::numeric_limits<int32_t>::min()) {
			this->appendIntegerExt(static_cast<uint32_t>(jsonData));
		} else {
			this->appendUnsignedLongLong(static_cast<uint64_t>(jsonData));
		}
	}

	void Jsonifier::writeEtfFloat(const FloatType jsonData) {
		this->appendNewFloatExt(jsonData);
	}

	void Jsonifier::writeEtfBool(const BoolType jsonData) {
		this->appendBool(jsonData);
	}

	void Jsonifier::writeEtfNull() {
		this->appendNil();
	}

	void Jsonifier::writeString(const char* data, std::size_t length) {
		this->string.append(data, length);
	}

	void Jsonifier::writeCharacter(const char charValue) {
		this->string.push_back(charValue);
	}

	void Jsonifier::appendBinaryExt(std::string_view bytes, uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<int8_t>(EtfType::Binary_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		this->writeString(newBuffer, std::size(newBuffer));
		this->writeString(bytes.data(), bytes.size());
	}

	void Jsonifier::appendUnsignedLongLong(uint64_t value) {
		char newBuffer[11]{ static_cast<int8_t>(EtfType::Small_Big_Ext) };
		char encodedBytes{};
		while (value > 0) {
			newBuffer[3 + encodedBytes] = value & 0xFF;
			value >>= 8;
			encodedBytes++;
		}
		newBuffer[1] = encodedBytes;
		newBuffer[2] = 0;
		this->writeString(newBuffer, 1ull + 2ull + static_cast<size_t>(encodedBytes));
	}

	void Jsonifier::appendNewFloatExt(const double FloatValue) {
		char newBuffer[9]{ static_cast<unsigned char>(EtfType::New_Float_Ext) };
		const void* punner{ &FloatValue };
		storeBits(newBuffer + 1, *static_cast<const uint64_t*>(punner));
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::appendSmallIntegerExt(const uint8_t value) {
		char newBuffer[2]{ static_cast<int8_t>(EtfType::Small_Integer_Ext), static_cast<char>(value) };
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::appendIntegerExt(const uint32_t value) {
		char newBuffer[5]{ static_cast<int8_t>(EtfType::Integer_Ext) };
		storeBits(newBuffer + 1, value);
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::appendListHeader(const uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<int8_t>(EtfType::List_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::appendMapHeader(const uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<int8_t>(EtfType::Map_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::appendBool(bool data) {
		if (data) {
			char newBuffer[6]{ static_cast<int8_t>(EtfType::Small_Atom_Ext), static_cast<int8_t>(4), 't', 'r', 'u', 'e' };
			this->writeString(newBuffer, std::size(newBuffer));

		} else {
			char newBuffer[7]{ static_cast<int8_t>(EtfType::Small_Atom_Ext), static_cast<int8_t>(5), 'f', 'a', 'l', 's', 'e' };
			this->writeString(newBuffer, std::size(newBuffer));
		}
	}

	void Jsonifier::appendVersion() {
		this->writeCharacter(static_cast<int8_t>(formatVersion));
	}

	void Jsonifier::appendNilExt() {
		this->writeCharacter(static_cast<int8_t>(EtfType::Nil_Ext));
	}

	void Jsonifier::appendNil() {
		char newBuffer[5]{ static_cast<int8_t>(EtfType::Small_Atom_Ext), static_cast<int8_t>(3), 'n', 'i', 'l' };
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void Jsonifier::setValue(JsonType typeNew) {
		this->destroy();
		this->type = typeNew;
		switch (this->type) {
			case JsonType::Object: {
				AllocatorType<ObjectType> allocator{};
				this->jsonValue.object = AllocatorTraits<ObjectType>::allocate(allocator, 1);
				AllocatorTraits<ObjectType>::construct(allocator, this->jsonValue.object);
				break;
			}
			case JsonType::Array: {
				AllocatorType<ArrayType> allocator{};
				this->jsonValue.array = AllocatorTraits<ArrayType>::allocate(allocator, 1);
				AllocatorTraits<ArrayType>::construct(allocator, this->jsonValue.array);
				break;
			}
			case JsonType::String: {
				AllocatorType<StringType> allocator{};
				this->jsonValue.string = AllocatorTraits<StringType>::allocate(allocator, 1);
				AllocatorTraits<StringType>::construct(allocator, this->jsonValue.string);
				break;
			}
		}
	}

	void Jsonifier::destroy() noexcept {
		switch (this->type) {
			case JsonType::Object: {
				AllocatorType<ObjectType> allocator{};
				AllocatorTraits<ObjectType>::destroy(allocator, this->jsonValue.object);
				AllocatorTraits<ObjectType>::deallocate(allocator, this->jsonValue.object, 1);
				break;
			}
			case JsonType::Array: {
				AllocatorType<ArrayType> allocator{};
				AllocatorTraits<ArrayType>::destroy(allocator, this->jsonValue.array);
				AllocatorTraits<ArrayType>::deallocate(allocator, this->jsonValue.array, 1);
				break;
			}
			case JsonType::String: {
				AllocatorType<StringType> allocator{};
				AllocatorTraits<StringType>::destroy(allocator, this->jsonValue.string);
				AllocatorTraits<StringType>::deallocate(allocator, this->jsonValue.string, 1);
				break;
			}
		}
	}

	Jsonifier::~Jsonifier() noexcept {
		this->destroy();
	}

	JsonSerializer& JsonSerializer::operator=(JsonSerializer&& data) noexcept {
		switch (data.type) {
			case JsonType::Object: {
				this->setValue(JsonType::Object);
				*this->jsonValue.object = std::move(*data.jsonValue.object);
				break;
			}
			case JsonType::Array: {
				this->setValue(JsonType::Array);
				*this->jsonValue.array = std::move(*data.jsonValue.array);
				break;
			}
			case JsonType::String: {
				this->setValue(JsonType::String);
				*this->jsonValue.string = std::move(*data.jsonValue.string);
				break;
			}
			case JsonType::Float: {
				this->jsonValue.numberDouble = data.jsonValue.numberDouble;
				break;
			}
			case JsonType::Uint64: {
				this->jsonValue.numberUint = data.jsonValue.numberUint;
				break;
			}
			case JsonType::Int64: {
				this->jsonValue.numberInt = data.jsonValue.numberInt;
				break;
			}
			case JsonType::Bool: {
				this->jsonValue.boolean = data.jsonValue.boolean;
				break;
			}
		}
		this->string = std::move(data.string);
		this->type = data.type;
		return *this;
	}

	JsonSerializer& JsonSerializer::operator=(Jsonifier&& data) noexcept {
		switch (data.type) {
			case JsonType::Object: {
				this->setValue(JsonType::Object);
				*this->jsonValue.object = std::move(*data.jsonValue.object);
				//std::cout << "THE OBJECT DATA: ";
				for (auto& [key, value]: *this->jsonValue.object) {
					//std::cout << "THE KEY: " << key << std::endl;
					if (value.type == JsonType::Object) {
						for (auto& [key01, value01]: *value.jsonValue.object) {
							//std::cout << "THE KEY 01: " << key << std::endl;
							for (auto& [key02, value02]: *value01.jsonValue.object) {
								//std::cout << "THE KEY 02: " << key << std::endl;
							}
						}
					}
				}
				break;
			}
			case JsonType::Array: {
				this->setValue(JsonType::Array);
				*this->jsonValue.array = std::move(*data.jsonValue.array);
				break;
			}
			case JsonType::String: {
				this->setValue(JsonType::String);
				*this->jsonValue.string = std::move(*data.jsonValue.string);
				break;
			}
			case JsonType::Float: {
				this->jsonValue.numberDouble = data.jsonValue.numberDouble;
				break;
			}
			case JsonType::Uint64: {
				this->jsonValue.numberUint = data.jsonValue.numberUint;
				break;
			}
			case JsonType::Int64: {
				this->jsonValue.numberInt = data.jsonValue.numberInt;
				break;
			}
			case JsonType::Bool: {
				this->jsonValue.boolean = data.jsonValue.boolean;
				break;
			}
		}
		this->type = data.type;
		return *this;
	}

	JsonSerializer::JsonSerializer(Jsonifier&& data) noexcept {
		*this = std::move(data);
	}

	JsonSerializer& JsonSerializer::operator=(const Jsonifier& data) noexcept {
		switch (data.type) {
			case JsonType::Object: {
				this->setValue(JsonType::Object);
				*this->jsonValue.object = *data.jsonValue.object;
				//std::cout << "THE OBJECT DATA: ";
				for (auto& [key, value]: *this->jsonValue.object) {
					//std::cout << "THE KEY: " << key << std::endl;
				}
				break;
			}
			case JsonType::Array: {
				this->setValue(JsonType::Array);
				*this->jsonValue.array = *data.jsonValue.array;
				break;
			}
			case JsonType::String: {
				this->setValue(JsonType::String);
				*this->jsonValue.string = *data.jsonValue.string;
				break;
			}
			case JsonType::Float: {
				this->jsonValue.numberDouble = data.jsonValue.numberDouble;
				break;
			}
			case JsonType::Uint64: {
				this->jsonValue.numberUint = data.jsonValue.numberUint;
				break;
			}
			case JsonType::Int64: {
				this->jsonValue.numberInt = data.jsonValue.numberInt;
				break;
			}
			case JsonType::Bool: {
				this->jsonValue.boolean = data.jsonValue.boolean;
				break;
			}
		}
		this->type = data.type;
		return *this;
	}

	JsonSerializer::JsonSerializer(const Jsonifier& data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(const JsonSerializer& data) noexcept {
		switch (data.type) {
			case JsonType::Object: {
				this->setValue(JsonType::Object);
				*this->jsonValue.object = *data.jsonValue.object;
				//std::cout << "THE OBJECT DATA: ";
				for (auto& [key, value]: *this->jsonValue.object) {
					//std::cout << "THE KEY: " << key << std::endl;
				}
				break;
			}
			case JsonType::Array: {
				this->setValue(JsonType::Array);
				*this->jsonValue.array = *data.jsonValue.array;
				break;
			}
			case JsonType::String: {
				this->setValue(JsonType::String);
				*this->jsonValue.string = *data.jsonValue.string;
				break;
			}
			case JsonType::Float: {
				this->jsonValue.numberDouble = data.jsonValue.numberDouble;
				break;
			}
			case JsonType::Uint64: {
				this->jsonValue.numberUint = data.jsonValue.numberUint;
				break;
			}
			case JsonType::Int64: {
				this->jsonValue.numberInt = data.jsonValue.numberInt;
				break;
			}
			case JsonType::Bool: {
				this->jsonValue.boolean = data.jsonValue.boolean;
				break;
			}
		}
		this->string = data.string;
		this->type = data.type;
		return *this;
	}

	JsonSerializer::JsonSerializer(const JsonSerializer& data) noexcept {
		*this = data;
	}

	JsonType JsonSerializer::getType() noexcept {
		return this->type;
	}

	JsonSerializer& JsonSerializer::operator=(ErrorCode data) {
		throw std::runtime_error{ std::string{ "Error Parsing Json Data - it is: " } + std::to_string(static_cast<int32_t>(data)) };
		return *this;
	}

	JsonSerializer::JsonSerializer(ErrorCode data) {
		*this = std::move(data);
	}

	JsonSerializer::operator std::string_view() noexcept {
		return static_cast<std::string_view>(this->string);
	}

	JsonSerializer& JsonSerializer::operator=(ObjectType&& data) noexcept {
		this->setValue(JsonType::Object);
		for (auto& [key, value]: data) {
			(*this->jsonValue.object)[key] = std::move(value);
		}
		return *this;
	}

	JsonSerializer::JsonSerializer(ObjectType&& data) noexcept {
		*this = std::move(data);
	}

	JsonSerializer& JsonSerializer::operator=(const ObjectType& data) noexcept {
		this->setValue(JsonType::Object);
		for (auto& [key, value]: data) {
			(*this->jsonValue.object)[key] = value;
		}
		return *this;
	}

	JsonSerializer::JsonSerializer(const ObjectType& data) noexcept {
		*this = data;
	}

	JsonSerializer::JsonSerializer(JsonSerializer&& data) noexcept {
		*this = std::move(data);
	}

	JsonSerializer& JsonSerializer::operator=(std::string_view&& data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = std::move(data);
		this->type = JsonType::String;
		return *this;
	}

	JsonSerializer::JsonSerializer(std::string_view&& data) noexcept {
		*this = std::move(data);
	}

	JsonSerializer& JsonSerializer::operator=(std::string_view& data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = data;
		this->type = JsonType::String;
		return *this;
	}

	JsonSerializer::JsonSerializer(std::string_view& data) noexcept {
		*this = data;
	}

	void JsonSerializer::refreshString(JsonifierSerializeType opCode) {
		this->string.clear();
		if (opCode == JsonifierSerializeType ::Etf) {
			this->appendVersion();
			this->serializeJsonToEtfString(this);
		} else {
			this->serializeJsonToJsonString(this);
		}
	}

	JsonSerializer& JsonSerializer::operator=(EnumConverter&& data) noexcept {
		if (data.isItAVector()) {
			this->setValue(JsonType::Array);
			for (auto& value: data.operator std::vector<uint64_t>()) {
				this->jsonValue.array->emplace_back(std::move(value));
			}
		} else {
			this->jsonValue.numberUint = uint64_t{ data };
			this->type = JsonType::Uint64;
		}
		return *this;
	}

	JsonSerializer::JsonSerializer(EnumConverter&& data) noexcept {
		*this = std::move(data);
	}

	JsonSerializer& JsonSerializer::operator=(const EnumConverter& data) noexcept {
		if (data.isItAVector()) {
			this->setValue(JsonType::Array);
			for (auto& value: data.operator std::vector<uint64_t>()) {
				this->jsonValue.array->emplace_back(value);
			}
		} else {
			this->jsonValue.numberUint = uint64_t{ data };
			this->type = JsonType::Uint64;
		}
		return *this;
	}

	JsonSerializer::JsonSerializer(const EnumConverter& data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(std::string&& data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = std::move(data);
		this->type = JsonType::String;
		return *this;
	}

	JsonSerializer::JsonSerializer(std::string&& data) noexcept {
		*this = std::move(data);
	}

	JsonSerializer& JsonSerializer::operator=(const std::string& data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = data;
		this->type = JsonType::String;
		return *this;
	}

	JsonSerializer::JsonSerializer(const std::string& data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(const char* data) noexcept {
		this->setValue(JsonType::String);
		*this->jsonValue.string = data;
		this->type = JsonType::String;
		return *this;
	}

	JsonSerializer::JsonSerializer(const char* data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(double data) noexcept {
		this->jsonValue.numberDouble = data;
		this->type = JsonType::Float;
		return *this;
	}

	JsonSerializer::JsonSerializer(double data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(float data) noexcept {
		this->jsonValue.numberDouble = data;
		this->type = JsonType::Float;
		return *this;
	}

	JsonSerializer::JsonSerializer(float data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(uint64_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	JsonSerializer::JsonSerializer(uint64_t data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(uint32_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	JsonSerializer::JsonSerializer(uint32_t data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(uint16_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	JsonSerializer::JsonSerializer(uint16_t data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(uint8_t data) noexcept {
		this->jsonValue.numberUint = data;
		this->type = JsonType::Uint64;
		return *this;
	}

	JsonSerializer::JsonSerializer(uint8_t data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(int64_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	JsonSerializer::JsonSerializer(int64_t data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(int32_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	JsonSerializer::JsonSerializer(int32_t data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(int16_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	JsonSerializer::JsonSerializer(int16_t data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(int8_t data) noexcept {
		this->jsonValue.numberInt = data;
		this->type = JsonType::Int64;
		return *this;
	}

	JsonSerializer::JsonSerializer(int8_t data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(std::nullptr_t) noexcept {
		this->type = JsonType::Null;
		return *this;
	}

	JsonSerializer::JsonSerializer(std::nullptr_t data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(bool data) noexcept {
		this->jsonValue.boolean = data;
		this->type = JsonType::Bool;
		return *this;
	}

	JsonSerializer::JsonSerializer(bool data) noexcept {
		*this = data;
	}

	JsonSerializer& JsonSerializer::operator=(JsonType typeNew) noexcept {
		this->type = typeNew;
		this->setValue(this->type);
		return *this;
	}

	JsonSerializer::JsonSerializer(JsonType type) noexcept {
		*this = type;
	}

	JsonSerializer& JsonSerializer::operator[](typename ObjectType::key_type key) {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Object);
			this->type = JsonType::Object;
		}

		if (this->type == JsonType::Object) {
			auto result = this->jsonValue.object->emplace(std::move(key), JsonSerializer{});
			return *static_cast<JsonSerializer*>(&result.first->second);
		}
		throw DCAException{ "Sorry, but the item-key: " + key + ", could not be produced / accessed." };
	}

	JsonSerializer& JsonSerializer::operator[](uint64_t index) {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Array);
			this->type = JsonType::Array;
		}

		if (this->type == JsonType::Array) {
			if (index >= this->jsonValue.array->size()) {
				this->jsonValue.array->resize(index + 1);
			}

			return *static_cast<JsonSerializer*>(&this->jsonValue.array->operator[](index));
		}
		throw DCAException{ "Sorry, but that index could not be produced/accessed." };
	}

	JsonSerializer& JsonSerializer::emplaceBack(JsonSerializer&& other) noexcept {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Array);
			this->type = JsonType::Array;
		}

		if (this->type == JsonType::Array) {
			this->jsonValue.array->emplace_back(std::move(other));
		}
		return *this;
	}

	JsonSerializer& JsonSerializer::emplaceBack(JsonSerializer& other) noexcept {
		if (this->type == JsonType::Null) {
			this->setValue(JsonType::Array);
			this->type = JsonType::Array;
		}

		if (this->type == JsonType::Array) {
			this->jsonValue.array->emplace_back(other);
		}
		return *this;
	}

	void JsonSerializer::serializeJsonToEtfString(const Jsonifier* dataToParse) {
		switch (dataToParse->type) {
			case JsonType::Object: {
				return this->writeEtfObject(*dataToParse->jsonValue.object);
			}
			case JsonType::Array: {
				return this->writeEtfArray(*dataToParse->jsonValue.array);
			}
			case JsonType::String: {
				return this->writeEtfString(*dataToParse->jsonValue.string);
			}
			case JsonType::Float: {
				return this->writeEtfFloat(dataToParse->jsonValue.numberDouble);
			}
			case JsonType::Uint64: {
				return this->writeEtfUint(dataToParse->jsonValue.numberUint);
			}
			case JsonType::Int64: {
				return this->writeEtfInt(dataToParse->jsonValue.numberInt);
			}
			case JsonType::Bool: {
				return this->writeEtfBool(dataToParse->jsonValue.boolean);
			}
			case JsonType::Null: {
				return this->writeEtfNull();
			}
		}
	}

	void JsonSerializer::serializeJsonToJsonString(const Jsonifier* dataToParse) {
		switch (dataToParse->type) {
			case JsonType::Object: {
				return this->writeJsonObject(*dataToParse->jsonValue.object);
			}
			case JsonType::Array: {
				return this->writeJsonArray(*dataToParse->jsonValue.array);
			}
			case JsonType::String: {
				return this->writeJsonString(*dataToParse->jsonValue.string);
			}
			case JsonType::Float: {
				return this->writeJsonFloat(dataToParse->jsonValue.numberDouble);
			}
			case JsonType::Uint64: {
				return this->writeJsonInt(dataToParse->jsonValue.numberUint);
			}
			case JsonType::Int64: {
				return this->writeJsonInt(dataToParse->jsonValue.numberInt);
			}
			case JsonType::Bool: {
				return this->writeJsonBool(dataToParse->jsonValue.boolean);
			}
			case JsonType::Null: {
				return this->writeJsonNull();
			}
		}
	}

	void JsonSerializer::writeJsonObject(const ObjectType& objectNew) {
		if (objectNew.empty()) {
			this->writeString("{}", 2);
			return;
		}
		this->writeCharacter('{');

		int32_t index{};
		for (auto& [key, value]: objectNew) {
			this->writeJsonString(key);
			this->writeCharacter(':');
			this->serializeJsonToJsonString(&value);

			if (index != objectNew.size() - 1) {
				this->writeCharacter(',');
			}
			++index;
		}

		this->writeCharacter('}');
	}

	void JsonSerializer::writeJsonArray(const ArrayType& arrayNew) {
		if (arrayNew.empty()) {
			this->writeString("[]", 2);
			return;
		}

		this->writeCharacter('[');

		int32_t index{};
		for (auto& value: arrayNew) {
			this->serializeJsonToJsonString(&value);
			if (index != arrayNew.size() - 1) {
				this->writeCharacter(',');
			}
			++index;
		}

		this->writeCharacter(']');
	}

	void JsonSerializer::writeJsonString(const StringType& stringNew) {
		this->writeCharacter('"');
		this->writeString(stringNew.data(), stringNew.size());
		this->writeCharacter('"');
	}

	void JsonSerializer::writeJsonFloat(const FloatType x) {
		auto floatValue = std::to_string(x);
		this->writeString(floatValue.data(), floatValue.size());
	}

	void JsonSerializer::writeJsonBool(const BoolType jsonValueNew) {
		if (jsonValueNew) {
			this->writeString("true", 4);
		} else {
			this->writeString("false", 5);
		}
	}

	void JsonSerializer::writeJsonNull() {
		this->writeString("null", 4);
	}

	void JsonSerializer::writeEtfObject(const ObjectType& jsonData) {
		this->appendMapHeader(static_cast<uint32_t>(jsonData.size()));
		for (auto& [key, value]: jsonData) {
			this->appendBinaryExt(key, static_cast<uint32_t>(key.size()));
			this->serializeJsonToEtfString(&value);
		}
	}

	void JsonSerializer::writeEtfArray(const ArrayType& jsonData) {
		this->appendListHeader(static_cast<uint32_t>(jsonData.size()));
		for (auto& value: jsonData) {
			this->serializeJsonToEtfString(&value);
		}
		this->appendNilExt();
	}

	void JsonSerializer::writeEtfString(const StringType& jsonData) {
		this->appendBinaryExt(jsonData, static_cast<uint32_t>(jsonData.size()));
	}

	void JsonSerializer::writeEtfUint(const UintType jsonData) {
		if (jsonData <= 255) {
			this->appendSmallIntegerExt(static_cast<uint8_t>(jsonData));
		} else if (jsonData <= std::numeric_limits<uint32_t>::max()) {
			this->appendIntegerExt(static_cast<uint32_t>(jsonData));
		} else {
			this->appendUnsignedLongLong(jsonData);
		}
	}

	void JsonSerializer::writeEtfInt(const IntType jsonData) {
		if (jsonData <= 127 && jsonData >= -127) {
			this->appendSmallIntegerExt(static_cast<uint8_t>(jsonData));
		} else if (jsonData <= std::numeric_limits<int32_t>::max() && jsonData >= std::numeric_limits<int32_t>::min()) {
			this->appendIntegerExt(static_cast<uint32_t>(jsonData));
		} else {
			this->appendUnsignedLongLong(static_cast<uint64_t>(jsonData));
		}
	}

	void JsonSerializer::writeEtfFloat(const FloatType jsonData) {
		this->appendNewFloatExt(jsonData);
	}

	void JsonSerializer::writeEtfBool(const BoolType jsonData) {
		this->appendBool(jsonData);
	}

	void JsonSerializer::writeEtfNull() {
		this->appendNil();
	}

	void JsonSerializer::writeString(const char* data, size_t length) {
		this->string.append(data, length);
	}

	void JsonSerializer::writeCharacter(const char charValue) {
		this->string.push_back(charValue);
	}

	bool operator==(const JsonSerializer& lhs, const JsonSerializer& rhs) {
		if (lhs.type != rhs.type) {
			return false;
		}
		switch (rhs.type) {
			case JsonType::Object: {
				if (*lhs.jsonValue.object != *rhs.jsonValue.object) {
					return false;
				}
				break;
			}
			case JsonType::Array: {
				if (*lhs.jsonValue.array != *rhs.jsonValue.array) {
					return false;
				}
				break;
			}
			case JsonType::String: {
				if (*lhs.jsonValue.string != *rhs.jsonValue.string) {
					return false;
				}
				break;
			}
			case JsonType::Float: {
				if (lhs.jsonValue.numberDouble != rhs.jsonValue.numberDouble) {
					return false;
				}
				break;
			}
			case JsonType::Uint64: {
				if (lhs.jsonValue.numberUint != rhs.jsonValue.numberUint) {
					return false;
				}
				break;
			}
			case JsonType::Int64: {
				if (lhs.jsonValue.numberInt != rhs.jsonValue.numberInt) {
					return false;
				}
				break;
			}
			case JsonType::Bool: {
				if (lhs.jsonValue.boolean != rhs.jsonValue.boolean) {
					return false;
				}
				break;
			}
		}
		return true;
	}

	void JsonSerializer::appendBinaryExt(std::string_view bytes, uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<int8_t>(EtfType::Binary_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		this->writeString(newBuffer, std::size(newBuffer));
		this->writeString(bytes.data(), bytes.size());
	}

	void JsonSerializer::appendUnsignedLongLong(uint64_t value) {
		char newBuffer[11]{ static_cast<int8_t>(EtfType::Small_Big_Ext) };
		char encodedBytes{};
		while (value > 0) {
			newBuffer[3 + encodedBytes] = value & 0xFF;
			value >>= 8;
			++encodedBytes;
		}
		newBuffer[1] = encodedBytes;
		newBuffer[2] = 0;
		this->writeString(newBuffer, 1ull + 2ull + static_cast<size_t>(encodedBytes));
	}

	void JsonSerializer::appendNewFloatExt(const double FloatValue) {
		char newBuffer[9]{ static_cast<uint8_t>(EtfType::New_Float_Ext) };
		const void* punner{ &FloatValue };
		storeBits(newBuffer + 1, *static_cast<const uint64_t*>(punner));
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void JsonSerializer::appendSmallIntegerExt(const uint8_t value) {
		char newBuffer[2]{ static_cast<uint8_t>(EtfType::Small_Integer_Ext), static_cast<char>(value) };
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void JsonSerializer::appendIntegerExt(const uint32_t value) {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::Integer_Ext) };
		storeBits(newBuffer + 1, value);
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void JsonSerializer::appendListHeader(const uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::List_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void JsonSerializer::appendMapHeader(const uint32_t sizeNew) {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::Map_Ext) };
		storeBits(newBuffer + 1, sizeNew);
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void JsonSerializer::appendBool(bool data) {
		if (data) {
			char newBuffer[6]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(4), 't', 'r', 'u', 'e' };
			this->writeString(newBuffer, std::size(newBuffer));

		} else {
			char newBuffer[7]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(5), 'f', 'a', 'l', 's', 'e' };
			this->writeString(newBuffer, std::size(newBuffer));
		}
	}

	void JsonSerializer::appendVersion() {
		char newBuffer[1]{ static_cast<int8_t>(formatVersion) };
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void JsonSerializer::appendNilExt() {
		this->writeCharacter(static_cast<uint8_t>(EtfType::Nil_Ext));
	}

	void JsonSerializer::appendNil() {
		char newBuffer[5]{ static_cast<uint8_t>(EtfType::Small_Atom_Ext), static_cast<uint8_t>(3), 'n', 'i', 'l' };
		this->writeString(newBuffer, std::size(newBuffer));
	}

	void JsonSerializer::setValue(JsonType typeNew) {
		this->destroy();
		this->type = typeNew;
		switch (this->type) {
			case JsonType::Object: {
				AllocatorType<ObjectType> allocator{};
				this->jsonValue.object = AllocatorTraits<ObjectType>::allocate(allocator, 1);
				AllocatorTraits<ObjectType>::construct(allocator, this->jsonValue.object);
				break;
			}
			case JsonType::Array: {
				AllocatorType<ArrayType> allocator{};
				this->jsonValue.array = AllocatorTraits<ArrayType>::allocate(allocator, 1);
				AllocatorTraits<ArrayType>::construct(allocator, this->jsonValue.array);
				break;
			}
			case JsonType::String: {
				AllocatorType<StringType> allocator{};
				this->jsonValue.string = AllocatorTraits<StringType>::allocate(allocator, 1);
				AllocatorTraits<StringType>::construct(allocator, this->jsonValue.string);
				break;
			}
		}
	}

	void JsonSerializer::destroy() noexcept {
		switch (this->type) {
			case JsonType::Object: {
				AllocatorType<ObjectType> allocator{};
				AllocatorTraits<ObjectType>::destroy(allocator, this->jsonValue.object);
				AllocatorTraits<ObjectType>::deallocate(allocator, this->jsonValue.object, 1);
				break;
			}
			case JsonType::Array: {
				AllocatorType<ArrayType> allocator{};
				AllocatorTraits<ArrayType>::destroy(allocator, this->jsonValue.array);
				AllocatorTraits<ArrayType>::deallocate(allocator, this->jsonValue.array, 1);
				break;
			}
			case JsonType::String: {
				AllocatorType<StringType> allocator{};
				AllocatorTraits<StringType>::destroy(allocator, this->jsonValue.string);
				AllocatorTraits<StringType>::deallocate(allocator, this->jsonValue.string, 1);
				break;
			}
		}
	}

	JsonSerializer::~JsonSerializer() noexcept {
		this->destroy();
	}
}