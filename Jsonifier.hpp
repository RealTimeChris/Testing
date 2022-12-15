#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)

#include <source_location>
#include <unordered_map>
#include <immintrin.h>
#include <string_view>
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
		const uint8_t byteSize{ 8 };
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

	enum ErrorCode : int8_t {
		Empty = 0,
		TapeError = 1,
		DepthError = 2,
		Success = 3,
		ParseError = 4,
		StringError = 5,
		TAtomError = 6,
		FAtomError = 7,
		NAtomError = 8,
		MemAlloc = 9
	};

	class JsonifierReal {};

	class Jsonifier;

	template<typename Ty>
	concept IsConvertibleToJsonifier = std::convertible_to<Ty, Jsonifier>;

	class Jsonifier {
	  public:
		using MapAllocatorType = std::allocator<std::pair<const std::string, Jsonifier>>;
		template<typename OTy> using AllocatorType = std::allocator<OTy>;
		template<typename OTy> using AllocatorTraits = std::allocator_traits<AllocatorType<OTy>>;
		using ObjectType = std::map<std::string, Jsonifier, std::less<>, MapAllocatorType>;
		using ArrayType = std::vector<Jsonifier, AllocatorType<Jsonifier>>;
		using StringType = std::string_view;
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
			return Ty{};
		}

		JsonType getType() noexcept;

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

		void appendSmallIntegerExt(const uint8_t value);

		void appendListHeader(const uint32_t sizeNew);

		void appendMapHeader(const uint32_t sizeNew);

		void appendIntegerExt(const uint32_t value);

		void appendBool(bool data);

		void appendVersion();

		void appendNilExt();

		void appendNil();

		void setValue(JsonType TypeNew);

		void destroy() noexcept;

		friend bool operator==(const Jsonifier& lhs, const Jsonifier& rhs);
	};

	template<> inline Jsonifier::ObjectType Jsonifier::getValue() {
		return *this->jsonValue.object;
	}

	template<> inline Jsonifier::ArrayType Jsonifier::getValue() {
		return *this->jsonValue.array;
	}

	template<> inline Jsonifier::StringType Jsonifier::getValue() {
		return *this->jsonValue.string;
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

	class SimdTape {
	  public:

		inline SimdTape& operator=(SimdTape&& other) noexcept {
			this->tapePtrs = std::move(other.tapePtrs);
			this->currentIndex = other.currentIndex;
			return *this;
		}

		inline SimdTape(SimdTape&& other) noexcept {
			*this = std::move(other);
		}

		inline SimdTape() noexcept = default;

		inline SimdTape(size_t count) noexcept {
			this->tapePtrs = std::make_unique<uint32_t[]>(count);
		};

		inline uint32_t* operator[](size_t index) {
			return &this->tapePtrs[index];
		}

		inline operator uint32_t*() {
			return &this->tapePtrs[0];
		}

		inline int64_t addTapeValues(int64_t& theBits, size_t currentIndexNew, size_t currentIndexIntoString) {
			int64_t value = static_cast<int64_t>(__popcnt64(theBits));
			for (size_t i = 0; i < value; i++) {
				this->tapePtrs[static_cast<size_t>(this->currentIndex) + i] =
					_tzcnt_u64(theBits) + (currentIndexNew * 64ull) + currentIndexIntoString;
				theBits = _blsr_u64(theBits);
			}

			this->currentIndex += value;
			return value;
		}

	  protected:
		std::unique_ptr<uint32_t[]> tapePtrs{};
		uint32_t currentIndex{};
	};

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

		inline operator __m128i&() {
			return this->value;
		}

	  protected:
		__m128i value{};
	};

	class SimdBase256 {
	  public:
		inline SimdBase256() noexcept = default;

		inline SimdBase256& operator=(__m256i other) {
			this->value = other;
			return *this;
		}

		inline SimdBase256(__m256i other) {
			*this = other;
		}

		inline SimdBase256& operator=(char other) {
			this->value = _mm256_set1_epi8(other);
			return *this;
		}

		inline SimdBase256(char other) {
			*this = other;
		}

		inline SimdBase256& operator=(const char* values){
			this->value = _mm256_loadu_epi8(values);
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

		inline SimdBase256& operator|(SimdBase256 other) {
			*this = _mm256_or_si256(this->value, other);
			return *this;
		}

		inline SimdBase256& operator&(SimdBase256 other) {
			*this = _mm256_and_si256(this->value, other);
			return *this;
		}

		inline SimdBase256& operator^(SimdBase256 other) {
			*this = _mm256_xor_si256(this->value, other);
			return *this;
		}

		inline SimdBase256& operator+(SimdBase256 other) {
			*this = _mm256_add_epi8(this->value, other);
			return *this;
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

		inline SimdBase256 operator<<(size_t amount) {
			int64_t values[4]{};
			values[0] = this->getInt64(0);
			values[1] = this->getInt64(1);
			values[2] = this->getInt64(2);
			values[3] = this->getInt64(3);
			SimdBase256 newValues{};
			newValues.insertInt64(values[0] << (amount % 64), 0);
			newValues.insertInt64(values[1] << (amount % 64) | ((values[0] & 1ull) << 63), 1);
			newValues.insertInt64(values[2] << (amount % 64) | ((values[1] & 1ull) << 63), 2);
			newValues.insertInt64(values[3] << (amount % 64) | ((values[2] & 1ull) << 63), 3);
			return newValues;
		}

		inline SimdBase256 operator~() {
			SimdBase256 newValues{};
			newValues.insertInt64(~this->getInt64(0), 0);
			newValues.insertInt64(~this->getInt64(1), 1);
			newValues.insertInt64(~this->getInt64(2), 2);
			newValues.insertInt64(~this->getInt64(3), 3);
			return newValues;
		}

		inline SimdBase256 carrylessMultiplication(int64_t& prevInString) {
			SimdBase128 allOnes{ '\xFF' };
			SimdBase256 returnValue{};
			auto inString00 =
				_mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(0)), allOnes, 0)) ^ prevInString;
			prevInString = inString00 >> 63;
			auto inString01 =
				_mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(1)), allOnes, 0)) ^ prevInString;
			prevInString = inString01 >> 63;
			auto inString02 =
				_mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(2)), allOnes, 0)) ^ prevInString;
			prevInString = inString02 >> 63;
			auto inString03 =
				_mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(3)), allOnes, 0)) ^ prevInString;
			prevInString = inString03 >> 63;
			return SimdBase256{ inString00, inString01, inString02, inString03 };
		}

		inline bool collectCarries(SimdBase256 other1, SimdBase256* result) {
			bool returnValue{};
			for (size_t x = 0; x < 4; ++x) {
				uint64_t returnValue64{};
				if (_addcarry_u64(0, this->getUint64(x), other1.getUint64(x), reinterpret_cast<unsigned long long*>(&returnValue64))) {
					returnValue = true;
				}
				result->insertInt64(returnValue64, x);
			}
			return returnValue;
		}

		inline void printBits(const std::string& valuesTitle) {
			std::cout << valuesTitle;
			for (size_t x = 0; x < 32; ++x) {
				for (size_t y = 0; y < 8; ++y) {
					std::cout << std::bitset<1>{ static_cast<uint64_t>(*(reinterpret_cast<int8_t*>(&this->value) + x)) >> y };
				}
			}
			std::cout << std::endl;
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

	inline uint64_t convertSimd256To64BitUint(SimdBase256 inputA, SimdBase256 inputB) {
		int32_t r_lo = _mm256_movemask_epi8(inputA);
		int32_t r_hi = _mm256_movemask_epi8(inputB);
		return static_cast<uint64_t>(r_lo) | static_cast<uint64_t>(r_hi) << 32;
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
				auto newValue = this->S256.getInt64(x);
				returnValue += jsonRawTape.addTapeValues(newValue, x, currentIndex);
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
			return { convertSimd256To64BitUint(whiteSpaceReal[0], whiteSpaceReal[1]), convertSimd256To64BitUint(whiteSpaceReal[2], whiteSpaceReal[3]),
				convertSimd256To64BitUint(whiteSpaceReal[4], whiteSpaceReal[5]), convertSimd256To64BitUint(whiteSpaceReal[6], whiteSpaceReal[7]) };
		}

		inline SimdBase256 collectStructuralCharacters() {
			char valuesNew[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
			SimdBase256 opTable{ valuesNew };
			SimdBase256 structural[8]{};
			for (size_t x = 0; x < 8; ++x) {
				SimdBase256 valuesNew00 = this->values[x] | SimdBase256{ 0x20 };
				structural[x] = this->values[x].shuffle(opTable) == valuesNew00;
			}

			return SimdBase256{ convertSimd256To64BitUint(structural[0], structural[1]), convertSimd256To64BitUint(structural[2], structural[3]),
				convertSimd256To64BitUint(structural[4], structural[5]), convertSimd256To64BitUint(structural[6], structural[7]) };
		}

		inline SimdBase256 collectQuotedRange(int64_t& prevInString) {
			SimdBase256 backslashes = _mm256_set1_epi8('\\');
			SimdBase256 backslashesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				backslashesReal[x] = this->values[x] == backslashes;
			}
			
			auto B256 = SimdBase256{ convertSimd256To64BitUint(backslashesReal[0], backslashesReal[1]),
				convertSimd256To64BitUint(backslashesReal[2], backslashesReal[3]), convertSimd256To64BitUint(backslashesReal[4], backslashesReal[5]),
				convertSimd256To64BitUint(backslashesReal[6], backslashesReal[7]) };

			SimdBase256 E{ _mm256_set1_epi8(0b01010101) };
			SimdBase256 O{ _mm256_set1_epi8(0b10101010) };
			this->S256 = B256 & ~(B256 << 1);
			SimdBase256 ES = E & this->S256;
			SimdBase256 EC{};
			B256.collectCarries(ES, &EC);
			SimdBase256 ECE = EC & ~B256;
			SimdBase256 OD1 = ECE & ~E;
			SimdBase256 OS = this->S256 & O;
			SimdBase256 OC = B256 + OS;
			SimdBase256 OCE = OC & ~B256;
			SimdBase256 OD2 = OCE & E;
			SimdBase256 OD = OD1 | OD2;
			this->Q256 = this->Q256.bitAndNot(OD);
			return this->Q256.carrylessMultiplication(prevInString);
		}

		inline SimdBase256 collectQuotes() {
			SimdBase256 quotes = _mm256_set1_epi8('"');
			SimdBase256 quotesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				quotesReal[x] = (this->values[x] == quotes);
			}

			return SimdBase256{ convertSimd256To64BitUint(quotesReal[0], quotesReal[1]), convertSimd256To64BitUint(quotesReal[2], quotesReal[3]),
				convertSimd256To64BitUint(quotesReal[4], quotesReal[5]), convertSimd256To64BitUint(quotesReal[6], quotesReal[7]) };
			
		}

		inline SimdBase256 collectFinalStructurals() {
			this->S256 = this->S256 & ~this->R256;
			this->S256 = this->S256 | this->Q256;
			SimdBase256 P = this->S256 | this->W256;
			P = P << 1;
			P &= (~this->W256).bitAndNot(this->R256);
			this->S256 = this->S256 | P;
			return S256.bitAndNot((this->Q256.bitAndNot(this->R256)));
		}

		inline SimdStringSection(const char*valueNew,  int64_t& prevInString) {
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

	class SimdJsonValue {
	  public:
		
		inline SimdJsonValue() noexcept = default;

		inline SimdJsonValue(char* stringNew, size_t tapeLength);

		inline uint32_t* getAndIncrementNextStructural() {
			return this->nextStructural++;
		}

		inline const char* getStringView() {
			return this->stringView;
		}

		inline uint32_t* getNextStructural() {
			return this->nextStructural;
		}

		inline Jsonifier getJsonData();

		inline void generateJsonEvents() {
			uint32_t stringSize = this->stringLength;
			this->jsonRawTape = SimdTape{ stringLength };
			std::allocator<bool> allocatorBool{};
			std::allocator_traits<std::allocator<bool>> allocTraitsBool{};
			if (this->isArray) {
				delete[] this->isArray;
			}
			this->isArray = new bool[12]{};
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
			this->nextStructural = this->jsonRawTape.operator uint32_t*();
		}

		inline uint32_t getMaxDepth() {
			return this->maxDepth;
		}

		inline uint32_t getTapeLength() {
			return this->tapeLength;
		}

		inline bool* getIsArray() {
			return this->isArray;
		}

		inline ~SimdJsonValue() noexcept {
			delete[] this->isArray;
		}

	  protected:
		uint32_t* nextStructural{};
		uint32_t maxDepth{ 500 };
		uint32_t tapeLength{ 0 };
		uint32_t stringLength{};
		SimdTape jsonRawTape{};
		uint32_t depth{ 0 };
		char* stringView{};
		bool* isArray{};
	};

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
		return std::stoull(str);
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

	enum class ParsingState {
		StartDocument = 0,
		ObjectBegin = 1,
		ObjectField = 2,
		ObjectContinue = 3,
		ArrayBegin = 4,
		ArrayValue = 5,
		ArrayContinue = 6,
		ScopeEnd = 7,
		EndDocument = 8
	};

	struct JsonConstructor {
		inline uint32_t nextStringIndex() noexcept;
		inline size_t getCurrentElementSize() noexcept;
		inline uint32_t currentStringIndex() noexcept;
		inline std::string_view visitKey() noexcept;
		inline Jsonifier visitEmptyObject();
		inline Jsonifier visitEmptyArray();
		inline Jsonifier startDocument(Jsonifier);
		inline Jsonifier objectBegin(Jsonifier);
		inline Jsonifier objectField(Jsonifier);
		inline Jsonifier objectContinue(Jsonifier);
		inline Jsonifier arrayBegin(Jsonifier);
		inline Jsonifier arrayValue(Jsonifier);
		inline Jsonifier arrayContinue(Jsonifier);
		inline Jsonifier scopeEnd(Jsonifier);
		inline Jsonifier endDocument(Jsonifier);
		inline std::string_view visitString() noexcept;
		inline Jsonifier visitNumber(const char value) noexcept;
		inline bool visitTrueAtom(const char value) noexcept;
		inline bool visitFalseAtom(const char value) noexcept;
		inline std::nullptr_t visitNullAtom(const char value) noexcept;
		inline Jsonifier visitPrimitive(const char value) noexcept;

		inline JsonConstructor(SimdJsonValue& doc) noexcept;

		inline const char peek() noexcept;

		inline const char advance() noexcept;

		inline bool atEof() noexcept;

	  protected:
		int32_t depth{};
		SimdJsonValue* masterParser{};
		std::string currentKey{};
	};

	inline std::string_view JsonConstructor::visitKey() noexcept {
		auto currentKey = visitString();
		return currentKey;
	}

	inline std::string_view JsonConstructor::visitString() noexcept {
		auto currentKey = std::string_view{ &this->masterParser->getStringView()[this->currentStringIndex()] + 1, this->getCurrentElementSize() - 2 };
		return currentKey;
	}

	inline size_t JsonConstructor::getCurrentElementSize() noexcept {
		return this->nextStringIndex() - this->currentStringIndex();
	}

	inline Jsonifier JsonConstructor::visitNumber(const char value) noexcept {
		auto currentKey = std::string{ &this->masterParser->getStringView()[this->currentStringIndex()], this->getCurrentElementSize() };
		return stoull(currentKey);
	}

	inline bool JsonConstructor::visitTrueAtom(const char value) noexcept {
		if (!isValidTrueAtom(&value)) {
			return false;
		}
		return true;
	}

	inline bool JsonConstructor::visitFalseAtom(const char value) noexcept {
		if (!isValidFalseAtom(&value)) {
			return false;
		}
		return false;
	}

	inline std::nullptr_t JsonConstructor::visitNullAtom(const char value) noexcept {
		if (!isValidNullAtom(&value)) {
			return nullptr;
		}
		return nullptr;
	}

	inline Jsonifier JsonConstructor::visitEmptyObject() {
		return Jsonifier{ JsonType::Object };
	}

	inline Jsonifier JsonConstructor::visitEmptyArray() {
		return Jsonifier{ JsonType::Array };
	}

	inline uint32_t JsonConstructor::currentStringIndex() noexcept {
		return *(this->masterParser->getNextStructural() - 1);
	}

	inline uint32_t JsonConstructor::nextStringIndex() noexcept {
		return *this->masterParser->getNextStructural();
	}
				
	inline Jsonifier JsonConstructor::startDocument(Jsonifier jsonData) {
		if (this->atEof()) {
			return jsonData;
		}
		auto value = this->advance();
		switch (value) {
			case '{': {
				if (this->peek() == '}') {
					this->advance();
					return this->visitEmptyObject();
				}
				return this->objectBegin(std::move(jsonData));
			}
			case '[': {
				if (this->peek() == ']') {
					this->advance();
					return this->visitEmptyObject();
				}
				return this->arrayBegin(std::move(jsonData));
			}
			default:
				return this->visitPrimitive(value);
		}

	}

	inline Jsonifier JsonConstructor::objectBegin(Jsonifier jsonData) {
		this->depth++;
		if (this->depth >= this->masterParser->getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		this->masterParser->getIsArray()[this->depth] = false;
		auto key = advance();
		if (key != '"') {
			return ErrorCode::TapeError;
		}
		auto currentKey = this->visitKey();
		this->currentKey = currentKey;
		return this->objectField(std::move(jsonData));
	}

	inline Jsonifier JsonConstructor::objectField(Jsonifier jsonData) {
		if (this->advance() != ':') {
				return ErrorCode::TapeError;
		}
		auto value = this->advance();
		switch (value) {
			case '{': {
				Jsonifier jsonDataNew{};
				if (this->peek() == '}') {
					this->advance();
					return this->visitEmptyObject();
				}
				auto currentKey = this->currentKey;
				jsonData[currentKey] = this->objectBegin(jsonDataNew);
				return jsonData;
			}
			case '[': {
				Jsonifier jsonDataNew{};
				if (this->peek() == ']') {
					this->advance();
					return this->visitEmptyArray();
				}
				auto currentKey = this->currentKey;
				jsonData[currentKey] = this->arrayBegin(jsonDataNew);
				return jsonData;
			}
				
			default:
				auto currentKey = this->currentKey;
				jsonData[currentKey] = this->visitPrimitive(value);
				return this->objectContinue(std::move(jsonData));
		}
	}

	inline Jsonifier JsonConstructor::objectContinue(Jsonifier jsonData) {
		auto newValue = this->advance();
		switch (newValue) {
			case ',': {
				auto key = this->advance();
				if (key != '"') {
					return ErrorCode::TapeError;
				}
				auto currentKey = this->visitKey();
				this->currentKey = currentKey;
				return this->objectField(std::move(jsonData));
			}
			case '}':
				return this->scopeEnd(std::move(jsonData));
			default:
				return ErrorCode::TapeError;
		}
	}

	inline Jsonifier JsonConstructor::arrayBegin(Jsonifier jsonData) {
		Jsonifier jsonDataNew{};
		this->depth++;
		this->masterParser->getIsArray()[this->depth] = true;
		if (this->depth >= this->masterParser->getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		jsonData.emplaceBack(this->arrayValue(std::move(jsonDataNew)));
		return jsonData;
	}

	inline Jsonifier JsonConstructor::arrayValue(Jsonifier jsonData) {
		auto value = this->advance();
		switch (value) {
			case '{': {
				Jsonifier jsonDataNew{};
				if (this->peek() == '}') {
					this->advance();
					return this->visitEmptyObject();
				}
				return this->objectBegin(std::move(jsonDataNew));
			}
			case '[': {
				Jsonifier jsonDataNew{};
				if (this->peek() == ']') {
					this->advance();
					return this->visitEmptyArray();
				}
				return this->arrayBegin(std::move(jsonDataNew));
			}
			default:
				return this->visitPrimitive(value);
		}
	}

	inline Jsonifier JsonConstructor::arrayContinue(Jsonifier jsonData) {
		
		switch (this->advance()) {
			case ',': {
				Jsonifier jsonDataNew{};
				jsonData.emplaceBack(this->arrayValue(std::move(jsonDataNew)));
				return std::move(jsonData);
			}
			case ']': {
				return this->scopeEnd(std::move(jsonData));
			}				
			default:
				return ErrorCode::TapeError;
		}
	}

	inline Jsonifier JsonConstructor::scopeEnd(Jsonifier jsonData) {
		Jsonifier jsonDataNew{};
		this->depth--;
		if (this->depth == 0) {
			return this->endDocument(std::move(jsonData));
		}
		if (this->masterParser->getIsArray()[depth]) {
			return this->arrayContinue(std::move(jsonData));
		}
		return this->objectContinue(std::move(jsonData));
	}
	inline Jsonifier JsonConstructor::endDocument(Jsonifier jsonData) {
		return jsonData;
	}
	
	inline JsonConstructor::JsonConstructor(SimdJsonValue& _dom_parser) noexcept : masterParser{ &_dom_parser } {
	};

	inline const char JsonConstructor::peek() noexcept {
		return this->masterParser->getStringView()[*(this->masterParser->getNextStructural())];
	}

	inline const char JsonConstructor::advance() noexcept {
		return this->masterParser->getStringView()[*(this->masterParser->getAndIncrementNextStructural())];
	}

	inline bool JsonConstructor::atEof() noexcept {
		if (*this->masterParser->getNextStructural() >= (this->masterParser->getTapeLength()) && (*this->masterParser->getNextStructural()) > 0) {
			return true;
		} else {
			return false;
		}
	}

	inline Jsonifier JsonConstructor::visitPrimitive(const char value) noexcept {
		switch (value) {
			case '"':
				return this->visitString();
			case 't':
				return this->visitTrueAtom(value);
			case 'f':
				return this->visitFalseAtom(value);
			case 'n':
				return this->visitNullAtom(value);
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
				return this->visitNumber(value);
			default:
				return ErrorCode::TapeError;
		}
	}

	inline SimdJsonValue::SimdJsonValue(char* stringNew, size_t length) {
		if (length == 0) {
			throw DCAException{ "Failed to parse as the string size is 0." };
		}
		this->stringLength = length;
		this->stringView = stringNew;
	}

	Jsonifier SimdJsonValue::getJsonData() {
		this->generateJsonEvents();
		JsonConstructor theBuilder{ *this };
		Jsonifier jsonData{};
		jsonData = theBuilder.startDocument(std::move(jsonData));
		return jsonData;
	}

};