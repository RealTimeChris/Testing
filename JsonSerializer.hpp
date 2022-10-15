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
/// JsonSerializer.hpp - Header for the json-JsonSerializer class.
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

	~JsonObject() noexcept;
};

struct SerializedJsonObject {
	JsonObject theObject{};
	String theString{};
};

class OutputStringAdapter {
  public:
	OutputStringAdapter() noexcept = default;
	
	void writeCharacter(char theChar) {
		theString.push_back(theChar);
	}

	void writeCharacters(const char* theData, std::size_t length) {
		theString.append(theData, length);
	}

	operator String() {
		return std::move(theString);
	}

  private:
	String theString{};
};

class JsonSerializer {
	using StringType = typename JsonObject::StringType;
	using FloatType = typename JsonObject::FloatType;
	using IntType = typename JsonObject::IntType;
	using UintType = typename JsonObject::UintType;
	static constexpr std::uint8_t UTF8_ACCEPT = 0;
  public:

	  
	JsonObject& operator[](JsonObject::ObjectType::key_type theKey);

	JsonObject theObject{};

	JsonSerializer(JsonObject&& theObjectNew) : loc(std::localeconv()), indentChar('\t') {
		this->theObject = std::move(theObjectNew);
	}

	JsonSerializer(const JsonObject& theObjectNew) : loc(std::localeconv()), indentChar('\t') {
		this->theObject = theObjectNew;
	}
	JsonSerializer(JsonSerializer&) noexcept = default;
	
	JsonSerializer& operator=(const JsonSerializer&) noexcept = default;
	JsonSerializer(JsonSerializer&&) noexcept = default;
	JsonSerializer& operator=(JsonSerializer&&) noexcept = default;
	~JsonSerializer() = default;

	void dump() {
		switch (this->theObject.theType) {
			case ValueType::Object: {
				if (this->theObject.theValue.object->empty()) {
					outputAdapter.writeCharacters("{}", 2);
					return;
				}
				outputAdapter.writeCharacter('{');

				auto i = this->theObject.theValue.object->cbegin();
				for (std::size_t cnt = 0; cnt < this->theObject.theValue.object->size() - 1; ++cnt, ++i) {
					outputAdapter.writeCharacter('\"');
					dumpEscaped(i->first, false);
					outputAdapter.writeCharacters("\":", 2);
					dump(i->second);
					outputAdapter.writeCharacter(',');
				}

				outputAdapter.writeCharacter('\"');
				dumpEscaped(i->first, false);
				outputAdapter.writeCharacters("\":", 2);
				dump(i->second);

				outputAdapter.writeCharacter('}');
			}

				return;

			case ValueType::Array: {
				if (this->theObject.theValue.array->empty()) {
					outputAdapter.writeCharacters("[]", 2);
					return;
				}

				outputAdapter.writeCharacter('[');

				for (auto i = this->theObject.theValue.array->cbegin(); i != this->theObject.theValue.array->cend() - 1; ++i) {
					dump(*i);
					outputAdapter.writeCharacter(',');
				}

				dump(this->theObject.theValue.array->back());

				outputAdapter.writeCharacter(']');

				return;
			}

			case ValueType::String: {
				outputAdapter.writeCharacter('\"');
				dumpEscaped(*this->theObject.theValue.string, false);
				outputAdapter.writeCharacter('\"');
				return;
			}

			case ValueType::Bool: {
				if (this->theObject.theValue.boolean) {
					outputAdapter.writeCharacters("true", 4);
				} else {
					outputAdapter.writeCharacters("false", 5);
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
				outputAdapter.writeCharacters("null", 4);
				return;
			}
		}
	}

	void dump(const JsonObject& val) {
		switch (val.theType) {
			case ValueType::Object: {
				if (val.theValue.object->empty()) {
					outputAdapter.writeCharacters("{}", 2);
					return;
				}
				outputAdapter.writeCharacter('{');
				auto i = val.theValue.object->cbegin();
				for (std::size_t cnt = 0; cnt < val.theValue.object->size() - 1; ++cnt, ++i) {
					outputAdapter.writeCharacter('\"');
					dumpEscaped(i->first, false);
					outputAdapter.writeCharacters("\":", 2);
					dump(i->second);
					outputAdapter.writeCharacter(',');
				}

				outputAdapter.writeCharacter('\"');
				dumpEscaped(i->first, false);
				outputAdapter.writeCharacters("\":", 2);
				dump(i->second);

				outputAdapter.writeCharacter('}');
			}

				return;

			case ValueType::Array: {
				if (val.theValue.array->empty()) {
					outputAdapter.writeCharacters("[]", 2);
					return;
				}

				outputAdapter.writeCharacter('[');
				for (auto i = val.theValue.array->cbegin(); i != val.theValue.array->cend() - 1; ++i) {
					dump(*i);
					outputAdapter.writeCharacter(',');
				}

				dump(val.theValue.array->back());

				outputAdapter.writeCharacter(']');

				return;
			}

			case ValueType::String: {
				outputAdapter.writeCharacter('\"');
				dumpEscaped(*val.theValue.string, false);
				outputAdapter.writeCharacter('\"');
				return;
			}

			case ValueType::Bool: {
				if (val.theValue.boolean) {
					outputAdapter.writeCharacters("true", 4);
				} else {
					outputAdapter.writeCharacters("false", 5);
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
				outputAdapter.writeCharacters("null", 4);
				return;
			}
		}
	}

	operator String() {
		return std::move(this->outputAdapter.operator DiscordCoreAPI::String());
	}

	void dumpEscaped(const StringType& s, const bool ensure_ascii) {
		std::uint32_t codepoint{};
		std::uint8_t state = UTF8_ACCEPT;
		std::size_t bytes = 0;

		std::size_t postAcceptBytes = 0;
		std::size_t nonDumpedCharacters = 0;

		for (std::size_t i = 0; i < s.size(); ++i) {
			const auto byte = static_cast<std::uint8_t>(s[i]);

			switch (decode(state, codepoint, byte)) {
				case UTF8_ACCEPT:
				{
					switch (codepoint) {
						case 0x08:
						{
							stringBuffer[bytes++] = '\\';
							stringBuffer[bytes++] = 'b';
							break;
						}

						case 0x09:
						{
							stringBuffer[bytes++] = '\\';
							stringBuffer[bytes++] = 't';
							break;
						}

						case 0x0A:
						{
							stringBuffer[bytes++] = '\\';
							stringBuffer[bytes++] = 'n';
							break;
						}

						case 0x0C:
						{
							stringBuffer[bytes++] = '\\';
							stringBuffer[bytes++] = 'f';
							break;
						}

						case 0x0D:
						{
							stringBuffer[bytes++] = '\\';
							stringBuffer[bytes++] = 'r';
							break;
						}

						case 0x22:
						{
							stringBuffer[bytes++] = '\\';
							stringBuffer[bytes++] = '\"';
							break;
						}

						case 0x5C:
						{
							stringBuffer[bytes++] = '\\';
							stringBuffer[bytes++] = '\\';
							break;
						}

						default: {
							if ((codepoint <= 0x1F) || (ensure_ascii && (codepoint >= 0x7F))) {
								if (codepoint <= 0xFFFF) {
									static_cast<void>((std::snprintf)(stringBuffer.data() + bytes, 7, "\\u%04x", static_cast<std::uint16_t>(codepoint)));
									bytes += 6;
								} else {
									static_cast<void>((std::snprintf)(stringBuffer.data() + bytes, 13, "\\u%04x\\u%04x", static_cast<std::uint16_t>(0xD7C0u + (codepoint >> 10u)),
										static_cast<std::uint16_t>(0xDC00u + (codepoint & 0x3FFu))));
									bytes += 12;
								}
							} else {
								stringBuffer[bytes++] = s[i];
							}
							break;
						}
					}

					if (stringBuffer.size() - bytes < 13) {
						outputAdapter.writeCharacters(stringBuffer.data(), bytes);
						bytes = 0;
					}

					postAcceptBytes = bytes;
					nonDumpedCharacters = 0;
					break;
				}

				default:
				{
					if (!ensure_ascii) {
						stringBuffer[bytes++] = s[i];
					}
					++nonDumpedCharacters;
					break;
				}
			}
		}

		if (bytes > 0) {
			outputAdapter.writeCharacters(stringBuffer.data(), bytes);
		}
	}

  private:

	inline unsigned int countDigits(UintType x) noexcept {
		unsigned int digitCount = 1;
		for (;;) {
			if (x < 10) {
				return digitCount;
			}
			if (x < 100) {
				return digitCount + 1;
			}
			if (x < 1000) {
				return digitCount + 2;
			}
			if (x < 10000) {
				return digitCount + 3;
			}
			x = x / 10000u;
			digitCount += 4;
		}
	}

	template<typename NumberType, std::enable_if_t<std::is_signed<NumberType>::value, int> = 0> bool is_negative_number(NumberType x) {
		return x < 0;
	}
	template<typename NumberType, std::enable_if_t<std::is_unsigned<NumberType>::value, int> = 0> bool is_negative_number(NumberType /*unused*/) {
		return false;
	}
	
	template<typename NumberType,
		std::enable_if_t<std::is_integral<NumberType>::value || std::is_same<NumberType, UintType>::value || std::is_same<NumberType, IntType>::value, int> = 0>
	void dumpInt(NumberType x) {
		static constexpr std::array<std::array<char, 2>, 100> digits_to_99{ {
			{ { '0', '0' } },
			{ { '0', '1' } },
			{ { '0', '2' } },
			{ { '0', '3' } },
			{ { '0', '4' } },
			{ { '0', '5' } },
			{ { '0', '6' } },
			{ { '0', '7' } },
			{ { '0', '8' } },
			{ { '0', '9' } },
			{ { '1', '0' } },
			{ { '1', '1' } },
			{ { '1', '2' } },
			{ { '1', '3' } },
			{ { '1', '4' } },
			{ { '1', '5' } },
			{ { '1', '6' } },
			{ { '1', '7' } },
			{ { '1', '8' } },
			{ { '1', '9' } },
			{ { '2', '0' } },
			{ { '2', '1' } },
			{ { '2', '2' } },
			{ { '2', '3' } },
			{ { '2', '4' } },
			{ { '2', '5' } },
			{ { '2', '6' } },
			{ { '2', '7' } },
			{ { '2', '8' } },
			{ { '2', '9' } },
			{ { '3', '0' } },
			{ { '3', '1' } },
			{ { '3', '2' } },
			{ { '3', '3' } },
			{ { '3', '4' } },
			{ { '3', '5' } },
			{ { '3', '6' } },
			{ { '3', '7' } },
			{ { '3', '8' } },
			{ { '3', '9' } },
			{ { '4', '0' } },
			{ { '4', '1' } },
			{ { '4', '2' } },
			{ { '4', '3' } },
			{ { '4', '4' } },
			{ { '4', '5' } },
			{ { '4', '6' } },
			{ { '4', '7' } },
			{ { '4', '8' } },
			{ { '4', '9' } },
			{ { '5', '0' } },
			{ { '5', '1' } },
			{ { '5', '2' } },
			{ { '5', '3' } },
			{ { '5', '4' } },
			{ { '5', '5' } },
			{ { '5', '6' } },
			{ { '5', '7' } },
			{ { '5', '8' } },
			{ { '5', '9' } },
			{ { '6', '0' } },
			{ { '6', '1' } },
			{ { '6', '2' } },
			{ { '6', '3' } },
			{ { '6', '4' } },
			{ { '6', '5' } },
			{ { '6', '6' } },
			{ { '6', '7' } },
			{ { '6', '8' } },
			{ { '6', '9' } },
			{ { '7', '0' } },
			{ { '7', '1' } },
			{ { '7', '2' } },
			{ { '7', '3' } },
			{ { '7', '4' } },
			{ { '7', '5' } },
			{ { '7', '6' } },
			{ { '7', '7' } },
			{ { '7', '8' } },
			{ { '7', '9' } },
			{ { '8', '0' } },
			{ { '8', '1' } },
			{ { '8', '2' } },
			{ { '8', '3' } },
			{ { '8', '4' } },
			{ { '8', '5' } },
			{ { '8', '6' } },
			{ { '8', '7' } },
			{ { '8', '8' } },
			{ { '8', '9' } },
			{ { '9', '0' } },
			{ { '9', '1' } },
			{ { '9', '2' } },
			{ { '9', '3' } },
			{ { '9', '4' } },
			{ { '9', '5' } },
			{ { '9', '6' } },
			{ { '9', '7' } },
			{ { '9', '8' } },
			{ { '9', '9' } },
		} };


		if (x == 0) {
			outputAdapter.writeCharacter('0');
			return;
		}

		
		auto buffer_ptr = numberBuffer.begin();

		UintType abs_value;

		unsigned int n_chars{};

		if (is_negative_number(x)) {
			*buffer_ptr = '-';
			abs_value = removeSign(static_cast<IntType>(x));

			n_chars = 1 + countDigits(abs_value);
		} else {
			abs_value = static_cast<UintType>(x);
			n_chars = countDigits(abs_value);
		}

		buffer_ptr += n_chars;

		while (abs_value >= 100) {
			const auto digits_index = static_cast<unsigned>((abs_value % 100));
			abs_value /= 100;
			*(--buffer_ptr) = digits_to_99[digits_index][1];
			*(--buffer_ptr) = digits_to_99[digits_index][0];
		}

		if (abs_value >= 10) {
			const auto digits_index = static_cast<unsigned>(abs_value);
			*(--buffer_ptr) = digits_to_99[digits_index][1];
			*(--buffer_ptr) = digits_to_99[digits_index][0];
		} else {
			*(--buffer_ptr) = static_cast<char>('0' + abs_value);
		}

		outputAdapter.writeCharacters(numberBuffer.data(), n_chars);
	}

	void dumpFloat(FloatType x) {
		if (!std::isfinite(x)) {
			outputAdapter.writeCharacters("null", 4);
			return;
		}

		static constexpr bool is_ieee_single_or_double =
			(std::numeric_limits<FloatType>::is_iec559 && std::numeric_limits<FloatType>::digits == 24 && std::numeric_limits<FloatType>::max_exponent == 128) ||
			(std::numeric_limits<FloatType>::is_iec559 && std::numeric_limits<FloatType>::digits == 53 && std::numeric_limits<FloatType>::max_exponent == 1024);
	}


	void dumpFloat(FloatType x, std::false_type ) {
		static constexpr auto d = std::numeric_limits<FloatType>::max_digits10;

		std::ptrdiff_t len = (std::snprintf)(numberBuffer.data(), numberBuffer.size(), "%.*g", d, x);

		if (thousandsSeparator != '\0') {
			const auto end = std::remove(numberBuffer.begin(), numberBuffer.begin() + len, thousandsSeparator);
			std::fill(end, numberBuffer.end(), '\0');
			len = (end - numberBuffer.begin());
		}

		if (decimalPoint != '\0' && decimalPoint != '.') {
			const auto dec_pos = std::find(numberBuffer.begin(), numberBuffer.end(), decimalPoint);
			if (dec_pos != numberBuffer.end()) {
				*dec_pos = '.';
			}
		}

		outputAdapter.writeCharacters(numberBuffer.data(), static_cast<std::size_t>(len));

		const bool value_is_int_like = std::none_of(numberBuffer.begin(), numberBuffer.begin() + len + 1, [](char c) {
			return c == '.' || c == 'e';
		});

		if (value_is_int_like) {
			outputAdapter.writeCharacters(".0", 2);
		}
	}

	static std::uint8_t decode(std::uint8_t& state, std::uint32_t& codep, const std::uint8_t byte) noexcept {
		static const std::array<std::uint8_t, 400> utf8d = { {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
			8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
			0xA, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x3,
			0xB, 0x6, 0x6, 0x6, 0x5, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
			0x0, 0x1, 0x2, 0x3, 0x5, 0x8, 0x7, 0x1, 0x1, 0x1, 0x4, 0x6, 0x1, 0x1, 0x1, 0x1,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1,
			1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1,
			1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
		} };

		const std::uint8_t type = utf8d[byte];

		codep = (state != UTF8_ACCEPT) ? (byte & 0x3fu) | (codep << 6u) : (0xFFu >> type) & (byte);

		const std::size_t index = 256u + static_cast<size_t>(state) * 16u + static_cast<size_t>(type);
		state = utf8d[index];
		return state;
	}

	UintType removeSign(UintType x) {
		return x;
	}

	inline UintType removeSign(IntType x) noexcept {
		return static_cast<UintType>(-(x + 1)) + 1;
	}

  private:
	OutputStringAdapter outputAdapter{};

	std::array<char, 64> numberBuffer{};
	const char thousandsSeparator{ '\0' };
	const char decimalPoint{ '\0' };
		const std::lconv* loc{ nullptr };
		std::array<char, 512> stringBuffer{};
		const char indentChar{};
};
;
	
	

#endif// !ERL_PACKER