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

enum class ValueType { Null = 0, Null_Ext = 1, Object = 2, Array = 3, Float = 4, String = 5, Bool = 6, Int64 = 7, Uint64 = 8, Unset = 9 };

template<typename TheType>
concept IsEnum = std::is_enum<TheType>::value;

template<typename TheType>
concept IsString = std::same_as<TheType, std::string>;

struct EnumConverter {
	template<IsEnum EnumType> EnumConverter(EnumType other) {
		this->thePtr = new uint64_t{};
		*static_cast<uint64_t*>(this->thePtr) = static_cast<uint64_t>(other);
	};

	EnumConverter& operator=(EnumConverter&&) noexcept;

	EnumConverter(EnumConverter&&) noexcept;

	EnumConverter& operator=(EnumConverter&) noexcept = delete;

	EnumConverter(EnumConverter&) noexcept = delete;

	template<IsEnum EnumType> EnumConverter& operator=(std::vector<EnumType> other) {
		this->thePtr = new std::vector<uint64_t>{};
		for (auto& value: other) {
			static_cast<std::vector<uint64_t>*>(this->thePtr)->emplace_back(static_cast<uint64_t>(value));
		}
		this->vectorType = true;
		return *this;
	};

	template<IsEnum EnumType> EnumConverter(std::vector<EnumType> other) {
		*this = other;
	};

	operator std::vector<uint64_t>();

	explicit operator uint64_t();

	~EnumConverter();

  protected:
	bool vectorType{ false };
	void* thePtr{ nullptr };
};

class JsonObject {
  public:
	using ObjectType = std::map<std::string, JsonObject, std::less<>, std::allocator<std::pair<const std::string, JsonObject>>>;
	using ArrayType = std::vector<JsonObject>;
	using StringType = std::string;
	using UintType = uint64_t;
	using FloatType = double;
	using IntType = int64_t;
	using BoolType = bool;

	ValueType theType{ ValueType::Null };
	StringType theString{};
	StringType theKey{};

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

		JsonValue& operator=(const StringType theData) noexcept;

		JsonValue& operator=(const char* theData) noexcept;

		JsonValue& operator=(uint64_t theData) noexcept;

		JsonValue& operator=(uint32_t theData) noexcept;

		JsonValue& operator=(uint16_t theData) noexcept;

		JsonValue& operator=(uint8_t theData) noexcept;

		JsonValue& operator=(int64_t theData) noexcept;

		JsonValue& operator=(int32_t theData) noexcept;

		JsonValue& operator=(int16_t theData) noexcept;

		JsonValue& operator=(int8_t theData) noexcept;

		JsonValue& operator=(double theData) noexcept;

		JsonValue& operator=(float theData) noexcept;

		JsonValue& operator=(bool theData) noexcept;

		JsonValue& operator=(ValueType t) noexcept;
		JsonValue(ValueType t) noexcept;

		void destroy(ValueType theType);
	};

	JsonValue theValue{ ValueType::Null };

	JsonObject() noexcept = default;

	template<typename ObjectType> JsonObject& operator=(std::vector<ObjectType> theData) noexcept {
		this->theType = ValueType::Array;
		int32_t theIndex{};
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
		int32_t theIndex{};
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

	JsonObject& operator=(const StringType theData) noexcept;
	JsonObject(const StringType) noexcept;

	JsonObject& operator=(const char* theData) noexcept;
	JsonObject(const char* theData) noexcept;

	JsonObject& operator=(uint64_t theData) noexcept;
	JsonObject(uint64_t) noexcept;

	JsonObject& operator=(uint32_t theData) noexcept;
	JsonObject(uint32_t) noexcept;

	JsonObject& operator=(uint16_t theData) noexcept;
	JsonObject(uint16_t) noexcept;

	JsonObject& operator=(uint8_t theData) noexcept;
	JsonObject(uint8_t) noexcept;

	JsonObject& operator=(int64_t theData) noexcept;
	JsonObject(int64_t) noexcept;

	JsonObject& operator=(int32_t theData) noexcept;
	JsonObject(int32_t) noexcept;

	JsonObject& operator=(int16_t theData) noexcept;
	JsonObject(int16_t) noexcept;

	JsonObject& operator=(int8_t theData) noexcept;
	JsonObject(int8_t) noexcept;

	JsonObject& operator=(double theData) noexcept;
	JsonObject(double) noexcept;

	JsonObject& operator=(float theData) noexcept;
	JsonObject(float) noexcept;

	JsonObject& operator=(bool theData) noexcept;
	JsonObject(bool) noexcept;

	JsonObject(const char*, ValueType) noexcept;

	JsonObject& operator=(ValueType) noexcept;

	JsonObject& operator[](size_t idx) const;
	JsonObject& operator[](size_t idx);

	JsonObject& operator[](const typename ObjectType::key_type& key) const;
	JsonObject& operator[](typename ObjectType::key_type key);

	operator std::string() noexcept;

	operator std::string() const noexcept;

	void pushBack(JsonObject other) noexcept;

	~JsonObject() noexcept;
};

	struct ErlPackError : public std::runtime_error {
	public:
		explicit ErlPackError(const std::string& message);
	};

	constexpr uint8_t formatVersion{ 131 };

	enum class ETFTokenType : uint8_t {
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

		std::string parseJsonToEtf(std::string&);

		std::string& parseEtfToJson(std::string_view dataToParse);
		~ErlPacker() {};
	protected:
		std::string comparisongStringFalse{ "false" };
		std::string comparisongStringNil{ "nil" };
		std::string falseString{ "false" };
		std::string nilString{ "nil" };
		std::string bufferString{};
		std::string_view buffer{};
		uint64_t offSet{};
		uint64_t size{};

		void singleValueJsonToETF(simdjson::ondemand::value jsonData);

		void writeString(simdjson::ondemand::value jsonData);

		void writeNumber(simdjson::ondemand::value jsonData);

		void writeBool(simdjson::ondemand::value jsonData);

		void writeToBuffer(const std::string&);

		void appendVersion();

		void appendSmallIntegerExt(uint8_t);

		void appendIntegerExt(uint32_t);

		void appendNewFloatExt(double);

		void appendNilExt();

		void appendNil();

		void appendFalse();

		void appendTrue();

		void appendUnsignedLongLong(uint64_t);

		void appendBinaryExt(const std::string&, uint32_t);

		void appendListHeader(uint32_t);

		void appendMapHeader(uint32_t);

		template<typename ReturnType> ReturnType readBits() {
			if (this->offSet + sizeof(ReturnType) > this->size) {
				throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of the buffer.\n\n" };
			}
			const ReturnType newValue = *reinterpret_cast<const ReturnType*>(this->buffer.data() + this->offSet);
			this->offSet += sizeof(ReturnType);
			return DiscordCoreAPI::reverseByteOrder<const ReturnType>(newValue);
		}

		const char* readString(uint32_t length);

		std::string singleValueETFToJson();

		std::string parseSmallIntegerExt();

		std::string parseBigint(uint32_t);

		std::string parseIntegerExt();

		std::string parseNewFloatExt();

		std::string parseFloatExt();

		std::string processAtom(const char* atom, uint32_t length);

		std::string parseTuple(uint32_t);

		std::string parseSmallTupleExt();

		std::string parseLargeTupleExt();

		std::string parseNilExt();

		std::string parseStringAsList();

		std::string parseListExt();

		std::string parseBinaryExt();

		std::string parseSmallBigExt();

		std::string parseLargeBigExt();

		std::string parseArray(uint32_t);

		std::string parseSmallAtomExt();

		std::string parseMapExt();

		std::string parseAtomUtf8Ext();
	};

#endif // !ERL_PACKER