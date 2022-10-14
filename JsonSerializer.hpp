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

	void writeCharacters(const char* C, size_t length, String&);

	void writeCharacter(char C, String&);

	Void destroy() noexcept;

	friend bool operator==(const JsonObject&, const JsonObject&);
	
	void dump(String& theString);

	operator String() noexcept;

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
	JsonSerializer(JsonSerializer&) = delete;
	JsonSerializer& operator=(const JsonSerializer&) = delete;
	JsonSerializer(JsonSerializer&&) = delete;
	JsonSerializer& operator=(JsonSerializer&&) = delete;
	~JsonSerializer() = default;

	void dump() {
		switch (this->theObject.theType) {
			case ValueType::Object: {
				if (this->theObject.theValue.object->empty()) {
					o->writeCharacters("{}", 2);
					return;
				}
				o->writeCharacter('{');

				// first n-1 elements
				auto i = this->theObject.theValue.object->cbegin();
				for (std::size_t cnt = 0; cnt < this->theObject.theValue.object->size() - 1; ++cnt, ++i) {
					o->writeCharacter('\"');
					dumpEscaped(i->first, false);
					o->writeCharacters("\":", 2);
					dump(i->second);
					o->writeCharacter(',');
				}

				o->writeCharacter('\"');
				dumpEscaped(i->first, false);
				o->writeCharacters("\":", 2);
				dump(i->second);

				o->writeCharacter('}');
			}

				return;

			case ValueType::Array: {
				if (this->theObject.theValue.array->empty()) {
					o->writeCharacters("[]", 2);
					return;
				}

				o->writeCharacter('[');

				// first n-1 elements
				for (auto i = this->theObject.theValue.array->cbegin(); i != this->theObject.theValue.array->cend() - 1; ++i) {
					dump(*i);
					o->writeCharacter(',');
				}

				// last element
				dump(this->theObject.theValue.array->back());

				o->writeCharacter(']');

				return;
			}

			case ValueType::String: {
				o->writeCharacter('\"');
				dumpEscaped(*this->theObject.theValue.string, false);
				o->writeCharacter('\"');
				return;
			}

			case ValueType::Bool: {
				if (this->theObject.theValue.boolean) {
					o->writeCharacters("true", 4);
				} else {
					o->writeCharacters("false", 5);
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
				o->writeCharacters("null", 4);
				return;
			}
		}
	}

	void dump(const JsonObject& val) {
		switch (val.theType) {
			case ValueType::Object: {
				if (val.theValue.object->empty()) {
					o->writeCharacters("{}", 2);
					return;
				}
				o->writeCharacter('{');

				// first n-1 elements
				auto i = val.theValue.object->cbegin();
				for (std::size_t cnt = 0; cnt < val.theValue.object->size() - 1; ++cnt, ++i) {
					o->writeCharacter('\"');
					dumpEscaped(i->first, false);
					o->writeCharacters("\":", 2);
					dump(i->second);
					o->writeCharacter(',');
				}

				o->writeCharacter('\"');
				dumpEscaped(i->first, false);
				o->writeCharacters("\":", 2);
				dump(i->second);

				o->writeCharacter('}');
			}

				return;

			case ValueType::Array: {
				if (val.theValue.array->empty()) {
					o->writeCharacters("[]", 2);
					return;
				}

				o->writeCharacter('[');

				// first n-1 elements
				for (auto i = val.theValue.array->cbegin(); i != val.theValue.array->cend() - 1; ++i) {
					dump(*i);
					o->writeCharacter(',');
				}

				// last element
				dump(val.theValue.array->back());

				o->writeCharacter(']');

				return;
			}

			case ValueType::String: {
				o->writeCharacter('\"');
				dumpEscaped(*val.theValue.string, false);
				o->writeCharacter('\"');
				return;
			}

			case ValueType::Bool: {
				if (val.theValue.boolean) {
					o->writeCharacters("true", 4);
				} else {
					o->writeCharacters("false", 5);
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
				o->writeCharacters("null", 4);
				return;
			}
		}
	}

	operator String() {
		return std::move(this->o->operator DiscordCoreAPI::String());
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
						o->writeCharacters(stringBuffer.data(), bytes);
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
			o->writeCharacters(stringBuffer.data(), bytes);
		}
	}

  private:
	/*!
    @brief count digits
    Count the number of decimal (base 10) digits for an input unsigned integer.
    @param[in] x  unsigned integer number to count its digits
    @return    number of decimal digits
    */
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

	// templates to avoid warnings about useless casts
	template<typename NumberType, std::enable_if_t<std::is_signed<NumberType>::value, int> = 0> bool is_negative_number(NumberType x) {
		return x < 0;
	}
	template<typename NumberType, std::enable_if_t<std::is_unsigned<NumberType>::value, int> = 0> bool is_negative_number(NumberType /*unused*/) {
		return false;
	}
	/*!
    @brief dump an integer
    Dump a given integer to output stream @a o. Works internally with
    @a numberBuffer.
    @param[in] x  integer number (signed or unsigned) to dump
    @tparam NumberType either @a IntType or @a UintType
    */
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


		// special case for "0"
		if (x == 0) {
			o->writeCharacter('0');
			return;
		}

		// use a pointer to fill the buffer
		auto buffer_ptr = numberBuffer.begin();// NOLINT(llvm-qualified-auto,readability-qualified-auto,cppcoreguidelines-pro-type-vararg,hicpp-vararg)

		UintType abs_value;

		unsigned int n_chars{};

		if (is_negative_number(x)) {
			*buffer_ptr = '-';
			abs_value = removeSign(static_cast<IntType>(x));

			// account one more byte for the minus sign
			n_chars = 1 + countDigits(abs_value);
		} else {
			abs_value = static_cast<UintType>(x);
			n_chars = countDigits(abs_value);
		}

	

		// jump to the end to generate the string from backward,
		// so we later avoid reversing the result
		buffer_ptr += n_chars;

		// Fast int2ascii implementation inspired by "Fastware" talk by Andrei Alexandrescu
		// See: https://www.youtube.com/watch?v=o4-CwDo2zpg
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

		o->writeCharacters(numberBuffer.data(), n_chars);
	}

	/*!
    @brief dump a floating-point number
    Dump a given floating-point number to output stream @a o. Works internally
    with @a numberBuffer.
    @param[in] x  floating-point number to dump
    */
	void dumpFloat(FloatType x) {
		// NaN / inf
		if (!std::isfinite(x)) {
			o->writeCharacters("null", 4);
			return;
		}

		// If FloatType is an IEEE-754 single or double precision number,
		// use the Grisu2 algorithm to produce short numbers which are
		// guaranteed to round-trip, using strtof and strtod, resp.
		//
		// NB: The test below works if <long double> == <double>.
		static constexpr bool is_ieee_single_or_double =
			(std::numeric_limits<FloatType>::is_iec559 && std::numeric_limits<FloatType>::digits == 24 && std::numeric_limits<FloatType>::max_exponent == 128) ||
			(std::numeric_limits<FloatType>::is_iec559 && std::numeric_limits<FloatType>::digits == 53 && std::numeric_limits<FloatType>::max_exponent == 1024);
	}


	void dumpFloat(FloatType x, std::false_type /*is_ieee_single_or_double*/) {
		// get number of digits for a float -> text -> float round-trip
		static constexpr auto d = std::numeric_limits<FloatType>::max_digits10;

		// the actual conversion
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
		std::ptrdiff_t len = (std::snprintf)(numberBuffer.data(), numberBuffer.size(), "%.*g", d, x);

		// negative value indicates an error

		// erase thousands separator
		if (thousandsSeparator != '\0') {
			// NOLINTNEXTLINE(readability-qualified-auto,llvm-qualified-auto): std::remove returns an iterator, see https://github.com/nlohmann/json/issues/3081
			const auto end = std::remove(numberBuffer.begin(), numberBuffer.begin() + len, thousandsSeparator);
			std::fill(end, numberBuffer.end(), '\0');
			len = (end - numberBuffer.begin());
		}

		// convert decimal point to '.'
		if (decimalPoint != '\0' && decimalPoint != '.') {
			// NOLINTNEXTLINE(readability-qualified-auto,llvm-qualified-auto): std::find returns an iterator, see https://github.com/nlohmann/json/issues/3081
			const auto dec_pos = std::find(numberBuffer.begin(), numberBuffer.end(), decimalPoint);
			if (dec_pos != numberBuffer.end()) {
				*dec_pos = '.';
			}
		}

		o->writeCharacters(numberBuffer.data(), static_cast<std::size_t>(len));

		// determine if we need to append ".0"
		const bool value_is_int_like = std::none_of(numberBuffer.begin(), numberBuffer.begin() + len + 1, [](char c) {
			return c == '.' || c == 'e';
		});

		if (value_is_int_like) {
			o->writeCharacters(".0", 2);
		}
	}

	/*!
    @brief check whether a string is UTF-8 encoded
    The function checks each byte of a string whether it is UTF-8 encoded. The
    result of the check is stored in the @a state parameter. The function must
    be called initially with state 0 (accept). State 1 means the string must
    be rejected, because the current byte is not allowed. If the string is
    completely processed, but the state is non-zero, the string ended
    prematurely; that is, the last byte indicated more bytes should have
    followed.
    @param[in,out] state  the state of the decoding
    @param[in,out] codep  codepoint (valid only if resulting state is UTF8_ACCEPT)
    @param[in] byte       next byte to decode
    @return               new state
    @note The function has been edited: a Array is used.
    @copyright Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
    @sa http://bjoern.hoehrmann.de/utf-8/decoder/dfa/
    */
	static std::uint8_t decode(std::uint8_t& state, std::uint32_t& codep, const std::uint8_t byte) noexcept {
		static const std::array<std::uint8_t, 400> utf8d = { {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,// 00..1F
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,// 20..3F
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,// 40..5F
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,// 60..7F
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,// 80..9F
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,// A0..BF
			8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,// C0..DF
			0xA, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x3,// E0..EF
			0xB, 0x6, 0x6, 0x6, 0x5, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,// F0..FF
			0x0, 0x1, 0x2, 0x3, 0x5, 0x8, 0x7, 0x1, 0x1, 0x1, 0x4, 0x6, 0x1, 0x1, 0x1, 0x1,// s0..s0
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1,// s1..s2
			1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1,// s3..s4
			1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1,// s5..s6
			1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1// s7..s8
		} };

		const std::uint8_t type = utf8d[byte];

		codep = (state != UTF8_ACCEPT) ? (byte & 0x3fu) | (codep << 6u) : (0xFFu >> type) & (byte);

		const std::size_t index = 256u + static_cast<size_t>(state) * 16u + static_cast<size_t>(type);
		state = utf8d[index];
		return state;
	}

	/*
     * Overload to make the compiler happy while it is instantiating
     * dumpInt for UintType.
     * Must never be called.
     */
	UintType removeSign(UintType x) {
		return x;// LCOV_EXCL_LINE
	}

	/*
     * Helper function for dumpInt
     *
     * This function takes a negative signed integer and returns its absolute
     * value as unsigned integer. The plus/minus shuffling is necessary as we can
     * not directly remove the sign of an arbitrary signed integer as the
     * absolute values of INT_MIN and INT_MAX are usually not the same. See
     * #1708 for details.
     */
	inline UintType removeSign(IntType x) noexcept {
		return static_cast<UintType>(-(x + 1)) + 1;
	}

  private:
	/// the output of the JsonSerializer
	std::unique_ptr<OutputStringAdapter> o{ std::make_unique<OutputStringAdapter>() };

	/// a (hopefully) large enough character buffer
	std::array<char, 64> numberBuffer{};
	/// the locale's thousand separator character
	const char thousandsSeparator{ '\0' };
	/// the locale's decimal point character
	const char decimalPoint{ '\0' };
		const std::lconv* loc{ nullptr };
		/// string buffer
		std::array<char, 512> stringBuffer{};

		/// the indentation character
		const char indentChar{};

		/// error_handler how to react on decoding errors
};
;
	
	

#endif// !ERL_PACKER