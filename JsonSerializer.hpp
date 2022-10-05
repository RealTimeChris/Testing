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

enum class ValueType : Int8 { Null = 0, Null_Ext = 1, Object = 2, Array = 3, Float = 4, String = 5, Bool = 6, Int64 = 7, Uint64 = 8 };

template<typename TheType>
concept IsEnum = std::is_enum<TheType>::value;

template<typename TheType>
concept IsString = std::same_as<TheType, String>;

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

class JsonObject {
  public:
	using ObjectType = std::map<String, JsonObject, std::less<>, std::allocator<std::pair<const String, JsonObject>>>;
	template<typename Type> using AllocatorType = std::allocator<Type>;
	using ArrayType = std::vector<JsonObject>;
	using ConstPointer = const JsonObject*;
	using Pointer = JsonObject*;
	using DifferenceType = std::ptrdiff_t;
	using StringType = String;
	using UintType = Uint64;
	using FloatType = Double;
	using ConstReference = const JsonObject&;
	using Reference = JsonObject&;
	using IntType = Int64;
	using BoolType = Bool;
	JsonObject() noexcept = default;

	DiscordCoreAPI::TextFormat theFormat{ DiscordCoreAPI::TextFormat::Etf };
	mutable size_t currentOffsetIntoStringStart{};
	mutable size_t currentOffsetIntoStringEnd{};
	ValueType theType{ ValueType::Null };
	mutable StringView theString{};
	
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

	template<typename ObjectType> JsonObject& operator=(std::vector<ObjectType> theData) noexcept {
		this->set(std::make_unique<ArrayType>());
		for (auto& value: theData) {
			this->theValue.array->push_back(JsonObject{ value });
		}
		return *this;
	}

	template<typename ObjectType> JsonObject(std::vector<ObjectType> theData) noexcept {
		*this = theData;
	}

	template<IsString KeyType, IsString ObjectType> JsonObject& operator=(std::unordered_map<KeyType, ObjectType> theData) noexcept {
		this->set(std::make_unique<ObjectType>());
		for (auto& [key, value]: theData) {
			this->theValue.object->at(key) = JsonObject{ value };
		}
		return *this;
	}

	template<IsString KeyType, IsString ObjectType> JsonObject(std::unordered_map<KeyType, ObjectType> theData) noexcept {
		*this = theData;
	};

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

	JsonObject& operator[](Uint64 idx) const;
	JsonObject& operator[](Uint64 idx);

	JsonObject& operator[](const typename ObjectType::key_type& key) const;
	JsonObject& operator[](typename ObjectType::key_type key);

	operator String() const noexcept;

	operator String() noexcept;

	void pushBack(JsonObject&& other) noexcept;
	void pushBack(JsonObject& other) noexcept;

	void set(std::unique_ptr<String> p);

	void set(std::unique_ptr<ArrayType> p);

	void set(std::unique_ptr<ObjectType> p);

	void destroy() noexcept;

	~JsonObject() noexcept;

	String& parseJsonToEtf(JsonObject&& dataToParse);
	String comparisongStringFalse{ "false" };
	String comparisongStringNil{ "nil" };
	String falseString{ "false" };
	String nilString{ "nil" };
	String bufferString{};
	StringView buffer{};
	Uint64 offSet{};
	Uint64 size{};

	void singleValueJsonToETF(JsonObject&& dataToParse);

	void writeObject(JsonObject::ObjectType&& jsonData);

	void writeString(JsonObject::StringType&& jsonData);

	void writeInt(JsonObject::IntType jsonData);

	void writeUint(JsonObject::UintType jsonData);

	void writeFloat(JsonObject::FloatType jsonData);

	void writeArray(JsonObject::ArrayType&& jsonData);

	void writeBool(JsonObject::BoolType jsonData);

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

class PrimitiveIterator {
  public:

	  using DifferenceType = std::ptrdiff_t;

	constexpr DifferenceType getValue() const noexcept {
		return this->iterator;
	}

	void setBegin() noexcept {
		this->iterator = beginValue;
	}

	void setEnd() noexcept {
		this->iterator = endValue;
	}

	constexpr bool isBegin() const noexcept {
		return this->iterator == beginValue;
	}

	constexpr bool isEnd() const noexcept {
		return this->iterator == endValue;
	}

	friend constexpr bool operator==(PrimitiveIterator lhs, PrimitiveIterator rhs) noexcept {
		return lhs.iterator == rhs.iterator;
	}

	friend constexpr bool operator<(PrimitiveIterator lhs, PrimitiveIterator rhs) noexcept {
		return lhs.iterator < rhs.iterator;
	}

	PrimitiveIterator operator+(DifferenceType n) noexcept {
		auto result = *this;
		result += n;
		return result;
	}

	friend constexpr DifferenceType operator-(PrimitiveIterator lhs, PrimitiveIterator rhs) noexcept {
		return lhs.iterator - rhs.iterator;
	}

	PrimitiveIterator& operator++() noexcept {
		++this->iterator;
		return *this;
	}

	PrimitiveIterator operator++(int) & noexcept {
		auto result = *this;
		++this->iterator;
		return result;
	}

	PrimitiveIterator& operator--() noexcept {
		--this->iterator;
		return *this;
	}

	PrimitiveIterator operator--(int) & noexcept {
		auto result = *this;
		--this->iterator;
		return result;
	}

	PrimitiveIterator& operator+=(DifferenceType n) noexcept {
		this->iterator += n;
		return *this;
	}

	PrimitiveIterator& operator-=(DifferenceType n) noexcept {
		this->iterator -= n;
		return *this;
	}

  protected:
	static constexpr DifferenceType beginValue = 0;
	static constexpr DifferenceType endValue = beginValue + 1;

	DifferenceType iterator = (std::numeric_limits<std::ptrdiff_t>::min)();
};


template<typename JsonObjectType> struct InternalIterator {

	typename JsonObjectType::ObjectType::iterator objectIterator{};

	typename JsonObjectType::ArrayType::iterator arrayIterator{};

	PrimitiveIterator primitiveIterator{};
};

class JsonIterator {
  public:
	using ObjectType = typename JsonObject::ObjectType;
	using ArrayType = typename JsonObject::ArrayType;
	using iterator_category = std::bidirectional_iterator_tag;
	using ValueTypeReal = typename JsonObject::JsonValue;
	using DifferenceType = typename JsonObject::DifferenceType;
	using Pointer = typename std::conditional<std::is_const<JsonObject>::value, typename JsonObject::ConstPointer, typename JsonObject::Pointer>::type;
	using Reference = typename std::conditional<std::is_const<JsonObject>::value, typename JsonObject::ConstReference, typename JsonObject::Reference>::type;

	JsonIterator() = default;
	
	JsonIterator& operator=(JsonIterator&&) noexcept = default;
	JsonIterator(JsonIterator&&) noexcept = default;
	
	~JsonIterator() = default;


	explicit JsonIterator(Pointer objectNew) noexcept : object(objectNew) {
		switch (objectNew->theType) {
			case ValueType::Object: {
				this->iterator.objectIterator = typename ObjectType::iterator();
				break;
			}

			case ValueType::Array: {
				this->iterator.arrayIterator = typename ArrayType::iterator();
				break;
			}

			case ValueType::Null:
			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default: {
				this->iterator.primitiveIterator = PrimitiveIterator();
				break;
			}
		}
	}

	JsonIterator(const JsonIterator& otherNew) noexcept : object(otherNew.object), iterator(otherNew.iterator) {}

	JsonIterator& operator=(const JsonIterator& otherNew) noexcept {
		object = otherNew.object;
		this->iterator = otherNew.iterator;
		return *this;
	}
	void setBegin() noexcept {
		switch (this->object->theType) {
			case ValueType::Object: {
				this->iterator.objectIterator = object->theValue.object->begin();
				break;
			}

			case ValueType::Array: {
				this->iterator.arrayIterator = object->theValue.array->begin();
				break;
			}

			case ValueType::Null: {
				this->iterator.primitiveIterator.setEnd();
				break;
			}

			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default: {
				this->iterator.primitiveIterator.setBegin();
				break;
			}
		}
	}

	void setEnd() noexcept {
		switch (this->object->theType) {
			case ValueType::Object: {
				this->iterator.objectIterator = object->theValue.object->end();
				break;
			}

			case ValueType::Array: {
				this->iterator.arrayIterator = object->theValue.array->end();
				break;
			}

			case ValueType::Null:
			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default: {
				this->iterator.primitiveIterator.setEnd();
				break;
			}
		}
	}
	
	Reference operator*() const {
		switch (this->object->theType) {
			case ValueType::Object: {
				return this->iterator.objectIterator->second;
			}

			case ValueType::Array: {
				return *this->iterator.arrayIterator;
			}

			case ValueType::Null:
				throw std::runtime_error{ "Cannot get value." };

			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default: {
				if (this->iterator.primitiveIterator.isBegin()) {
					return *object;
				}

				throw std::runtime_error{ "Cannot get value." };
			}
		}
	}

	Pointer operator->() const {
		switch (this->object->theType) {
			case ValueType::Object: {
				return &(this->iterator.objectIterator->second);
			}

			case ValueType::Array: {
				return &*this->iterator.arrayIterator;
			}

			case ValueType::Null:
			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default: {
				if (this->iterator.primitiveIterator.isBegin()) {
					return object;
				}

				throw std::runtime_error{ "Cannot get value." };
			}
		}
	}

	JsonIterator operator++(int) &// NOLINT(cert-dcl21-cpp)
	{
		auto result = *this;
		++(*this);
		return result;
	}

	JsonIterator& operator++() {
		switch (this->object->theType) {
			case ValueType::Object: {
				std::advance(this->iterator.objectIterator, 1);
				break;
			}

			case ValueType::Array: {
				std::advance(this->iterator.arrayIterator, 1);
				break;
			}

			case ValueType::Null:
			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default: {
				++this->iterator.primitiveIterator;
				break;
			}
		}

		return *this;
	}

	JsonIterator operator--(int) & {
		auto result = *this;
		--(*this);
		return result;
	}

	JsonIterator& operator--() {
		switch (this->object->theType) {
			case ValueType::Object: {
				std::advance(this->iterator.objectIterator, -1);
				break;
			}

			case ValueType::Array: {
				std::advance(this->iterator.arrayIterator, -1);
				break;
			}

			case ValueType::Null:
			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default: {
				--this->iterator.primitiveIterator;
				break;
			}
		}

		return *this;
	}

	template<typename Iterator, std::enable_if_t<(std::is_same<Iterator, JsonIterator>::value || std::is_same<Iterator, Iterator>::value), std::nullptr_t> = nullptr>
	bool operator==(const Iterator& other) const {
		switch (this->object->theType) {
			case ValueType::Object:
				return (this->iterator.objectIterator == other.iterator.objectIterator);

			case ValueType::Array:
				return (this->iterator.arrayIterator == other.iterator.arrayIterator);

			case ValueType::Null:
			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default:
				return (this->iterator.primitiveIterator == other.iterator.primitiveIterator);
		}
	}

	template<typename Iterator, std::enable_if_t<(std::is_same<Iterator, JsonIterator>::value || std::is_same<Iterator, Iterator>::value), std::nullptr_t> = nullptr>
	bool operator!=(const Iterator& other) const {
		return !operator==(other);
	}

	bool operator<(const JsonIterator& other) const {
		switch (this->object->theType) {
			case ValueType::Object:
				throw std::runtime_error{ "Cannot compare order of object iterators." };

			case ValueType::Array:
				return (this->iterator.arrayIterator < other.iterator.arrayIterator);

			case ValueType::Null:
			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default:
				return (this->iterator.primitiveIterator < other.iterator.primitiveIterator);
		}
	}

	bool operator<=(const JsonIterator& other) const {
		return !other.operator<(*this);
	}

	bool operator>(const JsonIterator& other) const {
		return !operator<=(other);
	}

	bool operator>=(const JsonIterator& other) const {
		return !operator<(other);
	}

	JsonIterator& operator+=(DifferenceType i) {
		switch (this->object->theType) {
			case ValueType::Object: {
				throw std::runtime_error{ "Cannot use offsets with object iterators." };
			}


			case ValueType::Array: {
				std::advance(this->iterator.arrayIterator, i);
				break;
			}

			case ValueType::Null:
			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default: {
				this->iterator.primitiveIterator += i;
				break;
			}
		}

		return *this;
	}

	JsonIterator& operator-=(DifferenceType i) {
		return operator+=(-i);
	}

	JsonIterator operator+(DifferenceType i) const {
		auto result = *this;
		result += i;
		return result;
	}

	friend JsonIterator operator+(DifferenceType i, const JsonIterator& it) {
		auto result = it;
		result += i;
		return result;
	}

	JsonIterator operator-(DifferenceType i) const {
		auto result = *this;
		result -= i;
		return result;
	}

	DifferenceType operator-(const JsonIterator& other) const {
		switch (this->object->theType) {
			case ValueType::Object: {
				throw std::runtime_error{ "Cannot use offsets with object iterators." };
			}
			case ValueType::Array: {
				return this->iterator.arrayIterator - other.iterator.arrayIterator;
			}
			case ValueType::Null:
			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default:
				return this->iterator.primitiveIterator - other.iterator.primitiveIterator;
		}
	}

	Reference operator[](DifferenceType n) const {
		switch (this->object->theType) {
			case ValueType::Object:
				throw std::runtime_error{ "Cannot use operator[] for object iterators." };

			case ValueType::Array:
				return *std::next(this->iterator.arrayIterator, n);

			case ValueType::Null:
				throw std::runtime_error{ "Cannot get value." };

			case ValueType::String:
			case ValueType::Bool:
			case ValueType::Int64:
			case ValueType::Uint64:
			case ValueType::Float:
			default: {
				if (this->iterator.primitiveIterator.getValue() == -n) {
					return *object;
				}

				throw std::runtime_error{ "Cannot get value." };
			}
		}
	}

	const typename ObjectType::key_type& key() const {
		if (this->object->theType == ValueType::Object) {
			return this->iterator.objectIterator->first;
		}

		throw std::runtime_error{ "Cannot use key() for non-object iterators." };
	}

	Reference value() const {
		return operator*();
	}

  protected:
	InternalIterator<typename std::remove_const<JsonObject>::type> iterator{};
	Pointer object{ nullptr };
};

class JsonIterator;

class JsonSerializer {
  public:

  using ObjectType = std::map<String, JsonSerializer, std::less<>, std::allocator<std::pair<const String, JsonSerializer>>>;
	template<typename Type> using AllocatorType = std::allocator<Type>;
	using ArrayType = std::vector<JsonSerializer>;
	using ConstPointer = const JsonSerializer*;
	using Pointer = JsonSerializer*;
	using IteratorType = JsonIterator;
	using DifferenceType = std::ptrdiff_t;
	using StringType = String;
	using UintType = Uint64;
	using FloatType = Double;
	using ConstReference = const JsonSerializer&;
	using Reference = JsonSerializer&;
	using IntType = Int64;
	using BoolType = Bool;
	JsonSerializer(JsonObject&& theValueNew) noexcept {
		this->theValue = std::move(theValueNew);
		this->theType = theValueNew.theType;
	}
	IteratorType begin() noexcept {
		IteratorType result(&this->theValue);
		result.setBegin();
		return result;
	}

	IteratorType end() noexcept {
		IteratorType result(&this->theValue);
		result.setEnd();
		return result;
	}
	JsonObject theValue{};
	ValueType theType{ ValueType::Null };
	StringView theCurrentStringMemory{};
	std::unique_ptr<String> theString{ std::make_unique<String>() };
	void writeString(const JsonObject& theObject, String& theString) const noexcept;
	void writeString(const JsonObject& theObject, String& theString) noexcept;
	JsonObject& operator[](Uint64 idx) const;
	JsonObject& operator[](Uint64 idx);
	operator String&() const;
	JsonObject& operator[](const typename ObjectType::key_type& key) const;
	JsonObject& operator[](typename ObjectType::key_type key);

	~JsonSerializer() noexcept = default;

	String comparisongStringFalse{ "false" };
	String comparisongStringNil{ "nil" };
	String falseString{ "false" };
	String nilString{ "nil" };
	String bufferString{};
	StringView buffer{};
	Uint64 offSet{};
	Uint64 size{};
};


#endif// !ERL_PACKER