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
using Bool = bool;
using Void = void;

template<typename TimeType> class StopWatch {
  public:
	StopWatch() = delete;

	StopWatch(TimeType maxNumberOfMsNew) {
		this->maxNumberOfMs = maxNumberOfMsNew.count();
		this->startTime = static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
	}

	Uint64 totalTimePassed() {
		Uint64 currentTime = static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
		Uint64 elapsedTime = currentTime - this->startTime;
		return elapsedTime;
	}

	bool hasTimePassed() {
		Uint64 currentTime = static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
		Uint64 elapsedTime = currentTime - this->startTime;
		if (elapsedTime >= this->maxNumberOfMs) {
			return true;
		} else {
			return false;
		}
	}

	void resetTimer() {
		this->startTime = static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
	}

  protected:
	Uint64 maxNumberOfMs{ 0 };
	Uint64 startTime{ 0 };
};

template<typename ReturnType> void storeBits(String& to, ReturnType num) {
	const Uint8 byteSize{ 8 };
	ReturnType newValue = reverseByteOrder<ReturnType>(num);
	for (Uint32 x = 0; x < sizeof(ReturnType); ++x) {
		to.push_back(static_cast<Uint8>(newValue >> (byteSize * x)));
	}
}

constexpr Uint8 formatVersion{ 131 };

enum class WebSocketOpCode : Int8 { Op_Continuation = 0x00, Op_Text = 0x01, Op_Binary = 0x02, Op_Close = 0x08, Op_Ping = 0x09, Op_Pong = 0x0a };

enum class ValueType : Int8 { Null = 0, Null_Ext = 1, Object = 2, Array = 3, Float = 4, String = 5, Bool = 6, Int64 = 7, Uint64 = 8 };

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

class JsonObject {
  public:
	using AllocatorTypeMap = std::allocator<std::pair<const String, JsonObject>>;
	template<typename ObjectType> using AllocatorType = std::allocator<ObjectType>;
	using ObjectType = std::map<String, JsonObject, std::less<>, AllocatorTypeMap>;
	using ArrayType = std::vector<JsonObject, AllocatorType<JsonObject>>;
	using StringType = String;
	using UintType = Uint64;
	using FloatType = Double;
	using IntType = Int64;
	using BoolType = Bool;

	ValueType theType{ ValueType::Null };

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
			(*this->theValue.object)[key] = JsonObject{};
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

	void writeCharacter(char theChar, String& theString) {
		theString.push_back(theChar);
	}

	void writeCharacters(const char* theData, std::size_t length, String& theString) {
		theString.append(theData, length);
	}

	operator String();

	~JsonObject() noexcept;
};



class JsonObjectFinal :public JsonObject{
  public:
	WebSocketOpCode theOpCode{};

	JsonObjectFinal() noexcept = default;
	JsonObject theObject{};
	JsonObjectFinal(JsonObject& theObjectNew, WebSocketOpCode theOpCodeNew) {
		this->theObject = std::move(theObjectNew);
		this->theOpCode = theOpCodeNew;
	}

	JsonObjectFinal& operator=(const JsonObjectFinal&) noexcept = default;
	JsonObjectFinal(JsonObjectFinal&) noexcept = default;

	JsonObjectFinal& operator=(JsonObjectFinal&&) noexcept = default;
	JsonObjectFinal(JsonObjectFinal&&) noexcept = default;

	JsonObject& operator[](JsonObject::ObjectType::key_type theKey);

	void parseJsonToEtf(const JsonObject& dataToParse) {
		this->theString.clear();
		this->appendVersion();
		this->singleValueJsonToETF(dataToParse);
		return;
	}

	void writeJsonObject(JsonObject::ObjectType& theObject) {
		if (theObject.empty()) {
			this->writeCharacters("{}", 2);
			return;
		}
		this->writeCharacter('{');

		Int32 theIndex{};
		for (auto x = theObject.cbegin(); x != theObject.cend(); ++x) {
			this->writeCharacter('\"');
			dumpEscaped(x->first, false);
			this->writeCharacters("\":", 2);
			dump(x->second);

			if (theIndex != theObject.size() - 1) {
				this->writeCharacter(',');
			}
			theIndex++;
		}

		this->writeCharacter('}');
	}

	void writeJsonArray(JsonObject::ArrayType& theArray) {
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

	void dump() {
		if (this->theOpCode == WebSocketOpCode::Op_Binary) {
			parseJsonToEtf(this->theObject);
		} else {
			switch (this->theObject.theType) {
				case ValueType::Object: {
					this->writeJsonObject(*this->theObject.theValue.object);
					return;
				}
				case ValueType::Array: {
					this->writeJsonArray(*this->theObject.theValue.array);
					return;
				}
				case ValueType::String: {
					this->writeCharacter('\"');
					dumpEscaped(*this->theObject.theValue.string, false);
					this->writeCharacter('\"');
					return;
				}
				case ValueType::Bool: {
					if (this->theObject.theValue.boolean) {
						this->writeCharacters("true", 4);
					} else {
						this->writeCharacters("false", 5);
					}
					return;
				}
				case ValueType::Int64: {
					dumpInt(this->theObject.theValue.numberInt);
					return;
				}
				case ValueType::Uint64: {
					dumpInt(this->theObject.theValue.numberUint);
					return;
				}
				case ValueType::Float: {
					dumpFloat(this->theObject.theValue.numberDouble);
					return;
				}
				case ValueType::Null: {
					this->writeCharacters("null", 4);
					return;
				}
			}
		}
	}

	operator String() {
		return std::move(this->theString);
	}

	~JsonObjectFinal() noexcept = default;

  private:
	void writeNullExt() {
		this->appendNilExt();
	}

	void writeNull() {
		this->appendNil();
	}

	void singleValueJsonToETF(const JsonObject& jsonData) {
		switch (jsonData.theType) {
			case ValueType::Array: {
				this->writeArray(jsonData.theValue.array);
				break;
			}
			case ValueType::Object: {
				this->writeObject(jsonData.theValue.object);
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

	void writeObject(JsonObject::ObjectType* jsonData) {
		Bool add_comma{ false };
		this->appendMapHeader(static_cast<Uint32>(jsonData->size()));
		for (auto& field: *jsonData) {
			if (add_comma) {
			}
			StringStream theStream{};
			theStream << field.first;
			String theKey = theStream.str();

			auto theSize = theKey.size();
			this->appendBinaryExt(theKey, static_cast<Uint32>(theSize));
			this->singleValueJsonToETF(field.second);
			add_comma = true;
		}
	}

	void writeString(JsonObject::StringType* jsonData) {
		this->appendBinaryExt(*jsonData, jsonData->size());
	}

	void writeFloat(JsonObject::FloatType jsonData) {
		auto theFloat = jsonData;
		this->appendNewFloatExt(theFloat);
	}

	void writeUint(JsonObject::UintType jsonData) {
		auto theInt = jsonData;
		if (theInt <= 255 && theInt >= 0) {
			this->appendSmallIntegerExt(static_cast<Uint8>(theInt));
		} else if (theInt <= std::numeric_limits<Uint32>::max() && theInt >= 0) {
			this->appendIntegerExt(static_cast<Uint32>(theInt));
		} else {
			this->appendUnsignedLongLong(theInt);
		}
	}

	void writeInt(JsonObject::IntType jsonData) {
		auto theInt = jsonData;
		if (theInt <= 127 && theInt >= -127) {
			this->appendSmallIntegerExt(static_cast<Uint8>(theInt));
		} else if (theInt <= std::numeric_limits<Int32>::max() && theInt >= std::numeric_limits<Int32>::min()) {
			this->appendIntegerExt(static_cast<Uint32>(theInt));
		} else {
			this->appendUnsignedLongLong(static_cast<Uint64>(theInt));
		}
	}

	void writeArray(JsonObject::ArrayType* jsonData) {
		Bool add_comma{ false };
		this->appendListHeader(static_cast<Uint32>(jsonData->size()));
		for (auto& element: *jsonData) {
			this->singleValueJsonToETF(element);
			add_comma = true;
		}
		this->appendNilExt();
	}

	void writeBool(JsonObject::BoolType jsonData) {
		if (jsonData) {
			this->appendTrue();
		} else {
			this->appendFalse();
		}
	}

	void writeToBuffer(String& bytes) {
		this->writeCharacters(bytes.data(), bytes.size());
	}

	void appendBinaryExt(String& bytes, Uint32 sizeNew) {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Binary_Ext) };
		storeBits(bufferNew, sizeNew);
		this->writeToBuffer(bufferNew);
		this->writeToBuffer(bytes);
	}

	void appendUnsignedLongLong(Uint64 value) {
		String bufferNew{};
		bufferNew.resize(static_cast<Uint64>(1) + 2 + sizeof(Uint64));
		bufferNew[0] = static_cast<Uint8>(ETFTokenType::Small_Big_Ext);
		StopWatch theStopWatch{ std::chrono::milliseconds{ 1500 } };
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
		this->writeToBuffer(bufferNew);
	}

	void appendSmallIntegerExt(Uint8 value) {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Integer_Ext), static_cast<char>(value) };
		this->writeToBuffer(bufferNew);
	}

	void appendIntegerExt(Uint32 value) {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Integer_Ext) };
		storeBits(bufferNew, value);
		this->writeToBuffer(bufferNew);
	}

	void appendListHeader(Uint32 sizeNew) {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::List_Ext) };
		storeBits(bufferNew, sizeNew);
		this->writeToBuffer(bufferNew);
	}

	void appendMapHeader(Uint32 sizeNew) {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Map_Ext) };
		storeBits(bufferNew, sizeNew);
		this->writeToBuffer(bufferNew);
	}

	void appendNewFloatExt(Double FloatValue) {
		String bufferNew{};
		bufferNew.push_back(static_cast<unsigned char>(ETFTokenType::New_Float_Ext));

		void* punner{ &FloatValue };
		storeBits(bufferNew, *static_cast<Uint64*>(punner));
		this->writeToBuffer(bufferNew);
	}

	void appendVersion() {
		String bufferNew{ static_cast<int8_t>(formatVersion) };
		this->writeToBuffer(bufferNew);
	}

	void appendNilExt() {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Nil_Ext) };
		this->writeToBuffer(bufferNew);
	}

	void appendFalse() {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 5, static_cast<Uint8>('f'), static_cast<Uint8>('a'), static_cast<Uint8>('l'), static_cast<Uint8>('s'),
			static_cast<Uint8>('e') };
		this->writeToBuffer(bufferNew);
	}

	void appendTrue() {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 4, static_cast<Uint8>('t'), static_cast<Uint8>('r'), static_cast<Uint8>('u'), static_cast<Uint8>('e') };
		this->writeToBuffer(bufferNew);
	}

	void appendNil() {
		String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 3, static_cast<Uint8>('n'), static_cast<Uint8>('x'), static_cast<Uint8>('l') };
		this->writeToBuffer(bufferNew);
	}

	void writeCharacter(char theChar) {
		theString.push_back(theChar);
	}

	void writeCharacters(const char* theData, std::size_t length) {
		theString.append(theData, length);
	}

	template<typename NumberType,
		std::enable_if_t<std::is_integral<NumberType>::value || std::is_same<NumberType, Uint64>::value || std::is_same<NumberType, Int64>::value, int> = 0>
	void dumpInt(NumberType x) {
		auto theFloat = std::to_string(x);
		this->writeCharacters(theFloat.data(), theFloat.size());
		return;
	}

	void dumpFloat(Float x) {
		auto theFloat = std::to_string(x);
		this->writeCharacters(theFloat.data(), theFloat.size());
	}

	void dumpFloat(Float x, std::false_type) {
		auto theFloat = std::to_string(x);
		this->writeCharacters(theFloat.data(), theFloat.size());
	}

	void dump(const JsonObject& val) {
		switch (val.theType) {
			case ValueType::Object: {
				if (val.theValue.object->empty()) {
					this->writeCharacters("{}", 2);
					return;
				}
				this->writeCharacter('{');
				auto x = val.theValue.object->cbegin();
				for (std::size_t cnt = 0; cnt < val.theValue.object->size() - 1; ++cnt, ++x) {
					this->writeCharacter('\"');
					dumpEscaped(x->first, false);
					this->writeCharacters("\":", 2);
					dump(x->second);
					this->writeCharacter(',');
				}

				this->writeCharacter('\"');
				dumpEscaped(x->first, false);
				this->writeCharacters("\":", 2);
				dump(x->second);

				this->writeCharacter('}');
			}

				return;

			case ValueType::Array: {
				if (val.theValue.array->empty()) {
					this->writeCharacters("[]", 2);
					return;
				}

				this->writeCharacter('[');
				for (auto x = val.theValue.array->cbegin(); x != val.theValue.array->cend() - 1; ++x) {
					dump(*x);
					this->writeCharacter(',');
				}

				dump(val.theValue.array->back());

				this->writeCharacter(']');

				return;
			}

			case ValueType::String: {
				this->writeCharacter('\"');
				dumpEscaped(*val.theValue.string, false);
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
		}
	}

	void dumpEscaped(const String& s, const bool ensure_ascii) {
		std::size_t bytes{};

		for (std::size_t x = 0; x < s.size(); ++x) {
			switch (static_cast<std::uint8_t>(s[x])) {
				case 0x08: {
					stringBuffer[bytes++] = '\\';
					stringBuffer[bytes++] = 'b';
					break;
				}
				case 0x09: {
					stringBuffer[bytes++] = '\\';
					stringBuffer[bytes++] = 't';
					break;
				}
				case 0x0A: {
					stringBuffer[bytes++] = '\\';
					stringBuffer[bytes++] = 'n';
					break;
				}
				case 0x0C: {
					stringBuffer[bytes++] = '\\';
					stringBuffer[bytes++] = 'f';
					break;
				}
				case 0x0D: {
					stringBuffer[bytes++] = '\\';
					stringBuffer[bytes++] = 'r';
					break;
				}
				case 0x22: {
					stringBuffer[bytes++] = '\\';
					stringBuffer[bytes++] = '\"';
					break;
				}
				case 0x5C: {
					stringBuffer[bytes++] = '\\';
					stringBuffer[bytes++] = '\\';
					break;
				}
				default: {
					stringBuffer[bytes++] = s[x];
					break;
				}
			}
		}

		if (bytes > 0) {
			this->writeCharacters(stringBuffer.data(), bytes);
		}
	}

	std::array<char, 512> stringBuffer{};
	String theString{};
};
	

#endif// !ERL_PACKER