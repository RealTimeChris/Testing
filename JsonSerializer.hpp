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

#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include <coroutine>
#include <stdint.h>
#include <variant>
#include <set>

template<typename ObjectType, typename DeleterType = void> using UniquePtrD = std::unique_ptr<ObjectType, DeleterType>;
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

enum class ValueType : Int8 { Null = 0, Null_Ext = 1, Object = 2, Array = 3, Float = 4, String = 5, Bool = 6, Int64 = 7, Uint64 = 8 };

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

class JsonObject {
  public:
	using AllocatorTypeMap = std::allocator<std::pair<const String, JsonObject>>;
	template<typename ObjectType>
	using AllocatorType = std::allocator<ObjectType>;
	using ObjectType = std::map<String, JsonObject, std::less<>, AllocatorTypeMap>;
	using ArrayType = std::vector<JsonObject, AllocatorType<JsonObject>>;
	using StringType = String;
	using UintType = Uint64;
	using FloatType = Double;
	using IntType = Int64;
	using BoolType = Bool;

	ValueType theType{ ValueType::Null };
	StringType theString{};

	union JsonValue {
		ObjectType* object;
		StringType* string;
		ArrayType* array;
		FloatType numberDouble;
		UintType numberUint;
		IntType numberInt;
		BoolType boolean;

		JsonValue() noexcept;

		JsonValue& operator=(JsonValue&&) noexcept = delete;
		JsonValue(JsonValue&&) noexcept = delete;

		JsonValue& operator=(const JsonValue&) noexcept = delete;
		JsonValue(const JsonValue&) noexcept = delete;

		~JsonValue() noexcept;
	};

	JsonValue theValue{};

	JsonObject() noexcept = default;

	template<typename ObjectType> JsonObject& operator=(Vector<ObjectType> theData) noexcept {
		this->set(ValueType::Array);
		for (auto& value: theData) {
			this->theValue.array->push_back(JsonObject{ value });
		}
		return *this;
	}

	template<typename ObjectType> JsonObject(Vector<ObjectType> theData) noexcept {
		*this = theData;
	}

	template<IsString KeyType, IsString ObjectType> JsonObject& operator=(UMap<KeyType, ObjectType> theData) noexcept {
		this->set(ValueType::Object);
		for (auto& [key, value]: theData) {
			(*this->theValue.object)[key] = JsonObject{ this->theString };
			(*this->theValue.object)[key] = value;
		}
		return *this;
	}

	template<IsString KeyType, IsString ObjectType> JsonObject(UMap<KeyType, ObjectType> theData) noexcept {
		*this = theData;
	};

	JsonObject& operator=(EnumConverter&& theData) noexcept;
	JsonObject(EnumConverter&&) noexcept;

	JsonObject& operator=(const EnumConverter& theData) noexcept;
	JsonObject(const EnumConverter&) noexcept;

	JsonObject& operator=(JsonObject&& theKey) noexcept;
	JsonObject(JsonObject&& theKey) noexcept;

	JsonObject& operator=(const JsonObject& theKey) noexcept;
	JsonObject(const JsonObject& theKey) noexcept;

	JsonObject& operator=(StringType&& theData) noexcept;
	JsonObject(StringType&&) noexcept;

	JsonObject& operator=(const StringType& theData) noexcept;
	JsonObject(const StringType&) noexcept;

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

	JsonObject& operator=(ValueType) noexcept;
	JsonObject(ValueType) noexcept;

	JsonObject& operator[](Uint64 index);

	JsonObject& operator[](typename ObjectType::key_type key);

	Void pushBack(JsonObject&& other) noexcept;
	Void pushBack(JsonObject& other) noexcept;

	Void set(ValueType theTypeNew);

	Void destroy() noexcept;

	friend bool operator==(const JsonObject&, const JsonObject&);
	
	void dump();

	void writeCharacters(const char* C, size_t length);

	void writeCharacter(char C);

	operator String() noexcept;

	~JsonObject() noexcept;
};
/*
class JsonSerializer {
  public:
	JsonSerializer() noexcept;
	String getString();
	JsonObject& operator[](JsonObject::ObjectType::key_type theKey);
  protected:
	JsonObject theObject{ &this->theString };
	String theString{};

};
*/

#endif// !ERL_PACKER