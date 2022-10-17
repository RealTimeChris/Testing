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
/// JsonObjectFinal.hpp - Header for the json-JsonObjectFinal class.
/// Oct 10, 2022
/// https://discordcoreapi.com
/// \file JsonObjectFinal.hpp

#ifndef JSON_SERIALIZER
#define JSON_SERIALIZER

#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include <coroutine>
#include <stdint.h>
#include <variant>
#include <set>
#include <charconv>

template<typename ObjectType, typename DeleterType = Void> using UniquePtrD = std::unique_ptr<ObjectType, DeleterType>;
template<typename KeyType, typename ObjectType> using UMap = std::unordered_map<KeyType, ObjectType>;
template<typename KeyType, typename ObjectType> using Map = std::map<KeyType, ObjectType>;
template<typename ObjectType> using UniquePtr = std::unique_ptr<ObjectType>;
template<typename ObjectType> using Atomic = std::atomic<ObjectType>;
template<typename ObjectType> using Vector = std::vector<ObjectType>;
template<typename ObjectType> using Deque = std::deque<ObjectType>;
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
using Void = void;

template<typename TimeType> class StopWatch {
  public:
	StopWatch() = delete;

	StopWatch<TimeType>& operator=(const StopWatch<TimeType>& other) {
		this->maxNumberOfMs.store(other.maxNumberOfMs.load());
		this->startTime.store(other.startTime.load());
		return *this;
	}

	StopWatch(const StopWatch<TimeType>& other) {
		*this = other;
	}

	StopWatch(TimeType maxNumberOfMsNew) {
		this->maxNumberOfMs.store(maxNumberOfMsNew.count());
		this->startTime.store(static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count()));
	}

	Uint64 totalTimePassed() {
		Uint64 currentTime = static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
		Uint64 elapsedTime = currentTime - this->startTime.load();
		return elapsedTime;
	}

	bool hasTimePassed() {
		Uint64 currentTime = static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
		Uint64 elapsedTime = currentTime - this->startTime.load();
		if (elapsedTime >= this->maxNumberOfMs.load()) {
			return true;
		} else {
			return false;
		}
	}

	Void resetTimer() {
		this->startTime.store(static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count()));
	}

  protected:
	AtomicUint64 maxNumberOfMs{ 0 };
	AtomicUint64 startTime{ 0 };
};

template<typename ReturnType> Void storeBits(String& to, ReturnType num) {
	const Uint8 byteSize{ 8 };
	ReturnType newValue = reverseByteOrder<ReturnType>(num);
	for (Uint32 x = 0; x < sizeof(ReturnType); ++x) {
		to.push_back(static_cast<Uint8>(newValue >> (byteSize * x)));
	}
}

enum class WebSocketOpCode : Int8 { Op_Continuation = 0x00, Op_Text = 0x01, Op_Binary = 0x02, Op_Close = 0x08, Op_Ping = 0x09, Op_Pong = 0x0a };

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

template<typename ReturnType> ReturnType reverseByteOrder(const ReturnType net) {
	switch (sizeof(ReturnType)) {
		case 1: {
			return net;
		}
		case 2: {
			return ntohs(static_cast<u_short>(net));
		}
		case 4: {
			return ntohl(static_cast<u_long>(net));
		}
		case 8: {
			return ntohll(static_cast<unsigned long long>(net));
		}
	}
	return ReturnType{};
}

template<typename TheType>
concept IsEnum = std::is_enum<TheType>::value;

template<typename TheType>
concept IsString = std::same_as<TheType, String>;

struct EnumConverter {
	template<IsEnum EnumType> EnumConverter& operator=(EnumType other) {
		this->theUint = static_cast<Uint64>(other);
		return *this;
	};

	template<IsEnum EnumType> EnumConverter(EnumType other) {
		*this = other;
	};

	EnumConverter& operator=(EnumConverter&&) noexcept;
	EnumConverter(EnumConverter&&) noexcept;

	EnumConverter& operator=(const EnumConverter&) noexcept = delete;
	EnumConverter(const EnumConverter&) noexcept = delete;

	template<IsEnum EnumType> EnumConverter& operator=(Vector<EnumType> other) {
		this->theVector = std::move(other);
		return *this;
	};

	template<IsEnum EnumType> EnumConverter(Vector<EnumType> other) {
		*this = other;
	};

	operator Vector<Uint64>() const noexcept;

	explicit operator Uint64() const noexcept;

	bool isItAVector() const noexcept;

  protected:
	Vector<Uint64> theVector{};
	Bool vectorType{ false };
	Uint64 theUint{};
};

enum class ValueType : Int8 { Null = 0, Null_Ext = 1, Object = 2, Array = 3, Float = 4, String = 5, Bool = 6, Int64 = 7, Uint64 = 8 };

class JsonSerializer {
  public:
	using AllocatorTypeMap = std::allocator<std::pair<const String, JsonSerializer>>;
	template<typename ObjectType> using AllocatorType = std::allocator<ObjectType>;
	template<typename ObjectType> using AllocatorTraits = std::allocator_traits<std::allocator<ObjectType>>;
	using ObjectType = std::map<String, JsonSerializer, std::less<>, AllocatorTypeMap>;
	using ArrayType = std::vector<JsonSerializer, AllocatorType<JsonSerializer>>;
	using StringType = String;
	using UintType = Uint64;
	using FloatType = Double;
	using IntType = Int64;
	using BoolType = Bool;

	JsonSerializer() noexcept = default;

	JsonSerializer& operator=(const JsonSerializer&) noexcept;
	JsonSerializer(const JsonSerializer&) noexcept;

	JsonSerializer& operator=(JsonSerializer&&) noexcept;
	JsonSerializer(JsonSerializer&&) noexcept;

	operator String&&() noexcept;

	operator String() noexcept;

	Void refreshString(WebSocketOpCode theOpCode);

	ValueType theType{ ValueType::Null };

	union JsonValue {
		ObjectType* object;
		StringType* string;
		ArrayType* array;
		FloatType numberDouble;
		UintType numberUint;
		IntType numberInt;
		BoolType boolean;
	};

	JsonValue theValue{};

	template<typename ObjectType> JsonSerializer& operator=(Vector<ObjectType> theData) noexcept {
		this->setValue(ValueType::Array);
		for (auto& value: theData) {
			this->theValue.array->push_back(JsonSerializer{ value });
		}
		return *this;
	}

	template<typename ObjectType> JsonSerializer(Vector<ObjectType> theData) noexcept {
		*this = theData;
	}

	template<IsString KeyType, IsString ObjectType> JsonSerializer& operator=(UMap<KeyType, ObjectType> theData) noexcept {
		this->setValue(ValueType::Object);
		for (auto& [key, value]: theData) {
			(*this->theValue.object)[key] = value;
		}
		return *this;
	}

	template<IsString KeyType, IsString ObjectType> JsonSerializer(UMap<KeyType, ObjectType> theData) noexcept {
		*this = theData;
	};

	JsonSerializer& operator=(EnumConverter&& theData) noexcept;
	JsonSerializer(EnumConverter&&) noexcept;

	JsonSerializer& operator=(const EnumConverter& theData) noexcept;
	JsonSerializer(const EnumConverter&) noexcept;

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

	JsonSerializer& operator[](typename ObjectType::key_type key);

	JsonSerializer& operator[](Uint64 index);

	Void pushBack(JsonSerializer&& other) noexcept;
	Void pushBack(JsonSerializer& other) noexcept;

	friend bool operator==(const JsonSerializer&, const JsonSerializer&);

	~JsonSerializer() noexcept;

  private:
	String theString{};

	Void parseJsonToEtf(const JsonSerializer* dataToParse);

	Void parseJsonToJson(const JsonSerializer* dataToParse);

	Void writeJsonObject(const JsonSerializer::ObjectType* theObjectNew);

	Void writeJsonArray(const JsonSerializer::ArrayType* theArray);

	Void writeJsonString(const JsonSerializer::StringType* string);

	template<typename NumberType,
		std::enable_if_t<std::is_integral<NumberType>::value || std::is_same<NumberType, Uint64>::value || std::is_same<NumberType, Int64>::value, int> = 0>
	Void writeJsonInt(NumberType theInt) {
		auto theIntNew = std::to_string(theInt);
		this->writeString(theIntNew.data(), theIntNew.size());
	}

	Void writeJsonFloat(const JsonSerializer::FloatType theFloat);

	Void writeJsonBool(const JsonSerializer::BoolType jsonData);

	Void writeJsonNullExt();

	Void writeJsonNull();

	Void writeEtfObject(const JsonSerializer::ObjectType* jsonData);

	Void writeEtfArray(const JsonSerializer::ArrayType* jsonData);

	Void writeEtfString(const JsonSerializer::StringType* jsonData);

	Void writeEtfUint(const JsonSerializer::UintType jsonData);

	Void writeEtfInt(const JsonSerializer::IntType jsonData);

	Void writeEtfFloat(const JsonSerializer::FloatType jsonData);

	Void writeEtfBool(const JsonSerializer::BoolType jsonData);

	Void writeEtfNullExt();

	Void writeEtfNull();

	Void writeString(const char* theData, std::size_t length);

	Void writeCharacter(const char theChar);

	Void writeToBuffer(const String& bytes);

	Void appendBinaryExt(const String& bytes, Uint32 sizeNew);

	Void appendUnsignedLongLong(const Uint64 value);

	Void appendNewFloatExt(const Double FloatValue);

	Void appendSmallIntegerExt(const Uint8 value);

	Void appendListHeader(const Uint32 sizeNew);

	Void appendMapHeader(const Uint32 sizeNew);

	Void appendIntegerExt(const Uint32 value);

	Void appendVersion();

	Void appendNilExt();

	Void appendFalse();

	Void appendTrue();

	Void appendNil();

	Void setValue(ValueType theTypeNew);

	Void destroy() noexcept;
};

#endif// !ERL_PACKER