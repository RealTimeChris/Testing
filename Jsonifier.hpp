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
	#pragma comment(lib, "Ws2_32.lib")
	#include <WinSock2.h>
	#include <process.h>
	#ifdef Jsonifier_EXPORTS
		#define Jsonifier_Dll __declspec(dllexport)
	#else
		#define Jsonifier_Dll __declspec(dllimport)
	#endif
	#ifdef max
		#undef max
	#endif
	#ifdef min
		#undef min
	#endif
#else
	#define Jsonifier_Dll
	#include <arpa/inet.h>
#endif

namespace Jsonifier {

	template<typename RTy> void reverseByteOrder(RTy& net) {
		if constexpr (std::endian::native == std::endian::little) {
			switch (sizeof(RTy)) {
				case 1: {
					return;
				}
				case 2: {
					net =
						_mm256_extract_epi16(_mm256_shuffle_epi8(_mm256_insert_epi16(__m256i{}, net, 0), _mm256_insert_epi16(__m256i{}, 0x01, 0)), 0);
					return;
				}
				case 4: {
					net = _mm256_extract_epi32(
						_mm256_shuffle_epi8(_mm256_insert_epi32(__m256i{}, net, 0), _mm256_insert_epi32(__m256i{}, 0x10203, 0)), 0);
					return;
				}
				case 8: {
					net = _mm256_extract_epi64(
						_mm256_shuffle_epi8(_mm256_insert_epi64(__m256i{}, net, 0), _mm256_insert_epi64(__m256i{}, 0x102030405060708, 0)), 0);
					return;
				}
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

		Jsonifier& operator=(ObjectType&& data) noexcept;

		Jsonifier(ObjectType&& data) noexcept;

		Jsonifier& operator=(const ObjectType& data) noexcept;

		Jsonifier(const ObjectType& data) noexcept;

		Jsonifier& operator=(Jsonifier&& data) noexcept;

		Jsonifier(Jsonifier&& data) noexcept;

		Jsonifier& operator=(const Jsonifier& data) noexcept;

		Jsonifier(const Jsonifier& data) noexcept;

		operator std::string&&() noexcept;

		operator std::string() noexcept;

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

		void emplaceBack(Jsonifier&& data) noexcept;
		void emplaceBack(Jsonifier& data) noexcept;

		~Jsonifier() noexcept;

	  protected:
		JsonType type{ JsonType::Null };
		JsonValue jsonValue{};
		std::string string{};

		void serializeJsonToEtfString(const Jsonifier* jsonDataNew);

		void serializeJsonToJsonString(const Jsonifier* jsonDataNew);

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

		void writeString(const char* data, std::size_t length);

		void writeCharacter(const char Char);

		void appendBinaryExt(const std::string& bytes, const uint32_t sizeNew);

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

	inline uint64_t convertSimd256To64BitUint(SimdBase256 inputA, SimdBase256 inputB);

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

		inline SimdBase128(int64_t value00, int64_t value01) {
			this->value = _mm_set_epi64x(value01, value00);
		}

		inline SimdBase128(uint64_t value00, uint64_t value01) {
			this->value = _mm_set_epi64x(static_cast<int64_t>(value01), static_cast<int64_t>(value00));
		}

		inline SimdBase128& operator=(__m128i other) {
			this->value = other;
			return *this;
		}

		inline SimdBase128(__m128i other) {
			*this = other;
		}

		inline operator __m128i() {
			return this->value;
		}

		inline SimdBase128 operator|(SimdBase128 other) {
			return _mm_or_si128(*this, other);
		}

		inline SimdBase128 operator&(SimdBase128 other) {
			return _mm_and_si128(*this, other);
		}

		inline SimdBase128 operator^(SimdBase128 other) {
			return _mm_xor_si128(*this, other);
		}

		inline SimdBase128 operator+(SimdBase128 other) {
			return _mm_add_epi8(*this, other);
		}

		inline SimdBase128 operator|=(SimdBase128 other) {
			*this = *this | other;
			return *this;
		}

		inline SimdBase128 operator&=(SimdBase128 other) {
			*this = *this & other;
			return *this;
		}

		inline SimdBase128 operator^=(SimdBase128 other) {
			*this = *this ^ other;
			return *this;
		}

		inline SimdBase128 operator==(SimdBase128 other) {
			return _mm_cmpeq_epi8(this->value, other);
		}

		inline SimdBase128 operator<<(size_t amount) {
			SimdBase128 value{};
			for (size_t x = 0; x < 2; ++x) {
				*(reinterpret_cast<int64_t*>(&value) + x) |= (*(reinterpret_cast<int64_t*>(&this->value) + x) << (amount % 64));
				if (x > 0) {
					*(reinterpret_cast<int64_t*>(&value) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x - 1) >> 63) & 1);
				}
			}
			return value;
		}

		inline SimdBase128 operator~() {
			SimdBase128 value{};
			for (size_t x = 0; x < 2; ++x) {
				*(reinterpret_cast<int64_t*>(&value) + x) = ~*(reinterpret_cast<int64_t*>(&this->value) + x);
			}
			return value;
		}

		inline SimdBase128 bitAndNot(SimdBase128 other) {
			return _mm_andnot_si128(other, *this);
		}

		inline SimdBase128 shuffle(SimdBase128 other) {
			return _mm_shuffle_epi8(other, *this);
		}

	  protected:
		__m128i value{};
	};

	class SimdBase256 {
	  public:
		inline SimdBase256() noexcept = default;

		inline SimdBase256& operator=(char other) {
			this->value = _mm256_set1_epi8(other);
			return *this;
		}

		inline SimdBase256(char other) {
			*this = other;
		}

		inline SimdBase256(char values[32]) {
			*this = _mm256_load_si256(reinterpret_cast<__m256i*>(values));
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

		inline operator __m256i() {
			return this->value;
		}

		uint64_t getUint64(size_t index) {
			switch (index) {
				case 0: {
					return _mm256_extract_epi64(*this, 0);
				}
				case 1: {
					return _mm256_extract_epi64(*this, 1);
				}
				case 2: {
					return _mm256_extract_epi64(*this, 2);
				}
				case 3: {
					return _mm256_extract_epi64(*this, 3);
				}
			}
		}

		void insertUint64(uint64_t value, size_t index) {
			switch (index) {
				case 0: {
					*this = _mm256_insert_epi64(*this, value, 0);
					break;
				}
				case 1: {
					*this = _mm256_insert_epi64(*this, value, 1);
					break;
				}
				case 2: {
					*this = _mm256_insert_epi64(*this, value, 2);
					break;
				}
				case 3: {
					*this = _mm256_insert_epi64(*this, value, 3);
					break;
				}
			}
		}

		uint32_t leadingZeroes() {
			for (size_t x = 0; x < 4; ++x) {
				return int32_t(_tzcnt_u64(this->getUint64(x)));
			}
		}

		void flipFirstBit() {
			SimdBase256 newValue{};
			newValue.insertUint64(1, 0);
			newValue = newValue << this->leadingZeroes();
			*this &= ~newValue;
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

		inline SimdBase256 operator|=(SimdBase256 other) {
			*this = *this | other;
			return *this;
		}

		inline SimdBase256 operator&=(SimdBase256 other) {
			*this = *this & other;
			return *this;
		}

		inline SimdBase256 operator^=(SimdBase256 other) {
			*this = *this ^ other;
			return *this;
		}

		inline SimdBase256 operator==(char other) {
			*this == _mm256_set1_epi8(other);
			return *this;
		}

		inline SimdBase256 operator==(SimdBase256 other) {
			return _mm256_cmpeq_epi8(this->value, other);
		}

		inline SimdBase256 operator<<(size_t amount) {
			SimdBase256 value{};
			for (size_t x = 0; x < 4; ++x) {
				*(reinterpret_cast<int64_t*>(&value) + x) |= (*(reinterpret_cast<int64_t*>(&this->value) + x) << (amount % 64));
				if (x > 0) {
					*(reinterpret_cast<int64_t*>(&value) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x - 1) >> 63) & 1);
				}
			}
			return value;
		}

		inline SimdBase256 operator>>(size_t amount) {
			SimdBase256 value{};
			for (size_t x = 0; x < 4; ++x) {
				*(reinterpret_cast<int64_t*>(&value) + x) |= (*(reinterpret_cast<int64_t*>(&this->value) + x) >> (amount % 64));
				if (x < 3) {
					*(reinterpret_cast<int64_t*>(&value) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x + 1) >> 63) & 1);
				}
			}
			return value;
		}

		inline SimdBase256 operator~() {
			SimdBase256 value{};
			for (size_t x = 0; x < 4; ++x) {
				*(reinterpret_cast<uint64_t*>(&value) + x) = ~*(reinterpret_cast<uint64_t*>(&this->value) + x);
			}
			return value;
		}

		inline SimdBase256 carrylessMultiplication(char operand) {
			auto inString01 = static_cast<int64_t>(_mm_cvtsi128_si64(
				_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<int64_t*>(&this->value) + 0)), SimdBase128{ operand }, 0)));
			auto prevInString = static_cast<int64_t>(inString01) >> 63;
			auto inString02 =
				static_cast<int64_t>(_mm_cvtsi128_si64(
					_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<int64_t*>(&this->value) + 1)), SimdBase128{ operand }, 0))) ^
				prevInString;
			prevInString = static_cast<int64_t>(inString02) >> 63;
			auto inString03 =
				static_cast<int64_t>(_mm_cvtsi128_si64(
					_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<int64_t*>(&this->value) + 2)), SimdBase128{ operand }, 0))) ^
				prevInString;
			prevInString = static_cast<int64_t>(inString03) >> 63;
			auto inString04 =
				static_cast<int64_t>(_mm_cvtsi128_si64(
					_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, *(reinterpret_cast<int64_t*>(&this->value) + 3)), SimdBase128{ operand }, 0))) ^
				prevInString;
			return SimdBase256{ inString01, inString02, inString03, inString04 };
		}

		inline SimdBase256 collectCarries(SimdBase256 other) {
			SimdBase256 returnValue{};
			for (size_t x = 0; x < 4; ++x) {
				uint64_t returnValue64{};
				_addcarry_u64(0, *(reinterpret_cast<uint64_t*>(&other) + x), *(reinterpret_cast<uint64_t*>(&this->value) + x),
					reinterpret_cast<unsigned long long*>(&returnValue64));
				*(reinterpret_cast<uint64_t*>(&returnValue) + x) = returnValue64;
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
			return _mm256_andnot_si256(other, *this);
		}

		inline SimdBase256 shuffle(SimdBase256 other) {
			return _mm256_shuffle_epi8(other, *this);
		}

		inline std::basic_string<uint32_t> getSetBitIndices() {
			std::basic_string<uint32_t> returnVector{};
			for (int64_t x = 0; x < 4; ++x) {
				for (int64_t y = 0; y < 64; ++y) {
					if (*(reinterpret_cast<int64_t*>(&this->value) + x) >> y & 1) {
						returnVector.push_back(static_cast<uint32_t>(y + (x * 64)));
					}
				}
			}
			return returnVector;
		}

	  protected:
		__m256i value{};
	};

	inline uint64_t convertSimd256To64BitUint(SimdBase256 inputA, SimdBase256 inputB) {
		uint64_t lowBytes = _mm256_movemask_epi8(inputA);
		uint64_t highBytes = _mm256_movemask_epi8(inputB);
		return lowBytes | (highBytes << 32);
	}

	enum class IndexTypes { Whitespace = 0, Quotes = 1, Structural = 2 };

	class SimdStringSection {
	  public:
		inline SimdStringSection() noexcept = default;

		inline void packStringIntoValue(SimdBase256* theValue, const char* string) {
			for (size_t x = 0; x < 32; ++x) {
				*(reinterpret_cast<int8_t*>(theValue) + x) = string[x];
			}
		}

		inline std::basic_string<uint32_t> getStructuralIndices() {
			return this->S256.getSetBitIndices();
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

		inline SimdBase256 collectStructuralCharacters(bool prevInString) {
			this->R256 = this->Q256;
			this->R256 = this->R256.carrylessMultiplication('\xFF');
			
			char valuesNew[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
			SimdBase256 opTable{ valuesNew };
			SimdBase256 structural[8]{};
			for (size_t x = 0; x < 8; ++x) {
				auto valuesNew00 = this->values[x] | SimdBase256{ 0x20 };
				structural[x] = this->values[x].shuffle(opTable) == valuesNew00;
			}

			this->S256 =
				SimdBase256{ convertSimd256To64BitUint(structural[0], structural[1]), convertSimd256To64BitUint(structural[2], structural[3]),
					convertSimd256To64BitUint(structural[4], structural[5]), convertSimd256To64BitUint(structural[6], structural[7]) };

			
			this->S256 = this->S256.bitAndNot(R256);
			this->S256 = this->S256 | this->Q256;
			auto P256 = this->S256 | this->W256;
			P256 = P256 << 1;
			P256 &= ~this->W256 & ~R256;

			this->S256 = this->S256 | P256;
			return this->S256.bitAndNot(this->Q256.bitAndNot(R256));
		}

		inline SimdBase256 collectQuotes() {
			SimdBase256 backslashes = _mm256_set1_epi8('\\');
			SimdBase256 backslashesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				backslashesReal[x] = this->values[x] == backslashes;
			}

			auto B256 = SimdBase256{ convertSimd256To64BitUint(backslashesReal[0], backslashesReal[1]),
				convertSimd256To64BitUint(backslashesReal[2], backslashesReal[3]), convertSimd256To64BitUint(backslashesReal[4], backslashesReal[5]),
				convertSimd256To64BitUint(backslashesReal[6], backslashesReal[7]) };

			SimdBase256 quotes = _mm256_set1_epi8('"');
			SimdBase256 quotesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				quotesReal[x] = this->values[x] == quotes;
			}

			this->Q256 =
				SimdBase256{ convertSimd256To64BitUint(quotesReal[0], quotesReal[1]), convertSimd256To64BitUint(quotesReal[2], quotesReal[3]),
					convertSimd256To64BitUint(quotesReal[4], quotesReal[5]), convertSimd256To64BitUint(quotesReal[6], quotesReal[7]) };
			auto S = B256.bitAndNot(B256 << 1);
			SimdBase256 E{ _mm256_set1_epi8(0b01010101) };
			SimdBase256 O{ _mm256_set1_epi8(0b10101010) };
			auto ES = E & S;
			auto EC = ES.collectCarries(B256);
			auto ECE = EC.bitAndNot(B256);
			auto OD1 = ECE.bitAndNot(E);
			auto OS = S & O;
			auto OC = B256 + OS;
			auto OCE = OC.bitAndNot(B256);
			auto OD2 = OCE & E;
			auto OD = OD1 | OD2;
			return this->Q256 & ~OD;
		}

		inline SimdStringSection(std::string_view valueNew, bool prevInString) {
			this->stringView = &valueNew;

			this->packStringIntoValue(&this->values[0], this->stringView->data());
			this->packStringIntoValue(&this->values[1], this->stringView->data() + 32);
			this->packStringIntoValue(&this->values[2], this->stringView->data() + 64);
			this->packStringIntoValue(&this->values[3], this->stringView->data() + 96);
			this->packStringIntoValue(&this->values[4], this->stringView->data() + 128);
			this->packStringIntoValue(&this->values[5], this->stringView->data() + 160);
			this->packStringIntoValue(&this->values[6], this->stringView->data() + 192);
			this->packStringIntoValue(&this->values[7], this->stringView->data() + 224);

			this->Q256 = this->collectQuotes();
			if (prevInString) {
				this->Q256.flipFirstBit();
			}
			this->W256 = this->collectWhiteSpace();
			this->S256 = this->collectStructuralCharacters(prevInString);
			
			
		}

	  protected:
		SimdBase256 Q256{};
		SimdBase256 W256{};
		SimdBase256 R256{};
		SimdBase256 S256{};
		SimdBase256 values[8]{};
		std::string_view* stringView{};
	};

	enum class ErrorCode {
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

	enum class TapeType : char {
		Root = 'r',
		StartArray = '[',
		StartObject = '{',
		EndArray = ']',
		EndObject = '}',
		String = '"',
		Int64 = 'l',
		Uint64 = 'u',
		Double = 'd',
		TrueValue = 't',
		FalseValue = 'f',
		NullValue = 'n',
		DocumentEnd = 'e'
	};

	struct JsonTapeEvent {
		TapeType type{};
		int64_t index{};
		size_t size{};
	};

	class JsonConstructor {
	  public:
		inline JsonConstructor(std::vector<JsonTapeEvent>* theEvents, std::string* stringNew) {
			this->jsonEvents = theEvents;
			this->stringView = stringNew;
			this->currentIndex = this->jsonEvents->data();
		}

		inline Jsonifier parseJsonToJsonObject() {
			if (!this->currentIndex) {
				this->currentIndex = this->jsonEvents->data();
			}
			Jsonifier jsonDataNew{};
			switch (currentIndex->type) {
				case TapeType::Root: {
					this->updateEventLog();
					return this->parseJsonToJsonObject();
				}
				case TapeType::StartObject: {
					this->updateEventLog();
					while (currentIndex->type != TapeType::EndObject) {
						auto key = this->collectString();
						jsonDataNew[key] = this->parseJsonToJsonObject();
					}
					this->updateEventLog();
					return jsonDataNew;
				}
				case TapeType::StartArray: {
					this->updateEventLog();
					while (currentIndex->type != TapeType::EndArray) {
						jsonDataNew.emplaceBack(this->parseJsonToJsonObject());
					}
					this->updateEventLog();
					return jsonDataNew;
				}
				case TapeType::String: {
					return this->collectString();
				}
				case TapeType::Double: {
					return this->collectFloat();
				}
				case TapeType::Uint64: {
					return this->collectUint64();
				}
				case TapeType::Int64: {
					return this->collectInt64();
				}
				case TapeType::TrueValue: {
					return this->collectTrueOrFalse(true);
				}
				case TapeType::FalseValue: {
					return this->collectTrueOrFalse(false);
				}
				case TapeType::NullValue: {
					return this->collectNull();
				}
				case TapeType::EndObject: {
					this->updateEventLog();
					return jsonDataNew;
				}
				case TapeType::EndArray: {
					this->updateEventLog();
					return jsonDataNew;
				}
				case TapeType::DocumentEnd: {
					this->updateEventLog();
					return jsonDataNew;
				}
				default: {
					return jsonDataNew;
				}
			}
			return jsonDataNew;
		}

		inline std::string collectString() {
			if (this->jsonEvents->size() > 0) {
				JsonTapeEvent value = *this->currentIndex;
				this->updateEventLog();
				return std::string{ this->stringView->data() + value.index + 1, value.size - 2 };
			} else {
				return {};
			}
		}

		inline Jsonifier collectTrueOrFalse(bool returnValue) {
			if (this->jsonEvents->size() > 0) {
				JsonTapeEvent value = *this->currentIndex;
				this->updateEventLog();
				return returnValue;
			} else {
				return {};
			}
		}

		inline Jsonifier collectFloat() {
			if (this->jsonEvents->size() > 0) {
				JsonTapeEvent value = *this->currentIndex;
				this->updateEventLog();
				return double{ stod(std::string{ this->stringView->data() + value.index , value.size }) };
			} else {
				return {};
			}
		}

		inline Jsonifier collectUint64() {
			if (this->jsonEvents->size() > 0) {
				JsonTapeEvent value = *this->currentIndex;
				this->updateEventLog();
				return uint64_t{ stoull(std::string{ this->stringView->data() + value.index, value.size }) };
			} else {
				return {};
			}
		}

		inline Jsonifier collectInt64() {
			if (this->jsonEvents->size() > 0) {
				JsonTapeEvent value = *this->currentIndex;
				this->updateEventLog();
				return int64_t{ stoll(std::string{ this->stringView->data() + value.index, value.size }) };
			} else {
				return {};
			}
		}

		inline Jsonifier collectNull() {
			if (this->jsonEvents->size() > 0) {
				this->updateEventLog();
				return nullptr;
			} else {
				return {};
			}
		}

		inline void updateEventLog() {
			this->currentIndex++;
		}

		inline operator Jsonifier() {
			return this->parseJsonToJsonObject();
		}

	  protected:
		std::vector<JsonTapeEvent>* jsonEvents{};
		std::string* stringView{};
		JsonTapeEvent* currentIndex{};
	};

	struct openContainer {
		uint32_t tapeIndex{};// where, on the tape, does the scope ([,{) begins
		uint32_t count{};// how many elements in the scope
	};

	class SimdJsonValue {
	  public:
		std::unique_ptr<uint64_t[]> tape{};
		std::unique_ptr<openContainer[]> openContainers{};
		uint8_t* stringBuf{};
		std::unique_ptr<bool[]> isArray{};
		uint32_t nStructuralIndexes{ 0 };
		std::unique_ptr<uint32_t[]> structuralIndexes{};
		uint32_t nextStructuralIndex{ 0 };
		uint8_t* buf{};
		uint32_t maxDepth{};
		size_t len{ 0 };
		SimdJsonValue() noexcept = default;

		inline SimdJsonValue(std::string_view stringNew) {
			if (stringNew.size() == 0) {
				throw std::runtime_error{ "Failed to parse as the string size is 0." };
			}
			this->stringView = stringNew;
			this->stringBuf = reinterpret_cast<uint8_t*>(this->stringView.data());
		}

		inline void appendTapeValue(TapeType typeNew, size_t sizeNew) {
			this->jsonEvents.emplace_back(JsonTapeEvent{ .type = typeNew, .index = this->getCurrentIndex(), .size = sizeNew });
		}

		inline void generateJsonEvents() {
			this->jsonEvents.clear();
			this->jsonRawTape.clear();
			size_t stringSize = this->stringView.size();
			uint32_t collectedSize{};
			bool prevInString{ false };
			while (stringSize > 256) {
				SimdStringSection section{ std::string_view{ this->stringView.data() + collectedSize, 256 }, prevInString };
				for (size_t x = 0; x < section.getStructuralIndices().size();++x) {
					this->jsonRawTape.emplace_back(collectedSize + section.getStructuralIndices()[x]);
				}
				if (this->stringView[this->jsonRawTape.back()] == '"') {
					if (std::string_view{ this->stringView.data() + collectedSize, 256-collectedSize }.find_first_of('"',
							this->jsonRawTape.back() + 1) == std::string::npos) {
						prevInString = true;
					}
				}
				stringSize -= 256;
				collectedSize += 256;
			}
			if (this->stringView.size() - collectedSize > 0) {
				SimdStringSection section{ std::string_view{ this->stringView.data() + collectedSize, this->stringView.size() - collectedSize },
					prevInString };
				for (size_t x = 0; x < section.getStructuralIndices().size(); ++x) {
					this->jsonRawTape.emplace_back(collectedSize + section.getStructuralIndices()[x] );
					if (x == section.getStructuralIndices().size() - 1) {
						break;
					}
				}
			}
			this->nextStructural = this->jsonRawTape.data();
			this->generateJsonData();
		}

		inline ErrorCode recordTrueAtom(const char* value) {
			if (strcmp(value, "true")) {
				this->appendTapeValue(TapeType::TrueValue, 4);
				this->advance();
				return this->generateJsonData();
			}
			throw std::runtime_error{ "Failed to parse the True atom." };
		}

		inline ErrorCode recordObjectStart() {
			this->appendTapeValue(TapeType::StartObject, 0);
			this->advance();
			return this->recordKey(this->peek());
		}

		inline ErrorCode recordArrayStart() {
			this->appendTapeValue(TapeType::StartArray, 0);
			this->advance();
			return this->generateJsonData();
		}

		inline ErrorCode recordObjectEnd() {
			this->appendTapeValue(TapeType::EndObject, 0);
			this->advance();
			return this->generateJsonData();
		}

		inline ErrorCode recordArrayEnd() {
			this->appendTapeValue(TapeType::EndArray, 0);
			this->advance();
			return this->generateJsonData();
		}
		
		inline ErrorCode recordFalseAtom(const char* value) {
			if (strcmp(value, "false")) {
				this->appendTapeValue(TapeType::FalseValue, 5);
				this->advance();
				return this->generateJsonData();
			}
			throw std::runtime_error{ "Failed to parse the False atom." };
		}

		inline ErrorCode recordNullAtom(const char* value) {
			if (strcmp(value, "null")) {
				this->appendTapeValue(TapeType::NullValue, 4);
				this->advance();
				return this->generateJsonData();
			}
			throw std::runtime_error{ "Failed to parse the Null atom." };
		}

		inline ErrorCode recordNumber(const char* value) {
			this->appendTapeValue(TapeType::Uint64, this->getNextIndex() - this->getCurrentIndex());
			this->advance();
			return this->generateJsonData();
		}

		inline ErrorCode recordKey(const char* value) {
			if (this->atEof()) {
				this->appendTapeValue(TapeType::String, this->stringView.size() - this->getCurrentIndex());
				this->advance();
			} else {
				this->appendTapeValue(TapeType::String, this->getNextIndex() - this->getCurrentIndex());
				this->advance();
			}
			return this->generateJsonData();
		}

		inline ErrorCode recordDocumentEnd() {
			this->appendTapeValue(TapeType::DocumentEnd, this->stringView.size() - this->getCurrentIndex());
			this->advance();
			return ErrorCode::Success;
		}

		inline ErrorCode recordRoot() {
			this->appendTapeValue(TapeType::Root, 0);
			return this->recordObjectStart();
		}

		inline ErrorCode recordString(const char* value) {
			if (this->atEof()) {
				this->appendTapeValue(TapeType::String, this->stringView.size() - this->getCurrentIndex());
				this->advance();
			} else {
				this->appendTapeValue(TapeType::String, this->getNextIndex() - this->getCurrentIndex());
				this->advance();
			}
			return this->generateJsonData();
		}

		uint32_t depth{ 0 };
		uint32_t* nextStructural{ nullptr };

		inline uint32_t getNextIndex() {
			return *(this->nextStructural + 1);
		}

		inline uint32_t getCurrentIndex() {
			return *(this->nextStructural);
		}

		inline uint32_t getPreviousIndex() {
			return *(this->nextStructural - 1);
		}

		inline bool atEof() {
			if (this->nextStructural - this->jsonRawTape.data() >= this->jsonRawTape.size()-1) {
				return true;
			} else {
				return false;
			}
		}

		inline bool atStart() {
			if (this->nextStructural == this->jsonRawTape.data()) {
				return true;
			} else {
				return false;
			}
		}

		inline const char* peekNext() noexcept {
			return &this->stringView[*(this->nextStructural + 1)];
		}

		inline const char* peek() noexcept {
			return &this->stringView[*this->nextStructural];
		}

		inline void advance() noexcept {
			this->nextStructural++;
		}

		inline ErrorCode generateJsonData() {
			if (this->atStart()) {
				return this->recordRoot();
			}
			if (this->atEof()) {
				return ErrorCode::Success;
			}
			switch (*this->peek()) {
				case '{': {
					return this->recordObjectStart();
				}
				case '[': {
					return this->recordArrayStart();
				}
				case '}': {
					return this->recordObjectEnd();
				}
				case ']': {
					return this->recordArrayEnd();
				}
				case '"': {
					return this->recordString(this->peek());
				}
				case ':': {
					this->advance();
					return this->generateJsonData();
				}
				case ',': {
					this->advance();
					return this->generateJsonData();
				}

				case 't': {
					return this->recordTrueAtom(this->peek());
				}

				case 'f': {
					return this->recordFalseAtom(this->peek());
				}

				case 'n': {
					return this->recordNullAtom(this->peek());
				}

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
				case '9': {
					return this->recordNumber(this->peek());
				}
				default: {
					return this->recordDocumentEnd();
				}
								
			}		

		}

		inline Jsonifier getJsonData() {
			this->generateJsonEvents();
			return this->jsonConstructor.operator Jsonifier();
		}

	  protected:
		std::vector<uint32_t> jsonRawTape{};
		std::vector<JsonTapeEvent> jsonEvents{};
		JsonConstructor jsonConstructor{ &this->jsonEvents, &this->stringView };
		std::string stringView{};
	};

	const bool structural_or_whitespace_negated[256] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,

		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1,

		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	const bool structural_or_whitespace[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	const uint32_t digit_to_val32[886] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
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


	inline uint32_t is_not_structural_or_whitespace(uint8_t c) {
		return structural_or_whitespace_negated[c];
	}

	namespace atomparsing {

		// The string_to_uint32 is exclusively used to map literal strings to 32-bit values.
		// We use memcpy instead of a pointer cast to avoid undefined behaviors since we cannot
		// be certain that the character pointer will be properly aligned.
		// You might think that using memcpy makes this function expensive, but you'd be wrong.
		// All decent optimizing compilers (GCC, clang, Visual Studio) will compile string_to_uint32("false");
		// to the compile-time constant 1936482662.
		inline uint32_t string_to_uint32(const char* str) {
			uint32_t val;
			std::memcpy(&val, str, sizeof(uint32_t));
			return val;
		}


		// Again in str4ncmp we use a memcpy to avoid undefined behavior. The memcpy may appear expensive.
		// Yet all decent optimizing compilers will compile memcpy to a single instruction, just about.
		inline uint32_t str4ncmp(const uint8_t* src, const char* atom) {
			uint32_t srcval;// we want to avoid unaligned 32-bit loads (undefined in C/C++)
			static_assert(sizeof(uint32_t) <= 256, "256 must be larger than 4 bytes");
			std::memcpy(&srcval, src, sizeof(uint32_t));
			return srcval ^ string_to_uint32(atom);
		}

		inline bool is_valid_true_atom(const uint8_t* src) {
			return (str4ncmp(src, "true") | is_not_structural_or_whitespace(src[4])) == 0;
		}

		inline bool is_valid_true_atom(const uint8_t* src, size_t len) {
			if (len > 4) {
				return is_valid_true_atom(src);
			} else if (len == 4) {
				return !str4ncmp(src, "true");
			} else {
				return false;
			}
		}

		inline bool is_valid_false_atom(const uint8_t* src) {
			return (str4ncmp(src + 1, "alse") | is_not_structural_or_whitespace(src[5])) == 0;
		}

		inline bool is_valid_false_atom(const uint8_t* src, size_t len) {
			if (len > 5) {
				return is_valid_false_atom(src);
			} else if (len == 5) {
				return !str4ncmp(src + 1, "alse");
			} else {
				return false;
			}
		}

		inline bool is_valid_null_atom(const uint8_t* src) {
			return (str4ncmp(src, "null") | is_not_structural_or_whitespace(src[4])) == 0;
		}

		inline bool is_valid_null_atom(const uint8_t* src, size_t len) {
			if (len > 4) {
				return is_valid_null_atom(src);
			} else if (len == 4) {
				return !str4ncmp(src, "null");
			} else {
				return false;
			}
		}
	}
	class json_iterator {
	  public:
		uint8_t* buf;
		uint32_t* next_structural;
		SimdJsonValue& dom_parser;
		uint32_t depth{ 0 };

		/**
   * Walk the JSON document.
   *
   * The visitor receives callbacks when values are encountered. All callbacks pass the iterator as
   * the first parameter; some callbacks have other parameters as well:
   *
   * - visit_document_start() - at the beginning.
   * - visit_document_end() - at the end (if things were successful).
   *
   * - visit_array_start() - at the start `[` of a non-empty array.
   * - visit_array_end() - at the end `]` of a non-empty array.
   * - visit_empty_array() - when an empty array is encountered.
   *
   * - visit_object_end() - at the start `]` of a non-empty object.
   * - visit_object_start() - at the end `]` of a non-empty object.
   * - visit_empty_object() - when an empty object is encountered.
   * - visit_key(const uint8_t *key) - when a key in an object field is encountered. key is
   *                                   guaranteed to point at the first quote of the string (`"key"`).
   * - visit_primitive(const uint8_t *value) - when a value is a string, number, boolean or null.
   * - visit_root_primitive(iter, uint8_t *value) - when the top-level value is a string, number, boolean or null.
   *
   * - increment_count(iter) - each time a value is found in an array or object.
   */
		template<bool STREAMING, typename V> inline ErrorCode walk_document(V& visitor) noexcept;

		/**
   * Create an iterator capable of walking a JSON document.
   *
   * The document must have already passed through stage 1.
   */
		inline json_iterator(SimdJsonValue& _dom_parser, size_t start_structural_index);

		/**
   * Look at the next token.
   *
   * Tokens can be strings, numbers, booleans, null, or operators (`[{]},:`)).
   *
   * They may include invalid JSON as well (such as `1.2.3` or `ture`).
   */
		inline uint8_t* peek()  noexcept;
		/**
   * Advance to the next token.
   *
   * Tokens can be strings, numbers, booleans, null, or operators (`[{]},:`)).
   *
   * They may include invalid JSON as well (such as `1.2.3` or `ture`).
   */
		inline uint8_t* advance() noexcept;
		/**
   * Get the remaining length of the document, from the start of the current token.
   */
		inline size_t remaining_len()  noexcept;
		/**
   * Check if we are at the end of the document.
   *
   * If this is true, there are no more tokens.
   */
		inline bool at_eof()  noexcept;
		/**
   * Check if we are at the beginning of the document.
   */
		inline bool at_beginning()  noexcept;
		inline uint8_t last_structural()  noexcept;

		template<typename V> inline ErrorCode visit_root_primitive(V& visitor, uint8_t* value) noexcept;
		template<typename V> inline ErrorCode visit_primitive(V& visitor, uint8_t* value) noexcept;
	};

	template<bool STREAMING, typename V> inline ErrorCode json_iterator::walk_document(V& visitor) noexcept {

		//
		// Start the document
		//
		if (at_eof()) {
			return ErrorCode::Empty;
		}
		(visitor.visit_document_start(*this));

		//
		// Read first value
		//
		{
			auto value = advance();

			// Make sure the outer object or array is closed before continuing; otherwise, there are ways we
			// could get into memory corruption. See https://github.com/simdjson/simdjson/issues/906
			if (!STREAMING) {
				switch (*value) {
					case '{':
						if (last_structural() != '}') {
							return ErrorCode::TapeError;
						};
						break;
					case '[':
						if (last_structural() != ']') {
							return ErrorCode::TapeError;
						};
						break;
				}
			}

			switch (*value) {
				case '{':
					if (*peek() == '}') {
						advance();
						(visitor.visit_empty_object(*this));
						break;
					}
					goto object_begin;
				case '[':
					if (*peek() == ']') {
						advance();
						(visitor.visit_empty_array(*this));
						break;
					}
					goto array_begin;
				default:
					(visitor.visit_root_primitive(*this, value));
					break;
			}
		}
		goto document_end;

	//
	// Object parser states
	//
	object_begin:
		depth++;
		if (depth >= dom_parser.maxDepth) {
			return ErrorCode::DepthError;
		}
		dom_parser.isArray[depth] = false;
		(visitor.visit_object_start(*this));

		{
			auto key = advance();
			if (*key != '"') {
				return ErrorCode::TapeError;
			}
			(visitor.increment_count(*this));
			(visitor.visit_key(*this, key));
		}

	object_field:
		if ((*advance() != ':')) {
			return ErrorCode::TapeError;
		}
		{
			auto value = advance();
			switch (*value) {
				case '{':
					if (*peek() == '}') {
						advance();
						(visitor.visit_empty_object(*this));
						break;
					}
					goto object_begin;
				case '[':
					if (*peek() == ']') {
						advance();
						(visitor.visit_empty_array(*this));
						break;
					}
					goto array_begin;
				default:
					(visitor.visit_primitive(*this, value));
					break;
			}
		}

	object_continue:
		switch (*advance()) {
			case ',':
				(visitor.increment_count(*this));
				{
					auto key = advance();
					if ((*key != '"')) {
						return ErrorCode::TapeError;
					}
					(visitor.visit_key(*this, key));
				}
				goto object_field;
			case '}':
				(visitor.visit_object_end(*this));
				goto scope_end;
			default:
				return ErrorCode::TapeError;
		}

	scope_end:
		depth--;
		if (depth == 0) {
			goto document_end;
		}
		if (dom_parser.isArray[depth]) {
			goto array_continue;
		}
		goto object_continue;

	//
	// Array parser states
	//
	array_begin:
		depth++;
		if (depth >= dom_parser.maxDepth) {
			return ErrorCode::DepthError;
		}
		dom_parser.isArray[depth] = true;
		(visitor.visit_array_start(*this));
		(visitor.increment_count(*this));

	array_value : {
		auto value = advance();
		switch (*value) {
			case '{':
				if (*peek() == '}') {
					advance();
					(visitor.visit_empty_object(*this));
					break;
				}
				goto object_begin;
			case '[':
				if (*peek() == ']') {
					advance();
					(visitor.visit_empty_array(*this));
					break;
				}
				goto array_begin;
			default:
				(visitor.visit_primitive(*this, value));
				break;
		}
	}

	array_continue:
		switch (*advance()) {
			case ',':
				(visitor.increment_count(*this));
				goto array_value;
			case ']':
				(visitor.visit_array_end(*this));
				goto scope_end;
			default:
				return ErrorCode::TapeError;
		}

	document_end:
		(visitor.visit_document_end(*this));

		dom_parser.nextStructuralIndex = uint32_t(next_structural - &dom_parser.structuralIndexes[0]);

		// If we didn't make it to the end, it's an error
		if (!STREAMING && dom_parser.nextStructuralIndex != dom_parser.nStructuralIndexes) {
			return ErrorCode::TapeError;
		}

		return ErrorCode::Success;

	}// walk_document()

	inline json_iterator::json_iterator(SimdJsonValue& _dom_parser, size_t start_structural_index)
		: buf{ _dom_parser.buf }, next_structural{ &_dom_parser.structuralIndexes[start_structural_index] }, dom_parser{ _dom_parser } {
	}

	inline uint8_t* json_iterator::peek()  noexcept {
		return &buf[*(next_structural)];
	}
	inline uint8_t* json_iterator::advance() noexcept {
		return &buf[*(next_structural++)];
	}
	inline size_t json_iterator::remaining_len()  noexcept {
		return dom_parser.len - *(next_structural - 1);
	}

	inline bool json_iterator::at_eof()  noexcept {
		return next_structural == &dom_parser.structuralIndexes[dom_parser.nStructuralIndexes];
	}
	inline bool json_iterator::at_beginning()  noexcept {
		return next_structural == dom_parser.structuralIndexes.get();
	}
	inline uint8_t json_iterator::last_structural()  noexcept {
		return buf[dom_parser.structuralIndexes[dom_parser.nStructuralIndexes - 1]];
	}

	template<typename V>
	inline ErrorCode json_iterator::visit_root_primitive(V& visitor, uint8_t* value) noexcept {
		switch (*value) {
			case '"':
				return visitor.visit_root_string(*this, value);
			case 't':
				return visitor.visit_root_true_atom(*this, value);
			case 'f':
				return visitor.visit_root_false_atom(*this, value);
			case 'n':
				return visitor.visit_root_null_atom(*this, value);
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
				return visitor.visit_root_number(*this, value);
			default:
				return ErrorCode::TapeError;
		}
	}
	template<typename V> inline ErrorCode json_iterator::visit_primitive(V& visitor, uint8_t* value) noexcept {
		switch (*value) {
			case '"':
				return visitor.visit_string(*this, value);
			case 't':
				return visitor.visit_true_atom(*this, value);
			case 'f':
				return visitor.visit_false_atom(*this, value);
			case 'n':
				return visitor.visit_null_atom(*this, value);
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
				return visitor.visit_number(*this, value);
			default:
				return ErrorCode::TapeError;
		}
	}

	struct tape_writer {
		/** The next place to write to tape */
		uint64_t* next_tape_loc;

		/** Write a signed 64-bit value to tape. */
		inline void append_s64(int64_t value) noexcept;

		/** Write an unsigned 64-bit value to tape. */
		inline void append_u64(uint64_t value) noexcept;

		/** Write a double value to tape. */
		inline void append_double(double value) noexcept;
		uint64_t* begin() {
			return this->next_tape_loc;
		}
		uint64_t* end() {
			return this->next_tape_loc;
		}
		/**
   * Append a tape entry (an 8-bit type,and 56 bits worth of value).
   */
		inline void append(uint64_t val, TapeType t) noexcept;

		/**
   * Skip the current tape entry without writing.
   *
   * Used to skip the start of the container, since we'll come back later to fill it in when the
   * container ends.
   */
		inline void skip() noexcept;

		/**
   * Skip the number of tape entries necessary to write a large u64 or i64.
   */
		inline void skip_large_integer() noexcept;

		/**
   * Skip the number of tape entries necessary to write a double.
   */
		inline void skip_double() noexcept;

		/**
   * Write a value to a known location on tape.
   *
   * Used to go back and write out the start of a container after the container ends.
   */
		inline static void write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept;

	  private:
		/**
   * Append both the tape entry, and a supplementary value following it. Used for types that need
   * all 64 bits, such as double and uint64_t.
   */
		template<typename T> inline void append2(uint64_t val, T val2, TapeType t) noexcept;
	};// struct number_writer

	inline void tape_writer::append_s64(int64_t value) noexcept {
		append2(0, value, TapeType ::Int64);
	}

	inline void tape_writer::append_u64(uint64_t value) noexcept {
		append(0, TapeType ::Uint64);
		*next_tape_loc = value;
		next_tape_loc++;
	}

	/** Write a double value to tape. */
	inline void tape_writer::append_double(double value) noexcept {
		append2(0, value, TapeType ::Double);
	}

	inline void tape_writer::skip() noexcept {
		next_tape_loc++;
	}

	inline void tape_writer::skip_large_integer() noexcept {
		next_tape_loc += 2;
	}

	inline void tape_writer::skip_double() noexcept {
		next_tape_loc += 2;
	}

	inline void tape_writer::append(uint64_t val, TapeType t) noexcept {
		*next_tape_loc = val | ((uint64_t(char(t))) << 56);
		next_tape_loc++;
	}

	template<typename T> inline void tape_writer::append2(uint64_t val, T val2, TapeType t) noexcept {
		append(val, t);
		static_assert(sizeof(val2) == sizeof(*next_tape_loc), "Type is not 64 bits!");
		memcpy(next_tape_loc, &val2, sizeof(val2));
		next_tape_loc++;
	}

	inline void tape_writer::write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept {
		tape_loc = val | ((uint64_t(char(t))) << 56);
	}

	struct tape_builder {
		template<bool STREAMING> static inline ErrorCode parse_document(SimdJsonValue& dom_parser) noexcept;

		/** Called when a non-empty document starts. */
		inline ErrorCode visit_document_start(json_iterator& iter) noexcept;
		/** Called when a non-empty document ends without error. */
		inline ErrorCode visit_document_end(json_iterator& iter) noexcept;

		/** Called when a non-empty array starts. */
		inline ErrorCode visit_array_start(json_iterator& iter) noexcept;
		/** Called when a non-empty array ends. */
		inline ErrorCode visit_array_end(json_iterator& iter) noexcept;
		/** Called when an empty array is found. */
		inline ErrorCode visit_empty_array(json_iterator& iter) noexcept;

		/** Called when a non-empty object starts. */
		inline ErrorCode visit_object_start(json_iterator& iter) noexcept;
		/**
   * Called when a key in a field is encountered.
   *
   * primitive, visit_object_start, visit_empty_object, visit_array_start, or visit_empty_array
   * will be called after this with the field value.
   */
		inline ErrorCode visit_key(json_iterator& iter, uint8_t* key) noexcept;
		/** Called when a non-empty object ends. */
		inline ErrorCode visit_object_end(json_iterator& iter) noexcept;
		/** Called when an empty object is found. */
		inline ErrorCode visit_empty_object(json_iterator& iter) noexcept;

		/**
   * Called when a string, number, boolean or null is found.
   */
		inline ErrorCode visit_primitive(json_iterator& iter, uint8_t* value) noexcept;
		/**
   * Called when a string, number, boolean or null is found at the top level of a document (i.e.
   * when there is no array or object and the entire document is a single string, number, boolean or
   * null.
   *
   * This is separate from primitive() because simdjson's normal primitive parsing routines assume
   * there is at least one more token after the value, which is only true in an array or object.
   */
		inline ErrorCode visit_root_primitive(json_iterator& iter, uint8_t* value) noexcept;

		inline ErrorCode visit_string(json_iterator& iter, uint8_t* value, bool key = false) noexcept;
		inline ErrorCode visit_number(json_iterator& iter, uint8_t* value) noexcept;
		inline ErrorCode visit_true_atom(json_iterator& iter, uint8_t* value) noexcept;
		inline ErrorCode visit_false_atom(json_iterator& iter, uint8_t* value) noexcept;
		inline ErrorCode visit_null_atom(json_iterator& iter, uint8_t* value) noexcept;

		inline ErrorCode visit_root_string(json_iterator& iter, uint8_t* value) noexcept;
		inline ErrorCode visit_root_number(json_iterator& iter, uint8_t* value) noexcept;
		inline ErrorCode visit_root_true_atom(json_iterator& iter, uint8_t* value) noexcept;
		inline ErrorCode visit_root_false_atom(json_iterator& iter, uint8_t* value) noexcept;
		inline ErrorCode visit_root_null_atom(json_iterator& iter, uint8_t* value) noexcept;

		/** Called each time a new field or element in an array or object is found. */
		inline ErrorCode increment_count(json_iterator& iter) noexcept;

		/** Next location to write to tape */
		tape_writer tape;

		inline tape_builder(SimdJsonValue& doc) noexcept;

	  private:
		/** Next write location in the string buf for stage 2 parsing */
		uint8_t* currentStringBufLoc;

		

		inline uint32_t next_tapeIndex(json_iterator& iter)  noexcept;
		inline void start_container(json_iterator& iter) noexcept;
		inline ErrorCode end_container(json_iterator& iter, TapeType start,
			TapeType end) noexcept;
		inline ErrorCode empty_container(json_iterator& iter, TapeType start,
			TapeType end) noexcept;
		inline uint8_t* on_start_string(json_iterator& iter) noexcept;
		inline void on_end_string(uint8_t* dst) noexcept;
	};// class tape_builder

	template<bool STREAMING> inline ErrorCode tape_builder::parse_document(SimdJsonValue& dom_parser) noexcept {
		json_iterator iter(dom_parser, STREAMING ? dom_parser.nextStructuralIndex : 0);
		tape_builder builder(dom_parser);
		return iter.walk_document<STREAMING>(builder);
	}

	inline ErrorCode tape_builder::visit_root_primitive(json_iterator& iter, uint8_t* value) noexcept {
		return iter.visit_root_primitive(*this, value);
	}
	inline ErrorCode tape_builder::visit_primitive(json_iterator& iter, uint8_t* value) noexcept {
		return iter.visit_primitive(*this, value);
	}
	inline ErrorCode tape_builder::visit_empty_object(json_iterator& iter) noexcept {
		return empty_container(iter, TapeType::StartObject, TapeType::EndObject);
	}
	inline ErrorCode tape_builder::visit_empty_array(json_iterator& iter) noexcept {
		return empty_container(iter, TapeType::StartArray, TapeType::EndArray);
	}

	inline ErrorCode tape_builder::visit_document_start(json_iterator& iter) noexcept {
		start_container(iter);
		return ErrorCode::Success;
	}
	inline ErrorCode tape_builder::visit_object_start(json_iterator& iter) noexcept {
		start_container(iter);
		return ErrorCode::Success;
	}
	inline ErrorCode tape_builder::visit_array_start(json_iterator& iter) noexcept {
		start_container(iter);
		return ErrorCode::Success;
	}

	inline ErrorCode tape_builder::visit_object_end(json_iterator& iter) noexcept {
		return end_container(iter, TapeType::StartObject, TapeType::EndObject);
	}
	inline ErrorCode tape_builder::visit_array_end(json_iterator& iter) noexcept {
		return end_container(iter, TapeType::StartArray, TapeType::EndArray);
	}
	inline ErrorCode tape_builder::visit_document_end(json_iterator& iter) noexcept {
		constexpr uint32_t start_tapeIndex = 0;
		tape.append(start_tapeIndex, TapeType::Root);
		tape_writer::write(iter.dom_parser.tape[start_tapeIndex], next_tapeIndex(iter), TapeType::Root);
		return ErrorCode::Success;
	}
	inline ErrorCode tape_builder::visit_key(json_iterator& iter, uint8_t* key) noexcept {
		return visit_string(iter, key, true);
	}

	inline ErrorCode tape_builder::increment_count(json_iterator& iter) noexcept {
		iter.dom_parser.openContainers[iter.depth].count++;// we have a key value pair in the object at parser.dom_parser.depth - 1
		return ErrorCode::Success;
	}

	inline tape_builder::tape_builder(SimdJsonValue& doc) noexcept : tape{ doc.tape.get() }, currentStringBufLoc{ doc.stringBuf } {};

	inline ErrorCode tape_builder::visit_string(json_iterator& iter, uint8_t* value, bool key) noexcept {
		uint8_t* dst = on_start_string(iter);
		if (dst == nullptr) {
			return ErrorCode::StringError;
		}
		on_end_string(dst);
		return ErrorCode::Success;
	}

	inline ErrorCode tape_builder::visit_root_string(json_iterator& iter, uint8_t* value) noexcept {
		return visit_string(iter, value);
	}

	inline ErrorCode tape_builder::visit_number(json_iterator& iter,  uint8_t* value) noexcept {
		std::copy(tape.next_tape_loc, tape.next_tape_loc, value);
		return ErrorCode::Success;
	}

	inline ErrorCode tape_builder::visit_root_number(json_iterator& iter, uint8_t* value) noexcept {
		//
		// We need to make a copy to make sure that the string is space terminated.
		// This is not about padding the input, which should already padded up
		// to len + 256. However, we have no control at this stage
		// on how the padding was done. What if the input string was padded with nulls?
		// It is quite common for an input string to have an extra null character (C string).
		// We do not want to allow 9\0 (where \0 is the null character) inside a JSON
		// document, but the string "9\0" by itself is fine. So we make a copy and
		// pad the input with spaces when we know that there is just one input element.
		// This copy is relatively expensive, but it will almost never be called in
		// practice unless you are in the strange scenario where you have many JSON
		// documents made of single atoms.
		//
		std::unique_ptr<uint8_t[]> copy(new (std::nothrow) uint8_t[iter.remaining_len() + 256]);
		if (copy.get() == nullptr) {
			return ErrorCode::MemAlloc;
		}
		std::memcpy(copy.get(), value, iter.remaining_len());
		std::memset(copy.get() + iter.remaining_len(), ' ', 256);
		ErrorCode error = visit_number(iter, copy.get());
		return error;
	}

	inline ErrorCode tape_builder::visit_true_atom(json_iterator& iter, uint8_t* value) noexcept {
		if (!atomparsing::is_valid_true_atom(value)) {
			return ErrorCode::TAtomError;
		}
		tape.append(0, TapeType::TrueValue);
		return ErrorCode::Success;
	}

	inline ErrorCode tape_builder::visit_root_true_atom(json_iterator& iter, uint8_t* value) noexcept {
		if (!atomparsing::is_valid_true_atom(value, iter.remaining_len())) {
			return ErrorCode::TAtomError;
		}
		tape.append(0, TapeType::TrueValue);
		return ErrorCode::Success;
	}

	inline ErrorCode tape_builder::visit_false_atom(json_iterator& iter, uint8_t* value) noexcept {
		if (!atomparsing::is_valid_false_atom(value)) {
			return ErrorCode::FAtomError;
		}
		tape.append(0, TapeType::FalseValue);
		return ErrorCode::Success;
	}

	inline ErrorCode tape_builder::visit_root_false_atom(json_iterator& iter, uint8_t* value) noexcept {
		if (!atomparsing::is_valid_false_atom(value, iter.remaining_len())) {
			return ErrorCode::FAtomError;
		}
		tape.append(0, TapeType::FalseValue);
		return ErrorCode::Success;
	}

	inline ErrorCode tape_builder::visit_null_atom(json_iterator& iter, uint8_t* value) noexcept {
		if (!atomparsing::is_valid_null_atom(value)) {
			return ErrorCode::NAtomError;
		}
		tape.append(0, TapeType::NullValue);
		return ErrorCode::Success;
	}

	inline ErrorCode tape_builder::visit_root_null_atom(json_iterator& iter, uint8_t* value) noexcept {
		if (!atomparsing::is_valid_null_atom(value, iter.remaining_len())) {
			return ErrorCode::NAtomError;
		}
		tape.append(0, TapeType::NullValue);
		return ErrorCode::Success;
	}

	// private:

	inline uint32_t tape_builder::next_tapeIndex(json_iterator& iter)  noexcept {
		return uint32_t(tape.next_tape_loc - iter.dom_parser.tape.get());
	}

	inline ErrorCode tape_builder::empty_container(json_iterator& iter, TapeType start,
		TapeType end) noexcept {
		auto start_index = next_tapeIndex(iter);
		tape.append(start_index + 2, start);
		tape.append(start_index, end);
		return ErrorCode::Success;
	}

	inline void tape_builder::start_container(json_iterator& iter) noexcept {
		iter.dom_parser.openContainers[iter.depth].tapeIndex = next_tapeIndex(iter);
		iter.dom_parser.openContainers[iter.depth].count = 0;
		tape.skip();// We don't actually *write* the start element until the end.
	}

	inline ErrorCode tape_builder::end_container(json_iterator& iter, TapeType start,
		TapeType end) noexcept {
		// Write the ending tape element, pointing at the start location
		const uint32_t start_tapeIndex = iter.dom_parser.openContainers[iter.depth].tapeIndex;
		tape.append(start_tapeIndex, end);
		// Write the start tape element, pointing at the end location (and including count)
		// count can overflow if it exceeds 24 bits... so we saturate
		// the convention being that a cnt of 0xffffff or more is undetermined in value (>=  0xffffff).
		const uint32_t count = iter.dom_parser.openContainers[iter.depth].count;
		const uint32_t cntsat = count > 0xFFFFFF ? 0xFFFFFF : count;
		tape_writer::write(iter.dom_parser.tape[start_tapeIndex], next_tapeIndex(iter) | (uint64_t(cntsat) << 32), start);
		return ErrorCode::Success;
	}

	inline uint8_t* tape_builder::on_start_string(json_iterator& iter) noexcept {
		// we advance the point, accounting for the fact that we have a NULL termination
		tape.append(currentStringBufLoc - iter.dom_parser.stringBuf, TapeType::String);
		return currentStringBufLoc + sizeof(uint32_t);
	}

	inline void tape_builder::on_end_string(uint8_t* dst) noexcept {
		uint32_t str_length = uint32_t(dst - (currentStringBufLoc + sizeof(uint32_t)));
		// TODO check for overflow in case someone has a crazy string (>=4GB?)
		// But only add the overflow check when the document itself exceeds 4GB
		// Currently unneeded because we refuse to parse docs larger or equal to 4GB.
		memcpy(currentStringBufLoc, &str_length, sizeof(uint32_t));
		// NULL termination is still handy if you expect all your strings to
		// be NULL terminated? It comes at a small cost
		*dst = 0;
		currentStringBufLoc = dst + 1;
	}

};

