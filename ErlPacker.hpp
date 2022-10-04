/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// ErlPacker.hpp - Header for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file ErlPacker.hpp

#ifndef ERL_PACKER_02
#define ERL_PACKER_02

#include <discordcoreapi/FoundationEntities.hpp>
#include <nlohmann/json.hpp>
#include <simdjson.h>
#include <coroutine>
#include <stdint.h>
#include <set>

using AtomicUint64 = std::atomic_uint64_t;
using AtomicUint32 = std::atomic_uint32_t;
using AtomicInt64 = std::atomic_int64_t;
using AtomicInt32 = std::atomic_int32_t;
using AtomicBool = std::atomic_bool;
using StringStream = std::stringstream;
using StringView = std::string_view;
using String = std::string;
using Uint64 = uint64_t;
using Uint32 = uint32_t;
using Uint16 = uint16_t;
using Uint8 = uint8_t;
using Int64 = int64_t;
using Int32 = int32_t;
using Int16 = int16_t;
using Int8 = int8_t;
using Float = float;
using Double = double;
using Snowflake = Uint64;
using Bool = bool;

enum class ValueType : Int8 { Null = 0, Null_Ext = 1, Object = 2, Array = 3, Float = 4, String = 5, Bool = 6, Int64 = 7, Uint64 = 8, Unset = 9 };

template<typename TheType>
concept IsEnum = std::is_enum<TheType>::value;

template<typename TheType>
concept IsString = std::same_as<TheType, String>;

struct EnumConverter {
	template<IsEnum EnumType> EnumConverter(EnumType other) {
		this->thePtr = new Uint64{};
		*static_cast<Uint64*>(this->thePtr) = static_cast<Uint64>(other);
	};

	EnumConverter& operator=(EnumConverter&&) noexcept;

	EnumConverter(EnumConverter&&) noexcept;

	EnumConverter& operator=(EnumConverter&) noexcept = delete;

	EnumConverter(EnumConverter&) noexcept = delete;

	template<IsEnum EnumType> EnumConverter& operator=(std::vector<EnumType> other) {
		this->thePtr = new std::vector<Uint64>{};
		for (auto& value: other) {
			static_cast<std::vector<Uint64>*>(this->thePtr)->emplace_back(static_cast<Uint64>(value));
		}
		this->vectorType = true;
		return *this;
	};

	template<IsEnum EnumType> EnumConverter(std::vector<EnumType> other) {
		*this = other;
	};

	operator std::vector<Uint64>();

	explicit operator Uint64();

	~EnumConverter();

  protected:
	Bool vectorType{ false };
	void* thePtr{ nullptr };
};

class JsonObject {
  public:
	using ObjectType = std::map<String, JsonObject, std::less<>, std::allocator<std::pair<const String, JsonObject>>>;
	using ArrayType = std::vector<JsonObject>;
	using StringType = String;
	using UintType = Uint64;
	using FloatType = Double;
	using IntType = Int64;
	using BoolType = Bool;

	ValueType theType{ ValueType::Null };

	union JsonValue {
		FloatType numberDouble;
		UintType numberUint;
		ObjectType* object;
		StringType* string;
		IntType numberInt;
		ArrayType* array;
		BoolType boolean;

		JsonValue& operator=(JsonValue&&) noexcept = delete;

		JsonValue(JsonValue&&) noexcept = delete;

		JsonValue& operator=(const JsonValue&) noexcept = delete;

		JsonValue(const JsonValue&) noexcept = delete;

		JsonValue& operator=(const StringType& theData) noexcept;

		JsonValue& operator=(StringType&& theData) noexcept;

		JsonValue& operator=(const char* theData) noexcept;

		JsonValue& operator=(Uint64 theData) noexcept;

		JsonValue& operator=(Uint32 theData) noexcept;

		JsonValue& operator=(Uint16 theData) noexcept;

		JsonValue& operator=(Uint8 theData) noexcept;

		JsonValue& operator=(Int64 theData) noexcept;

		JsonValue& operator=(Int32 theData) noexcept;

		JsonValue& operator=(Int16 theData) noexcept;

		JsonValue& operator=(Int8 theData) noexcept;

		JsonValue& operator=(Double theData) noexcept;

		JsonValue& operator=(Float theData) noexcept;

		JsonValue& operator=(Bool theData) noexcept;

		JsonValue& operator=(ValueType t) noexcept;
		JsonValue(ValueType t) noexcept;

		void destroy(ValueType theType);
	};

	JsonValue theValue{ ValueType::Null };

	JsonObject() noexcept = default;

	template<typename ObjectType> JsonObject& operator=(std::vector<ObjectType> theData) noexcept {
		this->theType = ValueType::Array;
		Int32 theIndex{};
		for (auto& value: theData) {
			this->theValue = ValueType::Array;
			this->theValue.array->push_back(JsonObject{ value });
			theIndex++;
		}
		return *this;
	}

	template<typename ObjectType> JsonObject(std::vector<ObjectType> theData) noexcept {
		*this = theData;
	}

	template<IsString KeyType, IsString ObjectType> JsonObject& operator=(std::unordered_map<KeyType, ObjectType> theData) noexcept {
		Int32 theIndex{};
		this->theType = ValueType::Array;

		for (auto& [key, value]: theData) {
			this->theValue = ValueType::Object;
			this->theValue.object->at(key) = JsonObject{ value };
		}
		theIndex++;
		return *this;
	}

	template<IsString KeyType, IsString ObjectType> JsonObject(std::unordered_map<KeyType, ObjectType> theData) noexcept {
		*this = theData;
	};

	JsonObject& operator=(EnumConverter theData) noexcept;
	JsonObject(EnumConverter) noexcept;

	JsonObject& operator=(const JsonObject& theKey) noexcept;
	JsonObject(const JsonObject& theKey) noexcept;

	JsonObject& operator=(JsonObject&& theKey) noexcept;
	JsonObject(JsonObject&& theKey) noexcept;

	JsonObject& operator=(const StringType& theData) noexcept;
	JsonObject(const StringType&) noexcept;

	JsonObject& operator=(StringType&& theData) noexcept;
	JsonObject(StringType&&) noexcept;

	JsonObject& operator=(const char* theData) noexcept;
	JsonObject(const char* theData) noexcept;

	JsonObject& operator=(Uint64 theData) noexcept;
	JsonObject(Uint64) noexcept;

	JsonObject& operator=(Uint32 theData) noexcept;
	JsonObject(Uint32) noexcept;

	JsonObject& operator=(Uint16 theData) noexcept;
	JsonObject(Uint16) noexcept;

	JsonObject& operator=(Uint8 theData) noexcept;
	JsonObject(Uint8) noexcept;

	JsonObject& operator=(Int64 theData) noexcept;
	JsonObject(Int64) noexcept;

	JsonObject& operator=(Int32 theData) noexcept;
	JsonObject(Int32) noexcept;

	JsonObject& operator=(Int16 theData) noexcept;
	JsonObject(Int16) noexcept;

	JsonObject& operator=(Int8 theData) noexcept;
	JsonObject(Int8) noexcept;

	JsonObject& operator=(Double theData) noexcept;
	JsonObject(Double) noexcept;

	JsonObject& operator=(Float theData) noexcept;
	JsonObject(Float) noexcept;

	JsonObject& operator=(Bool theData) noexcept;
	JsonObject(Bool) noexcept;

	JsonObject(const char*, ValueType) noexcept;

	JsonObject& operator=(ValueType) noexcept;

	JsonObject& operator[](Uint64 idx) const;
	JsonObject& operator[](Uint64 idx);

	JsonObject& operator[](const typename ObjectType::key_type& key) const;
	JsonObject& operator[](typename ObjectType::key_type key);

	explicit operator String() noexcept;

	explicit operator String() const noexcept;
	void pushBack(JsonObject& other) noexcept;
	void pushBack(JsonObject&& other) noexcept;

	~JsonObject() noexcept;
};

struct ErlPackError : public std::runtime_error {
  public:
	explicit ErlPackError(const String& message);
};

constexpr Uint8 formatVersion{ 131 };

enum class ETFTokenType : Uint8 {
	New_Float_Ext = 70,
	Small_Integer_Ext = 97,
	Integer_Ext = 98,
	Float_Ext = 99,
	Atom_Ext = 100,
	Small_Tuple_Ext = 104,
	Large_Tuple_Ext = 105,
	Nil_Ext = 106,
	String_Ext = 107,
	List_Ext = 108,
	Binary_Ext = 109,
	Small_Big_Ext = 110,
	Large_Big_Ext = 111,
	Small_Atom_Ext = 115,
	Map_Ext = 116,
	Atom_Utf8_Ext = 118
};

class ErlPacker {
  public:
	ErlPacker() noexcept {};

	String& parseJsonToEtf(JsonObject&& dataToParse);

	String& parseEtfToJson(StringView dataToParse);

	~ErlPacker(){};

  protected:
	String comparisongStringFalse{ "false" };
	String comparisongStringNil{ "nil" };
	String falseString{ "false" };
	String nilString{ "nil" };
	String bufferString{};
	StringView buffer{};
	Uint64 offSet{};
	Uint64 size{};

	void singleValueJsonToETF(JsonObject dataToParse);

	void writeObject(JsonObject::ObjectType jsonData);

	void writeString(JsonObject::StringType jsonData);

	void writeInt(JsonObject::IntType jsonData);

	void writeUint(JsonObject::UintType jsonData);

	void writeFloat(JsonObject::FloatType jsonData);

	void writeArray(JsonObject::ArrayType jsonData);

	void writeBool(JsonObject ::BoolType jsonData);

	void writeToBuffer(const String&);

	void appendBinaryExt(const String&, Uint32);

	void appendUnsignedLongLong(Uint64);

	void appendSmallIntegerExt(Uint8);

	void appendIntegerExt(Uint32);

	void appendListHeader(Uint32);

	void appendMapHeader(Uint32);

	void appendNewFloatExt(Double);

	void appendVersion();

	void appendNilExt();

	void appendFalse();

	void appendTrue();

	void appendNil();

	template<typename ReturnType> ReturnType readBits() {
		if (this->offSet + sizeof(ReturnType) > this->size) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of the buffer.\n\n" };
		}
		const ReturnType newValue = *reinterpret_cast<const ReturnType*>(this->buffer.data() + this->offSet);
		this->offSet += sizeof(ReturnType);
		return DiscordCoreAPI::reverseByteOrder<const ReturnType>(newValue);
	}

	Uint64 readString(Uint32 length);

	String processAtom(const char* atom, Uint32 length);

	String singleValueETFToJson();

	String parseSmallIntegerExt();

	String parseBigint(Uint32);

	String parseArray(Uint32);

	String parseTuple(Uint32);

	String parseSmallTupleExt();

	String parseLargeTupleExt();

	String parseSmallAtomExt();

	String parseStringAsList();

	String parseNewFloatExt();

	String parseSmallBigExt();

	String parseLargeBigExt();

	String parseAtomUtf8Ext();

	String parseIntegerExt();

	String parseBinaryExt();

	String parseFloatExt();

	String parseListExt();

	String parseNilExt();

	String parseMapExt();
};

#endif // !ERL_PACKER