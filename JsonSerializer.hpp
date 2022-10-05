/*
	, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

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
/// JsonSerializer.hpp - Header for the json-serializer class.
/// Oct 10, 2022
/// https://discordcoreapi.com
/// \file JsonSerializer.hpp

#ifndef JSON_SERIALIZER
#define JSON_SERIALIZER

#include "JsonObject.hpp"

#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include <coroutine>
#include <stdint.h>
#include <set>

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


class JsonSerializer {
  public:

  using ObjectType = std::map<String, JsonSerializer, std::less<>, std::allocator<std::pair<const String, JsonSerializer>>>;
	template<typename Type> using AllocatorType = std::allocator<Type>;
	using ArrayType = std::vector<JsonSerializer>;
	using StringType = String;
	using UintType = Uint64;
	using FloatType = Double;
	using IntType = Int64;
	using BoolType = Bool;

	ValueType theType{ ValueType::Null };
	StringView theCurrentStringMemory{};
	String theString{};

	union JsonValue {
		std::unique_ptr<ObjectType> object;
		std::unique_ptr<StringType> string;
		std::unique_ptr<ArrayType> array;
		FloatType numberDouble;
		UintType numberUint;
		IntType numberInt;
		BoolType boolean;

		JsonValue() noexcept;

		JsonValue& operator=(JsonValue&&) noexcept = delete;

		JsonValue(JsonValue&&) noexcept = delete;

		JsonValue& operator=(const JsonValue&) noexcept = delete;

		JsonValue(const JsonValue&) noexcept = delete;

		JsonValue& operator=(const ArrayType& theData) noexcept;

		JsonValue& operator=(ArrayType&& theData) noexcept;

		JsonValue& operator=(const ObjectType& theData) noexcept;

		JsonValue& operator=(ObjectType&& theData) noexcept;

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

		~JsonValue() noexcept;
	};

	JsonValue theValue{};

	void convertToString(String& theString);

	JsonSerializer() noexcept = default;

	template<typename ObjectType> JsonSerializer& operator=(std::vector<ObjectType> theData) noexcept {
		this->set(std::make_unique<ArrayType>());
		for (auto& value: theData) {
			this->theValue.array->push_back(JsonSerializer{ value });
		}
		return *this;
	}

	template<typename ObjectType> JsonSerializer(std::vector<ObjectType> theData) noexcept {
		*this = theData;
	}

	template<IsString KeyType, IsString ObjectType> JsonSerializer& operator=(std::unordered_map<KeyType, ObjectType> theData) noexcept {
		this->set(std::make_unique<ObjectType>());
		for (auto& [key, value]: theData) {
			this->theValue.object->at(key) = JsonSerializer{ value };
		}
		return *this;
	}

	template<IsString KeyType, IsString ObjectType> JsonSerializer(std::unordered_map<KeyType, ObjectType> theData) noexcept {
		*this = theData;
	};

	JsonSerializer& operator=(JsonSerializer&& theKey) noexcept;
	JsonSerializer(JsonSerializer&& theKey) noexcept;

	JsonSerializer& operator=(const JsonSerializer& theKey) noexcept;
	JsonSerializer(const JsonSerializer& theKey) noexcept;

	JsonSerializer& operator=(StringType&& theData) noexcept;
	JsonSerializer(StringType&&) noexcept;

	JsonSerializer& operator=(const StringType& theData) noexcept;
	JsonSerializer(const StringType&) noexcept;

	JsonSerializer& operator=(const char* theData) noexcept;
	JsonSerializer(const char* theData) noexcept;

	JsonSerializer& operator=(Uint64 theData) noexcept;
	JsonSerializer(Uint64) noexcept;

	JsonSerializer& operator=(Uint32 theData) noexcept;
	JsonSerializer(Uint32) noexcept;

	JsonSerializer& operator=(Uint16 theData) noexcept;
	JsonSerializer(Uint16) noexcept;

	JsonSerializer& operator=(Uint8 theData) noexcept;
	JsonSerializer(Uint8) noexcept;

	JsonSerializer& operator=(Int64 theData) noexcept;
	JsonSerializer(Int64) noexcept;

	JsonSerializer& operator=(Int32 theData) noexcept;
	JsonSerializer(Int32) noexcept;

	JsonSerializer& operator=(Int16 theData) noexcept;
	JsonSerializer(Int16) noexcept;

	JsonSerializer& operator=(Int8 theData) noexcept;
	JsonSerializer(Int8) noexcept;

	JsonSerializer& operator=(Double theData) noexcept;
	JsonSerializer(Double) noexcept;

	JsonSerializer& operator=(Float theData) noexcept;
	JsonSerializer(Float) noexcept;

	JsonSerializer& operator=(Bool theData) noexcept;
	JsonSerializer(Bool) noexcept;

	JsonSerializer& operator=(ValueType) noexcept;
	JsonSerializer(ValueType) noexcept;

	JsonSerializer& operator[](Uint64 idx) const;
	JsonSerializer& operator[](Uint64 idx);

	JsonSerializer& operator[](const typename ObjectType::key_type& key) const;
	JsonSerializer& operator[](typename ObjectType::key_type key);

	operator String() const noexcept;

	operator String() noexcept;

	void pushBack(JsonSerializer&& other) noexcept;
	void pushBack(JsonSerializer& other) noexcept;

	void set(std::unique_ptr<String> p);

	void set(std::unique_ptr<ArrayType> p);

	void set(std::unique_ptr<ObjectType> p);

	void destroy() noexcept;

	~JsonSerializer() noexcept;

	String& parseJsonToEtf(JsonSerializer&& dataToParse);
	String comparisongStringFalse{ "false" };
	String comparisongStringNil{ "nil" };
	String falseString{ "false" };
	String nilString{ "nil" };
	String bufferString{};
	StringView buffer{};
	Uint64 offSet{};
	Uint64 size{};

	void singleValueJsonToETF(JsonSerializer&& dataToParse);

	void writeObject(JsonSerializer::ObjectType&& jsonData);

	void writeString(JsonSerializer::StringType&& jsonData);

	void writeInt(JsonSerializer::IntType jsonData);

	void writeUint(JsonSerializer::UintType jsonData);

	void writeFloat(JsonSerializer::FloatType jsonData);

	void writeArray(JsonSerializer::ArrayType&& jsonData);

	void writeBool(JsonSerializer::BoolType jsonData);

	void writeNullExt();

	void writeNull();

	void writeToBuffer(String&&);

	void appendBinaryExt(String&&, Uint32);

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
};

#endif// !ERL_PACKER