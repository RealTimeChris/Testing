#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)

#include <source_location>
#include <unordered_map>
#include <immintrin.h>
#include <string_view>
#include <functional>
#include <concepts>
#include <iostream>
#include <vector>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <bitset>
#include <array>
#include <deque>
#include <map>

#ifdef _WIN32
	#ifdef Jsonifier_EXPORTS
		#define Jsonifier_Dll __declspec(dllexport)
	#else
		#define Jsonifier_Dll __declspec(dllimport)
	#endif
#else
	#define Jsonifier_Dll
#endif

namespace Jsonifier {

	struct DCAException : public std::runtime_error, std::string {
		inline DCAException(const std::string&, std::source_location = std::source_location::current()) noexcept;
	};

	DCAException::DCAException(const std::string& error, std::source_location location) noexcept : std::runtime_error(error) {
		std::stringstream stream{};
		stream << "Error Report: \n"
			   << "Thrown From: " << location.file_name() << " (" << std::to_string(location.line()) << ":" << std::to_string(location.column())
			   << ")"
			   << "\nThe Error: " << error << std::endl
			   << std::endl;
		*static_cast<std::runtime_error*>(this) = std::runtime_error{ stream.str() };
	}

	template<typename RTy> void reverseByteOrder(RTy& net) {
		if constexpr (std::endian::native == std::endian::little) {
			switch (sizeof(RTy)) {
				case 1: {
					return;
				}
				case 2: {
					net =
						_mm256_extract_epi16(_mm256_shuffle_epi8(_mm256_insert_epi16(__m256i{}, net, 0), _mm256_insert_epi16(__m256i{}, 0x01, 0)), 0);
				}
				case 4: {
					net = _mm256_extract_epi32(
						_mm256_shuffle_epi8(_mm256_insert_epi32(__m256i{}, net, 0), _mm256_insert_epi32(__m256i{}, 0x10203, 0)), 0);
				}
				case 8: {
					net = _mm256_extract_epi64(
						_mm256_shuffle_epi8(_mm256_insert_epi64(__m256i{}, net, 0), _mm256_insert_epi64(__m256i{}, 0x102030405060708, 0)), 0);
				}
				default:
					return;
			}
		}
	}

	template<typename RTy> void storeBits(char* to, RTy num) {
		uint8_t byteSize{ 8 };
		reverseByteOrder<RTy>(num);
		for (uint32_t x = 0; x < sizeof(RTy); ++x) {
			to[x] = static_cast<uint8_t>(num >> (byteSize * x));
		}
	}

	template<typename TTy> class StopWatch {
	  public:
		using HRClock = std::chrono::high_resolution_clock;

		StopWatch() = delete;

		StopWatch<TTy>& operator=(const StopWatch<TTy>& data) {
			this->maxNumberOfMs.store(data.maxNumberOfMs.load());
			this->startTime.store(data.startTime.load());
			return *this;
		}

		StopWatch(const StopWatch<TTy>& data) {
			*this = data;
		}

		StopWatch(TTy maxNumberOfMsNew) {
			this->maxNumberOfMs.store(maxNumberOfMsNew);
			this->startTime.store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
		}

		TTy totalTimePassed() {
			TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
			TTy elapsedTime = currentTime - this->startTime.load();
			return elapsedTime;
		}

		TTy getTotalWaitTime() {
			return this->maxNumberOfMs.load();
		}

		bool hasTimePassed() {
			TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
			TTy elapsedTime = currentTime - this->startTime.load();
			if (elapsedTime >= this->maxNumberOfMs.load()) {
				return true;
			} else {
				return false;
			}
		}

		void resetTimer() {
			this->startTime.store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
		}

	  protected:
		std::atomic<TTy> maxNumberOfMs{ TTy{ 0 } };
		std::atomic<TTy> startTime{ TTy{ 0 } };
	};

	constexpr uint8_t formatVersion{ 131 };

	enum class EtfType : uint8_t {
		New_Float_Ext = 70,
		Small_Integer_Ext = 97,
		Integer_Ext = 98,
		Atom_Ext = 100,
		Nil_Ext = 106,
		String_Ext = 107,
		List_Ext = 108,
		Binary_Ext = 109,
		Small_Big_Ext = 110,
		Small_Atom_Ext = 115,
		Map_Ext = 116,
	};

	template<typename Ty>
	concept IsEnum = std::is_enum<Ty>::value;

	struct EnumConverter {
		template<IsEnum EnumType> EnumConverter& operator=(std::vector<EnumType> data) {
			for (auto& value: data) {
				this->vector.emplace_back(std::move(static_cast<uint64_t>(value)));
			}
			return *this;
		};

		template<IsEnum EnumType> EnumConverter(std::vector<EnumType> data) {
			*this = data;
		};

		template<IsEnum EnumType> EnumConverter& operator=(EnumType data) {
			this->integer = static_cast<uint64_t>(data);
			return *this;
		};

		template<IsEnum EnumType> EnumConverter(EnumType data) {
			*this = data;
		};

		operator std::vector<uint64_t>() const noexcept;

		operator uint64_t() const noexcept;

		bool isItAVector() const noexcept;

	  protected:
		std::vector<uint64_t> vector{};
		bool vectorType{ false };
		uint64_t integer{};
	};

	enum class JsonType : int8_t { Object = 1, Array = 2, String = 3, Float = 4, Uint64 = 5, Int64 = 6, Bool = 7, Null = 8 };

	enum class JsonifierSerializeType { Etf = 0, Json = 1 };

	enum class ErrorCode : int8_t {
		Empty = 0,
		TapeError = 1,
		DepthError = 2,
		Success = 3,
		ParseError = 4,
		StringError = 5,
		TAtomError = 6,
		FAtomError = 7,
		NAtomError = 8,
		MemAlloc = 9,
		InvalidNumber = 10
	};

	class Jsonifier;

	class JsonSerializer;

	template<typename Ty>
	concept IsConvertibleToJsonifier = std::convertible_to<Ty, Jsonifier>;

	template<typename Ty>
	concept IsConvertibleToJsonSerializer = std::convertible_to<Ty, JsonSerializer>;

	class Jsonifier {
	  public:
		friend class JsonSerializer;
		using MapAllocatorType = std::allocator<std::pair<const std::string, Jsonifier>>;
		template<typename OTy> using AllocatorType = std::allocator<OTy>;
		template<typename OTy> using AllocatorTraits = std::allocator_traits<AllocatorType<OTy>>;
		using ObjectType = std::map<std::string, Jsonifier, std::less<>, MapAllocatorType>;
		using ArrayType = std::vector<Jsonifier, AllocatorType<Jsonifier>>;
		using StringType = std::string;
		using FloatType = double;
		using UintType = uint64_t;
		using IntType = int64_t;
		using BoolType = bool;

		union JsonValue {
			JsonValue() noexcept = default;
			JsonValue& operator=(JsonValue&&) noexcept = delete;
			JsonValue(JsonValue&&) noexcept = delete;
			JsonValue& operator=(const JsonValue&) noexcept = delete;
			JsonValue(const JsonValue&) noexcept = delete;
			ObjectType* object;
			StringType* string;
			ArrayType* array;
			FloatType numberDouble;
			UintType numberUint;
			IntType numberInt;
			BoolType boolean;
		};

		Jsonifier() noexcept = default;

		template<IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::vector<OTy>&& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& value: data) {
				this->jsonValue.array->push_back(std::move(value));
			}
			return *this;
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier(std::vector<OTy>&& data) noexcept {
			*this = std::move(data);
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::vector<OTy>& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& value: data) {
				this->jsonValue.array->push_back(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier(std::vector<OTy>& data) noexcept {
			*this = data;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::unordered_map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = std::move(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::unordered_map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::unordered_map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = value;
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::unordered_map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = std::move(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = value;
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsEnum Ty> Jsonifier& operator=(Ty data) noexcept {
			this->jsonValue.numberUint = static_cast<uint64_t>(data);
			this->type = JsonType::Uint64;
			return *this;
		}

		template<IsEnum Ty> Jsonifier(Ty data) noexcept {
			*this = data;
		}

		Jsonifier& operator=(ErrorCode data);

		Jsonifier(ErrorCode data);

		Jsonifier& operator=(ObjectType&& data) noexcept;

		Jsonifier(ObjectType&& data) noexcept;

		Jsonifier& operator=(const ObjectType& data) noexcept;

		Jsonifier(const ObjectType& data) noexcept;

		Jsonifier& operator=(Jsonifier&& data) noexcept;

		Jsonifier(Jsonifier&& data) noexcept;

		Jsonifier& operator=(const Jsonifier& data) noexcept;

		Jsonifier(const Jsonifier& data) noexcept;

		operator std::string_view() noexcept;

		void refreshString(JsonifierSerializeType OpCode);

		bool contains(std::string& key);

		Jsonifier& operator=(EnumConverter&& data) noexcept;
		Jsonifier(EnumConverter&& data) noexcept;

		Jsonifier& operator=(const EnumConverter& data) noexcept;
		Jsonifier(const EnumConverter& data) noexcept;

		Jsonifier& operator=(std::string&& data) noexcept;
		Jsonifier(std::string&& data) noexcept;

		Jsonifier& operator=(const std::string& data) noexcept;
		Jsonifier(const std::string& data) noexcept;

		Jsonifier& operator=(std::string_view&& data) noexcept;
		Jsonifier(std::string_view&& data) noexcept;

		Jsonifier& operator=(std::string_view& data) noexcept;
		Jsonifier(std::string_view& data) noexcept;

		Jsonifier& operator=(const char* data) noexcept;
		Jsonifier(const char* data) noexcept;

		Jsonifier& operator=(double data) noexcept;
		Jsonifier(double data) noexcept;

		Jsonifier& operator=(float data) noexcept;
		Jsonifier(float data) noexcept;

		Jsonifier& operator=(uint64_t data) noexcept;
		Jsonifier(uint64_t data) noexcept;

		Jsonifier& operator=(uint32_t data) noexcept;
		Jsonifier(uint32_t data) noexcept;

		Jsonifier& operator=(uint16_t data) noexcept;
		Jsonifier(uint16_t data) noexcept;

		Jsonifier& operator=(uint8_t data) noexcept;
		Jsonifier(uint8_t data) noexcept;

		Jsonifier& operator=(int64_t data) noexcept;
		Jsonifier(int64_t data) noexcept;

		Jsonifier& operator=(int32_t data) noexcept;
		Jsonifier(int32_t data) noexcept;

		Jsonifier& operator=(int16_t data) noexcept;
		Jsonifier(int16_t data) noexcept;

		Jsonifier& operator=(int8_t data) noexcept;
		Jsonifier(int8_t data) noexcept;

		Jsonifier& operator=(bool data) noexcept;
		Jsonifier(bool data) noexcept;

		Jsonifier& operator=(JsonType TypeNew) noexcept;
		Jsonifier(JsonType type) noexcept;

		Jsonifier& operator=(std::nullptr_t) noexcept;
		Jsonifier(std::nullptr_t data) noexcept;

		Jsonifier& operator[](typename ObjectType::key_type key);

		Jsonifier& operator[](uint64_t index);

		template<typename Ty> Ty getValue() {
			switch (this->type) {
				case JsonType::Object: {
					if (!std::is_same<decltype(this), Ty>) {
						throw DCAException{ "Sorry, but this is not that type!" };
					} else {
						return *this->jsonValue.object;
					}
				}
				case JsonType::Array: {
					if (!std::is_same<decltype(this), Ty>) {
						throw DCAException{ "Sorry, but this is not that type!" };
					} else {
						return *this->jsonValue.array;
					}
				}
				case JsonType::String: {
					if (!std::is_same<decltype(this), Ty>) {
						throw DCAException{ "Sorry, but this is not that type!" };
					} else {
						return *this->jsonValue.string;
					}
				}
				case JsonType::Float: {
					if (!std::is_same<decltype(this), Ty>) {
						throw DCAException{ "Sorry, but this is not that type!" };
					} else {
						return this->jsonValue.numberDouble;
					}
				}
				case JsonType::Uint64: {
					if (!std::is_same<decltype(this), Ty>) {
						throw DCAException{ "Sorry, but this is not that type!" };
					} else {
						return this->jsonValue.numberUint;
					}
				}
				case JsonType::Int64: {
					if (!std::is_same<decltype(this), Ty>) {
						throw DCAException{ "Sorry, but this is not that type!" };
					} else {
						return this->jsonValue.numberInt;
					}
				}
				case JsonType::Bool: {
					if (!std::is_same<decltype(this), Ty>) {
						throw DCAException{ "Sorry, but this is not that type!" };
					} else {
						return this->jsonValue.boolean;
					}
				}
			}
		}

		JsonType getType() noexcept;

		size_t size() noexcept;

		Jsonifier& emplaceBack(Jsonifier&& data) noexcept;
		Jsonifier& emplaceBack(Jsonifier& data) noexcept;

		~Jsonifier() noexcept;

	  protected:
		JsonType type{ JsonType::Null };
		JsonValue jsonValue{};
		std::string string{};

		void serializeJsonToEtfString(const Jsonifier* jsonData);

		void serializeJsonToJsonString(const Jsonifier* jsonData);

		void writeJsonObject(const ObjectType& ObjectNew);

		void writeJsonArray(const ArrayType& Array);

		void writeJsonString(const StringType& StringNew);

		void writeJsonFloat(const FloatType x);

		template<typename NumberType,
			std::enable_if_t<
				std::is_integral<NumberType>::value || std::is_same<NumberType, uint64_t>::value || std::is_same<NumberType, int64_t>::value, int> =
				0>
		void writeJsonInt(NumberType Int) {
			auto IntNew = std::to_string(Int);
			this->writeString(IntNew.data(), IntNew.size());
		}

		void writeJsonBool(const BoolType ValueNew);

		void writeJsonNull();

		void writeEtfObject(const ObjectType& jsonData);

		void writeEtfArray(const ArrayType& jsonData);

		void writeEtfString(const StringType& jsonData);

		void writeEtfUint(const UintType jsonData);

		void writeEtfInt(const IntType jsonData);

		void writeEtfFloat(const FloatType jsonData);

		void writeEtfBool(const BoolType jsonData);

		void writeEtfNull();

		void writeString(const char* data, std::size_t tapeLength);

		void writeCharacter(const char Char);

		void appendBinaryExt(std::string_view bytes, uint32_t sizeNew);

		void appendUnsignedLongLong(const uint64_t value);

		void appendNewFloatExt(const double FloatValue);

		void appendSmallIntegerExt(uint8_t value);

		void appendListHeader(const uint32_t sizeNew);

		void appendMapHeader(const uint32_t sizeNew);

		void appendIntegerExt(const uint32_t value);

		void appendBool(bool data);

		void appendVersion();

		void appendNilExt();

		void appendNil();

		void setValue(JsonType TypeNew);

		void destroy() noexcept;
	};

	template<> inline Jsonifier::ObjectType Jsonifier::getValue() {
		return std::move(*this->jsonValue.object);
	}

	template<> inline Jsonifier::ArrayType Jsonifier::getValue() {
		return std::move(*this->jsonValue.array);
	}

	template<> inline Jsonifier::StringType Jsonifier::getValue() {
		return std::move(*this->jsonValue.string);
	}

	template<> inline Jsonifier::FloatType Jsonifier::getValue() {
		return this->jsonValue.numberDouble;
	}

	template<> inline Jsonifier::UintType Jsonifier::getValue() {
		return this->jsonValue.numberUint;
	}

	template<> inline Jsonifier::IntType Jsonifier::getValue() {
		return this->jsonValue.numberInt;
	}

	template<> inline Jsonifier::BoolType Jsonifier::getValue() {
		return this->jsonValue.boolean;
	}

	struct JsonifierException : public std::runtime_error, std::string {
		JsonifierException(const std::string&, std::source_location = std::source_location::current()) noexcept;
	};

	class SimdBase256;

	class SimdBase128 {
	  public:
		inline SimdBase128() noexcept = default;

		inline SimdBase128& operator=(char other) {
			this->value = _mm_set1_epi8(other);
			return *this;
		}

		inline SimdBase128(char other) {
			*this = other;
		}

		inline operator __m128i() {
			return this->value;
		}

	  protected:
		__m128i value{};
	};

	class SimdBase256 {
	  public:
		inline SimdBase256() noexcept {
			this->value = _mm256_set1_epi8(0x00);
		};


		inline SimdBase256& operator=(char other) {
			this->value = _mm256_set1_epi8(other);
			return *this;
		}

		inline SimdBase256(char other) {
			*this = other;
		}

		inline SimdBase256& operator=(const char* values) {
			*this = _mm256_loadu_epi8(values);
			return *this;
		}

		inline SimdBase256(const char* values) {
			*this = values;
		}

		inline SimdBase256(int64_t value00, int64_t value01, int64_t value02, int64_t value03) {
			this->value = _mm256_set_epi64x(value03, value02, value01, value00);
		}

		inline SimdBase256(uint64_t value00, uint64_t value01, uint64_t value02, uint64_t value03) {
			this->value = _mm256_set_epi64x(static_cast<int64_t>(value03), static_cast<int64_t>(value02), static_cast<int64_t>(value01),
				static_cast<int64_t>(value00));
		}

		inline SimdBase256& operator=(__m256i other) {
			this->value = other;
			return *this;
		}

		inline SimdBase256(__m256i other) {
			*this = other;
		}

		inline void store(char dst[32]) const {
			return _mm256_storeu_epi8(dst, this->value);
		}

		inline uint64_t getUint64(size_t index) {
			switch (index) {
				case 0: {
					return static_cast<uint64_t>(_mm256_extract_epi64(this->value, 0));
				}
				case 1: {
					return static_cast<uint64_t>(_mm256_extract_epi64(this->value, 1));
				}
				case 2: {
					return static_cast<uint64_t>(_mm256_extract_epi64(this->value, 2));
				}
				case 3: {
					return static_cast<uint64_t>(_mm256_extract_epi64(this->value, 3));
				}
				default: {
					return static_cast<uint64_t>(_mm256_extract_epi64(this->value, 0));
				}
			}
		}

		inline int64_t getInt64(size_t index) {
			switch (index) {
				case 0: {
					return _mm256_extract_epi64(this->value, 0);
				}
				case 1: {
					return _mm256_extract_epi64(this->value, 1);
				}
				case 2: {
					return _mm256_extract_epi64(this->value, 2);
				}
				case 3: {
					return _mm256_extract_epi64(this->value, 3);
				}
				default: {
					return _mm256_extract_epi64(this->value, 0);
				}
			}
		}

		inline void insertInt64(int64_t value, size_t index) {
			switch (index) {
				case 0: {
					*this = _mm256_insert_epi64(this->value, value, 0);
				}
				case 1: {
					*this = _mm256_insert_epi64(this->value, value, 1);
				}
				case 2: {
					*this = _mm256_insert_epi64(this->value, value, 2);
				}
				case 3: {
					*this = _mm256_insert_epi64(this->value, value, 3);
				}
				default: {
					*this = _mm256_insert_epi64(this->value, value, 0);
				}
			}
		}

		inline operator __m256i&() {
			return this->value;
		}

		inline SimdBase256 operator|(SimdBase256 other) {
			return _mm256_or_si256(this->value, other);
		}

		inline SimdBase256 operator&(SimdBase256 other) {
			return _mm256_and_si256(this->value, other);
		}

		inline SimdBase256 operator^(SimdBase256 other) {
			return _mm256_xor_si256(this->value, other);
		}

		inline SimdBase256 operator+(SimdBase256 other) {
			return _mm256_add_epi8(this->value, other);
		}

		inline SimdBase256& operator|=(SimdBase256 other) {
			*this = *this | other;
			return *this;
		}

		inline SimdBase256& operator&=(SimdBase256 other) {
			*this = *this & other;
			return *this;
		}

		inline SimdBase256& operator^=(SimdBase256 other) {
			*this = *this ^ other;
			return *this;
		}

		inline SimdBase256 operator==(SimdBase256 other) {
			return _mm256_cmpeq_epi8(this->value, other);
		}

		inline SimdBase256 operator==(char other) {
			return _mm256_cmpeq_epi8(this->value, _mm256_set1_epi8(other));
		}

		inline SimdBase256 operator<<(size_t amount) {
			int64_t values[4]{};
			values[0] = _mm256_extract_epi64(this->value, 0);
			values[1] = _mm256_extract_epi64(this->value, 1);
			values[2] = _mm256_extract_epi64(this->value, 2);
			values[3] = _mm256_extract_epi64(this->value, 3);
			SimdBase256 newValues{};
			newValues = _mm256_insert_epi64(newValues, (values[0] << (amount % 64)), 0);
			newValues = _mm256_insert_epi64(newValues, (values[1] << (amount % 64)) | ((values[0] & 1ull) << 63), 1);
			newValues = _mm256_insert_epi64(newValues, (values[2] << (amount % 64)) | ((values[1] & 1ull) << 63), 2);
			newValues = _mm256_insert_epi64(newValues, (values[3] << (amount % 64)) | ((values[2] & 1ull) << 63), 3);
			return newValues;
		}

		inline SimdBase256 operator~() {
			SimdBase256 newValues{};
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->value, 0), 0);
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->value, 1), 1);
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->value, 2), 2);
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->value, 3), 3);
			return newValues;
		}

		inline SimdBase256 carrylessMultiplication(int64_t& prevInString) {
			SimdBase128 allOnes{ '\xFF' };
			auto inString00 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(0)), allOnes, 0)) ^ prevInString;
			prevInString = inString00 >> 63;
			auto inString01 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(1)), allOnes, 0)) ^ prevInString;
			prevInString = inString01 >> 63;
			auto inString02 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(2)), allOnes, 0)) ^ prevInString;
			prevInString = inString02 >> 63;
			auto inString03 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(3)), allOnes, 0)) ^ prevInString;
			prevInString = inString03 >> 63;
			return SimdBase256{ inString00, inString01, inString02, inString03 };
		}

		inline bool collectCarries(SimdBase256 other1, SimdBase256* result) {
			bool returnValue{};
			uint64_t returnValue64{};
			for (size_t x = 0; x < 4; ++x) {
				if (_addcarry_u64(0, this->getUint64(x), other1.getUint64(x), reinterpret_cast<unsigned long long*>(&returnValue64))) {
					returnValue = true;
				}
				result->insertInt64(returnValue64, x);
			}
			return returnValue;
		}

		inline void printBits(const std::string& valuesTitle) {
			//std::cout << valuesTitle;
			for (size_t x = 0; x < 32; ++x) {
				for (size_t y = 0; y < 8; ++y) {
					//std::cout << std::bitset<1>{ static_cast<uint64_t>(*(reinterpret_cast<int8_t*>(&this->value) + x)) >> y };
				}
			}
			//std::cout << std::endl;
		}

		inline SimdBase256 bitAndNot(SimdBase256 other) {
			return _mm256_andnot_si256(other, this->value);
		}

		inline SimdBase256 shuffle(SimdBase256 other) {
			return _mm256_shuffle_epi8(other, this->value);
		}

	  protected:
		__m256i value{};
	};

	class SimdTape {
	  public:
		inline SimdTape& operator=(SimdTape&& other) noexcept {
			this->tapePtrs = std::move(other.tapePtrs);
			return *this;
		}

		inline SimdTape(SimdTape&& other) noexcept {
			*this = std::move(other);
		}

		inline SimdTape& operator=(const SimdTape&) noexcept = delete;
		inline SimdTape(const SimdTape&) noexcept = delete;

		inline SimdTape() noexcept = default;

		inline uint64_t* operator[](size_t index) {
			return &this->tapePtrs[index];
		}

		void reset() noexcept {
			this->tapePtrs.clear();
		}

		inline operator uint64_t*() {
			return &this->tapePtrs[0];
		}

		inline uint64_t addTapeValues(uint64_t* theBits, size_t currentIndexNew, size_t currentIndexIntoString) {
			uint64_t value = static_cast<uint64_t>(__popcnt64(*theBits));
			for (int i = 0; i < value; i++) {
				this->tapePtrs.emplace_back(_tzcnt_u64(*theBits) + (currentIndexNew * 64) + currentIndexIntoString);
				*theBits = _blsr_u64(*theBits);
			}

			return value;
		}

	  protected:
		std::vector<uint64_t> tapePtrs{};
	};

	inline SimdBase256 convertSimdBytesToBits(SimdBase256 input00, SimdBase256 input01, SimdBase256 input02, SimdBase256 input03, SimdBase256 input04,
		SimdBase256 input05, SimdBase256 input06, SimdBase256 input07) {
		SimdBase256 returnValue{};
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00), 0);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input01), 1);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input02), 2);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input03), 3);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input04), 4);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input05), 5);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input06), 6);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input07), 7);
		return returnValue;
	}

	inline uint32_t convertSimdBytesToBits(SimdBase256 input00) {
		auto returnValue = _mm256_movemask_epi8(input00);
		return static_cast<uint32_t>(returnValue);
	}

	class SimdStringSection {
	  public:
		inline SimdStringSection() noexcept = default;

		inline void packStringIntoValue(SimdBase256* theValue, const char string[32]) {
			for (size_t x = 0; x < 32; ++x) {
				*theValue = string;
			}
		}

		inline size_t getStructuralIndices(SimdTape& jsonRawTape, size_t currentIndex) {
			size_t returnValue{};
			for (size_t x = 0; x < 4; ++x) {
				auto newValue = this->S256.getUint64(x);
				returnValue += jsonRawTape.addTapeValues(&newValue, x, currentIndex);
			}
			return returnValue;
		}

		inline SimdBase256 collectWhiteSpace() {
			char valuesNew[32]{ ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2,
				100, '\t', '\n', 112, 100, '\r', 100, 100 };
			SimdBase256 whitespaceTable{ valuesNew };
			SimdBase256 whiteSpaceReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				whiteSpaceReal[x] = this->values[x].shuffle(whitespaceTable) == this->values[x];
			}
			return convertSimdBytesToBits(whiteSpaceReal[0], whiteSpaceReal[1], whiteSpaceReal[2], whiteSpaceReal[3], whiteSpaceReal[4],
				whiteSpaceReal[5], whiteSpaceReal[6], whiteSpaceReal[7]);
		}

		inline SimdBase256 collectStructuralCharacters() {
			char valuesNew[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
			SimdBase256 opTable{ valuesNew };
			SimdBase256 structural[8]{};
			for (size_t x = 0; x < 8; ++x) {
				auto valuesNew00 = this->values[x] | SimdBase256{ 0x20 };
				structural[x] = this->values[x].shuffle(opTable) == valuesNew00;
			}

			return convertSimdBytesToBits(structural[0], structural[1], structural[2], structural[3], structural[4], structural[5], structural[6],
				structural[7]);
		}

		inline SimdBase256 collectQuotedRange(int64_t& prevInString) {
			SimdBase256 backslashes = _mm256_set1_epi8('\\');
			SimdBase256 backslashesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				backslashesReal[x] = this->values[x] == backslashes;
			}

			auto B256 = convertSimdBytesToBits(backslashesReal[0], backslashesReal[1], backslashesReal[2], backslashesReal[3], backslashesReal[4],
				backslashesReal[5], backslashesReal[6], backslashesReal[7]);

			SimdBase256 E{ _mm256_set1_epi8(0b01010101) };
			SimdBase256 O{ _mm256_set1_epi8(0b10101010) };
			this->S256 = B256.bitAndNot(B256 << 1);
			auto ES = E & this->S256;
			SimdBase256 EC{};
			B256.collectCarries(ES, &EC);
			auto ECE = EC.bitAndNot(B256);
			auto OD1 = ECE.bitAndNot(E);
			auto OS = this->S256 & O;
			auto OC = B256 + OS;
			auto OCE = OC.bitAndNot(B256);
			auto OD2 = OCE & E;
			auto OD = OD1 | OD2;
			this->Q256 = this->Q256.bitAndNot(OD);
			return this->Q256.carrylessMultiplication(prevInString);
		}

		inline SimdBase256 collectQuotes() {
			SimdBase256 quotes = _mm256_set1_epi8('"');
			SimdBase256 quotesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				quotesReal[x] = (this->values[x] == quotes);
			}

			return convertSimdBytesToBits(quotesReal[0], quotesReal[1], quotesReal[2], quotesReal[3], quotesReal[4], quotesReal[5], quotesReal[6],
				quotesReal[7]);
		}

		inline SimdBase256 collectFinalStructurals() {
			this->S256 = this->S256.bitAndNot(this->R256);
			this->S256 = this->S256 | this->Q256;
			auto P = this->S256 | this->W256;
			P = P << 1;
			P &= (~W256).bitAndNot(this->R256);
			this->S256 = this->S256 | P;
			return S256.bitAndNot((this->Q256.bitAndNot(this->R256)));
		}

		inline SimdStringSection(const char* valueNew, int64_t& prevInString) {
			this->packStringIntoValue(&this->values[0], valueNew);
			this->packStringIntoValue(&this->values[1], valueNew + 32);
			this->packStringIntoValue(&this->values[2], valueNew + 64);
			this->packStringIntoValue(&this->values[3], valueNew + 96);
			this->packStringIntoValue(&this->values[4], valueNew + 128);
			this->packStringIntoValue(&this->values[5], valueNew + 160);
			this->packStringIntoValue(&this->values[6], valueNew + 192);
			this->packStringIntoValue(&this->values[7], valueNew + 224);

			this->Q256 = this->collectQuotes();
			this->R256 = this->collectQuotedRange(prevInString);
			this->W256 = this->collectWhiteSpace();
			this->S256 = this->collectStructuralCharacters();
			this->S256 = this->collectFinalStructurals();
		}

	  protected:
		SimdBase256 values[8]{};
		SimdBase256 Q256{};
		SimdBase256 W256{};
		SimdBase256 R256{};
		SimdBase256 S256{};
	};

	class JsonConstructor;

	struct OpenContainer {
		uint32_t tapeIndex{};
		uint32_t count{};
	};

	class SimdJsonValue {
	  public:
		inline SimdJsonValue(char* stringNew, size_t tapeLength) : stringView(stringNew) {
			if (tapeLength == 0) {
				throw DCAException{ "Failed to parse as the string size is 0." };
			}
			this->stringLength = tapeLength;
			this->stringViewNew = new char[tapeLength];
			this->generateJsonEvents();
		}


		inline void generateJsonEvents() {
			int64_t stringSize = this->stringLength;
			this->jsonRawTape.reset();
			this->isArray = std::make_unique<bool[]>(12);
			this->openContainers = std::make_unique<OpenContainer[]>(this->maxDepth);
			uint32_t collectedSize{};
			size_t tapeSize{ 0 };
			int64_t prevInString{};
			while (stringSize > 0) {
				SimdStringSection section(this->stringView + collectedSize, prevInString);
				auto indexCount = section.getStructuralIndices(this->jsonRawTape, collectedSize);
				tapeSize += indexCount;

				stringSize -= 256;
				collectedSize += 256;
			}
			this->tapeLength = tapeSize;
			this->nextStructural = this->jsonRawTape.operator uint64_t*();
		}

		inline ~SimdJsonValue() noexcept {
			delete[] this->stringViewNew;
		}

		inline char* getStringView() {
			return this->stringView;
		}

		inline char* getStringViewNew() {
			return this->stringViewNew;
		}

		inline uint64_t* getNextStructural() {
			return this->nextStructural;
		}

		inline uint64_t* getStructuralIndexes() {
			return this->jsonRawTape.operator uint64_t*();
		}

		inline Jsonifier getJsonData();

		inline uint32_t getMaxDepth() {
			return this->maxDepth;
		}

		inline size_t getTapeLength() {
			return this->tapeLength;
		}

		inline bool* getIsArray() {
			return this->isArray.get();
		}

		std::unique_ptr<OpenContainer[]> openContainers{};
		Jsonifier* doc{};


		size_t stringLength{};

	  protected:
		std::unique_ptr<bool[]> isArray{};
		uint64_t* nextStructural{};
		uint32_t maxDepth{ 500 };
		size_t tapeLength{ 0 };
		SimdTape jsonRawTape{};
		char* stringViewNew{};
		uint32_t depth{ 0 };
		char* stringView{};
	};

	enum class TapeType : uint8_t {
		Root = 'r',
		Start_Array = '[',
		Start_Object = '{',
		End_Array = ']',
		End_Object = '}',
		STRING = '"',
		Int64 = 'l',
		Uint64 = 'u',
		Double = 'd',
		True_Value = 't',
		False_Value = 'f',
		Null_Value = 'n'
	};

	class TapeBuilder;

	class JsonIterator {
	  public:
		const uint8_t* buf;
		uint64_t* nextStructural;
		SimdJsonValue& masterParser;
		uint32_t depth{ 0 };

		inline Jsonifier walkDocument(TapeBuilder& visitor) noexcept;

		inline JsonIterator(SimdJsonValue& _dom_parser, size_t start_structural_index);

		inline const uint8_t* peek() const noexcept;

		inline const uint8_t* advance() noexcept;

		inline size_t remainingLen() const noexcept;

		inline bool atEof() const noexcept;

		inline bool atBeginning() const noexcept;

		inline uint8_t lastStructural() const noexcept;

		inline Jsonifier visitRootPrimitive(TapeBuilder& visitor, const uint8_t* value) noexcept;
		inline Jsonifier visitPrimitive(TapeBuilder& visitor, const uint8_t* value) noexcept;
	};

	inline JsonIterator::JsonIterator(SimdJsonValue& _dom_parser, size_t start_structural_index)
		: buf{ reinterpret_cast<const uint8_t*>(_dom_parser.getStringView()) },
		  nextStructural{ &_dom_parser.getStructuralIndexes()[start_structural_index] }, masterParser{ _dom_parser } {
	}

	inline const uint8_t* JsonIterator::peek() const noexcept {
		return &buf[*(nextStructural)];
	}

	inline const uint8_t* JsonIterator::advance() noexcept {
		return &buf[*(nextStructural++)];
	}

	inline size_t JsonIterator::remainingLen() const noexcept {
		return masterParser.getTapeLength() - *(nextStructural);
	}

	inline bool JsonIterator::atEof() const noexcept {
		return nextStructural == &masterParser.getStructuralIndexes()[masterParser.getTapeLength() - 1];
	}

	inline bool JsonIterator::atBeginning() const noexcept {
		return nextStructural == masterParser.getStructuralIndexes();
	}

	inline uint8_t JsonIterator::lastStructural() const noexcept {
		return buf[masterParser.getStructuralIndexes()[masterParser.getTapeLength() - 1]];
	}

	const bool structuralOrWhitespaceNegated[256] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,

		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,

		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	inline uint32_t isNotStructuralOrWhitespace(uint8_t c) {
		return structuralOrWhitespaceNegated[c];
	}

	inline uint32_t stringToUint32(const char* str) {
		return std::stoll(str);
	}

	inline uint32_t str4ncmp(const char* src, const char* atom) {
		uint32_t srcval{};
		static_assert(sizeof(uint32_t) <= 256, "256 must be larger than 4 bytes");
		return srcval ^ stringToUint32(atom);
	}

	inline bool isValidTrueAtom(const char* src) {
		return (str4ncmp(src, "true") | isNotStructuralOrWhitespace(src[4])) == 0;
	}

	inline bool isValidFalseAtom(const char* src) {
		return (str4ncmp(src + 1, "alse") | isNotStructuralOrWhitespace(src[5])) == 0;
	}

	inline bool isValidNullAtom(const char* src) {
		return (str4ncmp(src, "null") | isNotStructuralOrWhitespace(src[4])) == 0;
	}

	struct TapeWriter {
		TapeWriter(uint64_t* ptr) {
			this->nextTapeLocation = ptr;
		}
		uint64_t* nextTapeLocation;
		inline int64_t appendS64(int64_t value) noexcept;
		inline uint64_t appendU64(uint64_t value) noexcept;
		inline double appendDouble(double value) noexcept;
		inline void append(uint64_t val, TapeType t) noexcept;
		inline void skip() noexcept;
		inline void skipLargeInteger() noexcept;
		inline void skipDouble() noexcept;
		inline static void write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept;

	  private:
		template<typename T> inline void append2(uint64_t val, T val2, TapeType t) noexcept;
	};

	inline int64_t TapeWriter::appendS64(int64_t value) noexcept {
		append2(0, value, TapeType::Int64);
		return value;
	}

	inline uint64_t TapeWriter::appendU64(uint64_t value) noexcept {
		append(0, TapeType::Uint64);
		*nextTapeLocation = value;
		nextTapeLocation++;
		return value;
	}

	inline double TapeWriter::appendDouble(double value) noexcept {
		append2(0, value, TapeType::Double);
		return value;
	}

	inline void TapeWriter::skip() noexcept {
		nextTapeLocation++;
	}

	inline void TapeWriter::skipLargeInteger() noexcept {
		nextTapeLocation += 2;
	}

	inline void TapeWriter::skipDouble() noexcept {
		nextTapeLocation += 2;
	}

	inline void TapeWriter::append(uint64_t val, TapeType t) noexcept {
		*nextTapeLocation = val | ((uint64_t(char(t))) << 56);
		nextTapeLocation++;
	}

	template<typename T> inline void TapeWriter::append2(uint64_t val, T val2, TapeType t) noexcept {
		append(val, t);
		static_assert(sizeof(val2) == sizeof(*nextTapeLocation), "Type is not 64 bits!");
		memcpy(nextTapeLocation, &val2, sizeof(val2));
		nextTapeLocation++;
	}

	inline void TapeWriter::write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept {
		tape_loc = val | ((uint64_t(char(t))) << 56);
	}

	class JsonConstructor {
	  public:
		JsonConstructor() noexcept {
			this->currentPlace.emplace_back(&this->jsonData.operator=(JsonType::Object));
		};

		void setCurrentKey(std::string&& key) {
			this->currentKey.emplace_back(std::move(key));
		}

		void startNewObject() {
			if (this->currentPlace.back()->getType() == JsonType::Array) {
				this->currentPlace.emplace_back(&this->currentPlace.back()->emplaceBack(JsonType::Object));
			} else {
				(*this->currentPlace.back())[this->currentKey.back()] = JsonType::Object;
				this->currentPlace.emplace_back(&(*this->currentPlace.back())[std::move(this->currentKey.back())]);
			}
		}

		void startNewArray() {
			(*this->currentPlace.back())[this->currentKey.back()] = JsonType::Array;
			this->currentPlace.emplace_back(&(*this->currentPlace.back())[std::move(this->currentKey.back())]);
			this->type = JsonType::Array;
		}

		void endArray() {
			this->currentKey.erase(this->currentKey.end() - 1);
			this->currentPlace.erase(this->currentPlace.end() - 1);
		}

		void endObject() {
			this->currentKey.erase(this->currentKey.end() - 1);
			this->currentPlace.erase(this->currentPlace.end() - 1);
		}

		void setAppendType(JsonType typeNew) {
			this->type = typeNew;
		}

		template<typename OTy> void appendPrimitiveElement(OTy&& data) {
			if (this->currentPlace.back()->getType() == JsonType::Array) {
				this->currentPlace.back()->emplaceBack(data);
			} else {
				(*this->currentPlace.back())[std::move(this->currentKey.back())] = std::move(data);
			}
		}

		template<typename OTy> void appendPrimitiveElement(OTy& data) {
			if (this->currentPlace.back()->getType() == JsonType::Array) {
				this->currentPlace.back()->emplaceBack(data);
			} else {
				(*this->currentPlace.back())[std::move(this->currentKey.back())] = data;
			}
		}

		Jsonifier getResult() {
			return std::move(this->jsonData);
		}

	  protected:
		std::vector<Jsonifier*> currentPlace{};
		std::vector<std::string> currentKey{};
		Jsonifier jsonData{};
		JsonType type{};
	};


	struct TapeBuilder {
		static inline Jsonifier parseDocument(SimdJsonValue& masterParser) noexcept;

		inline ErrorCode visitDocumentStart(JsonIterator& iter) noexcept;

		inline ErrorCode visitDocumentEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitArrayStart(JsonIterator& iter) noexcept;

		inline ErrorCode visitArrayEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitEmptyArray(JsonIterator& iter) noexcept;

		inline ErrorCode visitObjectStart(JsonIterator& iter) noexcept;

		inline std::string_view visitKey(JsonIterator& iter, const uint8_t* key) noexcept;

		inline ErrorCode visitObjectEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitEmptyObject(JsonIterator& iter) noexcept;

		inline Jsonifier visitPrimitive(JsonIterator& iter, const uint8_t* value) noexcept;

		inline Jsonifier visitRootPrimitive(JsonIterator& iter, const uint8_t* value) noexcept;

		inline std::string_view visitString(JsonIterator& iter, const uint8_t* value) noexcept;
		inline Jsonifier visitNumber(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitNullAtom(JsonIterator& iter, const uint8_t* value) noexcept;

		inline std::string_view visitRootString(JsonIterator& iter, const uint8_t* value) noexcept;
		inline Jsonifier visitRootNumber(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitRootTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitRootFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitRootNullAtom(JsonIterator& iter, const uint8_t* value) noexcept;

		inline ErrorCode incrementCount(JsonIterator& iter) noexcept;

		TapeWriter tape;

	  private:
		uint8_t* currentStringBufferLocation{};

		inline TapeBuilder(SimdJsonValue& doc) noexcept;

		inline uint32_t nextTapeIndex(JsonIterator& iter) const noexcept;
		inline void startContainer(JsonIterator& iter) noexcept;
		inline ErrorCode endContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline ErrorCode emptyContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline uint8_t* onStartString(JsonIterator& iter) noexcept;
		inline void onEndString(uint8_t* dst) noexcept;
	};

	inline Jsonifier TapeBuilder::parseDocument(SimdJsonValue& masterParser) noexcept {
		JsonIterator iter(masterParser, 0);
		TapeBuilder builder(masterParser);
		auto result = iter.walkDocument(builder);
		return result;
	}

	inline Jsonifier TapeBuilder::visitRootPrimitive(JsonIterator& iter, const uint8_t* value) noexcept {
		return iter.visitRootPrimitive(*this, value);
	}
	inline Jsonifier TapeBuilder::visitPrimitive(JsonIterator& iter, const uint8_t* value) noexcept {
		return iter.visitPrimitive(*this, value);
	}
	inline ErrorCode TapeBuilder::visitEmptyObject(JsonIterator& iter) noexcept {
		return emptyContainer(iter, TapeType::Start_Object, TapeType::End_Object);
	}
	inline ErrorCode TapeBuilder::visitEmptyArray(JsonIterator& iter) noexcept {
		return emptyContainer(iter, TapeType::Start_Array, TapeType::End_Array);
	}

	inline ErrorCode TapeBuilder::visitDocumentStart(JsonIterator& iter) noexcept {
		startContainer(iter);
		return ErrorCode::Success;
	}
	inline ErrorCode TapeBuilder::visitObjectStart(JsonIterator& iter) noexcept {
		startContainer(iter);
		return ErrorCode::Success;
	}
	inline ErrorCode TapeBuilder::visitArrayStart(JsonIterator& iter) noexcept {
		startContainer(iter);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitObjectEnd(JsonIterator& iter) noexcept {
		return endContainer(iter, TapeType::Start_Object, TapeType::End_Object);
	}
	inline ErrorCode TapeBuilder::visitArrayEnd(JsonIterator& iter) noexcept {
		return endContainer(iter, TapeType::Start_Array, TapeType::End_Array);
	}
	inline ErrorCode TapeBuilder::visitDocumentEnd(JsonIterator& iter) noexcept {
		constexpr uint32_t startTapeIndex = 0;
		tape.append(startTapeIndex, TapeType::Root);
		TapeWriter::write(iter.masterParser.getStructuralIndexes()[startTapeIndex], nextTapeIndex(iter), TapeType::Root);
		return ErrorCode::Success;
	}
	inline std::string_view TapeBuilder::visitKey(JsonIterator& iter, const uint8_t* key) noexcept {
		return visitString(iter, key);
	}

	inline ErrorCode TapeBuilder::incrementCount(JsonIterator& iter) noexcept {
		iter.masterParser.openContainers[iter.depth].count++;
		return ErrorCode::Success;
	}

	inline TapeBuilder::TapeBuilder(SimdJsonValue& doc) noexcept
		: tape{ doc.getStructuralIndexes() }, currentStringBufferLocation{ reinterpret_cast<uint8_t*>(doc.getStringViewNew()) } {
	}

	const uint32_t digitToVal32[886] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa0,
		0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x800, 0x900, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa00, 0xb00, 0xc00, 0xd00, 0xe00, 0xf00, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa00,
		0xb00, 0xc00, 0xd00, 0xe00, 0xf00, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000, 0x8000, 0x9000, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xa000, 0xb000, 0xc000, 0xd000, 0xe000, 0xf000, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xa000, 0xb000, 0xc000, 0xd000, 0xe000, 0xf000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };

	static inline uint32_t hexToU32Nocheck(const uint8_t* src) {
		uint32_t v1 = digitToVal32[630 + src[0]];
		uint32_t v2 = digitToVal32[420 + src[1]];
		uint32_t v3 = digitToVal32[210 + src[2]];
		uint32_t v4 = digitToVal32[0 + src[3]];
		return v1 | v2 | v3 | v4;
	}

	inline size_t codepointToUtf8(uint32_t cp, uint8_t* c) {
		if (cp <= 0x7F) {
			c[0] = uint8_t(cp);
			return 1;
		}
		if (cp <= 0x7FF) {
			c[0] = uint8_t((cp >> 6) + 192);
			c[1] = uint8_t((cp & 63) + 128);
			return 2;
		} else if (cp <= 0xFFFF) {
			c[0] = uint8_t((cp >> 12) + 224);
			c[1] = uint8_t(((cp >> 6) & 63) + 128);
			c[2] = uint8_t((cp & 63) + 128);
			return 3;
		} else if (cp <= 0x10FFFF) {
			c[0] = uint8_t((cp >> 18) + 240);
			c[1] = uint8_t(((cp >> 12) & 63) + 128);
			c[2] = uint8_t(((cp >> 6) & 63) + 128);
			c[3] = uint8_t((cp & 63) + 128);
			return 4;
		}

		return 0;
	}

	struct AdjustedMantissa {
		uint64_t mantissa;
		int power2;
		AdjustedMantissa() : mantissa(0), power2(0) {
		}
	};

	inline bool handleUnicodeCodepoint(const uint8_t** srcPtr, uint8_t** dstPtr) {
		uint32_t codePoint = hexToU32Nocheck(*srcPtr + 2);
		*srcPtr += 6;
		if (codePoint >= 0xd800 && codePoint < 0xdc00) {
			const uint8_t* srcData = *srcPtr;
			if (((srcData[0] << 8) | srcData[1]) != ((static_cast<uint8_t>('\\') << 8) | static_cast<uint8_t>('u'))) {
				return false;
			}
			uint32_t codePoint2 = hexToU32Nocheck(srcData + 2);
			uint32_t lowBit = codePoint2 - 0xdc00;
			if (lowBit >> 10) {
				return false;
			}

			codePoint = (((codePoint - 0xd800) << 10) | lowBit) + 0x10000;
			*srcPtr += 6;
		} else if (codePoint >= 0xdc00 && codePoint <= 0xdfff) {
			return false;
		}
		size_t offset = codepointToUtf8(codePoint, *dstPtr);
		*dstPtr += offset;
		return offset > 0;
	}

	static const uint8_t escapeMap[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5c, 0, 0, 0, 0, 0, 0x08, 0, 0, 0, 0x0c, 0, 0, 0, 0, 0,
		0, 0, 0x0a, 0, 0, 0, 0x0d, 0, 0x09, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	struct BackslashAndQuote {
	  public:
		static constexpr uint32_t BYTES_PROCESSED = 32;
		inline static BackslashAndQuote copyAndFind(const uint8_t* src, uint8_t* dst);

		inline bool hasQuoteFirst() {
			return ((bsBits - 1) & quoteBits) != 0;
		}
		inline bool hasBackslash() {
			return ((quoteBits - 1) & bsBits) != 0;
		}
		inline int32_t quoteIndex() {
			return _tzcnt_u64(quoteBits);
		}
		inline int32_t backslashIndex() {
			return _tzcnt_u64(bsBits);
		}

		uint32_t bsBits{};
		uint32_t quoteBits{};
	};

	inline BackslashAndQuote BackslashAndQuote::copyAndFind(const uint8_t* src, uint8_t* dst) {
		static_assert(32 >= (BYTES_PROCESSED - 1), "backslash and quote finder must process fewer than 32 bytes");
		SimdBase256 values{ reinterpret_cast<const char*>(src) };
		values.store(reinterpret_cast<char*>(dst));
		auto result01 = convertSimdBytesToBits((values == '\\'));
		auto result02 = convertSimdBytesToBits((values == '"'));
		return { result01, result02 };
	}

	inline uint8_t* parseString(const uint8_t* src, uint8_t* dst) {
		int32_t index{};
		while (1) {
			index += 32;
			auto bsQuote = BackslashAndQuote::copyAndFind(src, dst);
			if (bsQuote.hasQuoteFirst()) {
				return dst + bsQuote.quoteIndex();
			}
			if (bsQuote.hasBackslash()) {
				auto bsDist = bsQuote.backslashIndex();
				uint8_t escapeChar = src[bsDist + 1];
				if (escapeChar == 'u') {
					src += bsDist;
					dst += bsDist;
					if (!handleUnicodeCodepoint(&src, &dst)) {
						return nullptr;
					}
				} else {
					uint8_t escapeResult = escapeMap[escapeChar];
					if (escapeResult == 0u) {
						return nullptr;
					}
					dst[bsDist] = escapeResult;
					src += bsDist + 2;
					dst += bsDist + 1;
				}
			} else {
				src += BackslashAndQuote::BYTES_PROCESSED;
				dst += BackslashAndQuote::BYTES_PROCESSED;
			}
		}
		return nullptr;
	}

	template<typename I> inline bool parseDigit(const uint8_t c, I& i) {
		const uint8_t digit = static_cast<uint8_t>(c - '0');
		if (digit > 9) {
			return false;
		}
		i = 10 * i + digit;
		return true;
	}

	inline ErrorCode parseDecimal(const uint8_t* const src, const uint8_t*& p, uint64_t& i, int64_t& exponent) {
		const uint8_t* const firstAfterPeriod = p;

		if (parseDigit(*p, i)) {
			++p;
		}
		while (parseDigit(*p, i)) {
			p++;
		}
		exponent = firstAfterPeriod - p;
		if (exponent == 0) {
			return ErrorCode::InvalidNumber;
		}
		return ErrorCode::Success;
	}

	inline ErrorCode parseExponent(const uint8_t* const src, const uint8_t*& p, int64_t& exponent) {
		bool negExp = ('-' == *p);
		if (negExp || '+' == *p) {
			p++;
		}
		auto startExp = p;
		int64_t expNumber = 0;
		while (parseDigit(*p, expNumber)) {
			++p;
		}
		if (p == startExp) {
			return ErrorCode::InvalidNumber;
		}
		if (p > startExp + 18) {
			while (*startExp == '0') {
				startExp++;
			}
			if (p > startExp + 18) {
				expNumber = 999999999999999999;
			}
		}
		exponent += (negExp ? -expNumber : expNumber);
		return ErrorCode::Success;
	}

	inline uint32_t isNotStructuralOrWhiteSpace(uint8_t c) {
		return structuralOrWhitespaceNegated[c];
	}

	template<typename T> struct binaryFormat {
		static constexpr int mantissaExplicitBits();
		static constexpr int minimumExponent();
		static constexpr int infinitePower();
		static constexpr int signIndex();
	};

	template<> constexpr int binaryFormat<double>::mantissaExplicitBits() {
		return 52;
	}

	template<> constexpr int binaryFormat<double>::minimumExponent() {
		return -1023;
	}
	template<> constexpr int binaryFormat<double>::infinitePower() {
		return 0x7FF;
	}

	template<> constexpr int binaryFormat<double>::signIndex() {
		return 63;
	}

	constexpr uint32_t maxDigits = 768;

	inline bool isInteger(char c) noexcept {
		return (c >= '0' && c <= '9');
	}

	struct Decimal {
		uint32_t numDigits;
		int32_t decimalPoint;
		bool negative;
		bool truncated;
		uint8_t digits[maxDigits];
	};

	inline Decimal parseDecimal(const char*& p) noexcept {
		Decimal answer;
		answer.numDigits = 0;
		answer.decimalPoint = 0;
		answer.truncated = false;
		answer.negative = (*p == '-');
		if ((*p == '-') || (*p == '+')) {
			++p;
		}

		while (*p == '0') {
			++p;
		}
		while (isInteger(*p)) {
			if (answer.numDigits < maxDigits) {
				answer.digits[answer.numDigits] = uint8_t(*p - '0');
			}
			answer.numDigits++;
			++p;
		}
		if (*p == '.') {
			++p;
			const char* firstAfterPeriod = p;
			if (answer.numDigits == 0) {
				while (*p == '0') {
					++p;
				}
			}
			while (isInteger(*p)) {
				if (answer.numDigits < maxDigits) {
					answer.digits[answer.numDigits] = uint8_t(*p - '0');
				}
				answer.numDigits++;
				++p;
			}
			answer.decimalPoint = int32_t(firstAfterPeriod - p);
		}
		if (answer.numDigits > 0) {
			const char* preverse = p - 1;
			int32_t trailingZeros = 0;
			while ((*preverse == '0') || (*preverse == '.')) {
				if (*preverse == '0') {
					trailingZeros++;
				};
				--preverse;
			}
			answer.decimalPoint += int32_t(answer.numDigits);
			answer.numDigits -= uint32_t(trailingZeros);
		}
		if (answer.numDigits > maxDigits) {
			answer.numDigits = maxDigits;
			answer.truncated = true;
		}
		if (('e' == *p) || ('E' == *p)) {
			++p;
			bool negExp = false;
			if ('-' == *p) {
				negExp = true;
				++p;
			} else if ('+' == *p) {
				++p;
			}
			int32_t expNumber = 0;
			while (isInteger(*p)) {
				uint8_t digit = uint8_t(*p - '0');
				if (expNumber < 0x10000) {
					expNumber = 10 * expNumber + digit;
				}
				++p;
			}
			answer.decimalPoint += (negExp ? -expNumber : expNumber);
		}
		return answer;
	}

	inline uint32_t numberOfDigitsDecimalLeftShift(Decimal &h, uint32_t shift) {
  shift &= 63;
  const static uint16_t numberOfDigitsDecimalLeftShift_table[65] = {
      0x0000, 0x0800, 0x0801, 0x0803, 0x1006, 0x1009, 0x100D, 0x1812, 0x1817,
      0x181D, 0x2024, 0x202B, 0x2033, 0x203C, 0x2846, 0x2850, 0x285B, 0x3067,
      0x3073, 0x3080, 0x388E, 0x389C, 0x38AB, 0x38BB, 0x40CC, 0x40DD, 0x40EF,
      0x4902, 0x4915, 0x4929, 0x513E, 0x5153, 0x5169, 0x5180, 0x5998, 0x59B0,
      0x59C9, 0x61E3, 0x61FD, 0x6218, 0x6A34, 0x6A50, 0x6A6D, 0x6A8B, 0x72AA,
      0x72C9, 0x72E9, 0x7B0A, 0x7B2B, 0x7B4D, 0x8370, 0x8393, 0x83B7, 0x83DC,
      0x8C02, 0x8C28, 0x8C4F, 0x9477, 0x949F, 0x94C8, 0x9CF2, 0x051C, 0x051C,
      0x051C, 0x051C,
  };
  uint32_t x_a = numberOfDigitsDecimalLeftShift_table[shift];
  uint32_t x_b = numberOfDigitsDecimalLeftShift_table[shift + 1];
  uint32_t num_new_digits = x_a >> 11;
  uint32_t pow5_a = 0x7FF & x_a;
  uint32_t pow5_b = 0x7FF & x_b;
  const static uint8_t
      numberOfDigitsDecimalLeftShift_table_powers_of_5[0x051C] = {
          5, 2, 5, 1, 2, 5, 6, 2, 5, 3, 1, 2, 5, 1, 5, 6, 2, 5, 7, 8, 1, 2, 5,
          3, 9, 0, 6, 2, 5, 1, 9, 5, 3, 1, 2, 5, 9, 7, 6, 5, 6, 2, 5, 4, 8, 8,
          2, 8, 1, 2, 5, 2, 4, 4, 1, 4, 0, 6, 2, 5, 1, 2, 2, 0, 7, 0, 3, 1, 2,
          5, 6, 1, 0, 3, 5, 1, 5, 6, 2, 5, 3, 0, 5, 1, 7, 5, 7, 8, 1, 2, 5, 1,
          5, 2, 5, 8, 7, 8, 9, 0, 6, 2, 5, 7, 6, 2, 9, 3, 9, 4, 5, 3, 1, 2, 5,
          3, 8, 1, 4, 6, 9, 7, 2, 6, 5, 6, 2, 5, 1, 9, 0, 7, 3, 4, 8, 6, 3, 2,
          8, 1, 2, 5, 9, 5, 3, 6, 7, 4, 3, 1, 6, 4, 0, 6, 2, 5, 4, 7, 6, 8, 3,
          7, 1, 5, 8, 2, 0, 3, 1, 2, 5, 2, 3, 8, 4, 1, 8, 5, 7, 9, 1, 0, 1, 5,
          6, 2, 5, 1, 1, 9, 2, 0, 9, 2, 8, 9, 5, 5, 0, 7, 8, 1, 2, 5, 5, 9, 6,
          0, 4, 6, 4, 4, 7, 7, 5, 3, 9, 0, 6, 2, 5, 2, 9, 8, 0, 2, 3, 2, 2, 3,
          8, 7, 6, 9, 5, 3, 1, 2, 5, 1, 4, 9, 0, 1, 1, 6, 1, 1, 9, 3, 8, 4, 7,
          6, 5, 6, 2, 5, 7, 4, 5, 0, 5, 8, 0, 5, 9, 6, 9, 2, 3, 8, 2, 8, 1, 2,
          5, 3, 7, 2, 5, 2, 9, 0, 2, 9, 8, 4, 6, 1, 9, 1, 4, 0, 6, 2, 5, 1, 8,
          6, 2, 6, 4, 5, 1, 4, 9, 2, 3, 0, 9, 5, 7, 0, 3, 1, 2, 5, 9, 3, 1, 3,
          2, 2, 5, 7, 4, 6, 1, 5, 4, 7, 8, 5, 1, 5, 6, 2, 5, 4, 6, 5, 6, 6, 1,
          2, 8, 7, 3, 0, 7, 7, 3, 9, 2, 5, 7, 8, 1, 2, 5, 2, 3, 2, 8, 3, 0, 6,
          4, 3, 6, 5, 3, 8, 6, 9, 6, 2, 8, 9, 0, 6, 2, 5, 1, 1, 6, 4, 1, 5, 3,
          2, 1, 8, 2, 6, 9, 3, 4, 8, 1, 4, 4, 5, 3, 1, 2, 5, 5, 8, 2, 0, 7, 6,
          6, 0, 9, 1, 3, 4, 6, 7, 4, 0, 7, 2, 2, 6, 5, 6, 2, 5, 2, 9, 1, 0, 3,
          8, 3, 0, 4, 5, 6, 7, 3, 3, 7, 0, 3, 6, 1, 3, 2, 8, 1, 2, 5, 1, 4, 5,
          5, 1, 9, 1, 5, 2, 2, 8, 3, 6, 6, 8, 5, 1, 8, 0, 6, 6, 4, 0, 6, 2, 5,
          7, 2, 7, 5, 9, 5, 7, 6, 1, 4, 1, 8, 3, 4, 2, 5, 9, 0, 3, 3, 2, 0, 3,
          1, 2, 5, 3, 6, 3, 7, 9, 7, 8, 8, 0, 7, 0, 9, 1, 7, 1, 2, 9, 5, 1, 6,
          6, 0, 1, 5, 6, 2, 5, 1, 8, 1, 8, 9, 8, 9, 4, 0, 3, 5, 4, 5, 8, 5, 6,
          4, 7, 5, 8, 3, 0, 0, 7, 8, 1, 2, 5, 9, 0, 9, 4, 9, 4, 7, 0, 1, 7, 7,
          2, 9, 2, 8, 2, 3, 7, 9, 1, 5, 0, 3, 9, 0, 6, 2, 5, 4, 5, 4, 7, 4, 7,
          3, 5, 0, 8, 8, 6, 4, 6, 4, 1, 1, 8, 9, 5, 7, 5, 1, 9, 5, 3, 1, 2, 5,
          2, 2, 7, 3, 7, 3, 6, 7, 5, 4, 4, 3, 2, 3, 2, 0, 5, 9, 4, 7, 8, 7, 5,
          9, 7, 6, 5, 6, 2, 5, 1, 1, 3, 6, 8, 6, 8, 3, 7, 7, 2, 1, 6, 1, 6, 0,
          2, 9, 7, 3, 9, 3, 7, 9, 8, 8, 2, 8, 1, 2, 5, 5, 6, 8, 4, 3, 4, 1, 8,
          8, 6, 0, 8, 0, 8, 0, 1, 4, 8, 6, 9, 6, 8, 9, 9, 4, 1, 4, 0, 6, 2, 5,
          2, 8, 4, 2, 1, 7, 0, 9, 4, 3, 0, 4, 0, 4, 0, 0, 7, 4, 3, 4, 8, 4, 4,
          9, 7, 0, 7, 0, 3, 1, 2, 5, 1, 4, 2, 1, 0, 8, 5, 4, 7, 1, 5, 2, 0, 2,
          0, 0, 3, 7, 1, 7, 4, 2, 2, 4, 8, 5, 3, 5, 1, 5, 6, 2, 5, 7, 1, 0, 5,
          4, 2, 7, 3, 5, 7, 6, 0, 1, 0, 0, 1, 8, 5, 8, 7, 1, 1, 2, 4, 2, 6, 7,
          5, 7, 8, 1, 2, 5, 3, 5, 5, 2, 7, 1, 3, 6, 7, 8, 8, 0, 0, 5, 0, 0, 9,
          2, 9, 3, 5, 5, 6, 2, 1, 3, 3, 7, 8, 9, 0, 6, 2, 5, 1, 7, 7, 6, 3, 5,
          6, 8, 3, 9, 4, 0, 0, 2, 5, 0, 4, 6, 4, 6, 7, 7, 8, 1, 0, 6, 6, 8, 9,
          4, 5, 3, 1, 2, 5, 8, 8, 8, 1, 7, 8, 4, 1, 9, 7, 0, 0, 1, 2, 5, 2, 3,
          2, 3, 3, 8, 9, 0, 5, 3, 3, 4, 4, 7, 2, 6, 5, 6, 2, 5, 4, 4, 4, 0, 8,
          9, 2, 0, 9, 8, 5, 0, 0, 6, 2, 6, 1, 6, 1, 6, 9, 4, 5, 2, 6, 6, 7, 2,
          3, 6, 3, 2, 8, 1, 2, 5, 2, 2, 2, 0, 4, 4, 6, 0, 4, 9, 2, 5, 0, 3, 1,
          3, 0, 8, 0, 8, 4, 7, 2, 6, 3, 3, 3, 6, 1, 8, 1, 6, 4, 0, 6, 2, 5, 1,
          1, 1, 0, 2, 2, 3, 0, 2, 4, 6, 2, 5, 1, 5, 6, 5, 4, 0, 4, 2, 3, 6, 3,
          1, 6, 6, 8, 0, 9, 0, 8, 2, 0, 3, 1, 2, 5, 5, 5, 5, 1, 1, 1, 5, 1, 2,
          3, 1, 2, 5, 7, 8, 2, 7, 0, 2, 1, 1, 8, 1, 5, 8, 3, 4, 0, 4, 5, 4, 1,
          0, 1, 5, 6, 2, 5, 2, 7, 7, 5, 5, 5, 7, 5, 6, 1, 5, 6, 2, 8, 9, 1, 3,
          5, 1, 0, 5, 9, 0, 7, 9, 1, 7, 0, 2, 2, 7, 0, 5, 0, 7, 8, 1, 2, 5, 1,
          3, 8, 7, 7, 7, 8, 7, 8, 0, 7, 8, 1, 4, 4, 5, 6, 7, 5, 5, 2, 9, 5, 3,
          9, 5, 8, 5, 1, 1, 3, 5, 2, 5, 3, 9, 0, 6, 2, 5, 6, 9, 3, 8, 8, 9, 3,
          9, 0, 3, 9, 0, 7, 2, 2, 8, 3, 7, 7, 6, 4, 7, 6, 9, 7, 9, 2, 5, 5, 6,
          7, 6, 2, 6, 9, 5, 3, 1, 2, 5, 3, 4, 6, 9, 4, 4, 6, 9, 5, 1, 9, 5, 3,
          6, 1, 4, 1, 8, 8, 8, 2, 3, 8, 4, 8, 9, 6, 2, 7, 8, 3, 8, 1, 3, 4, 7,
          6, 5, 6, 2, 5, 1, 7, 3, 4, 7, 2, 3, 4, 7, 5, 9, 7, 6, 8, 0, 7, 0, 9,
          4, 4, 1, 1, 9, 2, 4, 4, 8, 1, 3, 9, 1, 9, 0, 6, 7, 3, 8, 2, 8, 1, 2,
          5, 8, 6, 7, 3, 6, 1, 7, 3, 7, 9, 8, 8, 4, 0, 3, 5, 4, 7, 2, 0, 5, 9,
          6, 2, 2, 4, 0, 6, 9, 5, 9, 5, 3, 3, 6, 9, 1, 4, 0, 6, 2, 5,
      };
  const uint8_t *pow5 =
      &numberOfDigitsDecimalLeftShift_table_powers_of_5[pow5_a];
  uint32_t i = 0;
  uint32_t n = pow5_b - pow5_a;
  for (; i < n; i++) {
    if (i >= h.numDigits) {
      return num_new_digits - 1;
    } else if (h.digits[i] == pow5[i]) {
      continue;
    } else if (h.digits[i] < pow5[i]) {
      return num_new_digits - 1;
    } else {
      return num_new_digits;
    }
  }
  return num_new_digits;
}

	inline void trim(Decimal& h) {
	while ((h.numDigits > 0) && (h.digits[h.numDigits- 1] == 0)) {
		h.numDigits--;
	}
}

	constexpr int32_t decimalPointRange = 2047;

	inline void decimalRightShift(Decimal& h, uint32_t shift) {
	uint32_t read_index = 0;
	uint32_t write_index = 0;

	uint64_t n = 0;

	while ((n >> shift) == 0) {
		if (read_index < h.numDigits) {
			n = (10 * n) + h.digits[read_index++];
		} else if (n == 0) {
			return;
		} else {
			while ((n >> shift) == 0) {
				n = 10 * n;
				read_index++;
			}
			break;
		}
	}
	h.decimalPoint-= int32_t(read_index - 1);
	if (h.decimalPoint< -decimalPointRange) {
		h.numDigits = 0;
		h.decimalPoint = 0;
		h.negative = false;
		h.truncated = false;
		return;
	}
	uint64_t mask = (uint64_t(1) << shift) - 1;
	while (read_index < h.numDigits) {
		uint8_t new_digit = uint8_t(n >> shift);
		n = (10 * (n & mask)) + h.digits[read_index++];
		h.digits[write_index++] = new_digit;
	}
	while (n > 0) {
		uint8_t new_digit = uint8_t(n >> shift);
		n = 10 * (n & mask);
		if (write_index < maxDigits) {
			h.digits[write_index++] = new_digit;
		} else if (new_digit > 0) {
			h.truncated = true;
		}
	}
	h.numDigits = write_index;
	trim(h);
}
	inline uint64_t round(Decimal& h) {
		if ((h.numDigits== 0) || (h.decimalPoint < 0)) {
			return 0;
		} else if (h.decimalPoint > 18) {
			return UINT64_MAX;
		}
		uint32_t dp = uint32_t(h.decimalPoint);
		uint64_t n = 0;
		for (uint32_t i = 0; i < dp; i++) {
			n = (10 * n) + ((i < h.numDigits) ? h.digits[i] : 0);
		}
		bool round_up = false;
		if (dp < h.numDigits) {
			round_up = h.digits[dp] >= 5;
			if ((h.digits[dp] == 5) && (dp + 1 == h.numDigits)) {
				round_up = h.truncated || ((dp > 0) && (1 & h.digits[dp - 1]));
			}
		}
		if (round_up) {
			n++;
		}
		return n;
	}



	inline void decimalLeftShift(Decimal& h, uint32_t shift) {
		if (h.numDigits == 0) {
			return;
		}
		uint32_t num_new_digits = numberOfDigitsDecimalLeftShift(h, shift);
		int32_t read_index = int32_t(h.numDigits - 1);
		uint32_t write_index = h.numDigits - 1 + num_new_digits;
		uint64_t n = 0;

		while (read_index >= 0) {
			n += uint64_t(h.digits[read_index]) << shift;
			uint64_t quotient = n / 10;
			uint64_t remainder = n - (10 * quotient);
			if (write_index < maxDigits) {
				h.digits[write_index] = uint8_t(remainder);
			} else if (remainder > 0) {
				h.truncated = true;
			}
			n = quotient;
			write_index--;
			read_index--;
		}
		while (n > 0) {
			uint64_t quotient = n / 10;
			uint64_t remainder = n - (10 * quotient);
			if (write_index < maxDigits) {
				h.digits[write_index] = uint8_t(remainder);
			} else if (remainder > 0) {
				h.truncated = true;
			}
			n = quotient;
			write_index--;
		}
		h.numDigits += num_new_digits;
		if (h.numDigits > maxDigits) {
			h.numDigits = maxDigits;
		}
		h.decimalPoint += int32_t(num_new_digits);
		trim(h);
	}


	template<typename binary> inline AdjustedMantissa computeFloat(Decimal& d) {
		AdjustedMantissa answer;
		if (d.numDigits == 0) {
			answer.power2 = 0;
			answer.mantissa = 0;
			return answer;
		}
		if (d.decimalPoint < -324) {
			answer.power2 = 0;
			answer.mantissa = 0;
			return answer;
		} else if (d.decimalPoint >= 310) {
			answer.power2 = binary::infinitePower();
			answer.mantissa = 0;
			return answer;
		}

		static const uint32_t max_shift = 60;
		static const uint32_t num_powers = 19;
		static const uint8_t powers[19] = {
			0, 3, 6, 9, 13, 16, 19, 23, 26, 29,
			33, 36, 39, 43, 46, 49, 53, 56, 59,
		};
		int32_t exp2 = 0;
		while (d.decimalPoint > 0) {
			uint32_t n = uint32_t(d.decimalPoint);
			uint32_t shift = (n < num_powers) ? powers[n] : max_shift;
			decimalRightShift(d, shift);
			if (d.decimalPoint < -decimalPointRange) {
				answer.power2 = 0;
				answer.mantissa = 0;
				return answer;
			}
			exp2 += int32_t(shift);
		}
		while (d.decimalPoint <= 0) {
			uint32_t shift;
			if (d.decimalPoint == 0) {
				if (d.digits[0] >= 5) {
					break;
				}
				shift = (d.digits[0] < 2) ? 2 : 1;
			} else {
				uint32_t n = uint32_t(-d.decimalPoint);
				shift = (n < num_powers) ? powers[n] : max_shift;
			}
			decimalLeftShift(d, shift);
			if (d.decimalPoint > decimalPointRange) {
				answer.power2 = 0xFF;
				answer.mantissa = 0;
				return answer;
			}
			exp2 -= int32_t(shift);
		}
		exp2--;
		constexpr int32_t minimumExponent = binary::minimumExponent();
		while ((minimumExponent + 1) > exp2) {
			uint32_t n = uint32_t((minimumExponent + 1) - exp2);
			if (n > max_shift) {
				n = max_shift;
			}
			decimalRightShift(d, n);
			exp2 += int32_t(n);
		}
		if ((exp2 - minimumExponent) >= binary::infinitePower()) {
			answer.power2 = binary::infinitePower();
			answer.mantissa = 0;
			return answer;
		}

		const int mantissa_size_in_bits = binary::mantissaExplicitBits() + 1;
		decimalLeftShift(d, mantissa_size_in_bits);

		uint64_t mantissa = round(d);
		if (mantissa >= (uint64_t(1) << mantissa_size_in_bits)) {
			decimalRightShift(d, 1);
			exp2 += 1;
			mantissa = round(d);
			if ((exp2 - minimumExponent) >= binary::infinitePower()) {
				answer.power2 = binary::infinitePower();
				answer.mantissa = 0;
				return answer;
			}
		}
		answer.power2 = exp2 - binary::minimumExponent();
		if (mantissa < (uint64_t(1) << binary::mantissaExplicitBits())) {
			answer.power2--;
		}
		answer.mantissa = mantissa & ((uint64_t(1) << binary::mantissaExplicitBits()) - 1);
		return answer;
	}

	template<typename binary> inline AdjustedMantissa parseLongMantissa(const char* first) {
		Decimal d = parseDecimal(first);
		return computeFloat<binary>(d);
	}

	inline double fromChars(const char* first) noexcept {
		bool negative = first[0] == '-';
		if (negative) {
			first++;
		}
		AdjustedMantissa am = parseLongMantissa<binaryFormat<double>>(first);
		uint64_t word = am.mantissa;
		word |= uint64_t(am.power2) << binaryFormat<double>::mantissaExplicitBits();
		word = negative ? word | (uint64_t(1) << binaryFormat<double>::signIndex()) : word;
		double value;
		std::memcpy(&value, &word, sizeof(double));
		return value;
	}

	inline size_t significantDigits(const uint8_t* start_digits, size_t digit_count) {
		const uint8_t* start = start_digits;
		while ((*start == '0') || (*start == '.')) {
			++start;
		}
		return digit_count - size_t(start - start_digits);
	}

	inline static bool parseFloatFallback(const uint8_t* ptr, double* outDouble) {
		*outDouble = fromChars(reinterpret_cast<const char*>(ptr));
		return !(*outDouble > (std::numeric_limits<double>::max)() || *outDouble < std::numeric_limits<double>::lowest());
	}

	inline Jsonifier slowFloatParsing(const uint8_t* src, TapeWriter writer) {
		double d;
		if (parseFloatFallback(src, &d)) {
			return writer.appendDouble(d);
		}
		return ErrorCode::InvalidNumber;
	}

	constexpr int smallestPower = -342;
	constexpr int largestPower = 308;

	const double powerOfTen[] = { 1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20,
		1e21, 1e22 };

	inline bool computeFloat64(int64_t power, uint64_t i, bool negative, double& d) {
		if (0 <= power && power <= 22 && i <= 9007199254740991) {

			if (power < 0) {
				d = d / powerOfTen[-power];
			} else {
				d = d * powerOfTen[power];
			}
			if (negative) {
				d = -d;
			}
			return true;
		}
		return false;
	}

	inline Jsonifier writeFloat(const uint8_t* const src, bool negative, uint64_t i, const uint8_t* start_digits, size_t digit_count,
		int64_t exponent, TapeWriter& writer) {
		if (digit_count > 19 && significantDigits(start_digits, digit_count) > 19) {
			writer.skipDouble();
			return slowFloatParsing(src, writer);
		}
		if ((exponent < smallestPower) || (exponent > largestPower)) {
			static_assert(smallestPower <= -342, "smallestPower is not small enough");
			if ((exponent < smallestPower) || (i == 0)) {
				return writer.appendDouble(negative ? -0.0 : 0.0);
			} else {
				return ErrorCode::InvalidNumber;
			}
		}
		double d;
		if (!computeFloat64(exponent, i, negative, d)) {
			if (!parseFloatFallback(src, &d)) {
				return ErrorCode::InvalidNumber;
			}
		}
		return writer.appendDouble(d);
	}

	inline Jsonifier parseNumber(const uint8_t* const src, TapeWriter& writer) {
		bool negative = (*src == '-');
		const uint8_t* p = src + uint8_t(negative);
		const uint8_t* const startDigits = p;
		uint64_t i = 0;
		while (parseDigit(*p, i)) {
			p++;
		}
		size_t digitCount = size_t(p - startDigits);
		if (digitCount == 0 || ('0' == *startDigits && digitCount > 1)) {
			return ErrorCode::InvalidNumber;
		}
		int64_t exponent = 0;
		bool isFloat = false;
		if ('.' == *p) {
			isFloat = true;
			++p;
			parseDecimal(src, p, i, exponent);
			digitCount = int(p - startDigits);
		}
		if (('e' == *p) || ('E' == *p)) {
			isFloat = true;
			++p;
			parseExponent(src, p, exponent);
		}
		if (isFloat) {
			const bool dirtyEnd = isNotStructuralOrWhiteSpace(*p);
			auto returnValue = writeFloat(src, negative, i, startDigits, digitCount, exponent, writer);
			if (dirtyEnd) {
				return ErrorCode::InvalidNumber;
			}
			return returnValue;
		}

		size_t longestDigitCount = negative ? 19 : 20;
		if (digitCount > longestDigitCount) {
			return ErrorCode::InvalidNumber;
		}
		if (digitCount == longestDigitCount) {
			if (negative) {
				if (i > uint64_t(INT64_MAX) + 1) {
					return ErrorCode::InvalidNumber;
				}
				if (isNotStructuralOrWhiteSpace(*p)) {
					return ErrorCode::InvalidNumber;
				}
				return writer.appendS64(~i);
			} else if (src[0] != uint8_t('1') || i <= uint64_t(INT64_MAX)) {
				return ErrorCode::InvalidNumber;
			}
		}
		Jsonifier returnValue{};
		if (i > uint64_t(INT64_MAX)) {
			returnValue = writer.appendS64(i);
		} else {
			returnValue = writer.appendS64(negative ? (~i + 1) : i);
		}
		if (isNotStructuralOrWhiteSpace(*p)) {
			return ErrorCode::InvalidNumber;
		}
		return returnValue;
	}

	inline std::string_view TapeBuilder::visitString(JsonIterator& iter, const uint8_t* value) noexcept {
		uint8_t* dst01 = onStartString(iter);
		auto dst02 = parseString(value + 1, dst01);
		if (dst02 == nullptr) {
			return std::string_view{};
		}
		onEndString(dst02);
		return std::string_view{ reinterpret_cast<char*>(dst01), static_cast<size_t>(dst02 - dst01) };
	}

	inline std::string_view TapeBuilder::visitRootString(JsonIterator& iter, const uint8_t* value) noexcept {
		return visitString(iter, value);
	}

	inline Jsonifier TapeBuilder::visitNumber(JsonIterator& iter, const uint8_t* value) noexcept {
		return parseNumber(value, this->tape);
	}

	inline Jsonifier TapeBuilder::visitRootNumber(JsonIterator& iter, const uint8_t* value) noexcept {
		std::unique_ptr<uint8_t[]> copy(new (std::nothrow) uint8_t[iter.remainingLen() + 256]);
		if (copy.get() == nullptr) {
			return ErrorCode::MemAlloc;
		}
		std::memcpy(copy.get(), value, iter.remainingLen());
		std::memset(copy.get() + iter.remainingLen(), ' ', 256);
		Jsonifier error = visitNumber(iter, copy.get());
		return error;
	}

	inline ErrorCode TapeBuilder::visitTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidTrueAtom(reinterpret_cast<const char*>(value))) {
			return ErrorCode::TAtomError;
		}
		tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidTrueAtom(reinterpret_cast<const char*>(value))) {
			return ErrorCode::TAtomError;
		}
		tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidFalseAtom(reinterpret_cast<const char*>(value))) {
			return ErrorCode::FAtomError;
		}
		tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidFalseAtom(reinterpret_cast<const char*>(value))) {
			return ErrorCode::FAtomError;
		}
		tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitNullAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidNullAtom(reinterpret_cast<const char*>(value))) {
			return ErrorCode::NAtomError;
		}
		tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootNullAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidNullAtom(reinterpret_cast<const char*>(value))) {
			return ErrorCode::NAtomError;
		}
		tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline uint32_t TapeBuilder::nextTapeIndex(JsonIterator& iter) const noexcept {
		auto startTapeIndex = uint32_t(*tape.nextTapeLocation - *iter.masterParser.getStructuralIndexes());
		return startTapeIndex;
	}

	inline ErrorCode TapeBuilder::emptyContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept {
		auto startIndex = nextTapeIndex(iter);
		tape.append(startIndex + 2, start);
		tape.append(startIndex, end);
		return ErrorCode::Success;
	}

	inline void TapeBuilder::startContainer(JsonIterator& iter) noexcept {
		iter.masterParser.openContainers[iter.depth].tapeIndex = nextTapeIndex(iter);
		iter.masterParser.openContainers[iter.depth].count = 0;
		tape.skip();
	}

	inline ErrorCode TapeBuilder::endContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept {
		const uint32_t startTapeIndex = iter.masterParser.openContainers[iter.depth].tapeIndex;
		tape.append(startTapeIndex, end);
		const uint32_t count = iter.masterParser.openContainers[iter.depth].count;
		const uint32_t cntsat = count > 0xFFFFFF ? 0xFFFFFF : count;
		TapeWriter::write(iter.masterParser.getStructuralIndexes()[startTapeIndex], nextTapeIndex(iter) | (uint64_t(cntsat) << 32), start);
		return ErrorCode::Success;
	}

	inline uint8_t* TapeBuilder::onStartString(JsonIterator& iter) noexcept {
		tape.append(currentStringBufferLocation - reinterpret_cast<uint8_t*>(iter.masterParser.getStringViewNew()), TapeType::STRING);
		return currentStringBufferLocation + sizeof(uint32_t);
	}

	inline void TapeBuilder::onEndString(uint8_t* dst) noexcept {
		uint32_t stringLength = uint32_t(dst - (currentStringBufferLocation + sizeof(uint32_t)));
		memcpy(currentStringBufferLocation, &stringLength, sizeof(uint32_t));
		*dst = 0;
		currentStringBufferLocation = dst + 1;
	}

	inline Jsonifier JsonIterator::walkDocument(TapeBuilder& visitor) noexcept {
		JsonConstructor constructor{};
		if (atEof()) {
			return ErrorCode::Empty;
		}

		{
			auto value = this->advance();

			switch (*value) {
				case '{':
					if (*peek() == '}') {
						advance();
						visitor.visitEmptyObject(*this);
						break;
					}
					goto object_begin;
				case '[':
					if (*peek() == ']') {
						advance();
						visitor.visitEmptyArray(*this);
						break;
					}
					goto array_begin;
				default:
					visitor.visitRootPrimitive(*this, value);
					break;
			}
		}
		goto document_end;

	object_begin:
		depth++;
		if (depth >= masterParser.getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		masterParser.getIsArray()[depth] = false;
		visitor.visitObjectStart(*this);

		{
			auto key = this->advance();
			if (*key != '"') {
				return ErrorCode::TapeError;
			}
			visitor.incrementCount(*this);
			std::string newKey = static_cast<std::string>(visitor.visitKey(*this, key));

			constructor.setCurrentKey(std::move(newKey));
		}

	object_field:
		if (*advance() != ':') {
			return ErrorCode::TapeError;
		}
		{
			auto value = this->advance();
			switch (*value) {
				case '{':
					if (*peek() == '}') {
						advance();
						visitor.visitEmptyObject(*this);
						break;
					}

					constructor.startNewObject();
					goto object_begin;
				case '[':
					if (*peek() == ']') {
						advance();
						visitor.visitEmptyArray(*this);
						break;
					}
					constructor.startNewArray();
					goto array_begin;
				default:
					constructor.appendPrimitiveElement(visitor.visitPrimitive(*this, value));
					break;
			}
		}

	object_continue:
		switch (*advance()) {
			case ',':
				visitor.incrementCount(*this);
				{
					auto key = this->advance();
					if (*key != '"') {
						return ErrorCode::TapeError;
					}
					auto newKey = static_cast<std::string>(visitor.visitKey(*this, key));
					constructor.setCurrentKey(std::move(newKey));
				}
				constructor.setAppendType(JsonType::Object);
				goto object_field;
			case '}':
				constructor.endObject();
				visitor.visitObjectEnd(*this);
				goto scope_end;
			default:
				return ErrorCode::TapeError;
		}

	scope_end:
		depth--;
		if (depth == 0) {
			goto document_end;
		}
		if (masterParser.getIsArray()[depth]) {
			goto array_continue;
		}
		goto object_continue;

	array_begin:
		depth++;
		if (depth >= masterParser.getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		masterParser.getIsArray()[depth] = true;
		visitor.visitArrayStart(*this);
		visitor.incrementCount(*this);

	array_value : {
		auto value = this->advance();
		switch (*value) {
			case '{':
				if (*peek() == '}') {
					advance();
					visitor.visitEmptyObject(*this);
					break;
				}
				constructor.startNewObject();
				goto object_begin;
			case '[':
				if (*peek() == ']') {
					advance();
					visitor.visitEmptyArray(*this);
					break;
				}
				constructor.startNewArray();
				goto array_begin;
			default:
				constructor.appendPrimitiveElement(visitor.visitPrimitive(*this, value));
				break;
		}
	}

	array_continue:
		switch (*advance()) {
			case ',':
				visitor.incrementCount(*this);
				goto array_value;
			case ']':
				constructor.endArray();
				visitor.visitArrayEnd(*this);
				goto scope_end;
			default:
				return ErrorCode::TapeError;
		}

	document_end:
		visitor.visitDocumentEnd(*this);

		*masterParser.getStructuralIndexes() = nextStructural - masterParser.getStructuralIndexes();

		//if (*masterParser.getNextStructural() != masterParser.getTapeLength() - 1) {
		//return ErrorCode::TapeError;
		//}

		return constructor.getResult();
	}

	inline Jsonifier JsonIterator::visitRootPrimitive(TapeBuilder& visitor, const uint8_t* value) noexcept {
		switch (*value) {
			case '"':
				return visitor.visitRootString(*this, value);
			case 't':
				return visitor.visitRootTrueAtom(*this, value);
			case 'f':
				return visitor.visitRootFalseAtom(*this, value);
			case 'n':
				return visitor.visitRootNullAtom(*this, value);
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return visitor.visitRootNumber(*this, value);
			default:
				return ErrorCode::TapeError;
		}
	}

	inline Jsonifier JsonIterator::visitPrimitive(TapeBuilder& visitor, const uint8_t* value) noexcept {
		switch (*value) {
			case '"':
				return visitor.visitString(*this, value);
			case 't':
				return visitor.visitTrueAtom(*this, value);
			case 'f':
				return visitor.visitFalseAtom(*this, value);
			case 'n':
				return visitor.visitNullAtom(*this, value);
			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return visitor.visitNumber(*this, value);
			default:
				return ErrorCode::TapeError;
		}
	}

	Jsonifier SimdJsonValue::getJsonData() {
		return TapeBuilder::parseDocument(*this);
	}

};