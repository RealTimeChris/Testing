#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)

#include "NumberParsingUtils.hpp"

namespace Jsonifier {

	struct JsonifierException : public std::runtime_error, std::string {
		inline JsonifierException(const std::string&, std::source_location = std::source_location::current()) noexcept;
	};

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

	class Jsonifier;

	class JsonSerializer;

	template<typename Ty>
	concept IsConvertibleToJsonifier = std::convertible_to<Ty, Jsonifier>;

	template<typename Ty>
	concept IsConvertibleToJsonSerializer = std::convertible_to<Ty, JsonSerializer>;

	class SimdJsonValue;

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
				this->jsonValue.array->emplace_back(std::move(value));
			}
			return *this;
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier(std::vector<OTy>&& data) noexcept {
			*this = std::move(data);
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::vector<OTy>& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& value: data) {
				this->jsonValue.array->emplace_back(value);
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

		bool parseString(const std::string&) noexcept;

		size_t size() noexcept;

		Jsonifier& emplaceBack(Jsonifier&& data) noexcept;
		Jsonifier& emplaceBack(Jsonifier& data) noexcept;

		~Jsonifier() noexcept;

	  protected:
		std::unique_ptr<SimdJsonValue> parser{};
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
		if (this->type != JsonType::Object) {
			return Jsonifier::ObjectType{};
		}
		return std::move(*this->jsonValue.object);
	}

	template<> inline Jsonifier::ArrayType Jsonifier::getValue() {
		if (this->type != JsonType::Array) {
			return Jsonifier::ArrayType{};
		}
		return std::move(*this->jsonValue.array);
	}

	template<> inline Jsonifier::StringType Jsonifier::getValue() {
		if (this->type != JsonType::String) {
			return Jsonifier::StringType{};
		}
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

		explicit operator bool() {
			for (size_t x = 0; x < 4; ++x) {
				if (this->getUint64(x) != 0) {
					return true;
				}
			}
			return false;
		}

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
					break;
				}
				case 1: {
					*this = _mm256_insert_epi64(this->value, value, 1);
					break;
				}
				case 2: {
					*this = _mm256_insert_epi64(this->value, value, 2);
					break;
				}
				case 3: {
					*this = _mm256_insert_epi64(this->value, value, 3);
					break;
				}
				default: {
					*this = _mm256_insert_epi64(this->value, value, 0);
					break;
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

		SimdBase256 operator-(int32_t other) {
			auto currentValue = *this;
			if (other == 1) {
				currentValue = ~currentValue & 1;
			}
			return currentValue;
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

	constexpr const uint64_t JSON_VALUE_MASK = 0x00FFFFFFFFFFFFFF;
	constexpr const uint32_t JSON_COUNT_MASK = 0xFFFFFF;

	inline SimdBase256 convertSimdBytesToBits(SimdBase256 input00[8]) {
		SimdBase256 returnValue{};
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[0]), 0);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[1]), 1);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[2]), 2);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[3]), 3);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[4]), 4);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[5]), 5);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[6]), 6);
		returnValue = _mm256_insert_epi32(returnValue, _mm256_movemask_epi8(input00[7]), 7);
		return returnValue;
	}

	class SimdStringSection {
	  public:
		inline SimdStringSection() noexcept = default;

		inline void packStringIntoValue(SimdBase256* theValue, const char string[32]) {
			for (size_t x = 0; x < 32; ++x) {
				*theValue = string;
			}
		}

		inline uint64_t addTapeValues(std::unique_ptr < uint64_t[] >& tapePtrs, uint64_t* theBits, size_t currentIndexNew, size_t currentIndexIntoString,
			size_t& currentIndexIntoTape) {
			uint64_t value = static_cast<uint64_t>(__popcnt64(*theBits));
			for (int i = 0; i < value; i++) {
				tapePtrs[currentIndexIntoTape++] = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + currentIndexIntoString;
				*theBits = _blsr_u64(*theBits);
			}
			
			return value;
		}

		inline size_t getStructuralIndices(std::unique_ptr<uint64_t[]>& currentPtr, size_t currentIndex, size_t& currentIndexIntoTape) {
			size_t returnValue{};
			for (size_t x = 0; x < 4; ++x) {
				auto newValue = this->S256.getUint64(x);
				returnValue += this->addTapeValues(currentPtr, &newValue, x, currentIndex, currentIndexIntoTape);
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
			return convertSimdBytesToBits(whiteSpaceReal);
		}

		inline SimdBase256 collectStructuralCharacters() {
			char valuesNew[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
			SimdBase256 opTable{ valuesNew };
			SimdBase256 structural[8]{};
			for (size_t x = 0; x < 8; ++x) {
				auto valuesNew00 = this->values[x] | SimdBase256{ 0x20 };
				structural[x] = this->values[x].shuffle(opTable) == valuesNew00;
			}

			return convertSimdBytesToBits(structural);
		}

		inline SimdBase256 collectQuotedRange(int64_t& prevInString) {
			SimdBase256 backslashes = _mm256_set1_epi8('\\');
			SimdBase256 backslashesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				backslashesReal[x] = this->values[x] == backslashes;
			}

			auto B256 = convertSimdBytesToBits(backslashesReal);

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

			return convertSimdBytesToBits(quotesReal);
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
		inline SimdJsonValue() {}

		int32_t round(int32_t a, int32_t n) {
			return (((a) + (( n )-1)) & ~(( n )-1));
		}

		inline ErrorCode allocate(size_t capacity) noexcept {
			if (capacity == 0) {
				this->stringBuffer.reset(nullptr);
				this->tape.reset(nullptr);
				this->allocatedCapacity = 0;
				return ErrorCode::Success;
			}

			size_t tapeCapacity = round(capacity + 3, 64);
			size_t stringCapacity = round(5 * capacity / 3 + 64, 64);
			this->stringBuffer = std::make_unique<uint8_t[]>(stringCapacity);
			this->tape = std::make_unique<uint64_t[]>(tapeCapacity);
			if (!(this->stringBuffer && this->tape)) {
				this->allocatedCapacity = 0;
				this->stringBuffer.reset(nullptr);
				this->tape.reset(nullptr);
				return ErrorCode::MemAlloc;
			}
			this->isArray = std::make_unique<bool[]>(12);
			this->openContainers = std::make_unique<OpenContainer[]>(this->maxDepth);
			this->allocatedCapacity = capacity;
			return ErrorCode::Success;
		}

		inline void generateJsonEvents(const char* stringNew, size_t stringLength) {
			if (stringLength == 0) {
				throw JsonifierException{ "Failed to parse as the string size is 0." };
			}
			this->tapeLength = 0;
			this->stringView = stringNew;
			this->stringLengthRaw = stringLength;
			if (this->allocatedCapacity < this->stringLengthRaw) {
				if (this->allocate(this->stringLengthRaw) != ErrorCode::Success) {
					throw JsonifierException{ "Failed to allocate properly!" };
				}
			}
			int64_t stringSize = this->allocatedCapacity;
			uint32_t collectedSize{};
			size_t tapeCurrentIndex{ 0 };
			int64_t prevInString{};
			while (stringSize > 0) {
				SimdStringSection section(this->stringView + collectedSize, prevInString);
				auto indexCount = section.getStructuralIndices(this->tape, collectedSize, tapeCurrentIndex);
				this->tapeLength+= indexCount;
				stringSize -= 256;
				collectedSize += 256;
			}
			for (size_t x = this->tapeLength-1; x >0; --x) {
				if (this->tape[x] > this->stringLengthRaw - 1) {
					continue;
				} else {
					this->tapeLength = x + 1;
					break;
				}
				//std::cout << "THE CURRENT VALUE: " << this->getStructuralIndexes()[x] << std::endl;
			}
		}

		inline ~SimdJsonValue() noexcept {}

		inline const char* getStringView() {
			return this->stringView;
		}

		inline uint8_t* getStringViewNew() {
			return this->stringBuffer.get();
		}

		OpenContainer* getOpenContainers() {
			return this->openContainers.get();
		}

		inline uint32_t& getCurrentDepth() {
			return this->depth;
		}

		inline uint64_t* getStructuralIndexes() {
			return this->tape.get();
		}

		inline Jsonifier getJsonData(const char* stringNew, size_t stringLength);

		inline uint32_t getMaxDepth() {
			return this->maxDepth;
		}

		inline size_t getTapeLength() {
			return this->tapeLength;
		}

		inline bool* getIsArray() {
			return this->isArray.get();
		}
		
	  protected:
		std::unique_ptr<OpenContainer[]> openContainers{};
		std::unique_ptr<uint8_t[]> stringBuffer{};
		std::unique_ptr<uint64_t[]> tape{};
		std::unique_ptr<bool[]> isArray{};
		size_t allocatedCapacity{};
		size_t stringLengthRaw{};
		const char* stringView{};
		uint32_t maxDepth{ 500 };
		size_t tapeLength{ 0 };
		uint32_t depth{ 0 };
	};

	enum class TapeType : uint8_t {
		Root = 'r',
		Start_Array = '[',
		Start_Object = '{',
		End_Array = ']',
		End_Object = '}',
		String = '"',
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
		uint64_t* nextStructural{ nullptr };
		uint32_t nextStructuralIndex{};
		const uint8_t* buf{ nullptr };
		SimdJsonValue* masterParser;
		
		inline Jsonifier walkDocument(TapeBuilder&& visitor) noexcept;

		inline JsonIterator(SimdJsonValue* masterParserNew, size_t start_structural_index);

		inline const uint8_t* peek() const noexcept;

		inline const uint8_t* advance() noexcept;

		inline size_t remainingLen() const noexcept;

		inline bool atEof() const noexcept;

		inline bool atBeginning() const noexcept;

		inline uint8_t lastStructural() const noexcept;

		inline Jsonifier visitRootPrimitive(TapeBuilder& visitor, const uint8_t* value) noexcept;
		inline Jsonifier visitPrimitive(TapeBuilder& visitor, const uint8_t* value) noexcept;
	};

	inline JsonIterator::JsonIterator(SimdJsonValue* masterParserNew, size_t start_structural_index)
		: nextStructural(masterParserNew->getStructuralIndexes()), buf{ reinterpret_cast<const uint8_t*>(masterParserNew->getStringView()) }, masterParser{
			  masterParserNew
		  } {};

	inline const uint8_t* JsonIterator::peek() const noexcept {
		return &buf[masterParser->getStructuralIndexes()[*this->nextStructural]];
	}

	inline const uint8_t* JsonIterator::advance() noexcept {
		return &buf[((*this->nextStructural++) & 0x0fffffff)];
	}

	inline size_t JsonIterator::remainingLen() const noexcept {
		return masterParser->getTapeLength() - *this->nextStructural;
	}

	inline bool JsonIterator::atEof() const noexcept {
		return this->nextStructural == &masterParser->getStructuralIndexes()[masterParser->getTapeLength() - 1];
	}

	inline bool JsonIterator::atBeginning() const noexcept {
		return this->nextStructural == masterParser->getStructuralIndexes();
	}

	inline uint8_t JsonIterator::lastStructural() const noexcept {
		return buf[masterParser->getStructuralIndexes()[masterParser->getTapeLength() - 1]];
	}

	const bool structuralOrWhitespaceNegated[256]{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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

	inline uint32_t string_to_uint32(const char* str) {
		uint32_t val{};
		std::memcpy(&val, str, sizeof(uint32_t));
		return val;
	}

	inline uint32_t isNotStructuralOrWhiteSpace(uint8_t c) {
		return structuralOrWhitespaceNegated[c];
	}

	 inline uint32_t str4ncmp(const uint8_t* src, const char* atom) {
		uint32_t srcval{};
		 static_assert(sizeof(uint32_t) <= 256, "SIMDJSON_PADDING must be larger than 4 bytes");
		std::memcpy(&srcval, src, sizeof(uint32_t));
		 return srcval ^ string_to_uint32(atom);
	}

	 inline bool isValidTrueAtom(const uint8_t* src) {
		return (str4ncmp(src, "true") | isNotStructuralOrWhitespace(src[4])) == 0;
	}

	 inline bool isValidTrueAtom(const uint8_t* src, size_t len) {
		if (len > 4) {
			 return isValidTrueAtom(src);
		} else if (len == 4) {
			return !str4ncmp(src, "true");
		} else {
			return false;
		}
	}

	 inline bool isValidFalseAtom(const uint8_t* src) {
		return (str4ncmp(src + 1, "alse") | isNotStructuralOrWhitespace(src[5])) == 0;
	}

	 inline bool isValidFalseAtom(const uint8_t* src, size_t len) {
		if (len > 5) {
			 return isValidFalseAtom(src);
		} else if (len == 5) {
			return !str4ncmp(src + 1, "alse");
		} else {
			return false;
		}
	}

	 inline bool isValidNullAtom(const uint8_t* src) {
		return (str4ncmp(src, "null") | isNotStructuralOrWhitespace(src[4])) == 0;
	}

	 inline bool isValidNullAtom(const uint8_t* src, size_t len) {
		if (len > 4) {
			 return isValidNullAtom(src);
		} else if (len == 4) {
			return !str4ncmp(src, "null");
		} else {
			return false;
		}
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

	class JsonConstructor : public Jsonifier {
	  public:
		JsonConstructor() noexcept {
			this->currentPlace.emplace_back(&Jsonifier::operator=(JsonType::Object));
		};

		void setCurrentKey(std::string_view key) {
			//std ::cout << "THE KEY: " << key << std::endl;
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
		}

		void endArray() {
			this->currentKey.erase(this->currentKey.end() - 1);
			this->currentPlace.erase(this->currentPlace.end() - 1);
		}

		void endObject() {
			this->currentKey.erase(this->currentKey.end() - 1);
			this->currentPlace.erase(this->currentPlace.end() - 1);
		}

		template<typename OTy> void appendPrimitiveElement(OTy&& data) {
			if (this->currentPlace.back()->getType() == JsonType::Array) {
				this->currentPlace.back()->emplaceBack(std::move(data));
			} else {
				(*this->currentPlace.back())[std::move(this->currentKey.back())] = std::move(data);
			}
		}

		Jsonifier getResult() {
			//this->jsonData.refreshString(JsonifierSerializeType::Json);
			//std::cout << "THE DATA: " << this->jsonData.operator std::basic_string_view<char, std::char_traits<char>>() << std::endl;
			return std::move(*this);
		}

	  protected:
		std::vector<Jsonifier*> currentPlace{};
		std::vector<std::string> currentKey{};
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
		inline bool visitTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline bool visitFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline nullptr_t visitNullAtom(JsonIterator& iter, const uint8_t* value) noexcept;

		inline std::string_view visitRootString(JsonIterator& iter, const uint8_t* value) noexcept;
		inline Jsonifier visitRootNumber(JsonIterator& iter, const uint8_t* value) noexcept;
		inline bool visitRootTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline bool visitRootFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline nullptr_t visitRootNullAtom(JsonIterator& iter, const uint8_t* value) noexcept;

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
		JsonIterator iter(&masterParser, 0);
		TapeBuilder builder(masterParser);
		return iter.walkDocument(std::move(builder));
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
		TapeWriter::write(iter.masterParser->getStructuralIndexes()[startTapeIndex], nextTapeIndex(iter), TapeType::Root);
		return ErrorCode::Success;
	}
	inline std::string_view TapeBuilder::visitKey(JsonIterator& iter, const uint8_t* key) noexcept {
		return visitString(iter, key);
	}

	inline ErrorCode TapeBuilder::incrementCount(JsonIterator& iter) noexcept {
		iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].count++;
		return ErrorCode::Success;
	}

	inline TapeBuilder::TapeBuilder(SimdJsonValue& doc) noexcept
		: tape{ doc.getStructuralIndexes() }, currentStringBufferLocation{ reinterpret_cast<uint8_t*>(doc.getStringViewNew()) } {
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

	static const uint8_t escapeMap[256]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x2f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5c, 0, 0, 0, 0, 0, 0x08, 0, 0, 0, 0x0c, 0, 0, 0, 0, 0,
		0, 0, 0x0a, 0, 0, 0, 0x0d, 0, 0x09, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	inline int32_t trailingZeroCount(SimdBase256 toCount) {
		int32_t returnValue{};
		for (size_t x = 0; x < 4; ++x) {
			auto value = _tzcnt_u64(toCount.getUint64(x));
			if (value < 64) {
				return value;
			}
			returnValue += value;
		}
		return returnValue;
	}

	struct BackslashAndQuote {
	  public:
		static constexpr uint32_t BYTES_PROCESSED = 256;
		inline static BackslashAndQuote copyAndFind(const uint8_t* src, uint8_t* dst);

		inline bool hasQuoteFirst() {
			return bool{ ((bsBits - 1) & quoteBits) == 0 };
		}
		inline bool hasBackslash() {
			return bool{ ((quoteBits - 1) & bsBits) == 0 };
		}
		inline int32_t quoteIndex() {
			return trailingZeroCount(quoteBits);
		}
		inline int32_t backslashIndex() {
			return trailingZeroCount(bsBits);
		}

		SimdBase256 bsBits{};
		SimdBase256 quoteBits{};
	};

	inline BackslashAndQuote BackslashAndQuote::copyAndFind(const uint8_t* src, uint8_t* dst) {
		static_assert(256 >= (BYTES_PROCESSED - 1), "backslash and quote finder must process fewer than 256 bytes");
		SimdBase256 values[8]{};
		SimdBase256 returnValues01[8]{};
		SimdBase256 returnValues02[8]{};
		for (size_t x = 0; x < 4; ++x) {
			values[x] = reinterpret_cast<const char*>(src + x * 32);
			values[x].store(reinterpret_cast<char*>(dst + x * 32));
			returnValues01[x] = values[x] == '\\';
			returnValues02[x] = values[x] == '"';
		}
		
		auto result01 = convertSimdBytesToBits(returnValues01);
		auto result02 = convertSimdBytesToBits(returnValues02);
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
					src += bsDist + 2ull;
					dst += bsDist + 1ull;
				}
			} else {
				src += BackslashAndQuote::BYTES_PROCESSED;
				dst += BackslashAndQuote::BYTES_PROCESSED;
			}
		}
		return nullptr;
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
		return parseNumber<Jsonifier, TapeWriter>(value, this->tape);
	}

	inline Jsonifier TapeBuilder::visitRootNumber(JsonIterator& iter, const uint8_t* value) noexcept {
		std::unique_ptr<uint8_t[]> copy(new (std::nothrow) uint8_t[iter.remainingLen() + 256]);
		if (copy.get() == nullptr) {
			return ErrorCode::MemAlloc;
		}
		std::memcpy(copy.get(), value, iter.remainingLen());
		std::memset(copy.get() + iter.remainingLen(), ' ', 256);
		return visitNumber(iter, copy.get());
	}

	inline bool TapeBuilder::visitTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidTrueAtom(value, static_cast<size_t>(iter.masterParser->getStringView()[*iter.nextStructural]- (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return false;
		}
		tape.append(0, TapeType::True_Value);
		return true;
	}

	inline bool TapeBuilder::visitRootTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidTrueAtom(value,
				static_cast<size_t>(
					iter.masterParser->getStringView()[*iter.nextStructural] - (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return false;
		}
		tape.append(0, TapeType::True_Value);
		return true;
	}

	inline bool TapeBuilder::visitFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidFalseAtom(value,
				static_cast<size_t>(
					iter.masterParser->getStringView()[*iter.nextStructural] - (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return true;
		}
		tape.append(0, TapeType::False_Value);
		return false;
	}

	inline bool TapeBuilder::visitRootFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidFalseAtom(value,
				static_cast<size_t>(
					iter.masterParser->getStringView()[*iter.nextStructural] - (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return true;
		}
		tape.append(0, TapeType::False_Value);
		return false;
	}

	inline nullptr_t TapeBuilder::visitNullAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidNullAtom(value,
				static_cast<size_t>(
					iter.masterParser->getStringView()[*iter.nextStructural] - (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return nullptr;
		}
		tape.append(0, TapeType::Null_Value);
		return nullptr;
	}

	inline nullptr_t TapeBuilder::visitRootNullAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!isValidNullAtom(value,
				static_cast<size_t>(
					iter.masterParser->getStringView()[*iter.nextStructural] - (iter.masterParser->getStringView()[*(iter.nextStructural - 1)])))) {
			return nullptr;
		}
		tape.append(0, TapeType::Null_Value);
		return nullptr;
	}

	inline uint32_t TapeBuilder::nextTapeIndex(JsonIterator& iter) const noexcept {
		auto startTapeIndex = uint32_t(*tape.nextTapeLocation - *iter.masterParser->getStructuralIndexes());
		return startTapeIndex;
	}

	inline ErrorCode TapeBuilder::emptyContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept {
		auto startIndex = nextTapeIndex(iter);
		tape.append(startIndex + 2ull, start);
		tape.append(startIndex, end);
		return ErrorCode::Success;
	}

	inline void TapeBuilder::startContainer(JsonIterator& iter) noexcept {
		iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].tapeIndex = nextTapeIndex(iter);
		iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].count = 0;
		tape.skip();
	}

	inline ErrorCode TapeBuilder::endContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept {
		const uint32_t startTapeIndex = iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].tapeIndex;
		tape.append(startTapeIndex, end);
		const uint32_t count = iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].count;
		const uint32_t cntsat = count > 0xFFFFFF ? 0xFFFFFF : count;
		TapeWriter::write(iter.masterParser->getStructuralIndexes()[startTapeIndex], nextTapeIndex(iter) | (uint64_t(cntsat) << 32), start);
		return ErrorCode::Success;
	}

	inline uint8_t* TapeBuilder::onStartString(JsonIterator& iter) noexcept {
		tape.append(currentStringBufferLocation - reinterpret_cast<uint8_t*>(iter.masterParser->getStringViewNew()), TapeType::String);
		return currentStringBufferLocation + sizeof(uint32_t);
	}

	inline void TapeBuilder::onEndString(uint8_t* dst) noexcept {
		uint32_t stringLength = uint32_t(dst - (currentStringBufferLocation + sizeof(uint32_t)));
		memcpy(currentStringBufferLocation, &stringLength, sizeof(uint32_t));
		*dst = 0;
		currentStringBufferLocation = dst + 1;
	}

	inline Jsonifier JsonIterator::walkDocument(TapeBuilder&& visitor) noexcept {
#ifndef _TESTING
		JsonConstructor constructor{};
#endif
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
					goto Object_Begin;
				case '[':
					if (*peek() == ']') {
						advance();
						visitor.visitEmptyArray(*this);
						break;
					}
					goto Array_Begin;
				default:
					visitor.visitRootPrimitive(*this, value);
					break;
			}
		}
		goto Document_End;

	Object_Begin:
		masterParser->getCurrentDepth()++;
		if (masterParser->getCurrentDepth() >= masterParser->getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		masterParser->getIsArray()[masterParser->getCurrentDepth()] = false;
		visitor.visitObjectStart(*this);

		{
			auto key = this->advance();
			if (*key != '"') {
				return ErrorCode::TapeError;
			}
			visitor.incrementCount(*this);
#ifndef _TESTING
			constructor.setCurrentKey(visitor.visitKey(*this, key));
#endif
		}

	Object_Field:
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
#ifndef _TESTING
					constructor.startNewObject();
#endif
					goto Object_Begin;
				case '[':
					if (*peek() == ']') {
						advance();
						visitor.visitEmptyArray(*this);
						break;
					}
#ifndef _TESTING
					constructor.startNewArray();
#endif
					goto Array_Begin;
				default:
#ifndef _TESTING
					constructor.appendPrimitiveElement(visitor.visitPrimitive(*this, value));
#endif
					break;
			}
		}

	Object_Continue:
		switch (*advance()) {
			case ',':
				visitor.incrementCount(*this);
				{
					auto key = this->advance();
					if (*key != '"') {
						return ErrorCode::TapeError;
					}
#ifndef _TESTING
					constructor.setCurrentKey(visitor.visitKey(*this, key));
#endif
				}
				goto Object_Field;
			case '}':
#ifndef _TESTING
				constructor.endObject();
#endif
				visitor.visitObjectEnd(*this);
				goto Scope_End;
			default:
				return ErrorCode::TapeError;
		}

	Scope_End:
		masterParser->getCurrentDepth()--;
		if (masterParser->getCurrentDepth() == 0) {
			goto Document_End;
		}
		if (masterParser->getIsArray()[masterParser->getCurrentDepth()]) {
			goto Array_Continue;
		}
		goto Object_Continue;

	Array_Begin:
		masterParser->getCurrentDepth()++;
		if (masterParser->getCurrentDepth() >= masterParser->getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		masterParser->getIsArray()[masterParser->getCurrentDepth()] = true;
		visitor.visitArrayStart(*this);
		visitor.incrementCount(*this);

	Array_Value : {
		auto value = this->advance();
		switch (*value) {
			case '{':
				if (*peek() == '}') {
					advance();
					visitor.visitEmptyObject(*this);
					break;
				}
#ifndef _TESTING
				constructor.startNewObject();
#endif
				goto Object_Begin;
			case '[':
				if (*peek() == ']') {
					advance();
					visitor.visitEmptyArray(*this);
					break;
				}
#ifndef _TESTING
				constructor.startNewArray();
#endif
				goto Array_Begin;
			default:
#ifndef _TESTING
				constructor.appendPrimitiveElement(visitor.visitPrimitive(*this, value));
#endif
				break;
		}
	}

	Array_Continue:
		switch (*advance()) {
			case ',':
				visitor.incrementCount(*this);
				goto Array_Value;
			case ']':
#ifndef _TESTING
				constructor.endArray();
#endif
				visitor.visitArrayEnd(*this);
				goto Scope_End;
			default:
				return ErrorCode::TapeError;
		}

	Document_End:
		visitor.visitDocumentEnd(*this);

		this->nextStructuralIndex = uint32_t(nextStructural - &masterParser->getStructuralIndexes()[0]);

		if (this->nextStructuralIndex != masterParser->getTapeLength()) {
			return ErrorCode::TapeError;
		}

#ifndef _TESTING
		return constructor.getResult();
#else
		return Jsonifier{};
#endif
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

	Jsonifier SimdJsonValue::getJsonData(const char* stringNew, size_t stringLength) {
		this->generateJsonEvents(stringNew, stringLength);
		return TapeBuilder::parseDocument(*this);
	}

};