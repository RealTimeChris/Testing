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

	class JsonSerializer : public Jsonifier {
	  public:

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

		JsonSerializer() noexcept = default;

		template<IsConvertibleToJsonSerializer OTy> JsonSerializer& operator=(std::vector<OTy>&& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& value: data) {
				this->jsonValue.array->push_back(std::move(value));
			}
			return *this;
		}

		template<IsConvertibleToJsonSerializer OTy> JsonSerializer(std::vector<OTy>&& data) noexcept {
			*this = std::move(data);
		}

		template<IsConvertibleToJsonSerializer OTy> JsonSerializer& operator=(std::vector<OTy>& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& value: data) {
				this->jsonValue.array->push_back(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonSerializer OTy> JsonSerializer(std::vector<OTy>& data) noexcept {
			*this = data;
		}

		template<IsConvertibleToJsonSerializer KTy, IsConvertibleToJsonSerializer OTy> JsonSerializer& operator=(std::unordered_map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = std::move(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonSerializer KTy, IsConvertibleToJsonSerializer OTy> JsonSerializer(std::unordered_map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonSerializer KTy, IsConvertibleToJsonSerializer OTy> JsonSerializer& operator=(std::unordered_map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = value;
			}
			return *this;
		}

		template<IsConvertibleToJsonSerializer KTy, IsConvertibleToJsonSerializer OTy> JsonSerializer(std::unordered_map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsConvertibleToJsonSerializer KTy, IsConvertibleToJsonSerializer OTy> JsonSerializer& operator=(std::map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = std::move(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonSerializer KTy, IsConvertibleToJsonSerializer OTy> JsonSerializer(std::map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonSerializer KTy, IsConvertibleToJsonSerializer OTy> JsonSerializer& operator=(std::map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::Object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = value;
			}
			return *this;
		}

		template<IsConvertibleToJsonSerializer KTy, IsConvertibleToJsonSerializer OTy> JsonSerializer(std::map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsEnum Ty> JsonSerializer& operator=(Ty data) noexcept {
			this->jsonValue.numberUint = static_cast<uint64_t>(data);
			this->type = JsonType::Uint64;
			return *this;
		}

		template<IsEnum Ty> JsonSerializer(Ty data) noexcept {
			*this = data;
		}

		JsonSerializer& operator=(ErrorCode data);

		JsonSerializer(ErrorCode data);

		JsonSerializer& operator=(ObjectType&& data) noexcept;

		JsonSerializer(ObjectType&& data) noexcept;

		JsonSerializer& operator=(const ObjectType& data) noexcept;

		JsonSerializer(const ObjectType& data) noexcept;

		JsonSerializer& operator=(JsonSerializer&& data) noexcept;

		JsonSerializer(JsonSerializer&& data) noexcept;

		JsonSerializer& operator=(const JsonSerializer& data) noexcept;

		JsonSerializer(const JsonSerializer& data) noexcept;

		JsonSerializer& operator=(Jsonifier&& data) noexcept;

		JsonSerializer(Jsonifier&& data) noexcept;

		JsonSerializer& operator=(const Jsonifier& data) noexcept;

		JsonSerializer(const Jsonifier& data) noexcept;

		operator std::string_view() noexcept;

		void refreshString(JsonifierSerializeType OpCode);

		bool contains(std::string& key);

		JsonSerializer& operator=(EnumConverter&& data) noexcept;
		JsonSerializer(EnumConverter&& data) noexcept;

		JsonSerializer& operator=(const EnumConverter& data) noexcept;
		JsonSerializer(const EnumConverter& data) noexcept;

		JsonSerializer& operator=(std::string&& data) noexcept;
		JsonSerializer(std::string&& data) noexcept;

		JsonSerializer& operator=(const std::string& data) noexcept;
		JsonSerializer(const std::string& data) noexcept;

		JsonSerializer& operator=(std::string_view&& data) noexcept;
		JsonSerializer(std::string_view&& data) noexcept;

		JsonSerializer& operator=(std::string_view& data) noexcept;
		JsonSerializer(std::string_view& data) noexcept;

		JsonSerializer& operator=(const char* data) noexcept;
		JsonSerializer(const char* data) noexcept;

		JsonSerializer& operator=(double data) noexcept;
		JsonSerializer(double data) noexcept;

		JsonSerializer& operator=(float data) noexcept;
		JsonSerializer(float data) noexcept;

		JsonSerializer& operator=(uint64_t data) noexcept;
		JsonSerializer(uint64_t data) noexcept;

		JsonSerializer& operator=(uint32_t data) noexcept;
		JsonSerializer(uint32_t data) noexcept;

		JsonSerializer& operator=(uint16_t data) noexcept;
		JsonSerializer(uint16_t data) noexcept;

		JsonSerializer& operator=(uint8_t data) noexcept;
		JsonSerializer(uint8_t data) noexcept;

		JsonSerializer& operator=(int64_t data) noexcept;
		JsonSerializer(int64_t data) noexcept;

		JsonSerializer& operator=(int32_t data) noexcept;
		JsonSerializer(int32_t data) noexcept;

		JsonSerializer& operator=(int16_t data) noexcept;
		JsonSerializer(int16_t data) noexcept;

		JsonSerializer& operator=(int8_t data) noexcept;
		JsonSerializer(int8_t data) noexcept;

		JsonSerializer& operator=(bool data) noexcept;
		JsonSerializer(bool data) noexcept;

		JsonSerializer& operator=(JsonType TypeNew) noexcept;
		JsonSerializer(JsonType type) noexcept;

		JsonSerializer& operator=(std::nullptr_t) noexcept;
		JsonSerializer(std::nullptr_t data) noexcept;

		JsonSerializer& operator[](typename ObjectType::key_type key);

		JsonSerializer& operator[](uint64_t index);

		template<typename Ty> Ty getValue() {
			return Ty{};
		}

		JsonType getType() noexcept;

		JsonSerializer& emplaceBack(JsonSerializer&& data) noexcept;
		JsonSerializer& emplaceBack(JsonSerializer& data) noexcept;

		~JsonSerializer() noexcept;

	  protected:
		JsonType type{ JsonType::Null };
		JsonValue jsonValue{};
		std::string string{};

		void serializeJsonToEtfString(const Jsonifier* dataToParse);

		void serializeJsonToJsonString(const Jsonifier* dataToParse);

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

		friend bool operator==(const JsonSerializer& lhs, const JsonSerializer& rhs);
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

		inline operator __m128i() {
			return this->value;
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
			//std::cout << "STRUCTURAL INDEX COUNT: " << returnValue << std::endl;
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
		uint32_t tape_index{};
		uint32_t count{};
	};

	class SimdJsonValue {
	  public:

		inline SimdJsonValue(const char* stringNew, size_t tapeLength, size_t capacity)
			: stringView(stringNew){
			if (tapeLength == 0) {
				throw DCAException{ "Failed to parse as the string size is 0." };
			}
			this->stringLength = tapeLength;
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
			//std::cout << "CURRENT STRING VIEW (SECTION): " << this->stringView << std::endl;
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

		inline ~SimdJsonValue() noexcept {}

		inline const char* getStringView() {
			//std::cout << "CURRENT STRING VIEW: " << this->stringView << std::endl;
			return this->stringView;
		}

		inline uint64_t* getAndIncrementNextStructural() {
			return this->nextStructural++;
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

	  protected:
		std::unique_ptr<bool[]> isArray{};
		uint64_t* nextStructural{};
		uint32_t maxDepth{ 500 };
		size_t tapeLength{ 0 };
		SimdTape jsonRawTape{};
		size_t stringLength{};
		uint32_t depth{ 0 };
		const char* stringView{};
	};

	enum class TapeType : int8_t {
		ROOT = 'r',
		START_ARRAY = '[',
		START_OBJECT = '{',
		END_ARRAY = ']',
		END_OBJECT = '}',
		STRING = '"',
		INT64 = 'l',
		UINT64 = 'u',
		DOUBLE = 'd',
		TRUE_VALUE = 't',
		FALSE_VALUE = 'f',
		NULL_VALUE = 'n'
	};// enum class TapeType

	class JsonIterator {
	  public:
		const uint8_t* buf;
		uint64_t* next_structural;
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
   * - visit_key(uint8_t *key) - when a key in an object field is encountered. key is
   *                                   guaranteed to point at the first quote of the string (`"key"`).
   * - visit_primitive(uint8_t *value) - when a value is a string, number, boolean or null.
   * - visit_root_primitive(iter, uint8_t *value) - when the top-level value is a string, number, boolean or null.
   *
   * - increment_count(iter) - each time a value is found in an array or object.
   */
		template<bool STREAMING, typename V>  inline ErrorCode walk_document(V& visitor) noexcept;

		/**
   * Create an iterator capable of walking a JSON document.
   *
   * The document must have already passed through stage 1.
   */
		inline JsonIterator(SimdJsonValue& _dom_parser, size_t start_structural_index);

		/**
   * Look at the next token.
   *
   * Tokens can be strings, numbers, booleans, null, or operators (`[{]},:`)).
   *
   * They may include invalid JSON as well (such as `1.2.3` or `ture`).
   */
		inline const uint8_t* peek() const noexcept;
		/**
   * Advance to the next token.
   *
   * Tokens can be strings, numbers, booleans, null, or operators (`[{]},:`)).
   *
   * They may include invalid JSON as well (such as `1.2.3` or `ture`).
   */
		inline const uint8_t* advance() noexcept;
		/**
   * Get the remaining length of the document, from the start of the current token.
   */
		inline size_t remaining_len() const noexcept;
		/**
   * Check if we are at the end of the document.
   *
   * If this is true, there are no more tokens.
   */
		inline bool at_eof() const noexcept;
		/**
   * Check if we are at the beginning of the document.
   */
		inline bool at_beginning() const noexcept;
		inline uint8_t last_structural() const noexcept;

		/**
   * Log that a value has been found.
   *
   * Set LOG_ENABLED=true in logger.h to see logging.
   */
		inline void log_value(const char* type) const noexcept;
		/**
   * Log the start of a multipart value.
   *
   * Set LOG_ENABLED=true in logger.h to see logging.
   */
		inline void log_start_value(const char* type) const noexcept;
		/**
   * Log the end of a multipart value.
   *
   * Set LOG_ENABLED=true in logger.h to see logging.
   */
		inline void log_end_value(const char* type) const noexcept;
		/**
   * Log an error.
   *
   * Set LOG_ENABLED=true in logger.h to see logging.
   */
		inline void log_error(const char* error) const noexcept;

		template<typename V>  inline ErrorCode visit_root_primitive(V& visitor, const uint8_t* value) noexcept;
		template<typename V>  inline ErrorCode visit_primitive(V& visitor, const uint8_t* value) noexcept;
	};

	template<bool STREAMING, typename V>  inline ErrorCode JsonIterator::walk_document(V& visitor) noexcept {
		
		//
		// Start the document
		//
		if (at_eof()) {
			return Empty;
		}
		log_start_value("document");

		//
		// Read first value
		//
		{
			auto value = advance();

			switch (*value) {
				case '{':
					if (*peek() == '}') {
						advance();
						log_value("empty object");
						visitor.visit_empty_object(*this);
						break;
					}
					goto object_begin;
				case '[':
					if (*peek() == ']') {
						advance();
						log_value("empty array");
						visitor.visit_empty_array(*this);
						break;
					}
					goto array_begin;
				default:
					visitor.visit_root_primitive(*this, value);
					break;
			}
		}
		goto document_end;

	//
	// Object parser states
	//
	object_begin:
		depth++;
		if (depth >= dom_parser.getMaxDepth()) {
			return DepthError;
		}
		dom_parser.getIsArray()[depth] = false;
		visitor.visit_object_start(*this);

		{
			auto key = advance();
			if (*key != '"') {
				log_error("Object does not start with a key");
				return TapeError;
			}
			visitor.increment_count(*this);
			visitor.visit_key(*this, key);
		}

	object_field:
		if (*advance() != ':') {
			return TapeError;
		}
		{
			auto value = advance();
			switch (*value) {
				case '{':
					if (*peek() == '}') {
						advance();
						visitor.visit_empty_object(*this);
						break;
					}
					goto object_begin;
				case '[':
					if (*peek() == ']') {
						advance();
						visitor.visit_empty_array(*this);
						break;
					}
					goto array_begin;
				default:
					visitor.visit_primitive(*this, value);
					break;
			}
		}

	object_continue:
		switch (*advance()) {
			case ',':
				visitor.increment_count(*this);
				{
					auto key = advance();
					if (*key != '"') {
						log_error("Key string missing at beginning of field in object");
						return TapeError;
					}
					visitor.visit_key(*this, key);
				}
				goto object_field;
			case '}':
				log_end_value("object");
				visitor.visit_object_end(*this);
				goto scope_end;
			default:
				log_error("No comma between object fields");
				return TapeError;
		}

	scope_end:
		depth--;
		if (depth == 0) {
			goto document_end;
		}
		if (dom_parser.getIsArray()[depth]) {
			goto array_continue;
		}
		goto object_continue;

	//
	// Array parser states
	//
	array_begin:
		log_start_value("array");
		depth++;
		if (depth >= dom_parser.getMaxDepth()) {
			log_error("Exceeded max depth!");
			return DepthError;
		}
		dom_parser.getIsArray()[depth] = true;
		visitor.visit_array_start(*this);
		visitor.increment_count(*this);

	array_value : {
		auto value = advance();
		switch (*value) {
			case '{':
				if (*peek() == '}') {
					advance();
					log_value("empty object");
					visitor.visit_empty_object(*this);
					break;
				}
				goto object_begin;
			case '[':
				if (*peek() == ']') {
					advance();
					log_value("empty array");
					visitor.visit_empty_array(*this);
					break;
				}
				goto array_begin;
			default:
				visitor.visit_primitive(*this, value);
				break;
		}
	}

	array_continue:
		switch (*advance()) {
			case ',':
				visitor.increment_count(*this);
				goto array_value;
			case ']':
				log_end_value("array");
				visitor.visit_array_end(*this);
				goto scope_end;
			default:
				log_error("Missing comma between array values");
				return TapeError;
		}

	document_end:
		log_end_value("document");
		visitor.visit_document_end(*this);

		*dom_parser.getNextStructural() = uint32_t(next_structural - &dom_parser.getStructuralIndexes()[0]);

		// If we didn't make it to the end, it's an error
		if (!STREAMING && *dom_parser.getNextStructural() != dom_parser.getTapeLength()) {
			log_error("More than one JSON value at the root of the document, or extra characters at the end of the JSON!");
			return TapeError;
		}

		return Success;

	}// walk_document()

	inline JsonIterator::JsonIterator(SimdJsonValue& _dom_parser, size_t start_structural_index)
		: buf{ reinterpret_cast<const uint8_t*>(_dom_parser.getStringView()) }, next_structural{ &_dom_parser.getStructuralIndexes()[start_structural_index] }, dom_parser{ _dom_parser } {
	}

	inline const uint8_t* JsonIterator::peek() const noexcept {
		//std::cout << "CURRENT PEEKED VALUE: " << buf[*(next_structural)] << std::endl;
		return &buf[*(next_structural)];
	}

	inline const uint8_t* JsonIterator::advance() noexcept {
		auto returnValue = &buf[*(next_structural++)];
		//std::cout << "CURRENT ADVANCED VALUE: " << *returnValue << std::endl;
		return returnValue;
	}

	inline size_t JsonIterator::remaining_len() const noexcept {
		return dom_parser.getTapeLength() - *(next_structural - 1);
	}

	inline bool JsonIterator::at_eof() const noexcept {
		//std::cout << "THE TAPE LENGTH: " << dom_parser.getTapeLength() << std::endl;
		//std::cout << "THE TAPE LENGTH: " << +dom_parser.getStructuralIndexes()[dom_parser.getTapeLength() - 1] << std::endl;
		return next_structural == &dom_parser.getStructuralIndexes()[dom_parser.getTapeLength() - 1];
	}

	inline bool JsonIterator::at_beginning() const noexcept {
		return next_structural == dom_parser.getStructuralIndexes();
	}

	inline uint8_t JsonIterator::last_structural() const noexcept {
		return buf[dom_parser.getStructuralIndexes()[dom_parser.getTapeLength() - 1]];
	}

	inline void JsonIterator::log_value(const char* type) const noexcept {
		//logger::log_line(*this, "", type, "");
	}

	inline void JsonIterator::log_start_value(const char* type) const noexcept {
		//logger::log_line(*this, "+", type, "");
		//if (logger::LOG_ENABLED) {
				//logger::log_depth++;
		//}
	}

	inline void JsonIterator::log_end_value(const char* type) const noexcept {
		//if (logger::LOG_ENABLED) {
		//			logger::log_depth--;
		//}
		//logger::log_line(*this, "-", type, "");
	}

	inline void JsonIterator::log_error(const char* error) const noexcept {
		//logger::log_line(*this, "", "ERROR", error);
	}

	template<typename V>
	 inline ErrorCode JsonIterator::visit_root_primitive(V& visitor, const uint8_t* value) noexcept {
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
				log_error("Document starts with a non-value character");
				return TapeError;
		}
	}
	template<typename V>  inline ErrorCode JsonIterator::visit_primitive(V& visitor, const uint8_t* value) noexcept {
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
				log_error("Non-value found when value was expected!");
				return TapeError;
		}
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
		return uint32_t{};
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
		/** The next place to write to tape */
		uint64_t* next_tape_loc;
		inline void append_s64(int64_t value) noexcept;
		inline void append_u64(uint64_t value) noexcept;
		inline void append_double(double value) noexcept;
		inline void append(uint64_t val, TapeType t) noexcept;
		inline void skip() noexcept;
		inline void skip_large_integer() noexcept;
		inline void skip_double() noexcept;
		inline static void write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept;

	  private:
		template<typename T> inline void append2(uint64_t val, T val2, TapeType t) noexcept;
	};

	inline void TapeWriter::append_s64(int64_t value) noexcept {
		append2(0, value, TapeType::INT64);
	}

	inline void TapeWriter::append_u64(uint64_t value) noexcept {
		append(0, TapeType::UINT64);
		*next_tape_loc = value;
		next_tape_loc++;
	}

	inline void TapeWriter::append_double(double value) noexcept {
		append2(0, value, TapeType::DOUBLE);
	}

	inline void TapeWriter::skip() noexcept {
		next_tape_loc++;
	}

	inline void TapeWriter::skip_large_integer() noexcept {
		next_tape_loc += 2;
	}

	inline void TapeWriter::skip_double() noexcept {
		next_tape_loc += 2;
	}

	inline void TapeWriter::append(uint64_t val, TapeType t) noexcept{
		//std::cout << "VALUE 01: " << (val) << std::endl;
		//std::cout << "VALUE 02: " << (val << 56) << std::endl;
		//std::cout << "VALUE 03: " << (val | (uint64_t(char(t) & 0xffffffff) << 56)) << std::endl;
		*next_tape_loc = val | ((uint64_t(char(t))) << 56);
		//std::cout << "CURRENT TAPE LOC: " << *next_tape_loc << std::endl;
		//std::cout << "CURRENT TAPE LOC: " << ((*next_tape_loc) & 0x00ffffff) << std::endl;
		next_tape_loc++;
	}

	template<typename T> inline void TapeWriter::append2(uint64_t val, T val2, TapeType t) noexcept {
		append(val, t);
		static_assert(sizeof(val2) == sizeof(*next_tape_loc), "Type is not 64 bits!");
		memcpy(next_tape_loc, &val2, sizeof(val2));
		next_tape_loc++;
	}

	inline void TapeWriter::write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept {
		tape_loc = val | ((uint64_t(char(t))) << 56);
	}

	struct TapeBuilder {
		template<bool STREAMING> static inline ErrorCode parse_document(SimdJsonValue& dom_parser, Jsonifier& doc) noexcept;

		/** Called when a non-empty document starts. */
		inline ErrorCode visit_document_start(JsonIterator& iter) noexcept;
		/** Called when a non-empty document ends without error. */
		inline ErrorCode visit_document_end(JsonIterator& iter) noexcept;

		/** Called when a non-empty array starts. */
		inline ErrorCode visit_array_start(JsonIterator& iter) noexcept;
		/** Called when a non-empty array ends. */
		inline ErrorCode visit_array_end(JsonIterator& iter) noexcept;
		/** Called when an empty array is found. */
		inline ErrorCode visit_empty_array(JsonIterator& iter) noexcept;

		/** Called when a non-empty object starts. */
		inline ErrorCode visit_object_start(JsonIterator& iter) noexcept;
		/**
   * Called when a key in a field is encountered.
   *
   * primitive, visit_object_start, visit_empty_object, visit_array_start, or visit_empty_array
   * will be called after this with the field value.
   */
		inline ErrorCode visit_key(JsonIterator& iter, const uint8_t* key) noexcept;
		/** Called when a non-empty object ends. */
		 inline ErrorCode visit_object_end(JsonIterator& iter) noexcept;
		/** Called when an empty object is found. */
		 inline ErrorCode visit_empty_object(JsonIterator& iter) noexcept;

		/**
   * Called when a string, number, boolean or null is found.
   */
		 inline ErrorCode visit_primitive(JsonIterator& iter, const uint8_t* value) noexcept;
		/**
   * Called when a string, number, boolean or null is found at the top level of a document (i.e.
   * when there is no array or object and the entire document is a single string, number, boolean or
   * null.
   *
   * This is separate from primitive() because simdjson's normal primitive parsing routines assume
   * there is at least one more token after the value, which is only true in an array or object.
   */
		 inline ErrorCode visit_root_primitive(JsonIterator& iter, const uint8_t* value) noexcept;

		 inline ErrorCode visit_string(JsonIterator& iter, const uint8_t* value, bool key = false) noexcept;
		 inline ErrorCode visit_number(JsonIterator& iter, const uint8_t* value) noexcept;
		 inline ErrorCode visit_true_atom(JsonIterator& iter, const uint8_t* value) noexcept;
		 inline ErrorCode visit_false_atom(JsonIterator& iter, const uint8_t* value) noexcept;
		 inline ErrorCode visit_null_atom(JsonIterator& iter, const uint8_t* value) noexcept;

		 inline ErrorCode visit_root_string(JsonIterator& iter, const uint8_t* value) noexcept;
		 inline ErrorCode visit_root_number(JsonIterator& iter, const uint8_t* value) noexcept;
		 inline ErrorCode visit_root_true_atom(JsonIterator& iter, const uint8_t* value) noexcept;
		 inline ErrorCode visit_root_false_atom(JsonIterator& iter, const uint8_t* value) noexcept;
		 inline ErrorCode visit_root_null_atom(JsonIterator& iter, const uint8_t* value) noexcept;

		/** Called each time a new field or element in an array or object is found. */
		 inline ErrorCode increment_count(JsonIterator& iter) noexcept;

		/** Next location to write to tape */
		 TapeWriter tape;

	  private:
		/** Next write location in the string buf for stage 2 parsing */
		const uint8_t* current_string_buf_loc;

		inline TapeBuilder(SimdJsonValue& doc) noexcept;

		inline uint32_t next_tape_index(JsonIterator& iter) const noexcept;
		inline void start_container(JsonIterator& iter) noexcept;
		inline ErrorCode end_container(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline ErrorCode empty_container(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline const uint8_t* on_start_string(JsonIterator& iter) noexcept;
		inline void on_end_string(const uint8_t* dst) noexcept;
	};// class TapeBuilder

	template<bool STREAMING>
	 inline ErrorCode TapeBuilder::parse_document(SimdJsonValue& dom_parser, Jsonifier& doc) noexcept {
		dom_parser.doc = &doc;
		JsonIterator iter(dom_parser, STREAMING ? *dom_parser.getNextStructural() : 0);
		TapeBuilder builder(dom_parser);
		return iter.walk_document<STREAMING>(builder);
	}

	 inline ErrorCode TapeBuilder::visit_root_primitive(JsonIterator& iter, const uint8_t* value) noexcept {
		return iter.visit_root_primitive(*this, value);
	}
	 inline ErrorCode TapeBuilder::visit_primitive(JsonIterator& iter, const uint8_t* value) noexcept {
		return iter.visit_primitive(*this, value);
	}
	 inline ErrorCode TapeBuilder::visit_empty_object(JsonIterator& iter) noexcept {
		return empty_container(iter, TapeType::START_OBJECT, TapeType::END_OBJECT);
	}
	 inline ErrorCode TapeBuilder::visit_empty_array(JsonIterator& iter) noexcept {
		return empty_container(iter, TapeType::START_ARRAY, TapeType::END_ARRAY);
	}

	 inline ErrorCode TapeBuilder::visit_document_start(JsonIterator& iter) noexcept {
		start_container(iter);
		return ErrorCode::Success;
	}
	 inline ErrorCode TapeBuilder::visit_object_start(JsonIterator& iter) noexcept {
		start_container(iter);
		return ErrorCode::Success;
	}
	 inline ErrorCode TapeBuilder::visit_array_start(JsonIterator& iter) noexcept {
		start_container(iter);
		return ErrorCode::Success;
	}

	 inline ErrorCode TapeBuilder::visit_object_end(JsonIterator& iter) noexcept {
		return end_container(iter, TapeType::START_OBJECT, TapeType::END_OBJECT);
	}
	 inline ErrorCode TapeBuilder::visit_array_end(JsonIterator& iter) noexcept {
		return end_container(iter, TapeType::START_ARRAY, TapeType::END_ARRAY);
	}
	 inline ErrorCode TapeBuilder::visit_document_end(JsonIterator& iter) noexcept {
		constexpr uint32_t start_tape_index = 0;
		tape.append(start_tape_index, TapeType::ROOT);
		TapeWriter::write(iter.dom_parser.getStructuralIndexes()[start_tape_index], next_tape_index(iter), TapeType::ROOT);
		return ErrorCode::Success;
	}
	 inline ErrorCode TapeBuilder::visit_key(JsonIterator& iter, const uint8_t* key) noexcept {
		return visit_string(iter, key, true);
	}

	 inline ErrorCode TapeBuilder::increment_count(JsonIterator& iter) noexcept {
		iter.dom_parser.openContainers[iter.depth].count++;// we have a key value pair in the object at parser.dom_parser.depth - 1
		 return ErrorCode::Success;
	}

	inline TapeBuilder::TapeBuilder(SimdJsonValue& doc) noexcept
		: tape{ doc.getStructuralIndexes() }, current_string_buf_loc{ reinterpret_cast<const uint8_t*>(doc.getStringView()) } {
	}

	 inline ErrorCode TapeBuilder::visit_string(JsonIterator& iter, const uint8_t* value, bool key) noexcept {
		iter.log_value(key ? "key" : "string");
		 const uint8_t* dst = on_start_string(iter);
		dst = value + 1;
		if (dst == nullptr) {
			iter.log_error("Invalid escape in string");
			return ErrorCode::StringError;
		}
		on_end_string(dst);
		return ErrorCode::Success;
	}

	 inline ErrorCode TapeBuilder::visit_root_string(JsonIterator& iter, const uint8_t* value) noexcept {
		return visit_string(iter, value);
	}

	 inline ErrorCode TapeBuilder::visit_number(JsonIterator& iter, const uint8_t* value) noexcept {
		iter.log_value("number");
		 return ErrorCode::Success;
	}

	 inline ErrorCode TapeBuilder::visit_root_number(JsonIterator& iter, const uint8_t* value) noexcept {
		//
		// We need to make a copy to make sure that the string is space terminated.
		// This is not about padding the input, which should already padded up
		// to tapeLength + SIMDJSON_PADDING. However, we have no control at this stage
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
			return MemAlloc;
		}
		std::memcpy(copy.get(), value, iter.remaining_len());
		std::memset(copy.get() + iter.remaining_len(), ' ', 256);
		ErrorCode error = visit_number(iter, copy.get());
		return error;
	}

	 inline ErrorCode TapeBuilder::visit_true_atom(JsonIterator& iter, const uint8_t* value) noexcept {
		iter.log_value("true");
		if (!isValidTrueAtom(reinterpret_cast<const char*>(value))) {
			return TAtomError;
		}
		tape.append(0, TapeType::TRUE_VALUE);
		return ErrorCode::Success;
	}

	 inline ErrorCode TapeBuilder::visit_root_true_atom(JsonIterator& iter, const uint8_t* value) noexcept {
		iter.log_value("true");
		if (!isValidTrueAtom(reinterpret_cast<const char*>(value))) {
			return TAtomError;
		}
		tape.append(0, TapeType::TRUE_VALUE);
		return ErrorCode::Success;
	}

	 inline ErrorCode TapeBuilder::visit_false_atom(JsonIterator& iter, const uint8_t* value) noexcept {
		iter.log_value("false");
		if (!isValidFalseAtom(reinterpret_cast<const char*>(value))) {
			return FAtomError;
		}
		tape.append(0, TapeType::FALSE_VALUE);
		return ErrorCode::Success;
	}

	 inline ErrorCode TapeBuilder::visit_root_false_atom(JsonIterator& iter, const uint8_t* value) noexcept {
		iter.log_value("false");
		if (!isValidFalseAtom(reinterpret_cast<const char*>(value))) {
			return FAtomError;
		}
		tape.append(0, TapeType::FALSE_VALUE);
		return ErrorCode::Success;
	}

	 inline ErrorCode TapeBuilder::visit_null_atom(JsonIterator& iter, const uint8_t* value) noexcept {
		iter.log_value("null");
		if (!isValidNullAtom(reinterpret_cast<const char*>(value))) {
			return NAtomError;
		}
		tape.append(0, TapeType::NULL_VALUE);
		return ErrorCode::Success;
	}

	 inline ErrorCode TapeBuilder::visit_root_null_atom(JsonIterator& iter, const uint8_t* value) noexcept {
		iter.log_value("null");
		if (!isValidNullAtom(reinterpret_cast<const char*>(value))) {
			return NAtomError;
		}
		tape.append(0, TapeType::NULL_VALUE);
		return ErrorCode::Success;
	}

	// private:

	inline uint32_t TapeBuilder::next_tape_index(JsonIterator& iter) const noexcept {
		return uint32_t(tape.next_tape_loc - iter.dom_parser.getStructuralIndexes());
	}

	 inline ErrorCode TapeBuilder::empty_container(JsonIterator& iter, TapeType start,
		TapeType end) noexcept {
		auto start_index = next_tape_index(iter);
		tape.append(start_index + 2, start);
		tape.append(start_index, end);
		return ErrorCode::Success;
	}

	inline void TapeBuilder::start_container(JsonIterator& iter) noexcept {
		iter.dom_parser.openContainers[iter.depth].tape_index = next_tape_index(iter);
		iter.dom_parser.openContainers[iter.depth].count = 0;
		tape.skip();// We don't actually *write* the start element until the end.
	}

	 inline ErrorCode TapeBuilder::end_container(JsonIterator& iter, TapeType start,
		TapeType end) noexcept {
		// Write the ending tape element, pointing at the start location
		const uint32_t start_tape_index = iter.dom_parser.openContainers[iter.depth].tape_index;
		tape.append(start_tape_index, end);
		// Write the start tape element, pointing at the end location (and including count)
		// count can overflow if it exceeds 24 bits... so we saturate
		// the convention being that a cnt of 0xffffff or more is undetermined in value (>=  0xffffff).
		const uint32_t count = iter.dom_parser.openContainers[iter.depth].count;
		const uint32_t cntsat = count > 0xFFFFFF ? 0xFFFFFF : count;
		TapeWriter::write(iter.dom_parser.getStructuralIndexes()[start_tape_index], next_tape_index(iter) | (uint64_t(cntsat) << 32), start);
		return ErrorCode::Success;
	}

	inline const uint8_t* TapeBuilder::on_start_string(JsonIterator& iter) noexcept {
		// we advance the point, accounting for the fact that we have a NULL termination
		tape.append(current_string_buf_loc - reinterpret_cast<const uint8_t*>(iter.dom_parser.getStringView()), TapeType::STRING);
		return current_string_buf_loc + sizeof(uint32_t);
	}

	inline void TapeBuilder::on_end_string(const uint8_t* dst) noexcept {
		uint32_t str_length = uint32_t(dst - (current_string_buf_loc + sizeof(uint32_t)));
		// TODO check for overflow in case someone has a crazy string (>=4GB?)
		// But only add the overflow check when the document itself exceeds 4GB
		// Currently unneeded because we refuse to parse docs larger or equal to 4GB.
		//memcpy(current_string_buf_loc, &str_length, sizeof(uint32_t));
		// NULL termination is still handy if you expect all your strings to
		// be NULL terminated? It comes at a small cost
		//*dst = 0;
		current_string_buf_loc = dst + 1;
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
		inline uint32_t getCurrentElementSize() noexcept;
		inline uint32_t currentStringIndex() noexcept;
		inline std::string_view visitKey() noexcept;
		inline Jsonifier visitEmptyObject();
		inline Jsonifier visitEmptyArray();
		inline Jsonifier startDocument();
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
		return visitString();
	}

	inline std::string_view JsonConstructor::visitString() noexcept {
		return std::string_view{ &this->masterParser->getStringView()[this->currentStringIndex()] + 1, this->getCurrentElementSize() - 2 };
	}

	inline uint32_t JsonConstructor::getCurrentElementSize() noexcept {
		return this->nextStringIndex() - this->currentStringIndex();
	}

	inline Jsonifier JsonConstructor::visitNumber(const char value) noexcept {
		return stoull(std::string{ &this->masterParser->getStringView()[this->currentStringIndex()], this->getCurrentElementSize() });
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

	inline Jsonifier JsonConstructor::startDocument() {
		Jsonifier jsonData{};
		return jsonData;
		if (this->atEof()) {
			
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
		this->currentKey = this->visitKey();
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
				std::string currentKey = std::move(this->currentKey);
				jsonData[currentKey] = this->objectBegin(std::move(jsonDataNew));
				return std::move(jsonData);
			}
			case '[': {
				Jsonifier jsonDataNew{};
				if (this->peek() == ']') {
					this->advance();
					return this->visitEmptyArray();
				}
				std::string currentKey = std::move(this->currentKey);
				jsonData[currentKey] = this->arrayBegin(std::move(jsonDataNew));
				return std::move(jsonData);
			}

			default:
				std::string currentKey = std::move(this->currentKey);
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
				this->currentKey = this->visitKey();
				return this->objectField(std::move(jsonData));
			}
			case '}':
				return this->scopeEnd(std::move(jsonData));
			default:
				return ErrorCode::TapeError;
		}
	}

	inline Jsonifier JsonConstructor::arrayBegin(Jsonifier jsonData) {
		this->depth++;
		this->masterParser->getIsArray()[this->depth] = true;
		if (this->depth >= this->masterParser->getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		return this->arrayValue(std::move(jsonData));
	}

	inline Jsonifier JsonConstructor::arrayValue(Jsonifier jsonData) {
		auto value = this->advance();
		switch (value) {
			case '{': {
				Jsonifier jsonDataNew{};
				if (this->peek() == '}') {
					this->advance();
					jsonData.emplaceBack(this->visitEmptyObject());
					return std::move(jsonData);
				}
				jsonData.emplaceBack(this->objectBegin(std::move(jsonDataNew)));
				return std::move(jsonData);
			}
			case '[': {
				Jsonifier jsonDataNew{};
				if (this->peek() == ']') {
					this->advance();
					jsonData.emplaceBack(this->visitEmptyArray());
					return std::move(jsonData);
				}
				jsonData.emplaceBack(this->arrayBegin(std::move(jsonDataNew)));
				return std::move(jsonData);
			}
			default:
				jsonData.emplaceBack(this->visitPrimitive(value));
				return this->arrayContinue(std::move(jsonData));
		}
	}

	inline Jsonifier JsonConstructor::arrayContinue(Jsonifier jsonData) {
		switch (this->advance()) {
			case ',': {
				return this->arrayValue(std::move(jsonData));
			}
			case ']': {
				return this->scopeEnd(std::move(jsonData));
			}
			default:
				return ErrorCode::TapeError;
		}
	}

	inline Jsonifier JsonConstructor::scopeEnd(Jsonifier jsonData) {
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
		return std::move(jsonData);
	}

	inline JsonConstructor::JsonConstructor(SimdJsonValue& _dom_parser) noexcept : masterParser{ &_dom_parser } {};

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

	Jsonifier SimdJsonValue::getJsonData() {
		//auto jsonData = JsonConstructor{ *this }.startDocument();
		Jsonifier jsonData{};
		//std::cout << "THE VALUE (REAL): " << this->stringView << std::endl;
		SimdJsonValue value{ this->stringView, this->tapeLength, 0 };
		//std::cout << "THE VALUE: " << ( int32_t )TapeBuilder::parse_document<false>(value, jsonData) << std::endl;
		TapeBuilder::parse_document<false>(value, jsonData);
		for (size_t x = 0; x < value.tapeLength; ++x) {
			//std::cout << "THE CURRENT VALUE: " << ( char )this->stringView[*value.jsonRawTape[x]] << std::endl;
			std::cout << "THE CURRENT VALUE 01: " << ((*value.jsonRawTape[x]) & 0x00ffffff) << std::endl;
			std::cout << "THE CURRENT VALUE 02: " << (( char )(value.stringView[((*value.jsonRawTape[x]) & 0x00ffffff)])) << std::endl;
		}
		//std::cout << "THE VALUE (FINAL): " << this->stringView << std::endl;
		//jsonData.refreshString(JsonifierSerializeType::Json);
		//std::cout << "THE DATA: " << jsonData.operator std::basic_string_view<char, std::char_traits<char>>() << std::endl;
		return jsonData;
	}

};