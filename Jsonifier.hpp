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

		inline SimdBase256 collectStructuralCharacters() {
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

		inline SimdStringSection(std::string_view valueNew) {
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
			this->Q256.printBits("THE Q VALUES: ");
			this->W256 = this->collectWhiteSpace();
			this->W256.printBits("THE W VALUES: ");
			this->S256 = this->collectStructuralCharacters();
			this->S256.printBits("THE S VALUES: ");
		}

	  protected:
		SimdBase256 Q256{};
		SimdBase256 W256{};
		SimdBase256 R256{};
		SimdBase256 S256{};
		SimdBase256 values[8]{};
		std::string_view* stringView{};
	};

	enum class ErrorCode { Empty = 0, TapeError = 1, DepthError = 2, Success = 3, ParseError = 4 };

	enum class JsonTapeEventStates {
		DocumentStart = 0,
		ObjectBegin = 1,
		ObjectField = 2,
		ObjectContinue = 3,
		ScopeEnd = 4,
		ArrayBegin = 5,
		ArrayValue = 6,
		ArrayContinue = 7,
		DocumentEnd = 8
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
		inline JsonConstructor(std::deque<JsonTapeEvent>* theEvents, std::string_view* stringNew) {
			this->jsonEvents = theEvents;
			this->stringView = stringNew;
		}

		inline Jsonifier parseJsonToJsonObject() {
			Jsonifier jsonDataNew{};
			if (this->jsonEvents->size() <= 0) {
				return jsonDataNew;
			}
			std::cout << "THE TYPE: " << ( char )this->jsonEvents->front().type << std::endl;
			switch (this->jsonEvents->front().type) {
				case TapeType::StartObject: {
					this->updateEventLog();
					while (this->jsonEvents->size() > 0 && this->jsonEvents->front().type != TapeType::EndObject) {
						auto key = this->collectString();
						jsonDataNew[key] = this->parseJsonToJsonObject();
					}
					this->updateEventLog();
					return jsonDataNew;
				}
				case TapeType::StartArray: {
					this->updateEventLog();
					while (this->jsonEvents->size() > 0) {
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
				JsonTapeEvent value = this->jsonEvents->front();
				this->updateEventLog();
				std::cout << "THE CURRENT INDEX REALER: " << this->jsonEvents->size() << std::endl;
				std::cout << "THE CURRENT SIZE REALER: " << value.size - 2 << std::endl;
				std::cout << "THE CURRENT INDEX REALER: " << value.index << std::endl;
				std::cout << "THE STRING: " << std::string{ this->stringView->data() + value.index + 1, value.size - 2 } << std::endl;
				return std::string{ this->stringView->data() + value.index + 1, value.size - 2 };
			} else {
				return {};
			}
		}

		inline Jsonifier collectTrueOrFalse(bool returnValue) {
			if (this->jsonEvents->size() > 0) {
				JsonTapeEvent value = this->jsonEvents->front();
				this->updateEventLog();
				return returnValue;
			} else {
				return {};
			}
		}

		inline Jsonifier collectFloat() {
			if (this->jsonEvents->size() > 0) {
				JsonTapeEvent value = this->jsonEvents->front();
				this->updateEventLog();
				return double{ stod(std::string{ this->stringView->data() + value.index , value.size }) };
			} else {
				return {};
			}
		}

		inline Jsonifier collectUint64() {
			if (this->jsonEvents->size() > 0) {
				JsonTapeEvent value = this->jsonEvents->front();
				this->updateEventLog();
				return uint64_t{ stoull(std::string{ this->stringView->data() + value.index, value.size }) };
			} else {
				return {};
			}
		}

		inline Jsonifier collectInt64() {
			if (this->jsonEvents->size() > 0) {
				JsonTapeEvent value = this->jsonEvents->front();
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

		void updateEventLog() {
			if (this->jsonEvents->size() > 0) {
				std::cout << "THE POPPING EVENT: " << ( char )this->jsonEvents->front().type << std::endl;
				this->jsonEvents->pop_front();
			}
		}

		inline operator Jsonifier() {
			return this->parseJsonToJsonObject();
		}

	  protected:
		std::deque<JsonTapeEvent>* jsonEvents{};
		std::string_view* stringView{};
	};

	class SimdJsonValue {
	  public:
		inline SimdJsonValue(std::string_view stringNew) {
			if (stringNew.size() == 0) {
				throw std::runtime_error{ "Failed to parse as the string size is 0." };
			}
			this->stringView = stringNew;
		}

		inline void appendTapeValue(TapeType typeNew, size_t sizeNew) {
			if (this->jsonRawTape.size() > 0) {
				std::cout << "THE EVENT TYPE: " << ( int32_t )typeNew << ", THE INDEX: " << this->getCurrentIndex() << std::endl;
				this->jsonEvents.emplace_back(JsonTapeEvent{ .type = typeNew, .index = this->getCurrentIndex(), .size = sizeNew });
			} else {
				this->jsonEvents.emplace_back(
					JsonTapeEvent{ .type = TapeType::DocumentEnd, .index = static_cast<int64_t>(this->stringView.size() - 1), .size = 0 });
			}
		}

		inline void generateJsonEvents() {
			this->jsonEvents.clear();
			this->jsonRawTape.clear();
			size_t stringSize = this->stringView.size();
			uint32_t collectedSize{};
			while (stringSize > 256) {
				SimdStringSection section(std::string_view{ this->stringView.data() + collectedSize, 256 });
				bool haveWeCollectedZero{ false };
				for (size_t x = 0; x < section.getStructuralIndices().size();++x) {
					this->jsonRawTape.emplace_back(collectedSize + section.getStructuralIndices()[x]);
				}
				stringSize -= 256;
				collectedSize += 256;
			}
			if (this->stringView.size() - collectedSize > 0) {
				SimdStringSection section((std::string_view{ this->stringView.data() + collectedSize, this->stringView.size() - collectedSize }));
				bool haveWeCollectedZero{ false };
				for (size_t x = 0; x < section.getStructuralIndices().size(); ++x) {
					this->jsonRawTape.emplace_back(collectedSize + section.getStructuralIndices()[x]);
					std::cout << "TAPE INDEX VALUE: " << this->jsonRawTape.back() << ", THE VALUE: " << this->stringView[this->jsonRawTape.back()]
							  << std::endl;
				}
			}
			this->generateJsonData();
		}

		inline void recordTrueAtom(const char* value) {
			if (strcmp(value, "true")) {
				this->appendTapeValue(TapeType::TrueValue, 4);
			}
		}

		inline void recordObjectStart() {
			this->appendTapeValue(TapeType::StartObject, 0);
			return;
		}

		inline void recordArrayStart() {
			this->appendTapeValue(TapeType::StartArray, 0);
			return;
		}

		inline void recordObjectEnd() {
			this->appendTapeValue(TapeType::EndObject, 0);
			return;
		}

		inline void recordArrayEnd() {
			this->appendTapeValue(TapeType::EndArray, 0);
			return;
		}
		
		inline void recordFalseAtom(const char* value) {
			if (strcmp(value, "false")) {
				this->appendTapeValue(TapeType::FalseValue, 5);
				return;
			}
		}

		inline void recordNullAtom(const char* value) {
			if (strcmp(value, "null")) {
				this->appendTapeValue(TapeType::NullValue, 4);
				return;
			}
		}

		inline void recordNumber(const char* value) {
			if (this->jsonRawTape.size() > 0) {
				this->appendTapeValue(TapeType::Uint64, this->jsonRawTape[this->appendIndex] - this->getCurrentIndex());
			}
			return;
		}

		inline void recordKey(const char* value) {
			if (this->jsonRawTape.size() > 0) {
				this->appendTapeValue(TapeType::String, this->jsonRawTape[this->appendIndex] - this->getCurrentIndex());
			}
			return;
		}

		inline void recordEmptyObject() {
			this->appendTapeValue(TapeType::StartObject, 0);
			this->appendTapeValue(TapeType::EndObject, 0);
			return;
		}

		inline void recordDocumentEnd() {
			this->appendTapeValue(TapeType::DocumentEnd, 0);
			return;
		}

		inline void recordEmptyArray() {
			this->appendTapeValue(TapeType::StartArray, 0);
			this->appendTapeValue(TapeType::EndArray, 0);
			return;
		}

		inline void recordString(const char* value) {
			
			if (this->jsonRawTape.size() > this->appendIndex) {
				this->appendTapeValue(TapeType::String, this->jsonRawTape[this->appendIndex] - this->getCurrentIndex());
			} else {
				std::cout << "THE FINAL SIZE: " << this->stringView.size() - this->getCurrentIndex() << std::endl;
				this->appendTapeValue(TapeType::String, this->stringView.size() - this->getCurrentIndex());
			}
			return;
		}

		inline void recordPrimitive(const char* value) {
			switch (*value) {
				case '"':
					return this->recordString(value);
				case 't':
					return this->recordTrueAtom(value);
				case 'f':
					return this->recordFalseAtom(value);
				case 'n':
					return this->recordNullAtom(value);
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
					return this->recordNumber(value);
				default:
					throw JsonifierException{ "Failed to generate Json data: Value: " + *value + std::string{ " Reason : " } +
						std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
			}
		}

		uint32_t depth{ 0 };

		uint32_t getCurrentIndex() {
			if (this->appendIndex == 0) {
				return this->jsonRawTape[0];
			} else {
				return this->jsonRawTape[this->appendIndex - 1];
			}
		}

		inline bool atEof() {
			if (this->appendIndex >= this->jsonRawTape.size()){
				return true;
			} else {
				return false;
			}
		}

		inline const char* peek() noexcept {
			return &this->stringView[this->appendIndex];
		}

		inline const char* advance() noexcept {
			if (this->appendIndex >= this->jsonRawTape.size()) {
				return nullptr;
			}
			auto returnValue = &this->stringView[this->jsonRawTape[this->appendIndex]];
			this->appendIndex++;
			return returnValue;
		}

		inline ErrorCode generateJsonData() {
			if (this->atEof()) {
				return ErrorCode::Success;
			}
			switch (this->currentState) {
				case JsonTapeEventStates::DocumentStart: {
					auto value = this->advance();
					if (!value) {
						return ErrorCode::Success;
					}
					switch (*value) {
						case '{':
							if (*this->peek() == '}') {
								this->advance();
								this->recordEmptyObject();
								break;
							}
							this->currentState = JsonTapeEventStates::ObjectBegin;
							return this->generateJsonData();

						case '[':
							if (*this->peek() == ']') {
								this->advance();
								this->recordEmptyArray();
								break;
							}
							this->currentState = JsonTapeEventStates::ArrayBegin;
							return this->generateJsonData();
						default:
							this->recordPrimitive(value);

							return this->generateJsonData();
					}
				}
				case JsonTapeEventStates::ObjectBegin: {
					this->depth++;
					if (this->isArray.size() < this->depth) {
						this->isArray.push_back(false);
					} else {
						this->isArray[this->depth - 1] = false;
					}
					this->recordObjectStart();
					auto key = this->advance();
					if (!key) {
						return ErrorCode::Success;
					}
					if (*key != '"') {
						throw JsonifierException{ "Failed to generate Json data: Reason: " +
							std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
					}
					this->recordKey(key);
					this->currentState = JsonTapeEventStates::ObjectField;
					return this->generateJsonData();
				}
				case JsonTapeEventStates::ObjectField: {
					if ((*this->advance() != ':')) {
						throw JsonifierException{ "Failed to generate Json data: Reason: " +
							std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
					}
					auto value = this->advance();
					if (!value) {
						return ErrorCode::Success;
					}
					switch (*value) {
						case '{':
							if (*this->peek() == '}') {
								this->advance();
								this->recordEmptyObject();
								this->currentState = JsonTapeEventStates::ObjectContinue;
								return this->generateJsonData();
							}
							this->currentState = JsonTapeEventStates::ObjectBegin;
							return this->generateJsonData();
						case '[':
							if (*this->peek() == ']') {
								this->advance();
								this->recordEmptyArray();
								this->currentState = JsonTapeEventStates::ObjectContinue;
								return this->generateJsonData();
							}
							this->currentState = JsonTapeEventStates::ArrayBegin;
							return this->generateJsonData();
						default:
							this->currentState = JsonTapeEventStates::ObjectContinue;
							this->recordPrimitive(value);
							break;
					}
				}
				case JsonTapeEventStates::ObjectContinue: {
					auto value = this->advance();
					if (!value) {
						return ErrorCode::Success;
					}
					switch (*value) {
						case ',': {
							auto key = this->advance();
							if (*key != '"') {
								throw JsonifierException{ "Failed to generate Json data: Reason: " +
									std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
							}
							this->recordKey(key);
							this->currentState = JsonTapeEventStates::ObjectField;
							return this->generateJsonData();
						}
						case '}':
							this->recordObjectEnd();
							this->currentState = JsonTapeEventStates::ScopeEnd;
							return this->generateJsonData();
						default:
							this->currentState = JsonTapeEventStates::ObjectField;
							return this->generateJsonData();
					}
				}
				case JsonTapeEventStates::ScopeEnd: {
					depth--;
					if (depth == 0) {
						this->currentState = JsonTapeEventStates::DocumentEnd;
						std::cout << "DOCUMENT ENDING!" << std::endl;
						return this->generateJsonData();
					}
					if (this->isArray[depth]) {
						this->currentState = JsonTapeEventStates::ArrayContinue;
						return this->generateJsonData();
					}
					this->currentState = JsonTapeEventStates::ObjectContinue;
					return this->generateJsonData();
				}
				case JsonTapeEventStates::ArrayBegin: {
					depth++;
					if (this->isArray.size() < this->depth) {
						this->isArray.push_back(true);
					} else {
						this->isArray[this->depth - 1] = true;
					}
					this->recordArrayStart();
					this->currentState = JsonTapeEventStates::ArrayValue;
					return this->generateJsonData();
				}
				case JsonTapeEventStates::ArrayValue: {
					auto value = this->advance();
					if (!value) {
						return ErrorCode::Success;
					}
					switch (*value) {
						case '{':
							if (*this->peek() == '}') {
								this->advance();
								this->recordEmptyObject();
								this->currentState = JsonTapeEventStates::ArrayContinue;
								return this->generateJsonData();
							}
							this->currentState = JsonTapeEventStates::ObjectBegin;
							return this->generateJsonData();
						case '[':
							if (*this->peek() == ']') {
								this->advance();
								this->advance();
								this->recordEmptyObject();
								this->currentState = JsonTapeEventStates::ArrayContinue;
								return this->generateJsonData();
							}
							this->currentState = JsonTapeEventStates::ArrayContinue;
							return this->generateJsonData();
						default:
							this->recordPrimitive(value);
							this->currentState = JsonTapeEventStates::ArrayContinue;
							return this->generateJsonData();
					}
				}
				case JsonTapeEventStates::ArrayContinue: {
					auto value = this->advance();
					if (!value) {
						return ErrorCode::Success;
					}
					switch (*value) {
						case ',':
							this->currentState = JsonTapeEventStates::ArrayValue;
							return this->generateJsonData();
						case ']':
							this->currentState = JsonTapeEventStates::ScopeEnd;
							return this->generateJsonData();
						default:
							throw JsonifierException{ "Failed to generate Json data: Reason: " +
								std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
					}
				}
				case JsonTapeEventStates::DocumentEnd: {
					this->recordDocumentEnd();
					return ErrorCode::Success;
				}				
								
			}		

		}



		inline Jsonifier getJsonData() {
			this->generateJsonEvents();
			std::cout << "JSON EVENTS SIZE: " << this->jsonEvents.size() << std::endl;
			this->currentState = JsonTapeEventStates::DocumentStart;
			return this->jsonConstructor.operator Jsonifier();
		}

	  protected:
		JsonTapeEventStates currentState{ JsonTapeEventStates::DocumentStart};
		std::deque<uint32_t> jsonRawTape{};
		uint32_t appendIndex{};
		std::deque<JsonTapeEvent> jsonEvents{};
		JsonConstructor jsonConstructor{ &this->jsonEvents, &this->stringView };
		std::string_view stringView{};
		std::vector<bool> isArray{ false };
	};
};