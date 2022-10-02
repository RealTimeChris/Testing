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

struct EnumConverter {
	template<IsEnum EnumType> EnumConverter(EnumType other) {
		this->thePtr = new uint64_t{};
		*static_cast<uint64_t*>(this->thePtr) = static_cast<uint64_t>(other);
	};

	EnumConverter& operator=(EnumConverter&&) noexcept;

	EnumConverter(EnumConverter&&) noexcept;

	EnumConverter& operator=(EnumConverter&) noexcept;

	EnumConverter(EnumConverter&) noexcept;

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

	void* thePtr{ nullptr };
	bool vectorType{ false };
};

class JsonObject {
  public:
	template<typename ObjectType> using AllocatorType = std::allocator<ObjectType>;
	using StringType = std::string;
	using ObjectType = std::map<std::string, JsonObject, std::less<>, std::allocator<std::pair<const std::string, JsonObject>>>;
	using ArrayType = std::vector<JsonObject>;
	using UintType = uint64_t;
	using FloatType = double;
	using IntType = int64_t;
	using BoolType = bool;
	std::string theString{};

	std::string dump(const JsonObject& theData, std::string& theString, const unsigned int indent_step, const unsigned int current_indent = 0);
	JsonObject() noexcept = default;
	ValueType theType{ ValueType::Null };
	bool areWeStarting{ true };
	bool areWeTopLevel{ true };
	std::string theKey{};

	union JsonValue {
		ObjectType* object;
		ArrayType* array;
		StringType* string;
		BoolType boolean;
		IntType numberInt;
		UintType numberUint;
		FloatType numberDouble;
		JsonValue() noexcept = default;

		JsonValue& operator=(const char* theData) noexcept;

		JsonValue& operator=(const std::string theData) noexcept;

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

		JsonValue(const char* theData) noexcept;

		JsonValue(const std::string theData) noexcept;

		JsonValue(uint64_t theData) noexcept;

		JsonValue(uint32_t theData) noexcept;

		JsonValue(uint16_t theData) noexcept;

		JsonValue(uint8_t theData) noexcept;

		JsonValue(int64_t theData) noexcept;

		JsonValue(int32_t theData) noexcept;

		JsonValue(int16_t theData) noexcept;

		JsonValue(int8_t theData) noexcept;

		JsonValue(double theData) noexcept;

		JsonValue(float theData) noexcept;

		JsonValue(bool theData) noexcept;

		JsonValue(ValueType t) noexcept;
	};

	JsonValue theValue;

	template<typename ObjectType, typename... Args> static ObjectType* create(Args&&... args) {
		std::allocator<ObjectType> allocator{};

		auto deleter = [&](ObjectType* obj) {
			std::allocator_traits<AllocatorType<ObjectType>>::deallocate(allocator, obj, 1);
		};
		std::unique_ptr<ObjectType, decltype(deleter)> object(std::allocator_traits<AllocatorType<ObjectType>>::allocate(allocator, 1), deleter);
		std::allocator_traits<AllocatorType<ObjectType>>::construct(allocator, object.get(), std::forward<Args>(args)...);
		return object.release();
	}
	/*
	template<typename ObjectType> JsonObject& operator=(std::vector<ObjectType> theData) noexcept {
		this->theType = ValueType::Array;
		int32_t theIndex{};
		for (auto& value: theData) {
			this->theValues[std::to_string(theIndex)] = std::make_unique<JsonObject>(value);
			theIndex++;
		}
		return *this;
	}

	template<typename ObjectType> JsonObject(std::vector<ObjectType> theData) noexcept {
		*this = theData;
	}

	template<typename KeyType, typename ObjectType> JsonObject& operator=(std::unordered_map<KeyType, ObjectType> theData) noexcept {
		int32_t theIndex{};
		this->theType = ValueType::Array;

		for (auto& [key, value]: theData) {
			this->theValues[key] = std::make_unique<JsonObject>();
			this->theValues[key]->theType = ValueType::Object;
			this->theValues[key]->theValues[key] = std::make_unique<JsonObject>(value);
			this->theValues[key]->theValues[key]->theType = ValueType::String;
			this->theValues[key]->theValues[key]->theKey = key;
		}
		theIndex++;
		return *this;
	}
	*/
	template<typename KeyType, typename ObjectType> JsonObject(std::unordered_map<KeyType, ObjectType> theData) noexcept {
		*this = theData;
	};

	JsonObject(const char*, ValueType) noexcept;

	JsonObject& operator=(EnumConverter theData) noexcept;
	JsonObject(EnumConverter) noexcept;

	JsonObject& operator=(const JsonObject& theKey) noexcept;
	JsonObject(const JsonObject& theKey) noexcept;

	JsonObject& operator=(const char* theData) noexcept;
	JsonObject(const char* theData) noexcept;

	JsonObject& operator=(const std::string theData) noexcept;
	JsonObject(const std::string) noexcept;

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

	JsonObject& operator[](const char* theKey) noexcept;

	operator std::string() noexcept;

	void pushBack(const char* theKey, std::string other) noexcept;
	void pushBack(const char* theKey, JsonObject other) noexcept;
	void pushBack(const char* theKey, uint64_t other) noexcept;
	void pushBack(const char* theKey, uint32_t other) noexcept;
	void pushBack(const char* theKey, uint16_t other) noexcept;
	void pushBack(const char* theKey, uint8_t other) noexcept;
	void pushBack(const char* theKey, int64_t other) noexcept;
	void pushBack(const char* theKey, int32_t other) noexcept;
	void pushBack(const char* theKey, int16_t other) noexcept;
	void pushBack(const char* theKey, int8_t other) noexcept;
};

bool operator==(const JsonObject::JsonValue&lhs, const JsonObject::JsonValue&rhs) {
	if (lhs.string && rhs.string) {
		if (lhs.string == rhs.string) {
			return true;
		} else {
			return false;
		}
	} else if (lhs.array && rhs.array) {
		if (lhs.array== rhs.array) {
			return true;
		} else {
			return false;
		}
	} else if (lhs.object && rhs.object) {
		if (lhs.object == rhs.object) {
			return true;
		} else {
			return false;
		}
	} else if (lhs.boolean && rhs.boolean) {
		if (lhs.boolean == rhs.boolean) {
			return true;
		} else {
			return false;
		}
	} else if (lhs.numberDouble && rhs.numberDouble) {
		if (lhs.numberDouble == rhs.numberDouble) {
			return true;
		} else {
			return false;
		}
	} else if (lhs.numberInt && rhs.numberInt) {
		if (lhs.numberInt== rhs.numberInt) {
			return true;
		} else {
			return false;
		}
	} else if (lhs.numberUint && rhs.numberUint) {
		if (lhs.numberUint == rhs.numberUint) {
			return true;
		} else {
			return false;
		}
	} 
}

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