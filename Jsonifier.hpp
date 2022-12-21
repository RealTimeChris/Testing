#pragma once

#include "NumberParsingUtils.hpp"
#include "StringParsingUtils.hpp"

namespace Jsonifier {

	struct JsonifierException : public std::runtime_error, std::string {
		inline JsonifierException(const std::string&, std::source_location = std::source_location::current()) noexcept;
	};

	constexpr int64_t JSON_VALUE_MASK = 0x00FFFFFFFFFFFFFF;
	constexpr uint32_t JSON_COUNT_MASK = 0xFFFFFF;

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
			this->maxNumberOfMs->store(data.maxNumberOfMs->load());
			this->startTime->store(data.startTime->load());
			return *this;
		}

		StopWatch(const StopWatch<TTy>& data) {
			*this = data;
		}

		StopWatch(TTy maxNumberOfMsNew) {
			this->maxNumberOfMs->store(maxNumberOfMsNew);
			this->startTime->store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
		}

		TTy totalTimePassed() {
			TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
			TTy elapsedTime = currentTime - this->startTime->load();
			return elapsedTime;
		}

		TTy getTotalWaitTime() {
			return this->maxNumberOfMs->load();
		}

		bool hasTimePassed() {
			TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
			TTy elapsedTime = currentTime - this->startTime->load();
			if (elapsedTime >= this->maxNumberOfMs->load()) {
				return true;
			} else {
				return false;
			}
		}

		void resetTimer() {
			this->startTime->store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
		}

	  protected:
		std::unique_ptr<std::atomic<TTy>> maxNumberOfMs{ std::make_unique<std::atomic<TTy>>(TTy{ 0 }) };
		std::unique_ptr<std::atomic<TTy>> startTime{ std::make_unique<std::atomic<TTy>>(TTy{ 0 }) };
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

	enum class JsonType : uint8_t { Object = 1, Array = 2, String = 3, Float = 4, Uint64 = 5, Int64 = 6, Bool = 7, Null = 8 };

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

		Jsonifier& operator=(Jsonifier&& data) noexcept;

		Jsonifier& operator=(const Jsonifier& data) noexcept;

		Jsonifier(const Jsonifier& data) noexcept;

		operator std::string&&() noexcept;

		operator std::string() noexcept;

		void refreshString(JsonifierSerializeType OpCode);

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

		void writeString(const char* data, size_t length);

		void writeCharacter(const char Char);

		void appendBinaryExt(const std::string& bytes, const uint32_t sizeNew);

		void appendNewFloatExt(const double FloatValue);

		void appendListHeader(const uint32_t sizeNew);

		void appendMapHeader(const uint32_t sizeNew);

		void appendUint64(const uint64_t value);

		void appendUint32(const uint32_t value);

		void appendInt64(const int64_t value);

		void appendInt32(const int32_t value);

		void appendUint8(const uint8_t value);

		void appendInt8(const int8_t value);

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

	class EscapeJsonString {
	  public:
		EscapeJsonString(std::string_view _str) noexcept : str{ _str } {
		}
		operator std::string() noexcept {
			std::stringstream s;
			s << *this;
			return s.str();
		}

	  private:
		std::string_view str;
		friend std::ostream& operator<<(std::ostream& out, const EscapeJsonString& unescaped);
	};

	inline std::ostream& operator<<(std::ostream& out, const EscapeJsonString& unescaped) {
		for (size_t i = 0; i < unescaped.str.length(); i++) {
			switch (unescaped.str[i]) {
				case '\b':
					out << "\\b";
					break;
				case '\f':
					out << "\\f";
					break;
				case '\n':
					out << "\\n";
					break;
				case '\r':
					out << "\\r";
					break;
				case '\"':
					out << "\\\"";
					break;
				case '\t':
					out << "\\t";
					break;
				case '\\':
					out << "\\\\";
					break;
				default:
					if (static_cast<unsigned char>(unescaped.str[i]) <= 0x1F) {
						std::ios::fmtflags f(out.flags());
						out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << int(unescaped.str[i]);
						out.flags(f);
					} else {
						out << unescaped.str[i];
					}
			}
		}
		return out;
	}

	inline bool dumpRawTape(std::ostream& os, uint64_t* tape, const uint8_t* stringBuffer) noexcept {
		uint32_t stringLength;
		size_t tapeIndex = 0;
		uint64_t tapeValue = tape[tapeIndex];
		uint8_t type = uint8_t(tapeValue >> 56);
		os << tapeIndex << " : " << type;
		tapeIndex++;
		size_t quantity = 0;
		if (type == 'r') {
			quantity = size_t(tapeValue & JSON_VALUE_MASK);
		} else {
			return false;
		}
		os << "\t// pointing to " << quantity << " (right after last node)\n";
		uint64_t payload;
		for (; tapeIndex < quantity; tapeIndex++) {
			os << tapeIndex << " : ";
			tapeValue = tape[tapeIndex];
			payload = tapeValue & JSON_VALUE_MASK;
			type = uint8_t(tapeValue >> 56);
			switch (type) {
				case '"':
					os << "string \"";
					std::memcpy(&stringLength, stringBuffer + payload, sizeof(uint32_t));
					os << EscapeJsonString(std::string_view(reinterpret_cast<const char*>(stringBuffer + payload + sizeof(uint32_t)), stringLength));
					os << '"';
					os << '\n';
					break;
				case 'l':
					if (tapeIndex + 1 >= quantity) {
						return false;
					}
					os << "integer " << static_cast<int64_t>(tape[++tapeIndex]) << "\n";
					break;
				case 'u':
					if (tapeIndex + 1 >= quantity) {
						return false;
					}
					os << "unsigned integer " << tape[++tapeIndex] << "\n";
					break;
				case 'd':
					os << "float ";
					if (tapeIndex + 1 >= quantity) {
						return false;
					}
					double answer;
					std::memcpy(&answer, &tape[++tapeIndex], sizeof(answer));
					os << answer << '\n';
					break;
				case 'n':
					os << "null\n";
					break;
				case 't':
					os << "true\n";
					break;
				case 'f':
					os << "false\n";
					break;
				case '{':
					os << "{\t// pointing to next tape location " << uint32_t(payload) << " (first node after the scope), "
					   << " saturated count " << ((payload >> 32) & JSON_COUNT_MASK) << "\n";
					break;
				case '}':
					os << "}\t// pointing to previous tape location " << uint32_t(payload) << " (start of the scope)\n";
					break;
				case '[':
					os << "[\t// pointing to next tape location " << uint32_t(payload) << " (first node after the scope), "
					   << " saturated count " << ((payload >> 32) & JSON_COUNT_MASK) << "\n";
					break;
				case ']':
					os << "]\t// pointing to previous tape location " << uint32_t(payload) << " (start of the scope)\n";
					break;
				case 'r':
					return false;
				default:
					return false;
			}
		}
		tapeValue = tape[tapeIndex];
		payload = tapeValue & JSON_VALUE_MASK;
		type = uint8_t(tapeValue >> 56);
		os << tapeIndex << " : " << type << "\t// pointing to " << payload << " (start root)\n";
		return true;
	}

	class JsonParser {
	  public:
		JsonParser& operator=(JsonParser&& other) noexcept {
			this->currentStructuralCount = other.currentStructuralCount;
			this->currenPositionInTape = other.currenPositionInTape;
			other.currenPositionInTape = 0;
			other.currentStructuralCount = 0;
			other.currentStringSize = 0;
			this->stringView = other.stringView;
			other.stringView = nullptr;
			this->currentStringSize = other.currentStringSize;
			this->stringBuffer = std::move(other.stringBuffer);
			this->ptrs = std::move(other.ptrs);
			return *this;
		}

		JsonParser(JsonParser&& other) noexcept {
			*this = std::move(other);
		}

		JsonParser& operator=(const JsonParser&) noexcept = delete;
		JsonParser(const JsonParser&) noexcept = delete;

		JsonParser() noexcept {};

		JsonParser(uint32_t* startingPtr, size_t currentStructuralCountNew, const char* stringViewNew) {
			this->ptrs = std::make_unique<uint64_t[]>(currentStructuralCountNew);
			this->currentStructuralCount = currentStructuralCountNew;
			for (size_t x = 0; x < currentStructuralCount; ++x) {
				this->ptrs[x] = startingPtr[x];
			}
			this->stringView = stringViewNew;
		}

		template<typename OTy> OTy getValue();

		template<> double getValue() {
			double returnValue{};
			return returnValue;
		}

		template<> int32_t getValue() {
			int32_t returnValue{};
			return returnValue;
		}

		template<> int8_t getValue() {
			int8_t returnValue{};
			return returnValue;
		}

		template<> std::string getValue() {
			std::string returnValue{ reinterpret_cast<char*>(this->stringView[(this->ptrs[this->currenPositionInTape - 1] & JSON_VALUE_MASK)]),
				static_cast<size_t>(
					(this->ptrs[this->currenPositionInTape] & JSON_VALUE_MASK) - (this->ptrs[this->currenPositionInTape - 1] & JSON_VALUE_MASK)) };
			//std::cout << "RETURN VALUE: " << returnValue << std::endl;
			return returnValue;
		}

		template<> std::vector<JsonParser> getValue() {
			std::vector<JsonParser> returnValue{};
			auto newValue = (this->ptrs[this->currenPositionInTape] >> 56);
			std::cout << "CURRENT INDEX'S VALUE: 0202 " << newValue << std::endl;
			std::cout << "CURRENT INDEX: 0202 " << (this->ptrs[this->currenPositionInTape - 1] & JSON_VALUE_MASK) << std::endl;
			return std::vector<JsonParser>{};
		}

		JsonParser& operator[](const std::string& key) {
			dumpRawTape(std::cout, this->ptrs.get(), reinterpret_cast<const uint8_t*>(this->stringView));

			auto newValue = (this->ptrs[this->currenPositionInTape++] >> 56);
			//std::cout << "CURRENT INDEX'S VALUE: " << newValue << std::endl;
			if (newValue == 'r') {
				//std::cout << "CURRENT INDEX: " << (this->ptrs[this->currenPositionInTape - 1] & JSON_VALUE_MASK) << std::endl;
				return *this;
			}
			if (newValue == '[') {
				//std::cout << "CURRENT INDEX: " << (this->ptrs[this->currenPositionInTape - 1] & JSON_COUNT_MASK) << std::endl;
				return *this;
			}
			if (newValue == '{') {
				//std::cout << "CURRENT INDEX: " << (this->ptrs[this->currenPositionInTape - 1] & JSON_COUNT_MASK) << std::endl;
				return *this;
			}
			if (newValue == '\"') {
				//std::cout << "CURRENT INDEX: " << (this->ptrs[this->currenPositionInTape - 1] & JSON_VALUE_MASK) << std::endl;
				return *this;
			}
			return *this;
		};

		const char* getStringView() {
			return this->stringView;
		}

		char* getStringViewNew() {
			return this->stringBuffer.get();
		}

		JsonParser(ErrorCode error) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " + std::to_string(( int32_t )error) };
		}

		void reset(size_t count, size_t stringSizeNew, const char* newStringView) {
			if (count > this->currentStructuralCount) {
				this->ptrs = std::make_unique<uint64_t[]>(count);
				this->currentStructuralCount = count;
			}
			if (stringSizeNew > this->currentStringSize) {
				this->stringBuffer = std::make_unique<char[]>(count);
				this->currentStringSize = stringSizeNew;
			}
			this->stringView = newStringView;
			this->currenPositionInTape = 0;
		}

		operator uint64_t*() {
			return this->ptrs.get();
		}

		void setTapeCount(size_t count) {
			this->currentStructuralCount = count;
		}

	  protected:
		std::unique_ptr<char[]> stringBuffer{ nullptr };
		std::unique_ptr<uint64_t[]> ptrs{};
		const char* stringView{ nullptr };
		size_t currentStructuralCount{};
		size_t currenPositionInTape{};
		size_t currentStringSize{};
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

		inline SimdBase256& operator=(uint8_t* values) {
			*this = _mm256_loadu_epi8(values);
			return *this;
		}

		inline SimdBase256(uint8_t* values) {
			*this = values;
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

		inline void store(char dst[32])  {
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

		inline SimdBase256 carrylessMultiplication(uint64_t& prevInString) {
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

		inline SimdBase256 printBits(uint64_t value,const std::string& valuesTitle) {
			std::cout << valuesTitle;
			for (size_t x = 0; x < 8; ++x) {
				for (size_t y = 0; y < 8; ++y) {
					std::cout << std::bitset<1>{ static_cast<uint64_t>(*(reinterpret_cast<int8_t*>(&value) + x)) >> y };
				}
			}
			std::cout << std::endl;
			return *this;
		}

		inline SimdBase256 printBits(const std::string& valuesTitle) {
			std::cout << valuesTitle;
			for (size_t x = 0; x < 32; ++x) {
				for (size_t y = 0; y < 8; ++y) {
					std::cout << std::bitset<1>{ static_cast<uint64_t>(*(reinterpret_cast<int8_t*>(&this->value) + x)) >> y };
				}
			}
			std::cout << std::endl;
			return *this;
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

		inline uint64_t addTapeValues(uint32_t* tapePtrs, uint64_t* theBits, size_t currentIndexNew, size_t currentIndexIntoString,
			size_t& currentIndexIntoTape,size_t stringLength) {
			uint64_t value = static_cast<uint64_t>(__popcnt64(*theBits));
			for (int i = 0; i < value; i++) {
				auto valueNew = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + currentIndexIntoString;
				if (valueNew >= stringLength) {
					return i;
				} else {
					tapePtrs[currentIndexIntoTape++] = valueNew;
					*theBits = _blsr_u64(*theBits);
				}
			}

			return value;
		}

		inline size_t getStructuralIndices(uint32_t* currentPtr, size_t currentIndex, size_t& currentIndexIntoTape, size_t stringLength) {
			size_t returnValue{};
			for (size_t x = 0; x < 4; ++x) {
				auto newValue = this->S256.getUint64(x);
				returnValue += this->addTapeValues(currentPtr, &newValue, x, currentIndex, currentIndexIntoTape, stringLength);
			}
			return returnValue;
		}

		inline void collectStructuralAndWhiteSpaceCharacters() {
			char valuesNew[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
			SimdBase256 opTable{ valuesNew };
			SimdBase256 structural[8]{};
			for (size_t x = 0; x < 8; ++x) {
				auto valuesNew00 = this->values[x] | SimdBase256{ 0x20 };
				structural[x] = this->values[x].shuffle(opTable) == valuesNew00;
			}
			this->S256 = convertSimdBytesToBits(structural);
						
			char valuesNewTwo[32]{ ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2,
				100, '\t', '\n', 112, 100, '\r', 100, 100 };
			SimdBase256 whitespaceTable{ valuesNewTwo };
			SimdBase256 whiteSpaceReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				whiteSpaceReal[x] = this->values[x].shuffle(whitespaceTable) == this->values[x];
			}
			this->W256 = convertSimdBytesToBits(whiteSpaceReal);
		}

		inline SimdBase256 collectQuotedRange(uint64_t& prevInString) {
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
			this->S256 = S256.bitAndNot((this->Q256.bitAndNot(this->R256)));
			return this->S256;
		}

		inline SimdStringSection(const char* valueNew, uint64_t& prevInString, SimdBase256& followsPotentialNonQuoteScalar,
			SimdBase256& previousScalar) {
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
			this->collectStructuralAndWhiteSpaceCharacters();
			this->S256 = this->collectFinalStructurals();
			followsPotentialNonQuoteScalar.printBits("THE BITS FINAL: ");
			this->S256.printBits("THE BITS FINAL: ");
			//this->S256.printBits("BITS BEFORE: ");
			//this->S256 |= ~scalar | ~followsPotentialNonQuoteScalar;
			//this->S256.printBits("BITS AFTER: ");
			//this->S256 = this->structuralStart(followsPotentialNonQuoteScalar);
			//this->collectStructuralAndWhiteSpaceCharacters();
			//this->S256 = this->collectFinalStructurals();
			//this->S256.printBits("THE BITS: ");
		}

	  protected:
		SimdBase256 values[8]{};
		SimdBase256 Q256{};
		SimdBase256 W256{};
		SimdBase256 R256{};
		SimdBase256 S256{};
	};

	struct OpenContainer {
		uint32_t tapeIndex{};
		uint32_t count{};
	};

	class SimdJsonValue {
	  public:
		inline SimdJsonValue() {
		}

		int32_t round(int32_t a, int32_t n) {
			return (((a) + (( n )-1)) & ~(( n )-1));
		}

		inline ErrorCode allocate(size_t capacity, const char* stringViewNew) noexcept {
			if (capacity == 0) {
				this->tape.reset(0, 0, nullptr);
				this->allocatedCapacity = 0;
				return ErrorCode::Success;
			}
			
			int32_t tapeCapacity = round(capacity + 3u, 256u);
			int32_t stringCapacity = round(5 * static_cast<int32_t>(capacity) / 3 + 256, 256);
			this->tape.reset(tapeCapacity, stringCapacity, stringViewNew);
			this->structuralIndices = std::make_unique<uint32_t[]>(tapeCapacity);
			if (!(this->tape.getStringViewNew() && this->tape.operator uint64_t*())) {
				this->allocatedCapacity = 0;
				this->tape.reset(0, 0, nullptr);
				return ErrorCode::MemAlloc;
			}
			this->openContainers = std::make_unique<OpenContainer[]>(this->maxDepth);
			this->allocatedCapacity = capacity;
			return ErrorCode::Success;
		}

		inline void generateJsonEvents(const char* stringNew, size_t stringLength) {
			if (stringNew) {
				if (stringLength == 0) {
					throw JsonifierException{ "Failed to parse as the string size is 0." };
				}
				this->stringLengthRaw = stringLength;
				this->tapeLength = 0;
				if (this->allocatedCapacity < this->stringLengthRaw) {
					if (this->allocate(stringLength, stringNew) != ErrorCode::Success) {
						throw JsonifierException{ "Failed to allocate properly!" };
					}
				}
				int64_t stringSize = this->allocatedCapacity;
				uint32_t collectedSize{};
				size_t tapeCurrentIndex{ 0 };
				uint64_t prevInString{};
				SimdBase256 followsPotentialNonQuoteScalar02{};
				SimdBase256 previousScalar{};
				while (stringSize > 0) {
					SimdStringSection section(this->tape.getStringView() + collectedSize, prevInString, followsPotentialNonQuoteScalar02,
						previousScalar);
					auto indexCount = section.getStructuralIndices(this->structuralIndices.get(), collectedSize, tapeCurrentIndex, stringLength);
					this->tapeLength += indexCount;
					stringSize -= 256;
					collectedSize += 256;
				}
				for (size_t x = 0; x < this->tapeLength; ++x) {
					std::cout << "CURRENT INDEX: " << this->structuralIndices[x]
							  << ", THAT INDEX'S VALUE: " << this->getStringView()[this->structuralIndices[x]] << std::endl;
				}
				this->tape.setTapeCount(this->tapeLength);
			}
		}

		inline ~SimdJsonValue() noexcept {};

		inline char* getStringView() {
			return ( char* )this->tape.getStringView();
		}

		inline uint8_t* getStringViewNew() {
			return reinterpret_cast<uint8_t*>(this->tape.getStringViewNew());
		}

		OpenContainer* getOpenContainers() {
			return this->openContainers.get();
		}

		inline uint32_t& getCurrentDepth() {
			return this->depth;
		}

		inline uint64_t* getTape() {
			return this->tape.operator uint64_t*();
		}

		inline uint32_t* getStructuralIndices() {
			return this->structuralIndices.get();
		}

		inline JsonParser getJsonData(std::string& string);

		inline uint32_t getMaxDepth() {
			return this->maxDepth;
		}

		inline size_t getTapeLength() {
			return this->tapeLength;
		}

		inline std::vector<bool>& getIsArray() {
			return this->isArray;
		}

	  protected:
		std::unique_ptr<OpenContainer[]> openContainers{};
		std::unique_ptr<uint32_t[]> structuralIndices{};
		JsonParser tape{ 0, 0, nullptr };
		std::vector<bool> isArray{};
		size_t allocatedCapacity{};
		size_t stringLengthRaw{};
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

	class JsonIterator;

	struct TapeWriter {
		TapeWriter(uint64_t* ptr) {
			this->nextTapeLocation = ptr;
		}
		uint64_t* nextTapeLocation;
		inline void appendS64(int64_t value) noexcept;
		inline void appendU64(uint64_t value) noexcept;
		inline void appendDouble(double value) noexcept;
		inline void append(uint64_t val, TapeType t) noexcept;
		inline void skip() noexcept;
		inline void skipLargeInteger() noexcept;
		inline void skipDouble() noexcept;
		inline static void write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept;

	  private:
		template<typename T> inline void append2(uint64_t val, T val2, TapeType t) noexcept;
	};

	struct TapeBuilder {
		static inline ErrorCode parseDocument(SimdJsonValue& masterParser);

		inline ErrorCode visitDocumentStart(JsonIterator& iter) noexcept;

		inline ErrorCode visitDocumentEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitArrayStart(JsonIterator& iter) noexcept;

		inline ErrorCode visitArrayEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitEmptyArray(JsonIterator& iter) noexcept;

		inline ErrorCode visitObjectStart(JsonIterator& iter) noexcept;

		inline ErrorCode visitKey(JsonIterator& iter, const uint8_t* key) noexcept;

		inline ErrorCode visitObjectEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitEmptyObject(JsonIterator& iter) noexcept;

		inline ErrorCode visitPrimitive(JsonIterator& iter, const uint8_t* value) noexcept;

		inline ErrorCode visitRootPrimitive(JsonIterator& iter, const uint8_t* value) noexcept;

		inline ErrorCode visitString(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitNumber(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitNullAtom(JsonIterator& iter, const uint8_t* value) noexcept;

		inline ErrorCode visitRootString(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitRootNumber(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitRootTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitRootFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept;
		inline ErrorCode visitRootNullAtom(JsonIterator& iter, const uint8_t* value) noexcept;

		inline ErrorCode incrementCount(JsonIterator& iter) noexcept;

		TapeWriter tape;

	  private:
		uint8_t* currentStringBufferLocation{};

		inline TapeBuilder(SimdJsonValue& doc) noexcept;

		inline uint32_t nextTapeIndex(JsonIterator& iter) noexcept;
		inline ErrorCode startContainer(JsonIterator& iter) noexcept;
		inline ErrorCode endContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline ErrorCode emptyContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline uint8_t* onStartString(JsonIterator& iter) noexcept;
		inline ErrorCode onEndString(uint8_t* dst) noexcept;
	};

	class JsonIterator {
	  public:
		SimdJsonValue* masterParser{ nullptr };
		uint32_t* nextStructural{ nullptr };
		const uint8_t* buf{ nullptr };

		inline JsonIterator(SimdJsonValue* masterParserNew, size_t start_structural_index);

		inline ErrorCode visitRootPrimitive(TapeBuilder& visitor, const uint8_t* value);

		inline ErrorCode visitPrimitive(TapeBuilder& visitor, const uint8_t* value);

		inline const uint32_t currentIndexIntoString() noexcept;

		inline ErrorCode walkDocument(TapeBuilder&& visitor);

		inline uint8_t lastStructural()  noexcept;

		inline const uint8_t* advance() noexcept;

		inline size_t remainingLen() noexcept;

		inline const uint8_t* peek() noexcept;

		inline bool atBeginning() noexcept;

		inline bool atEof() noexcept;
	};

	inline JsonIterator::JsonIterator(SimdJsonValue* masterParserNew, size_t start_structural_index)
		: nextStructural(masterParserNew->getStructuralIndices()), buf{ reinterpret_cast<const uint8_t*>(masterParserNew->getStringView()) },
		  masterParser{ masterParserNew } {};


	inline ErrorCode JsonIterator::visitRootPrimitive(TapeBuilder& visitor, const uint8_t* value) {
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
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
		}
	}

	inline ErrorCode JsonIterator::visitPrimitive(TapeBuilder& visitor, const uint8_t* value) {
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
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
		}
	}

	inline const uint32_t JsonIterator::currentIndexIntoString() noexcept {
		return *this->nextStructural;
	}

	inline ErrorCode JsonIterator::walkDocument(TapeBuilder&& visitor) {
		if (atEof()) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::Empty) } +
				", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
		}

		auto value = this->advance();

		switch (*value) {
			case '{': {
				if (this->lastStructural() != '}') {
					throw JsonifierException{ "Sorry, but you've encountered the following error: " +
						std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
						", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
				};
				break;
			}
			case '[': {
				if (this->lastStructural() != ']') {
					throw JsonifierException{ "Sorry, but you've encountered the following error: " +
						std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
						", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
				};
				break;
			}
		}

		switch (*value) {
			case '{': {
				if (*this->peek() == '}') {
					this->advance();
					visitor.visitEmptyObject(*this);
					break;
				}
				goto Object_Begin;
			}

			case '[': {
				if (*this->peek() == ']') {
					this->advance();
					visitor.visitEmptyArray(*this);
					break;
				}
				goto Array_Begin;
			}
			default: {
				visitor.visitRootPrimitive(*this, value);
				break;
			}
		}
		goto Document_End;

		Object_Begin : {
			this->masterParser->getCurrentDepth()++;
			if (this->masterParser->getCurrentDepth() >= this->masterParser->getMaxDepth()) {
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::DepthError) } +
					", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
			}
			this->masterParser->getIsArray().push_back(false);
			visitor.visitObjectStart(*this);

			auto key = this->advance();
			if (*key != '"') {
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
			}
			visitor.incrementCount(*this);
			visitor.visitKey(*this, key);
		}

		Object_Field : {
			if (*this->advance() != ':') {
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
			}
			auto value = this->advance();
			switch (*value) {
				case '{': {
					if (*this->peek() == '}') {
						this->advance();
						visitor.visitEmptyObject(*this);
						break;
					}
					goto Object_Begin;
				}
				case '[': {
					if (*this->peek() == ']') {
						this->advance();
						visitor.visitEmptyArray(*this);
						break;
					}
					goto Array_Begin;
				}
				default: {
					visitor.visitPrimitive(*this, value);
					break;
				}
			}
		}

		Object_Continue : {
			switch (*this->advance()) {
				case ',': {
					visitor.incrementCount(*this);
					auto key = this->advance();
					if (*key != '"') {
						throw JsonifierException{ "Sorry, but you've encountered the following error: " +
							std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
							", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
					}
					visitor.visitKey(*this, key);
					goto Object_Field;
				}
				case '}': {
					visitor.visitObjectEnd(*this);
					goto Scope_End;
				}
				default: {
					throw JsonifierException{ "Sorry, but you've encountered the following error: " +
						std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
						", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
				}
			}
		}

		Scope_End : {
			this->masterParser->getCurrentDepth()--;
			this->masterParser->getIsArray().erase(this->masterParser->getIsArray().end() - 1);
			if (this->masterParser->getCurrentDepth() == 0) {
				goto Document_End;
			}
			if (this->masterParser->getIsArray()[this->masterParser->getCurrentDepth() - 1ull]) {
				goto Array_Continue;
			}
			goto Object_Continue;
		}

		Array_Begin : {
			this->masterParser->getCurrentDepth()++;
			if (this->masterParser->getCurrentDepth() >= this->masterParser->getMaxDepth()) {
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::DepthError) } +
					", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
			}

			this->masterParser->getIsArray().push_back(true);
			visitor.visitArrayStart(*this);
			visitor.incrementCount(*this);
		}

		Array_Value : {
			auto value = this->advance();
			switch (*value) {
				case '{': {
					if (*this->peek() == '}') {
						this->advance();
						visitor.visitEmptyObject(*this);
						break;
					}
					goto Object_Begin;
				}
				case '[': {
					if (*this->peek() == ']') {
						this->advance();
						visitor.visitEmptyArray(*this);
						break;
					}
					goto Array_Begin;
				}
				default: {
					visitor.visitPrimitive(*this, value);
					break;
				}
			}
		}

		Array_Continue : {
			switch (*this->advance()) {
				case ',': {
					visitor.incrementCount(*this);
					goto Array_Value;
				}
				case ']': {
					visitor.visitArrayEnd(*this);
					goto Scope_End;
				}
				default: {
					throw JsonifierException{ "Sorry, but you've encountered the following error: " +
						std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
						", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
				}
			}
		}

		Document_End : {
			visitor.visitDocumentEnd(*this);

			auto nextStructuralIndex = uint32_t(this->nextStructural - &this->masterParser->getStructuralIndices()[0]);

			if (nextStructuralIndex != this->masterParser->getTapeLength()) {
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(this->currentIndexIntoString()) };
			}

			return ErrorCode::Success;
		}
	}

	inline const uint8_t* JsonIterator::advance() noexcept {
		auto newIndex = (*this->nextStructural++);
		std::cout << "NEW INDEX: " << newIndex << ", THE INDEX'S VALUE: " << this->masterParser->getStringView()[newIndex] << std::endl;
		return &buf[newIndex];
	}

	inline uint8_t JsonIterator::lastStructural() noexcept {
		return buf[masterParser->getStructuralIndices()[masterParser->getTapeLength() - 1]];
	}

	inline size_t JsonIterator::remainingLen() noexcept {
		return masterParser->getTapeLength() - *this->nextStructural;
	}

	inline const uint8_t* JsonIterator::peek() noexcept {
		return &buf[masterParser->getStructuralIndices()[*this->nextStructural]];
	}

	inline bool JsonIterator::atBeginning() noexcept {
		return this->nextStructural == masterParser->getStructuralIndices();
	}

	inline bool JsonIterator::atEof() noexcept {
		return this->nextStructural == &masterParser->getStructuralIndices()[masterParser->getTapeLength() - 1];
	}

	inline void TapeWriter::appendS64(int64_t value) noexcept {
		this->append2(0, value, TapeType::Int64);
	}

	inline void TapeWriter::appendU64(uint64_t value) noexcept {
		this->append(0, TapeType::Uint64);
		*this->nextTapeLocation = value;
		this->nextTapeLocation++;
	}

	inline void TapeWriter::appendDouble(double value) noexcept {
		this->append2(0, value, TapeType::Double);
	}

	inline void TapeWriter::skip() noexcept {
		this->nextTapeLocation++;
	}

	inline void TapeWriter::skipLargeInteger() noexcept {
		this->nextTapeLocation += 2;
	}

	inline void TapeWriter::skipDouble() noexcept {
		this->nextTapeLocation += 2;
	}

	inline void TapeWriter::append(uint64_t val, TapeType t) noexcept {
		*this->nextTapeLocation = val | ((uint64_t(char(t))) << 56);
		this->nextTapeLocation++;
	}

	template<typename T> inline void TapeWriter::append2(uint64_t val, T val2, TapeType t) noexcept {
		this->append(val, t);
		static_assert(sizeof(val2) == sizeof(*this->nextTapeLocation), "Type is not 64 bits!");
		memcpy(this->nextTapeLocation, &val2, sizeof(val2));
		this->nextTapeLocation++;
	}

	inline void TapeWriter::write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept {
		tape_loc = val | ((uint64_t(char(t))) << 56);
	}

	inline ErrorCode TapeBuilder::parseDocument(SimdJsonValue& masterParser) {
		JsonIterator iter(&masterParser, 0);
		TapeBuilder builder(masterParser);
		return iter.walkDocument(std::move(builder));
	}

	inline ErrorCode TapeBuilder::visitRootPrimitive(JsonIterator& iter, const uint8_t* value) noexcept {
		return iter.visitRootPrimitive(*this, value);
	}
	inline ErrorCode TapeBuilder::visitPrimitive(JsonIterator& iter, const uint8_t* value) noexcept {
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
		TapeWriter::write(iter.masterParser->getTape()[startTapeIndex], nextTapeIndex(iter), TapeType::Root);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitKey(JsonIterator& iter, const uint8_t* key) noexcept {
		return visitString(iter, key);
	}

	inline ErrorCode TapeBuilder::incrementCount(JsonIterator& iter) noexcept {
		iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].count++;
		return ErrorCode::Success;
	}

	inline TapeBuilder::TapeBuilder(SimdJsonValue& doc) noexcept
		: tape{ doc.getTape() }, currentStringBufferLocation{ reinterpret_cast<uint8_t*>(doc.getStringViewNew()) } {
	}

	inline ErrorCode TapeBuilder::visitString(JsonIterator& iter, const uint8_t* value) noexcept {
		uint8_t* dst01 = onStartString(iter);
		auto dst02 = StringParser::parseString(value + 1ull, dst01, (*iter.nextStructural) - (*iter.nextStructural - 1ull));
		if (dst02 == nullptr) {
			return ErrorCode::StringError;
		}
		std::cout << "THE STRING LENGTH: " << static_cast<size_t>(dst02 - dst01)
				  << ", THE STRING: " << std::string_view{ reinterpret_cast<char*>(dst01), static_cast<size_t>(dst02-dst01) } << std::endl;
		onEndString(reinterpret_cast<uint8_t*>(dst02));
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootString(JsonIterator& iter, const uint8_t* value) noexcept {
		return visitString(iter, value);
	}

	inline ErrorCode TapeBuilder::visitNumber(JsonIterator& iter, const uint8_t* value) noexcept {
		return NumberParser::parseNumber<ErrorCode, TapeWriter>(value, this->tape);
	}

	inline ErrorCode TapeBuilder::visitRootNumber(JsonIterator& iter, const uint8_t* value) noexcept {
		std::unique_ptr<uint8_t[]> copy(new (std::nothrow) uint8_t[iter.remainingLen() + 256]);
		if (copy.get() == nullptr) {
			return ErrorCode::MemAlloc;
		}
		std::memcpy(copy.get(), value, iter.remainingLen());
		std::memset(copy.get() + iter.remainingLen(), ' ', 256);
		return visitNumber(iter, copy.get());
	}

	inline ErrorCode TapeBuilder::visitTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!StringParser::isValidTrueAtom(value,
				static_cast<size_t>(iter.masterParser->getStringView()[static_cast<size_t>(*iter.nextStructural)] -
					static_cast<size_t>((iter.masterParser->getStringView()[static_cast<size_t>(*(iter.nextStructural) - 1ull)]))))) {
			return ErrorCode::TAtomError;
		}
		tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!StringParser::isValidTrueAtom(value,
				static_cast<size_t>(iter.masterParser->getStringView()[static_cast<size_t>(*iter.nextStructural)] -
					static_cast<size_t>((iter.masterParser->getStringView()[static_cast<size_t>(*(iter.nextStructural) - 1ull)]))))) {
			return ErrorCode::NAtomError;
		}
		tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!StringParser::isValidFalseAtom(value,
				static_cast<size_t>(iter.masterParser->getStringView()[static_cast<size_t>(*iter.nextStructural)] -
					static_cast<size_t>((iter.masterParser->getStringView()[static_cast<size_t>(*(iter.nextStructural) - 1ull)]))))) {
			return ErrorCode::FAtomError;
		}
		tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!StringParser::isValidFalseAtom(value,
				static_cast<size_t>(iter.masterParser->getStringView()[static_cast<size_t>(*iter.nextStructural)] -
					static_cast<size_t>((iter.masterParser->getStringView()[static_cast<size_t>(*(iter.nextStructural) - 1ull)]))))) {
			return ErrorCode::FAtomError;
		}
		tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitNullAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!StringParser::isValidNullAtom(value,
				static_cast<size_t>(iter.masterParser->getStringView()[static_cast<size_t>(*iter.nextStructural)] -
					static_cast<size_t>((iter.masterParser->getStringView()[static_cast<size_t>(*(iter.nextStructural) - 1ull)]))))) {
			return ErrorCode::NAtomError;
		}
		tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootNullAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!StringParser::isValidNullAtom(value,
				static_cast<size_t>(iter.masterParser->getStringView()[static_cast<size_t>(*iter.nextStructural)] -
					static_cast<size_t>((iter.masterParser->getStringView()[static_cast<size_t>(*(iter.nextStructural) - 1ull)]))))) {
			return ErrorCode::NAtomError;
		}
		tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline uint32_t TapeBuilder::nextTapeIndex(JsonIterator& iter) noexcept {
		return uint32_t(tape.nextTapeLocation - iter.masterParser->getTape());
	}

	inline ErrorCode TapeBuilder::emptyContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept {
		auto startIndex = nextTapeIndex(iter);
		tape.append(startIndex + 2ull, start);
		tape.append(startIndex, end);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::startContainer(JsonIterator& iter) noexcept {
		iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].tapeIndex = nextTapeIndex(iter);
		iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].count = 0;
		tape.skip();
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::endContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept {
		const uint32_t startTapeIndex = iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].tapeIndex;
		tape.append(startTapeIndex, end);
		const uint32_t count = iter.masterParser->getOpenContainers()[iter.masterParser->getCurrentDepth()].count;
		const uint32_t cntsat = count > 0xFFFFFF ? 0xFFFFFF : count;
		TapeWriter::write(iter.masterParser->getTape()[startTapeIndex], nextTapeIndex(iter) | (uint64_t(cntsat) << 32), start);
		return ErrorCode::Success;
	}

	inline uint8_t* TapeBuilder::onStartString(JsonIterator& iter) noexcept {
		tape.append(currentStringBufferLocation - reinterpret_cast<uint8_t*>(iter.masterParser->getStringViewNew()), TapeType::String);
		return currentStringBufferLocation + sizeof(uint32_t);
	}

	inline ErrorCode TapeBuilder::onEndString(uint8_t* dst) noexcept {
		uint32_t strLength = uint32_t(dst - (this->currentStringBufferLocation + sizeof(uint32_t)));
		memcpy(this->currentStringBufferLocation, &strLength, sizeof(uint32_t));
		*dst = 0;
		this->currentStringBufferLocation = dst + 1;
		return ErrorCode::Success;
	}

	JsonParser SimdJsonValue::getJsonData(std::string& string) {
		this->generateJsonEvents(string.data(), string.size());
		TapeBuilder::parseDocument(*this);
		return std::move(this->tape);
	}

};