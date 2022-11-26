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

		template<typename Ty> const Ty& getValue() const {
			return Ty{};
		}

		template<typename Ty> Ty& getValue() {
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

	template<> inline Jsonifier::ObjectType& Jsonifier::getValue() {
		return *this->jsonValue.object;
	}

	template<> inline Jsonifier::ArrayType& Jsonifier::getValue() {
		return *this->jsonValue.array;
	}

	template<> inline Jsonifier::StringType& Jsonifier::getValue() {
		return *this->jsonValue.string;
	}

	template<> inline Jsonifier::FloatType& Jsonifier::getValue() {
		return this->jsonValue.numberDouble;
	}

	template<> inline Jsonifier::UintType& Jsonifier::getValue() {
		return this->jsonValue.numberUint;
	}

	template<> inline Jsonifier::IntType& Jsonifier::getValue() {
		return this->jsonValue.numberInt;
	}

	template<> inline Jsonifier::BoolType& Jsonifier::getValue() {
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
		SimdBase128 newValue{};
		for (size_t x = 0; x < 2; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) |= (*(reinterpret_cast<int64_t*>(&this->value) + x) << (amount % 64));
			if (x > 0) {
				*(reinterpret_cast<int64_t*>(&newValue) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x - 1) >> 63) & 1);
			}
		}
		return newValue;
	}

	inline SimdBase128 operator~() {
		SimdBase128 newValue{};
		for (size_t x = 0; x < 2; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) = ~*(reinterpret_cast<int64_t*>(&this->value) + x);
		}
		return newValue;
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
		SimdBase256 newValue{};
		for (size_t x = 0; x < 4; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) |= (*(reinterpret_cast<int64_t*>(&this->value) + x) << (amount % 64));
			if (x > 0) {
				*(reinterpret_cast<int64_t*>(&newValue) + x) |= ((*(reinterpret_cast<int64_t*>(&this->value) + x - 1) >> 63) & 1);
			}
		}
		return newValue;
	}

	inline SimdBase256 operator~() {
		SimdBase256 newValue{};
		for (size_t x = 0; x < 4; ++x) {
			*(reinterpret_cast<int64_t*>(&newValue) + x) = ~*(reinterpret_cast<int64_t*>(&this->value) + x);
		}
		return newValue;
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

	inline std::vector<uint32_t> getSetBitIndices() {
		std::vector<uint32_t> returnVector{};
		for (int64_t x = 0; x < 4; ++x) {
			for (int64_t y = 0; y < 64; ++y) {
				if (*(reinterpret_cast<int64_t*>(&this->value) + x) >> y & 1) {
					returnVector.push_back(static_cast<uint8_t>(y + (x * 64)));
				}
			}
		}
		return returnVector;
	}

  protected:
	__m256i value{};
};

inline uint64_t convertSimd256To64BitUint(SimdBase256 inputA, SimdBase256 inputB) {
	uint64_t r_lo = _mm256_movemask_epi8(inputA);
	uint64_t r_hi = _mm256_movemask_epi8(inputB);
	return r_lo | (r_hi << 32);
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

	inline std::vector<uint32_t> getStructuralIndices() {
		return this->S256.getSetBitIndices();
	}

	inline SimdBase256 collectWhiteSpace() {
		char valuesNew[32]{ ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2, 100,
			'\t', '\n', 112, 100, '\r', 100, 100 };
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

		this->S256 = SimdBase256{ convertSimd256To64BitUint(structural[0], structural[1]), convertSimd256To64BitUint(structural[2], structural[3]),
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

		this->Q256 = SimdBase256{ convertSimd256To64BitUint(quotesReal[0], quotesReal[1]), convertSimd256To64BitUint(quotesReal[2], quotesReal[3]),
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
		this->stringView = valueNew;
		
		this->packStringIntoValue(&this->values[0], this->stringView.data());
		this->packStringIntoValue(&this->values[1], this->stringView.data() + 32);
		this->packStringIntoValue(&this->values[2], this->stringView.data() + 64);
		this->packStringIntoValue(&this->values[3], this->stringView.data() + 96);
		this->packStringIntoValue(&this->values[4], this->stringView.data() + 128);
		this->packStringIntoValue(&this->values[5], this->stringView.data() + 160);
		this->packStringIntoValue(&this->values[6], this->stringView.data() + 192);
		this->packStringIntoValue(&this->values[7], this->stringView.data() + 224);

		this->Q256 = this->collectQuotes();
		this->W256 = this->collectWhiteSpace();
		this->S256 = this->collectStructuralCharacters();
		//this->S256.printBits("S FINAL VALUES (256) ");
		//this->W256.printBits("W FINAL VALUES (256) ");
		//this->R256.printBits("R FINAL VALUES (256) ");
		//this->Q256.printBits("Q FINAL VALUES (256): ");
		std::cout << "THE STRING: " << this->stringView << std::endl;
	}

  protected:
	SimdBase256 Q256{};
	SimdBase256 W256{};
	SimdBase256 R256{};
	SimdBase256 S256{};
	SimdBase256 values[8]{};
	std::string_view stringView{};
};

enum class ErrorCode { Empty = 0, TapeError = 1, DepthError = 2, Success = 3, ParseError = 4 };

enum class JsonTapeEventStates {
	ObjectBegin = 0,
	ObjectField = 1,
	ObjectContinue = 2,
	ScopeEnd = 3,
	ArrayBegin = 4,
	ArrayValue = 5,
	ArrayContinue = 6,
	DocumentEnd = 7
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
	NullValue = 'n'
};

struct JsonEvent {
	JsonEvent* ptrTosStart{};
	TapeType type{};
	size_t index{};
	size_t size{};
};

struct JsonEventWriter {
	inline void appendTapeValue(size_t sizeNew, size_t stringIndexNew, TapeType eventTypeNew) {
		JsonEvent returnValue{};
		returnValue.index = stringIndexNew;
		returnValue.type = eventTypeNew;
		
		returnValue.size = sizeNew;
		this->jsonEvents.emplace_back(returnValue);
	}

	inline auto begin() {
		return this->jsonEvents.begin();
	}

	inline auto end() {
		return this->jsonEvents.end();
	}

	inline std::vector<JsonEvent>& getEvents() {
		return this->jsonEvents;
	}

  protected:
	std::vector<JsonEvent> jsonEvents{};
};

class JsonConstructor {
  public:
	inline JsonConstructor() noexcept = default;

	inline JsonConstructor(std::vector<JsonEvent>& theEvents, std::string_view stringNew) {
		this->jsonEvents = std::move(theEvents);
		this->string = stringNew;
	}

	inline Jsonifier parseJsonToJsonObject(std::vector<JsonEvent>& events,Jsonifier& jsonDataNew) {
		if (events.size() == 0) {
			std::cout << "THE FINAL REAL SIZE: 02 " << events.size() << std::endl;
			return jsonDataNew;
		}
		std::cout << "THE FINAL REAL SIZE: 01 " << events.size() << std::endl;
		for (auto iterator = events.begin(); iterator != events.end(); --iterator) {
			//std::cout << "THE TYPE: " << ( char )iterator->type << std::endl;
			switch (iterator->type) {
				case TapeType::StartObject: {
					return this->collectObject(events, jsonDataNew);
				}
				case TapeType::StartArray: {
					return this->collectArray(events, jsonDataNew);
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
				default: {
					std::cout << "THE FINAL REAL SIZE: 0333 " << events.size() << std::endl;
					return jsonDataNew;
				}
			}
			events.erase(iterator);
			if (events.size() == 0) {
				std::cout << "THE FINAL REAL SIZE: 0334 " << events.size() << std::endl;
				return jsonDataNew;
			}
		}
		return jsonDataNew;
	}


	inline std::string collectString() {
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		JsonEvent newValue = this->jsonEvents.front();
		this->jsonEvents.erase(this->jsonEvents.begin());
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		std::cout << "THE REAL SIZE: " << newValue.size << std::endl;
		std::cout << "THE REAL INDEX: " << newValue.index << std::endl;
		return static_cast<std::string>(std::string_view{ this->string.data() + newValue.index - (newValue.size + 1), newValue.size });
	}

	inline bool collectTrueOrFalse(bool returnValue) {
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		JsonEvent newValue = this->jsonEvents.front();
		this->jsonEvents.erase(this->jsonEvents.begin());
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		return returnValue;
	}

	inline double collectFloat() {
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		JsonEvent newValue = this->jsonEvents.front();
		this->jsonEvents.erase(this->jsonEvents.begin());
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		return double{ *reinterpret_cast<const double*>(this->string.data() + newValue.index) };
	}

	inline uint64_t collectUint64() {
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		JsonEvent newValue = this->jsonEvents.front();
		this->jsonEvents.erase(this->jsonEvents.begin());
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		return uint64_t{ *reinterpret_cast<const uint64_t*>(this->string.data() + newValue.index) };
	}

	inline int64_t collectInt64() {
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		JsonEvent newValue = this->jsonEvents.front();
		this->jsonEvents.erase(this->jsonEvents.begin());
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		return int64_t{ *reinterpret_cast<const int64_t*>(this->string.data() + newValue.index) };
	}

	inline nullptr_t collectNull() {
		return nullptr;
	}

	inline Jsonifier collectObject(std::vector<JsonEvent>& events, Jsonifier& jsonDataNew) {
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		this->jsonEvents.erase(this->jsonEvents.begin());
		std::string key = this->collectString();
		std::cout << "THE KEY: " << key << std::endl;
		jsonDataNew[key] = this->parseJsonToJsonObject(events, jsonDataNew);
		std::cout << "THE FINAL REAL SIZE: " << jsonEvents.size() << std::endl;
		return jsonDataNew;
	}

	inline Jsonifier collectArray(std::vector<JsonEvent>& events, Jsonifier& jsonDataNew) {
		std::cout << "THE FINAL REAL SIZE: 04 " << jsonEvents.size() << std::endl;
		this->jsonEvents.erase(this->jsonEvents.begin());
		jsonDataNew.emplaceBack(this->parseJsonToJsonObject(events, jsonDataNew));
		std::cout << "THE FINAL REAL SIZE: 03 " << jsonEvents.size() << std::endl;
		return jsonDataNew;
	}


	inline operator Jsonifier() {
		return this->parseJsonToJsonObject(this->jsonEvents, this->jsonData);
		
	}

  protected:
	std::vector<JsonEvent> jsonEvents{};
	std::string_view string{};
	Jsonifier jsonData{};
};


class SimdStringScanner {
  public:

	inline SimdStringScanner(std::string_view stringNew) noexcept {
		if (stringNew.size() % 256 != 0) {
			this->string = stringNew;
			this->string.resize(this->string.size() + 256 - (this->string.size() % 256));
			this->stringView = this->string;

		} else {
			this->stringView = stringNew;
		}
		size_t stringSize = this->stringView.size();
		size_t collectedSize{};
		while (stringSize > 256) {
			this->stringSections.emplace_back(std::string_view{ this->stringView.data() + collectedSize, 256 });
			stringSize -= 256;
			collectedSize += 256;
		}
		if (this->stringView.size() - collectedSize > 0) {
			this->stringSections.emplace_back(std::string_view{ this->stringView.data() + collectedSize, this->stringView.size() - collectedSize });
		}
		this->generateTapeRecord();
	}

	inline void generateTapeRecord() {
		for (auto& value: this->stringSections) {
			std::vector<uint32_t> setBitIndices{ value.getStructuralIndices() };
			this->jsonTape.insert(this->jsonTape.end(), setBitIndices.begin(), setBitIndices.end());
		}
		this->next_structural = &this->jsonTape[0];
	}

	inline ErrorCode recordTrueAtom(char* value) {
		if (strcmp(value, "true")) {

			this->jsonData.appendTapeValue(4, &this->string[*this->next_structural] - this->string.data(), TapeType::TrueValue);
			return ErrorCode::Success;
		} else {
			return ErrorCode::ParseError;
		}
	}

	inline ErrorCode recordObjectStart() {
		std::cout << "WERE OBJECT STARTING!" << std::endl;

		this->jsonData.appendTapeValue(0, &this->string[*this->next_structural] - this->string.data(), TapeType::StartObject);
		return ErrorCode::Success;
	}

	inline ErrorCode recordArrayStart() {
		std::cout << "WERE ARRAY STARTING!" << std::endl;
		this->jsonData.appendTapeValue(0, &this->string[*this->next_structural] - this->string.data(), TapeType::StartArray);
		return ErrorCode::Success;
	}

	inline ErrorCode recordObjectEnd() {
		std::cout << "WERE OBJECT ENDING!" << std::endl;
		this->jsonData.appendTapeValue(0, &this->string[*this->next_structural] - this->string.data(), TapeType::EndObject);
		return ErrorCode::Success;
	}

	inline ErrorCode recordArrayEnd() {
		std::cout << "WERE ARRAY ENDING!" << std::endl;
		this->jsonData.appendTapeValue(0, &this->string[*this->next_structural] - this->string.data(), TapeType::EndArray);
		return ErrorCode::Success;
	}

	inline ErrorCode recordFalseAtom(char* value) {
		if (strcmp(value, "false")) {
			this->jsonData.appendTapeValue(5, &this->string[*this->next_structural] - this->string.data(), TapeType::FalseValue);
			return ErrorCode::Success;
		} else {
			return ErrorCode::ParseError;
		}
	}

	inline ErrorCode recordNullAtom(char* value) {
		std::cout << "WERE NULL ATOMING!" << std::endl;
		if (strcmp(value, "null")) {
			this->jsonData.appendTapeValue(4, &this->string[*this->next_structural] - this->string.data(), TapeType::NullValue);
			return ErrorCode::Success;
		} else {
			return ErrorCode::ParseError;
		}
	}

	inline ErrorCode recordNumber(char* value) {
		std::cout << "WERE NUMBERING!" << std::endl;
		this->jsonData.appendTapeValue(8, &this->string[*this->next_structural] - this->string.data(), TapeType::Uint64);
		return ErrorCode::Success;
	}

	inline ErrorCode recordKey(char* value) {
		std::cout << "WERE KEYING!" << std::endl;
		this->jsonData.appendTapeValue(this->peek() - value - 2, &this->string[*this->next_structural] - this->string.data(), TapeType::String);
		//std::cout << "THE CURRENT KEY: " << this->currentKey << std::endl;
		return ErrorCode::Success;
	}

	inline ErrorCode recordEmptyObject() {
		std::cout << "WERE EMTPYING OBJECT!" << std::endl;
		this->jsonData.appendTapeValue(2, &this->string[*this->next_structural] - this->string.data(), TapeType::StartObject);
		this->jsonData.appendTapeValue(0, &this->string[*this->next_structural] - this->string.data() + 1, TapeType::EndObject);
		return ErrorCode::Success;
	}

	inline ErrorCode recordEmptyArray() {
		std::cout << "WERE EMTPYING ARRAY!" << std::endl;
		this->jsonData.appendTapeValue(2, &this->string[*this->next_structural] - this->string.data(), TapeType::StartArray);
		this->jsonData.appendTapeValue(0, &this->string[*this->next_structural] - this->string.data() + 1, TapeType::EndArray);
		return ErrorCode::Success;
	}

	inline ErrorCode recordString(char* value) {
		std::cout << "WERE STRINGING!" << std::endl;
		this->jsonData.appendTapeValue(this->peek() - value - 2, &this->string[*this->next_structural] - this->string.data(), TapeType::String);
		return ErrorCode::Success;
	}

	inline ErrorCode recordPrimitive(char* value) {
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
				throw JsonifierException{"Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(ErrorCode::TapeError))};
		}
	}

	uint32_t* next_structural{ nullptr };
	uint32_t depth{ 0 };

	inline bool atEof() {
		return &this->string[*this->next_structural] - this->string.data() == this->string.size();
	}

	inline char* peek() noexcept {
		auto returnValue = &this->string[*(this->next_structural)];
		return returnValue;
	}

	inline char* advance(std::source_location location = std::source_location::current()) noexcept {
		auto returnValue = &this->string[*(this->next_structural++)];
		return returnValue;
	}

	inline ErrorCode generateJsonData() {
		switch (this->currentState) {
			case JsonTapeEventStates::ObjectBegin:{
				this->depth++;
				this->recordObjectStart();
				auto key = this->advance();
				if (*key != '"') {
					throw JsonifierException{ "Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
				}
				this->recordKey(key);
				this->currentState = JsonTapeEventStates::ObjectField;
			}
			case JsonTapeEventStates::ObjectField: {
				if (*this->advance() != ':') {
					throw JsonifierException{ "Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
				}
				auto value = this->advance();
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
				}
				this->currentState = JsonTapeEventStates::ObjectContinue;
			}
			case JsonTapeEventStates::ObjectContinue: {
				switch (*this->advance()) {
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
					case '}': {
						this->recordObjectEnd();
						this->currentState = JsonTapeEventStates::ScopeEnd;
						return this->generateJsonData();
					}
					default: {
						return this->generateJsonData();
						throw JsonifierException{ "Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
					}
						
				}
			}
			case JsonTapeEventStates::ScopeEnd: {
				this->depth--;
				if (this->depth == 0) {
					this->currentState = JsonTapeEventStates::DocumentEnd;
					return this->generateJsonData();
				}
				this->currentState = JsonTapeEventStates::ObjectContinue;
				return this->generateJsonData();
			}
			case JsonTapeEventStates::ArrayBegin: {
				this->depth++;
				this->recordArrayStart();
				this->currentState = JsonTapeEventStates::ArrayValue;
			}
			case JsonTapeEventStates::ArrayValue: {
				auto value = this->advance();
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
				}
			}
			case JsonTapeEventStates::ArrayContinue: {
				switch (*this->advance()) {
					case ',':
						this->currentState = JsonTapeEventStates::ArrayValue;
						return this->generateJsonData();
					case ']':
						this->recordArrayEnd();
						this->currentState = JsonTapeEventStates::ScopeEnd;
						return this->generateJsonData();
					default:
						throw JsonifierException{ "Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(ErrorCode::TapeError)) };
				}
				break;
			}
			case JsonTapeEventStates::DocumentEnd: {
				break;
			}
			default: {
				break;
			}
		}
		return ErrorCode::Success;
	}

	inline Jsonifier getJsonData() {
		auto value = advance();
		ErrorCode resultCode{};
		switch (*value) {
			case '{':
				if (*this->peek() == '}') {
					this->advance();
					this->recordEmptyObject();
					break;
				}
				this->currentState = JsonTapeEventStates::ObjectBegin;
				resultCode = this->generateJsonData();
				break;
		  case '[':
				if (*this->peek() == ']') {
					this->advance();
					this->recordEmptyArray();
					break;
				}
			  this->currentState = JsonTapeEventStates::ArrayBegin;
			  resultCode = this->generateJsonData();
			  break;
		  default:
			  resultCode = this->recordPrimitive(value);
			  break;
		}
 
		if (resultCode!= ErrorCode::Success) {
			throw std::runtime_error{ "Failed to generate Json data: Reason: " + std::to_string(static_cast<int32_t>(resultCode)) };
		}
		std::cout << "THE FINAL DATA: " << std::endl;
		for (auto& value: this->jsonData) {
			std::cout << "INDEX: " << value.index << ", SIZE: " << value.size << ", TYPE: " << ( char )value.type << std::endl;
		}
		std::cout << "THE CURRENT SIZE: " << this->jsonData.getEvents().size() << std::endl;
		this->jsonDataFinal = JsonConstructor{ this->jsonData.getEvents(), this->string };
		return this->jsonDataFinal.operator Jsonifier();
	}

  protected:
	JsonTapeEventStates currentState{ JsonTapeEventStates::ObjectBegin };
	std::vector<SimdStringSection> stringSections{};
	std::vector<uint32_t> jsonTape{};
	JsonConstructor jsonDataFinal{};
	std::string_view stringView{};
	std::string currentString{};
	JsonEventWriter jsonData{};
	std::string currentKey{};
	std::string string{};
};
};