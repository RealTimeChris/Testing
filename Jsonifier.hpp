#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)

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

	const uint8_t formatVersion{ 131 };

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
		template<IsEnum EnumType> EnumConverter& operator=(const std::vector<EnumType>& data) {
			for (auto& value: data) {
				this->vector.emplace_back(std::move(static_cast<uint64_t>(value)));
			}
			return *this;
		};

		template<IsEnum EnumType> EnumConverter(const std::vector<EnumType>& data) {
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
		bool vectorType{};
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

	template<> inline const Jsonifier::ObjectType& Jsonifier::getValue() const {
		return *this->jsonValue.object;
	}

	template<> inline const Jsonifier::ArrayType& Jsonifier::getValue() const {
		return *this->jsonValue.array;
	}

	template<> inline const Jsonifier::StringType& Jsonifier::getValue() const {
		return *this->jsonValue.string;
	}

	template<> inline const Jsonifier::FloatType& Jsonifier::getValue() const {
		return this->jsonValue.numberDouble;
	}

	template<> inline const Jsonifier::UintType& Jsonifier::getValue() const {
		return this->jsonValue.numberUint;
	}

	template<> inline const Jsonifier::IntType& Jsonifier::getValue() const {
		return this->jsonValue.numberInt;
	}

	template<> inline const Jsonifier::BoolType& Jsonifier::getValue() const {
		return this->jsonValue.boolean;
	}

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
		operator std::string() const noexcept {
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
					if (static_cast<uint8_t>(unescaped.str[i]) <= 0x1F) {
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

	inline bool dumpRawTape(std::ostream& os , uint64_t* tape, const uint8_t* stringBuffer) noexcept {
		uint32_t string_length{};
		size_t tape_idx{ 0 };
		uint64_t tape_val{ tape[tape_idx] };
		uint8_t type{ uint8_t(tape_val >> 56) };
		std::cout  << tape_idx << " : " << type;
		tape_idx++;
		size_t how_many{ 0 };
		if (type == 'r') {
			how_many = size_t(tape_val & JSON_VALUE_MASK);
		} else {
			return false;
		}
		std::cout  << "\t// pointing to " << how_many << " (right after last node)\n";
		for (; tape_idx < how_many; tape_idx++) {
			std::cout  << tape_idx << " : ";
			tape_val = tape[tape_idx];
			type = uint8_t(tape_val >> 56);
			switch (type) {
				case '"':
					std::cout  << "string \"";
					std::memcpy(&string_length, stringBuffer + ( tape_val & JSON_VALUE_MASK), sizeof(uint32_t));
					std::cout << EscapeJsonString(std::string_view(
						reinterpret_cast<const char*>(stringBuffer + (tape_val & JSON_VALUE_MASK) + sizeof(uint32_t)), string_length));
					std::cout  << '"';
					std::cout  << '\n';
					break;
				case 'l':
					if (tape_idx + 1 >= how_many) {
						return false;
					}
					std::cout  << "integer " << static_cast<int64_t>(tape[++tape_idx]) << "\n";
					break;
				case 'u':
					if (tape_idx + 1 >= how_many) {
						return false;
					}
					std::cout  << "unsigned integer " << tape[++tape_idx] << "\n";
					break;
				case 'd':
					std::cout  << "float ";
					if (tape_idx + 1 >= how_many) {
						return false;
					}
					double answer;
					std::memcpy(&answer, &tape[++tape_idx], sizeof(answer));
					std::cout  << answer << '\n';
					break;
				case 'n':
					std::cout  << "null\n";
					break;
				case 't':
					std::cout  << "true\n";
					break;
				case 'f':
					std::cout  << "false\n";
					break;
				case '{':
					std::cout  << "{\t// pointing to next tape location " << uint32_t(( tape_val & JSON_VALUE_MASK)) << " (first node after the scope), "
					   << " saturated count " << ((( tape_val & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK) << "\n";
					break;
				case '}':
					std::cout  << "}\t// pointing to previous tape location " << uint32_t(( tape_val & JSON_VALUE_MASK)) << " (start of the scope)\n";
					break;
				case '[':
					std::cout  << "[\t// pointing to next tape location " << uint32_t(( tape_val & JSON_VALUE_MASK)) << " (first node after the scope), "
					   << " saturated count " << ((( tape_val & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK) << "\n";
					break;
				case ']':
					std::cout  << "]\t// pointing to previous tape location " << uint32_t(( tape_val & JSON_VALUE_MASK)) << " (start of the scope)\n";
					break;
				case 'r':
					return false;
				default:
					return false;
			}
		}
		tape_val = tape[tape_idx];
		type = uint8_t(tape_val >> 56);
		std::cout  << tape_idx << " : " << type << "\t// pointing to " << (tape_val & JSON_VALUE_MASK) << " (start root)\n";
		return true;
	}

	class TapeIterator {
	  public:
		TapeIterator() noexcept = default;

		TapeIterator(uint8_t* stringBufferNew, uint64_t* tapePositionNew, size_t currentStructuralCountNew) {
			this->currentStructuralCount = currentStructuralCountNew;
			this->initialTapePosition = tapePositionNew;
			this->tapePosition = tapePositionNew;
			this->stringBuffer = stringBufferNew;
		}

		inline uint64_t currentOffset() const noexcept {
			return tapePosition - initialTapePosition;
		}

		inline uint8_t advance() noexcept {
			return (*(tapePosition++)) >> 56;
		}

		inline bool atEof() {
			if((this->tapePosition - this->initialTapePosition) >= this->currentStructuralCount){
				//std::cout << "WERE AT EOF AT EOF!" << std::endl;
				return true;
			} else {
				return false;
			}
		}

		inline uint8_t peek() const noexcept {
			return ((*tapePosition) >> 56);
		}

		inline uint32_t peekIndex() const noexcept {
			return (tapePosition - this->initialTapePosition);
		}

		inline size_t peekLengthOrSize() const noexcept {
			if ( this->peek() == '[' || this->peek() == '{' || this->peek() == 'r') {
				return (((*tapePosition) & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK;
				
			} else if (this->peek() == '"') {
				size_t stringLength{};
				std::memcpy(&stringLength, stringBuffer + ((*tapePosition) & JSON_VALUE_MASK), sizeof(uint32_t));
				return stringLength;
			}
			else {
				return 1;
			}
		}

		inline uint8_t* getStringBuffer() {
			return this->stringBuffer;
		}

		inline char* getStringBufferAtCurrentPosition() {
			return reinterpret_cast<char*>(&this->stringBuffer[(*this->tapePosition) & JSON_VALUE_MASK]);
		}

		inline uint64_t* getTapePosition() {
			return this->tapePosition;
		}

		inline uint64_t* getTape() {
			return this->initialTapePosition;
		}

	  protected:
		size_t currentStructuralCount{};
		uint64_t* initialTapePosition{};
		uint64_t* tapePosition{};
		uint8_t* stringBuffer{};
	};
	
	class JsonParser {
	  public:
		using MapAllocatorType = std::allocator<std::pair<const std::string_view, JsonParser>>;
		template<typename OTy> using AllocatorType = std::allocator<OTy>;
		template<typename OTy> using AllocatorTraits = std::allocator_traits<AllocatorType<OTy>>;
		using ObjectType = std::map<std::string_view, JsonParser, std::less<>, MapAllocatorType>;
		using ArrayType = std::vector<JsonParser>;
		using StringType = std::string_view;
		using FloatType = double;
		using UintType = uint64_t;
		using IntType = int64_t;
		using BoolType = bool;

		union JsonValue {
			JsonValue() noexcept = default;
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

		JsonParser parseJson(JsonParser*dataToParse) {
			if (dataToParse->tapeIter.atEof()) {
				return *dataToParse;
			}
			switch (dataToParse->tapeIter.peek()) {
				case '{': {
					return dataToParse->parseJsonObject(dataToParse);
				}
				case '[': {
					return dataToParse->parseJsonArray(dataToParse);
				}
				case '"': {
					return dataToParse->parseJsonString(dataToParse);
				}
				case 'd': {
					return dataToParse->parseJsonFloat(dataToParse);
				}
				case 'l': {
					return dataToParse->parseJsonUint(dataToParse);
				}
				case 's': {
					return dataToParse->parseJsonInt(dataToParse);
				}
				case 'f': {
					[[fallthrough]];
				}
				case 't': {
					return dataToParse->parseJsonBool(dataToParse);
				}
				case 'n': {
					return dataToParse->parseJsonNull(dataToParse);
				}
				case 'r': {
					dataToParse->setValue(JsonType::Object);
					dataToParse->tapeIter.advance();
					return parseJson(dataToParse);
				}
				default: {
					return *dataToParse;
				}
			}
		}

		JsonParser parseJsonUint(JsonParser* dataToParse) {
			JsonParser returnData{};
			returnData.setValue(JsonType::Uint64);
			returnData.jsonValue.numberDouble = dataToParse->getUint64();
			dataToParse->tapeIter.advance();
			dataToParse->tapeIter.advance();
			return returnData;
		}

		JsonParser parseJsonInt(JsonParser* dataToParse) {
			JsonParser returnData{};
			returnData.setValue(JsonType::Int64);
			returnData.jsonValue.numberDouble = dataToParse->getInt64();
			dataToParse->tapeIter.advance();
			dataToParse->tapeIter.advance();
			return returnData;
		}

		JsonParser parseJsonObject(JsonParser* dataToParse) {
			JsonParser returnData{};
			returnData.setValue(JsonType::Object);
			auto currentSize = dataToParse->tapeIter.peekLengthOrSize();
			dataToParse->tapeIter.advance();
			if (currentSize > 0) {
				for (size_t x = 0; x < currentSize; ++x) {
					auto key = dataToParse->getKey();
					returnData.jsonValue.object->emplace(key, dataToParse->parseJson(dataToParse));
				}
			}

			return returnData;
		}

		JsonParser parseJsonArray(JsonParser* dataToParse) {
			JsonParser returnData{};
			returnData.setValue(JsonType::Array);
			auto currentSize = dataToParse->tapeIter.peekLengthOrSize();
			if (currentSize > 0) {
				for (size_t x = 0; x < currentSize; ++x) {
					dataToParse->tapeIter.advance();
					returnData.jsonValue.array->emplace_back(dataToParse->parseJson(dataToParse));
				}
			}
			return returnData;
		}

		JsonParser() noexcept = default; 

		JsonParser parseJsonString(JsonParser* dataToParse) {
			JsonParser returnData{};
			returnData.setValue(JsonType::String);
			*returnData.jsonValue.string = dataToParse->getString();
			dataToParse->tapeIter.advance();
			return returnData;
		}

		JsonParser parseJsonFloat(JsonParser* dataToParse) {
			JsonParser returnData{};
			returnData.setValue(JsonType::Float);
			returnData.jsonValue.numberDouble = dataToParse->getFloat();
			dataToParse->tapeIter.advance();
			dataToParse->tapeIter.advance();
			return returnData;
		}

		JsonParser parseJsonBool(JsonParser* dataToParse) {
			JsonParser returnData{};
			returnData.setValue(JsonType::Bool);
			returnData.jsonValue.boolean = dataToParse->getBool();
			dataToParse->tapeIter.advance();
			return returnData;
		}

		JsonParser parseJsonNull(JsonParser* dataToParse) {
			JsonParser returnData{};
			returnData.setValue(JsonType::Null);
			dataToParse->tapeIter.advance();
			return returnData;
		}

		void setValue(JsonType typeNew) const {
			this->destroy();
			this->type = typeNew;
			switch (this->type) {
				case JsonType::Object: {
					AllocatorType<ObjectType> allocator{};
					this->jsonValue.object = AllocatorTraits<ObjectType>::allocate(allocator, 1);
					AllocatorTraits<ObjectType>::construct(allocator, this->jsonValue.object);
					break;
				}
				case JsonType::Array: {
					AllocatorType<ArrayType> allocator{};
					this->jsonValue.array = AllocatorTraits<ArrayType>::allocate(allocator, 1);
					AllocatorTraits<ArrayType>::construct(allocator, this->jsonValue.array);
					break;
				}
				case JsonType::String: {
					AllocatorType<StringType> allocator{};
					this->jsonValue.string = AllocatorTraits<StringType>::allocate(allocator, 1);
					AllocatorTraits<StringType>::construct(allocator, this->jsonValue.string);
					break;
				}
			}
		}

		void setValue(JsonType typeNew) {
			this->destroy();
			this->type = typeNew;
			switch (this->type) {
				case JsonType::Object: {
					AllocatorType<ObjectType> allocator{};
					this->jsonValue.object = AllocatorTraits<ObjectType>::allocate(allocator, 1);
					AllocatorTraits<ObjectType>::construct(allocator, this->jsonValue.object);
					break;
				}
				case JsonType::Array: {
					AllocatorType<ArrayType> allocator{};
					this->jsonValue.array = AllocatorTraits<ArrayType>::allocate(allocator, 1);
					AllocatorTraits<ArrayType>::construct(allocator, this->jsonValue.array);
					break;
				}
				case JsonType::String: {
					AllocatorType<StringType> allocator{};
					this->jsonValue.string = AllocatorTraits<StringType>::allocate(allocator, 1);
					AllocatorTraits<StringType>::construct(allocator, this->jsonValue.string);
					break;
				}
			}
		}

		void destroy() const noexcept {
			switch (this->type) {
				case JsonType::Object: {
					AllocatorType<ObjectType> allocator{};
					AllocatorTraits<ObjectType>::destroy(allocator, this->jsonValue.object);
					AllocatorTraits<ObjectType>::deallocate(allocator, this->jsonValue.object, 1);
					break;
				}
				case JsonType::Array: {
					AllocatorType<ArrayType> allocator{};
					AllocatorTraits<ArrayType>::destroy(allocator, this->jsonValue.array);
					AllocatorTraits<ArrayType>::deallocate(allocator, this->jsonValue.array, 1);
					break;
				}
				case JsonType::String: {
					AllocatorType<StringType> allocator{};
					AllocatorTraits<StringType>::destroy(allocator, this->jsonValue.string);
					AllocatorTraits<StringType>::deallocate(allocator, this->jsonValue.string, 1);
					break;
				}
			}
		}

		void destroy() noexcept {
			switch (this->type) {
				case JsonType::Object: {
					AllocatorType<ObjectType> allocator{};
					AllocatorTraits<ObjectType>::destroy(allocator, this->jsonValue.object);
					AllocatorTraits<ObjectType>::deallocate(allocator, this->jsonValue.object, 1);
					break;
				}
				case JsonType::Array: {
					AllocatorType<ArrayType> allocator{};
					AllocatorTraits<ArrayType>::destroy(allocator, this->jsonValue.array);
					AllocatorTraits<ArrayType>::deallocate(allocator, this->jsonValue.array, 1);
					break;
				}
				case JsonType::String: {
					AllocatorType<StringType> allocator{};
					AllocatorTraits<StringType>::destroy(allocator, this->jsonValue.string);
					AllocatorTraits<StringType>::deallocate(allocator, this->jsonValue.string, 1);
					break;
				}
			}
		}

		size_t size() {
			switch (this->type) {
				case JsonType::Object: {
					return this->jsonValue.object->size();
				}
				case JsonType::Array: {
					return this->jsonValue.array->size();
				}
				case JsonType::String: {
					return this->jsonValue.string->size();
				}
				default: {
					return 1;
				}
			}
		}

		~JsonParser() noexcept {
			this->destroy();
		}

		JsonParser& operator=(JsonParser&& other) noexcept {
			this->setValue(other.type);
			switch (this->type) {
				case JsonType::Object: {
					*this->jsonValue.object = std::move(*other.jsonValue.object);
					break;
				}
				case JsonType::Array: {
					*this->jsonValue.array = std::move(*other.jsonValue.array);
					break;
				}
				case JsonType::String: {
					*this->jsonValue.string = std::move(*other.jsonValue.string);
					break;
				}
				case JsonType::Bool: {
					this->jsonValue.boolean= other.jsonValue.boolean;
					break;
				}
				case JsonType::Int64: {
					this->jsonValue.numberInt = other.jsonValue.numberInt;
					break;
				}
				case JsonType::Uint64: {
					this->jsonValue.numberUint = other.jsonValue.numberUint;
					break;
				}
				case JsonType::Float: {
					this->jsonValue.numberDouble = other.jsonValue.numberDouble;
					break;
				}
			}
			this->tapeIter = other.tapeIter;
			return *this;
		}

		JsonParser(JsonParser&& other) noexcept {
			*this = std::move(other);
		}

		JsonParser& operator=(const JsonParser& other) noexcept {
			this->setValue(other.type);
			switch (this->type) {
				case JsonType::Object: {
					*this->jsonValue.object = *other.jsonValue.object;
					break;
				}
				case JsonType::Array: {
					*this->jsonValue.array = *other.jsonValue.array;
					break;
				}
				case JsonType::String: {
					*this->jsonValue.string = *other.jsonValue.string;
					break;
				}
				case JsonType::Bool: {
					this->jsonValue.boolean = other.jsonValue.boolean;
					break;
				}
				case JsonType::Int64: {
					this->jsonValue.numberInt = other.jsonValue.numberInt;
					break;
				}
				case JsonType::Uint64: {
					this->jsonValue.numberUint = other.jsonValue.numberUint;
					break;
				}
				case JsonType::Float: {
					this->jsonValue.numberDouble = other.jsonValue.numberDouble;
					break;
				}
			}
			this->tapeIter = other.tapeIter;
			other.setValue(JsonType::Null);
			return *this;
		}

		JsonType getType() noexcept {
			return this->type;
		}

		JsonParser(const JsonParser& other) noexcept {
			*this = other;
		}

		JsonParser(uint64_t* tapePtrsNew, size_t count, uint8_t* stringBufferNew) {
			this->tapeIter = TapeIterator{ stringBufferNew, tapePtrsNew, count };
			*this = this->parseJson(this);
		}

		template<typename OTy> inline OTy getValue();

		template<> inline double getValue() {
			return this->jsonValue.numberDouble;
		}

		template<> inline float getValue() {
			return this->jsonValue.numberDouble;
		}

		template<> inline bool getValue() {
			if (this->type != JsonType::Bool) {
				throw JsonifierException{ "Sorry, but this object's type is not Bool!" };
			}
			return this->jsonValue.boolean;
		}

		template<> inline uint64_t getValue() {
			if (this->type != JsonType::Uint64) {
				throw JsonifierException{ "Sorry, but this object's type is not Uint!" };
			}
			return this->jsonValue.numberUint;
		}

		template<> inline uint32_t getValue() {
			if (this->type != JsonType::Uint64) {
				throw JsonifierException{ "Sorry, but this object's type is not Uint!" };
			}
			return this->jsonValue.numberUint;
		}

		template<> inline uint16_t getValue() {
			if (this->type != JsonType::Uint64) {
				throw JsonifierException{ "Sorry, but this object's type is not Uint!" };
			}
			return this->jsonValue.numberUint;
		}

		template<> inline uint8_t getValue() {
			if (this->type != JsonType::Uint64) {
				throw JsonifierException{ "Sorry, but this object's type is not Uint!" };
			}
			return this->jsonValue.numberUint;
		}

		template<> inline int64_t getValue() {
			if (this->type != JsonType::Int64) {
				throw JsonifierException{ "Sorry, but this object's type is not Int!" };
			}
			return this->jsonValue.numberInt;
		}

		template<> inline int32_t getValue() {
			if (this->type != JsonType::Int64) {
				throw JsonifierException{ "Sorry, but this object's type is not Int!" };
			}
			return this->jsonValue.numberInt;
		}

		template<> inline int16_t getValue() {
			if (this->type != JsonType::Int64) {
				throw JsonifierException{ "Sorry, but this object's type is not Int!" };
			}
			return this->jsonValue.numberInt;
		}

		template<> inline int8_t getValue() {
			if (this->type != JsonType::Int64) {
				throw JsonifierException{ "Sorry, but this object's type is not Int!" };
			}
			return this->jsonValue.numberInt;
		}

		template<> inline std::string getValue() {
			if (this->type != JsonType::String) {
				throw JsonifierException{ "Sorry, but this object's type is not String!" };
			}
			return static_cast<std::string>(*this->jsonValue.string);
		}

		template<> inline std::string_view getValue() {
			if (this->type != JsonType::String) {
				throw JsonifierException{ "Sorry, but this object's type is not String!" };
			}
			return std::move(*this->jsonValue.string);
		}

		bool getBool() {
			if (this->tapeIter.peek() == 'f') {
				return false;
			} else {
				return true;
			}
		}

		uint64_t getUint64() {
			uint64_t answer{};
			auto ptr = this->tapeIter.getTapePosition();
			std::memcpy(&answer, ++ptr, sizeof(answer));
			return answer;
		}

		int64_t getInt64() {
			int64_t answer{};
			auto ptr = this->tapeIter.getTapePosition();
			std::memcpy(&answer, ++ptr, sizeof(answer));
			return answer;
		}

		float getFloat() {
			double answer{};
			auto ptr = this->tapeIter.getTapePosition();
			std::memcpy(&answer, ++ptr, sizeof(answer));
			return answer;
		}

		inline std::string_view getString() {
			std::string_view returnValue{};
			if (this->tapeIter.peek() == '"') {
				size_t stringLength{};
				std::memcpy(&stringLength, this->tapeIter.getStringBuffer() + (*this->tapeIter.getTapePosition() & JSON_VALUE_MASK),
					sizeof(uint32_t));
				returnValue = std::string_view(reinterpret_cast<const char*>(this->tapeIter.getStringBuffer() +
												   (*this->tapeIter.getTapePosition() & JSON_VALUE_MASK) + sizeof(uint32_t)),
					stringLength);
			}
			return returnValue;
		}

		inline std::string_view getKey() {
			auto returnValue = this->getString();
			this->tapeIter.advance();
			return returnValue;
		}

		template<> inline JsonParser getValue() {
			return std::move(*this);
		}

		template<> inline std::vector<JsonParser> getValue() {
			return std::move(*this->jsonValue.array);
		}

		inline JsonParser& operator[](const std::string& key) {			
			if (this->jsonValue.object->contains(key)) {
				return this->jsonValue.object->at(static_cast<std::string_view>(key));
			} else {
				throw JsonifierException{ "Sorry, but that key does not exist!" };
			}
		};

		inline JsonParser(ErrorCode error) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " + std::to_string(( int32_t )error) };
		}

	  protected:
		mutable JsonType type{ JsonType::Null };
		mutable JsonValue jsonValue{};
		TapeIterator tapeIter{};
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

		inline SimdBase256& operator=(uint8_t other) {
			this->value = _mm256_set1_epi8(other);
			return *this;
		}

		inline SimdBase256(uint8_t other) {
			*this = other;
		}

		inline SimdBase256& operator=(const uint8_t* values) {
			*this = _mm256_loadu_epi8(values);
			return *this;
		}

		inline SimdBase256(const uint8_t* values) {
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

		inline void store(uint8_t dst[32]) const {
			return _mm256_storeu_epi8(dst, this->value);
		}

		inline uint64_t getUint64(size_t index) {
			switch (index) {
				case 0: {
					return static_cast<size_t>(_mm256_extract_epi64(this->value, 0));
				}
				case 1: {
					return static_cast<size_t>(_mm256_extract_epi64(this->value, 1));
				}
				case 2: {
					return static_cast<size_t>(_mm256_extract_epi64(this->value, 2));
				}
				case 3: {
					return static_cast<size_t>(_mm256_extract_epi64(this->value, 3));
				}
				default: {
					return static_cast<size_t>(_mm256_extract_epi64(this->value, 0));
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
					this->value = _mm256_insert_epi64(this->value, value, 0);
					break;
				}
				case 1: {
					this->value = _mm256_insert_epi64(this->value, value, 1);
					break;
				}
				case 2: {
					this->value = _mm256_insert_epi64(this->value, value, 2);
					break;
				}
				case 3: {
					this->value = _mm256_insert_epi64(this->value, value, 3);
					break;
				}
				default: {
					this->value = _mm256_insert_epi64(this->value, value, 0);
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

		inline SimdBase256 operator==(uint8_t other) {
			return _mm256_cmpeq_epi8(this->value, _mm256_set1_epi8(other));
		}

		inline SimdBase256 operator<<(size_t amount) {
			uint64_t values[4]{};
			values[0] = _mm256_extract_epi64(this->value, 0);
			values[1] = _mm256_extract_epi64(this->value, 1);
			values[2] = _mm256_extract_epi64(this->value, 2);
			values[3] = _mm256_extract_epi64(this->value, 3);
			SimdBase256 newValues{};
			newValues = _mm256_insert_epi64(newValues, (values[0] << (amount % 64)), 0);
			newValues = _mm256_insert_epi64(newValues, (values[1] << (amount % 64)) | ((values[0]) >> 64 - amount % 64), 1);
			newValues = _mm256_insert_epi64(newValues, (values[2] << (amount % 64)) | ((values[1]) >> 64 - amount % 64), 2);
			newValues = _mm256_insert_epi64(newValues, (values[3] << (amount % 64)) | ((values[2]) >> 64 - amount % 64), 3);
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

		inline void printBits(uint64_t values,const std::string& valuesTitle) {
			using std::cout;
			cout << valuesTitle;
			cout << std::bitset<64>{ values };
			cout << std::endl;
		}

		inline void printBits(const std::string& valuesTitle) {
		using std::cout;
			cout << valuesTitle;
			for (size_t x = 0; x < 32; ++x) {
				for (size_t y = 0; y < 8; ++y) {
					cout << std::bitset<1>{ static_cast<size_t>(*(reinterpret_cast<int8_t*>(&this->value) + x)) >> y };
				}
			}
			cout << std::endl;
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

	template<size_t StepSize> struct StringBlockReader {
	  public:
		inline StringBlockReader(const uint8_t* _buf, size_t _len);
		inline size_t getRemainder(uint8_t* dst) const;
		inline const uint8_t* fullBlock() const;
		inline bool hasFullBlock() const;
		inline size_t blockIndex();
		inline void advance();

	  private:
		const uint8_t* buf;
		const size_t len;
		const size_t lenminusstep;
		size_t idx;
	};

	template<size_t StepSize>
	inline StringBlockReader<StepSize>::StringBlockReader(const uint8_t* _buf, size_t _len)
		: buf{ _buf }, len{ _len }, lenminusstep{ len < StepSize ? 0 : len - StepSize }, idx{ 0 } {
	}

	template<size_t StepSize> inline size_t StringBlockReader<StepSize>::blockIndex() {
		return idx;
	}

	template<size_t StepSize> inline bool StringBlockReader<StepSize>::hasFullBlock() const {
		return idx < lenminusstep;
	}

	template<size_t StepSize> inline const uint8_t* StringBlockReader<StepSize>::fullBlock() const {
		return &buf[idx];
	}

	template<size_t StepSize> inline size_t StringBlockReader<StepSize>::getRemainder(uint8_t* dst) const {
		if (len == idx) {
			return 0;
		}
		std::memset(dst, 0x20, StepSize);
		std::memcpy(dst, buf + idx, len - idx);
		return len - idx;
	}

	template<size_t StepSize> inline void StringBlockReader<StepSize>::advance() {
		idx += StepSize;
	}


	class SimdStringSection {
	  public:
		inline SimdStringSection() noexcept = default;

		inline void packStringIntoValue(SimdBase256* theValue, const uint8_t string[32]) {
			*theValue = string;
		}

		inline uint64_t addTapeValues(uint32_t* tapePtrs, uint64_t* theBits, size_t currentIndexNew, size_t& currentIndexIntoTape,
			size_t stringLength) {
			int cnt = static_cast<int>(__popcnt64(*theBits));
			int64_t newValue{};
			for (int i = 0; i < 8; i++) {
				newValue = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + currentIndexIntoString;

				if (newValue >= stringLength) {
					currentIndexIntoTape += cnt;
					return cnt;

				} else {
					tapePtrs[i + currentIndexIntoTape] = newValue;
					*theBits = _blsr_u64(*theBits);
				}
			}

			if (cnt > 8) {
				for (int i = 8; i < 16; i++) {
					newValue = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + currentIndexIntoString;
					if (newValue >= stringLength) {
						currentIndexIntoTape += cnt;
						return cnt;

					} else {
						tapePtrs[i + currentIndexIntoTape] = newValue;
						*theBits = _blsr_u64(*theBits);
					}
				}

				if (cnt > 16) {
					int i = 16;
					do {
						newValue = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + currentIndexIntoString;
						if (newValue >= stringLength) {
							currentIndexIntoTape += cnt;
							return cnt;

						} else {
							tapePtrs[i + currentIndexIntoTape] = newValue;
							*theBits = _blsr_u64(*theBits);
						}
						i++;
					} while (i < cnt);
				}
			}
			currentIndexIntoTape += cnt;
			return cnt;
		}

		inline uint64_t follows(const uint64_t match, int64_t& overflow) {
			const uint64_t result = match << 1 | overflow;
			overflow = match >> 63;
			return result;
		}

		inline size_t getStructuralIndices(uint32_t* currentPtr,  size_t& currentIndexIntoTape, size_t stringLength) {
			size_t returnValue{};
			for (size_t x = 0; x < 4; ++x) {
				auto newValue = this->S256.getUint64(x);
				returnValue += this->addTapeValues(currentPtr, &newValue, x, currentIndexIntoTape, stringLength);
			}
			this->currentIndexIntoString += 256;
			return returnValue;
		}

		inline SimdBase256 collectWhiteSpace() {
			uint8_t valuesNew[32]{ ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2,
				100, '\t', '\n', 112, 100, '\r', 100, 100 };
			SimdBase256 whitespaceTable{ valuesNew };
			SimdBase256 whiteSpaceReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				whiteSpaceReal[x] = this->values[x].shuffle(whitespaceTable) == this->values[x];
			}
			return convertSimdBytesToBits(whiteSpaceReal);
		}

		inline SimdBase256 collectStructuralCharacters() {
			uint8_t newValues[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
			SimdBase256 opTable{ newValues };
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

		inline SimdBase256 collectFinalStructurals(SimdBase256& prevScalar, SimdBase256& followsPotentialNonQuoteScalar) {
			auto scalar = ~this->S256 | this->W256;
			auto stringTail = this->R256 ^ this->Q256;
			SimdBase256 nonquote_scalar = scalar.bitAndNot(this->Q256);
			for (size_t x = 0; x < 4; ++x) {
				int64_t prevScalarValue{ prevScalar.getInt64(0) };
				followsPotentialNonQuoteScalar.insertInt64(follows(nonquote_scalar.getInt64(x), prevScalarValue), x);
				prevScalar.insertInt64(prevScalarValue, 0);
			}
			return this->S256 | (~S256 | this->W256).bitAndNot(followsPotentialNonQuoteScalar).bitAndNot(stringTail);
		}

		void submitDataForProcessing(const uint8_t* valueNew, int64_t& prevInString, SimdBase256& prevScalar,
			SimdBase256& followsPotentialNonQuoteScalar) {
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
			this->S256 = this->collectFinalStructurals(prevScalar, followsPotentialNonQuoteScalar);
		}

	  protected:
		size_t currentIndexIntoString{};
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

	using depth_t = int32_t;

	using token_position = uint64_t*;

	class token_iterator {
	  public:
		/**
   * Create a new invalid token_iterator.
   *
   * Exists so you can declare a variable and later assign to it before use.
   */
		inline token_iterator() noexcept = default;
		inline token_iterator(token_iterator&& other) noexcept = default;
		inline token_iterator& operator=(token_iterator&& other) noexcept = default;
		inline token_iterator(const token_iterator& other) noexcept = default;
		inline token_iterator& operator=(const token_iterator& other) noexcept = default;

		/**
   * Advance to the next token (returning the current one).
   */
		inline const uint8_t* return_current_and_advance() noexcept;
		/**
   * Reports the current offset in bytes from the start of the underlying buffer.
   */
		inline uint32_t current_offset() const noexcept;
		/**
   * Get the JSON text for a given token (relative).
   *
   * This is not null-terminated; it is a view into the JSON.
   *
   * @param delta The relative position of the token to retrieve. e.g. 0 = current token,
   *              1 = next token, -1 = prev token.
   *
   * TODO consider a string_view, assuming the length will get stripped out by the optimizer when
   * it isn't used ...
   */
		inline const uint8_t* peek(int32_t delta = 0) const noexcept;
		/**
   * Get the maximum length of the JSON text for a given token.
   *
   * The length will include any whitespace at the end of the token.
   *
   * @param delta The relative position of the token to retrieve. e.g. 0 = current token,
   *              1 = next token, -1 = prev token.
   */
		inline uint32_t peek_length(int32_t delta = 0) const noexcept;

		/**
   * Get the JSON text for a given token.
   *
   * This is not null-terminated; it is a view into the JSON.
   *
   * @param position The position of the token.
   *
   */
		inline const uint8_t* peek(token_position position) const noexcept;
		/**
   * Get the maximum length of the JSON text for a given token.
   *
   * The length will include any whitespace at the end of the token.
   *
   * @param position The position of the token.
   */
		inline uint32_t peek_length(token_position position) const noexcept;

		/**
   * Return the current index.
   */
		inline token_position position() const noexcept;
		/**
   * Reset to a previously saved index.
   */
		inline void set_position(token_position target_position) noexcept;

		// NOTE: we don't support a full C++ iterator interface, because we expect people to make
		// different calls to advance the iterator based on *their own* state.

		inline bool operator==(const token_iterator& other) const noexcept;
		inline bool operator!=(const token_iterator& other) const noexcept;
		inline bool operator>(const token_iterator& other) const noexcept;
		inline bool operator>=(const token_iterator& other) const noexcept;
		inline bool operator<(const token_iterator& other) const noexcept;
		inline bool operator<=(const token_iterator& other) const noexcept;

	  protected:
		inline token_iterator(const uint8_t* buf, token_position position) noexcept;

		/**
   * Get the index of the JSON text for a given token (relative).
   *
   * This is not null-terminated; it is a view into the JSON.
   *
   * @param delta The relative position of the token to retrieve. e.g. 0 = current token,
   *              1 = next token, -1 = prev token.
   */
		inline uint32_t peek_index(int32_t delta = 0) const noexcept;
		/**
   * Get the index of the JSON text for a given token.
   *
   * This is not null-terminated; it is a view into the JSON.
   *
   * @param position The position of the token.
   *
   */
		inline uint32_t peek_index(token_position position) const noexcept;

		const uint8_t* buf{};
		token_position _position{};

		friend class json_iterator;
		friend class value_iterator;
		friend class object;
	};

	class SimdJsonValue;

	class json_iterator {
	  protected:
		token_iterator token{};
		SimdJsonValue* parser{};
		/**
   * Next free location in the string buffer.
   *
   * Used by raw_json_string::unescape() to have a place to unescape strings to.
   */
		uint8_t* _string_buf_loc{};
		/**
   * JSON error, if there is one.
   *
   * INCORRECT_TYPE and NO_SUCH_FIELD are *not* stored here, ever.
   *
   * PERF NOTE: we *hope* this will be elided into control flow, as it is only used (a) in the first
   * iteration of the loop, or (b) for the final iteration after a missing comma is found in ++. If
   * this is not elided, we should make sure it's at least not using up a register. Failing that,
   * we should store it in document so there's only one of them.
   */
		ErrorCode error{ ErrorCode::Success };
		/**
   * Depth of the current token in the JSON.
   *
   * - 0 = finished with document
   * - 1 = document root value (could be [ or {, not yet known)
   * - 2 = , or } inside root array/object
   * - 3 = key or value inside root array/object.
   */
		depth_t _depth{};
		/**
   * Beginning of the document indexes.
   * Normally we have root == parser->getTape().get()
   * but this may differ, especially in streaming mode (where we have several
   * documents);
   */
		token_position _root{};
		/**
   * Normally, a json_iterator operates over a single document, but in
   * some cases, we may have a stream of documents. This attribute is meant
   * as meta-data: the json_iterator works the same irrespective of the
   * value of this attribute.
   */
		bool _streaming{ false };

	  public:
		inline json_iterator() noexcept = default;
		inline json_iterator(json_iterator&& other) noexcept;
		inline json_iterator& operator=(json_iterator&& other) noexcept;
		inline explicit json_iterator(const json_iterator& other) noexcept = default;
		inline json_iterator& operator=(const json_iterator& other) noexcept = default;
		/**
   * Skips a JSON value, whether it is a scalar, array or object.
   */
		inline ErrorCode skip_child(depth_t parent_depth) noexcept;

		/**
   * Tell whether the iterator is still at the start
   */
		inline bool at_root() const noexcept;

		/**
   * Tell whether we should be expected to run in streaming
   * mode (iterating over many documents). It is pure metadata
   * that does not affect how the iterator works. It is used by
   * start_root_array() and start_root_object().
   */
		inline bool streaming() const noexcept;

		/**
   * Get the root value iterator
   */
		inline token_position root_position() const noexcept;
		/**
   * Assert that we are at the document depth (== 1)
   */
		inline void assert_at_document_depth() const noexcept;
		/**
   * Assert that we are at the root of the document
   */
		inline void assert_at_root() const noexcept;

		/**
   * Tell whether the iterator is at the EOF mark
   */
		inline bool at_end() const noexcept;

		/**
   * Tell whether the iterator is live (has not been moved).
   */
		inline bool is_alive() const noexcept;

		/**
   * Abandon this iterator, setting depth to 0 (as if the document is finished).
   */
		inline void abandon() noexcept;

		/**
   * Advance the current token without modifying depth.
   */
		inline const uint8_t* return_current_and_advance() noexcept;

		/**
   * Returns true if there is a single token in the index (i.e., it is
   * a JSON with a scalar value such as a single number).
   *
   * @return whether there is a single token
   */
		inline bool is_single_token() const noexcept;

		/**
   * Assert that there are at least the given number of tokens left.
   *
   * Has no effect in release builds.
   */
		inline void assert_more_tokens(uint32_t required_tokens = 1) const noexcept;
		/**
   * Assert that the given position addresses an actual token (is within bounds).
   *
   * Has no effect in release builds.
   */
		inline void assert_valid_position(token_position position) const noexcept;
		/**
   * Get the JSON text for a given token (relative).
   *
   * This is not null-terminated; it is a view into the JSON.
   *
   * @param delta The relative position of the token to retrieve. e.g. 0 = next token, -1 = prev token.
   *
   * TODO consider a string_view, assuming the length will get stripped out by the optimizer when
   * it isn't used ...
   */
		inline const uint8_t* peek(int32_t delta = 0) const noexcept;
		/**
   * Get the maximum length of the JSON text for the current token (or relative).
   *
   * The length will include any whitespace at the end of the token.
   *
   * @param delta The relative position of the token to retrieve. e.g. 0 = next token, -1 = prev token.
   */
		inline uint32_t peek_length(int32_t delta = 0) const noexcept;
		/**
   * Get a pointer to the current location in the input buffer.
   *
   * This is not null-terminated; it is a view into the JSON.
   *
   * You may be pointing outside of the input buffer: it is not generally
   * safe to dereference this pointer.
   */
		inline const uint8_t* unsafe_pointer() const noexcept;
		/**
   * Get the JSON text for a given token.
   *
   * This is not null-terminated; it is a view into the JSON.
   *
   * @param position The position of the token to retrieve.
   *
   * TODO consider a string_view, assuming the length will get stripped out by the optimizer when
   * it isn't used ...
   */
		inline const uint8_t* peek(token_position position) const noexcept;
		/**
   * Get the maximum length of the JSON text for the current token (or relative).
   *
   * The length will include any whitespace at the end of the token.
   *
   * @param position The position of the token to retrieve.
   */
		inline uint32_t peek_length(token_position position) const noexcept;
		/**
   * Get the JSON text for the last token in the document.
   *
   * This is not null-terminated; it is a view into the JSON.
   *
   * TODO consider a string_view, assuming the length will get stripped out by the optimizer when
   * it isn't used ...
   */
		inline const uint8_t* peek_last() const noexcept;

		/**
   * Ascend one level.
   *
   * Validates that the depth - 1 == parent_depth.
   *
   * @param parent_depth the expected parent depth.
   */
		inline void ascend_to(depth_t parent_depth) noexcept;

		/**
   * Descend one level.
   *
   * Validates that the new depth == child_depth.
   *
   * @param child_depth the expected child depth.
   */
		inline void descend_to(depth_t child_depth) noexcept;
		inline void descend_to(depth_t child_depth, int32_t delta) noexcept;

		/**
   * Get current depth.
   */
		inline depth_t depth() const noexcept;

		/**
   * Get current (writeable) location in the string buffer.
   */
		inline uint8_t*& string_buf_loc() noexcept;

		/**
   * Report an unrecoverable error, preventing further iteration.
   *
   * @param error The error to report. Must not be SUCCESS, UNINITIALIZED, INCORRECT_TYPE, or NO_SUCH_FIELD.
   * @param message An error message to report with the error.
   */
		inline ErrorCode report_error(ErrorCode error, const char* message) noexcept;

		/**
   * Log error, but don't stop iteration.
   * @param error The error to report. Must be INCORRECT_TYPE, or NO_SUCH_FIELD.
   * @param message An error message to report with the error.
   */
		inline ErrorCode optional_error(ErrorCode error, const char* message) noexcept;

		template<int N>
		inline bool copy_to_buffer(const uint8_t* json, uint32_t max_len, uint8_t (&tmpbuf)[N]) noexcept;

		inline token_position position() const noexcept;
		/**
   * Write the raw_json_string to the string buffer and return a string_view.
   * Each raw_json_string should be unescaped once, or else the string buffer might
   * overflow.
   */
		inline void reenter_child(token_position position, depth_t child_depth) noexcept;

#if SIMDJSON_DEVELOPMENT_CHECKS
		inline token_position start_position(depth_t depth) const noexcept;
		inline void set_start_position(depth_t depth, token_position position) noexcept;
#endif

		/* Useful for debugging and logging purposes. */
		inline std::string to_string() const noexcept;

		/**
   * Returns the current location in the document if in bounds.
   */
		inline const char* current_location() noexcept;

		/**
   * Updates this json iterator so that it is back at the beginning of the document,
   * as if it had just been created.
   */
		inline void rewind() noexcept;
		/**
   * This checks whether the {,},[,] are balanced so that the document
   * ends with proper zero depth. This requires scanning the whole document
   * and it may be expensive. It is expected that it will be rarely called.
   * It does not attempt to match { with } and [ with ].
   */
		inline bool balanced() const noexcept;

	  protected:
		inline json_iterator(const uint8_t* buf, SimdJsonValue* parser) noexcept;
		/// The last token before the end
		inline token_position last_position() const noexcept;
		/// The token *at* the end. This points at gibberish and should only be used for comparison.
		inline token_position end_position() const noexcept;
		/// The end of the buffer.
		inline token_position end() const noexcept;

		friend class document;
		friend class document_stream;
		friend class object;
		friend class array;
		friend class value;
		friend class raw_json_string;
		friend class parser;
		friend class value_iterator;
	};// json_iterator
	
	inline token_iterator::token_iterator(const uint8_t* _buf, token_position position) noexcept : buf{ _buf }, _position{ position } {
	}

	inline uint32_t token_iterator::current_offset() const noexcept {
		return *(_position);
	}


	inline const uint8_t* token_iterator::return_current_and_advance() noexcept {
		return &buf[*(_position++)];
	}

	inline const uint8_t* token_iterator::peek(token_position position) const noexcept {
		return &buf[*position];
	}
	inline uint32_t token_iterator::peek_index(token_position position) const noexcept {
		return *position;
	}
	inline uint32_t token_iterator::peek_length(token_position position) const noexcept {
		return *(position + 1) - *position;
	}

	inline const uint8_t* token_iterator::peek(int32_t delta) const noexcept {
		return &buf[*(_position + delta)];
	}
	inline uint32_t token_iterator::peek_index(int32_t delta) const noexcept {
		return *(_position + delta);
	}
	inline uint32_t token_iterator::peek_length(int32_t delta) const noexcept {
		return *(_position + delta + 1) - *(_position + delta);
	}

	inline token_position token_iterator::position() const noexcept {
		return _position;
	}
	inline void token_iterator::set_position(token_position target_position) noexcept {
		_position = target_position;
	}

	inline bool token_iterator::operator==(const token_iterator& other) const noexcept {
		return _position == other._position;
	}
	inline bool token_iterator::operator!=(const token_iterator& other) const noexcept {
		return _position != other._position;
	}
	inline bool token_iterator::operator>(const token_iterator& other) const noexcept {
		return _position > other._position;
	}
	inline bool token_iterator::operator>=(const token_iterator& other) const noexcept {
		return _position >= other._position;
	}
	inline bool token_iterator::operator<(const token_iterator& other) const noexcept {
		return _position < other._position;
	}
	inline bool token_iterator::operator<=(const token_iterator& other) const noexcept {
		return _position <= other._position;
	}

	

	class SimdJsonValue {
	  public:
		inline SimdJsonValue() {
		}

		int32_t round(int32_t a, int32_t n) {
			return (((a) + (( n )-1)) & ~(( n )-1));
		}

		inline ErrorCode allocate(uint8_t* stringViewNew) noexcept {
			if (this->stringLengthRaw == 0) {
				this->structuralIndexes.reset(nullptr);
				this->stringBuffer.reset(nullptr);
				this->isArray.reset(nullptr);
				this->tape.reset(nullptr);
				this->allocatedCapacity = 0;
				return ErrorCode::Success;
			}

			size_t tapeCapacity = round(this->stringLengthRaw + 3, 256);
			size_t stringCapacity = round(5 * this->stringLengthRaw / 3 + 256, 256);
			this->stringView = stringViewNew;
			this->stringBuffer.reset(new (std::nothrow) uint8_t[stringCapacity]);
			this->structuralIndexes.reset(new (std::nothrow) uint32_t[tapeCapacity]);
			this->openContainers.reset(new (std::nothrow) OpenContainer[this->maxDepth]);
			this->nStructuralIndexes = 0;
			this->allocatedCapacity = stringCapacity;
			this->tape.reset(new (std::nothrow) uint64_t[tapeCapacity]);
			this->isArray.reset(new (std::nothrow) bool[tapeCapacity]);

			if (!(this->tape.get() && this->structuralIndexes.get() && this->stringBuffer.get() && this->isArray.get() &&
					this->openContainers.get())) {
				this->structuralIndexes.reset(nullptr);
				this->stringBuffer.reset(nullptr);
				this->isArray.reset(nullptr);
				this->tape.reset(nullptr);
				this->allocatedCapacity = 0;
				return ErrorCode::MemAlloc;
			}
			
			return ErrorCode::Success;
		}
		int64_t totalTimePassed{};
		int64_t totalTimePassed02{};
		int64_t iterationCount{};			
		inline void generateJsonEvents(uint8_t* stringNew, size_t stringLength) {
			if (stringNew) {
				if (stringLength == 0) {
					throw JsonifierException{ "Failed to parse as the string size is 0." };
				}
				
				if (this->stringLengthRaw < stringLength) {
					this->stringLengthRaw = stringLength;
					if (this->allocate(stringNew) != ErrorCode::Success) {
						throw JsonifierException{ "Failed to allocate properly!" };
					}
				}
				
				iterationCount++;
				StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
				this->nStructuralIndexes= 0;
				size_t tapeCurrentIndex{ 0 };
				SimdBase256 prevInScalar{};
				SimdBase256 followsPotentialNonquoteScalar{};
				int64_t prevInString{};
				StringBlockReader<256> stringReader{ this->stringView, this->stringLengthRaw };
				while (stringReader.hasFullBlock()) {
					this->section.submitDataForProcessing(stringReader.fullBlock(), prevInString, prevInScalar, followsPotentialNonquoteScalar);
					auto indexCount =
						section.getStructuralIndices(this->structuralIndexes.get(), tapeCurrentIndex, this->stringLengthRaw);
					this->nStructuralIndexes += indexCount;
					stringReader.advance();
				}
				uint8_t block[256];
				stringReader.getRemainder(block);
				this->section.submitDataForProcessing(block, prevInString, prevInScalar, followsPotentialNonquoteScalar);
				auto indexCount = section.getStructuralIndices(this->structuralIndexes.get(), tapeCurrentIndex, this->stringLengthRaw);
				this->nStructuralIndexes += indexCount;
				
				totalTimePassed += stopWatch.totalTimePassed().count();
				std::cout << "TIME FOR STAGE1: " << totalTimePassed / iterationCount << std::endl;
			}
			this->nStructuralIndexes -= 1;
			
		}

		inline ~SimdJsonValue() noexcept {};

		inline uint8_t* getStringView() {
			return this->stringView;
		}

		inline uint8_t* getStringBuffer() {
			return this->stringBuffer.get();
		}

		OpenContainer* getOpenContainers() {
			return this->openContainers.get();
		}

		inline uint32_t* getStructuralIndexes() {
			return this->structuralIndexes.get();
		}

		inline uint64_t* getTape() {
			return this->tape.get();
		}

		inline JsonParser getJsonData(std::string& string);

		inline uint32_t getMaxDepth() {
			return this->maxDepth;
		}

		inline size_t& getTapeLength() {
			return this->tapeLength;
		}

		inline size_t getStructuralIndexCount() {
			return this->nStructuralIndexes;
		}

		inline bool* getIsArray() {
			return this->isArray.get();
		}

	  protected:
		std::unique_ptr<uint32_t[]> structuralIndexes{ nullptr };
		std::unique_ptr<OpenContainer[]> openContainers{};
		std::unique_ptr<uint64_t[]> tape{ nullptr };
		std::unique_ptr<uint8_t[]> stringBuffer{};
		std::unique_ptr<bool[]> isArray{};
		size_t nStructuralIndexes{ 0 };
		SimdStringSection section{};
		size_t allocatedCapacity{};
		size_t stringLengthRaw{};
		size_t tapeLength{ 0 };
		uint32_t maxDepth{ 512 };
		uint8_t* stringView{};
	};

	inline json_iterator::json_iterator(json_iterator&& other) noexcept
		: token(std::forward<token_iterator>(other.token)), parser{ other.parser }, _string_buf_loc{ other._string_buf_loc }, error{ other.error },
		  _depth{ other._depth }, _root{ other._root }, _streaming{ other._streaming } {
		other.parser = nullptr;
	}
	inline json_iterator& json_iterator::operator=(json_iterator&& other) noexcept {
		token = other.token;
		parser = other.parser;
		_string_buf_loc = other._string_buf_loc;
		error = other.error;
		_depth = other._depth;
		_root = other._root;
		_streaming = other._streaming;
		other.parser = nullptr;
		return *this;
	}

	inline json_iterator::json_iterator(const uint8_t* buf, SimdJsonValue* _parser) noexcept
		: token(buf, &_parser->getTape()[0]), parser{ _parser },
		  _string_buf_loc{ parser->getStringBuffer()}, _depth{ 1 }, _root{ parser->getTape() }, _streaming{ false }

	{

	}

	inline void json_iterator::rewind() noexcept {
		token.set_position(root_position());
		_string_buf_loc = parser->getStringBuffer();
		_depth = 1;
	}

	inline bool json_iterator::balanced() const noexcept {
		token_iterator ti(token);
		int32_t count{ 0 };
		ti.set_position(root_position());
		while (ti.peek() <= peek_last()) {
			switch (*ti.return_current_and_advance()) {
				case '[':
				case '{':
					count++;
					break;
				case ']':
				case '}':
					count--;
					break;
				default:
					break;
			}
		}
		return count == 0;
	}


	// GCC 7 warns when the first line of this function is inlined away into oblivion due to the caller
	// relating depth and parent_depth, which is a desired effect. The warning does not show up if the
	// skip_child() function is not marked inline).
	inline ErrorCode json_iterator::skip_child(depth_t parent_depth) noexcept {
		if (depth() <= parent_depth) {
			return ErrorCode::Success;
		}
		switch (*return_current_and_advance()) {
			// TODO consider whether matching braces is a requirement: if non-matching braces indicates
			// *missing* braces, then future lookups are not in the object/arrays they think they are,
			// violating the rule "validate enough structure that the user can be confident they are
			// looking at the right values."
			// PERF TODO we can eliminate the switch here with a lookup of how much to add to depth

			// For the first open array/object in a value, we've already incremented depth, so keep it the same
			// We never stop at colon, but if we did, it wouldn't affect depth
			case '[':
			case '{':
			case ':':
				break;
			// If there is a comma, we have just finished a value in an array/object, and need to get back in
			case ',':
				break;
			// ] or } means we just finished a value and need to jump out of the array/object
			case ']':
			case '}':
				_depth--;
				if (depth() <= parent_depth) {
					return ErrorCode::Success;
				}
				break;
			case '"':
				if (*peek() == ':') {
					// We are at a key!!!
					// This might happen if you just started an object and you skip it immediately.
					// Performance note: it would be nice to get rid of this check as it is somewhat
					// expensive.
					// https://github.com/simdjson/simdjson/issues/1742
					return_current_and_advance();// eat up the ':'
					break;// important!!!
				}
			// Anything else must be a scalar value
			default:
				// For the first scalar, we will have incremented depth already, so we decrement it here.
				_depth--;
				if (depth() <= parent_depth) {
					return ErrorCode::Success;
				}
				break;
		}

		// Now that we've considered the first value, we only increment/decrement for arrays/objects
		while (position() < end_position()) {
			switch (*return_current_and_advance()) {
				case '[':
				case '{':
					_depth++;
					break;
				// TODO consider whether matching braces is a requirement: if non-matching braces indicates
				// *missing* braces, then future lookups are not in the object/arrays they think they are,
				// violating the rule "validate enough structure that the user can be confident they are
				// looking at the right values."
				// PERF TODO we can eliminate the switch here with a lookup of how much to add to depth
				case ']':
				case '}':
					_depth--;
					if (depth() <= parent_depth) {
						return ErrorCode::Success;
					}
					break;
				default:
					break;
			}
		}

		return report_error(ErrorCode::TapeError, "not enough close braces");
	}

	inline bool json_iterator::at_root() const noexcept {
		return position() == root_position();
	}

	inline bool json_iterator::is_single_token() const noexcept {
		return parser->getStructuralIndexCount() == 1;
	}

	inline bool json_iterator::streaming() const noexcept {
		return _streaming;
	}

	inline token_position json_iterator::root_position() const noexcept {
		return _root;
	}

	inline void json_iterator::assert_at_document_depth() const noexcept {
	}

	inline void json_iterator::assert_at_root() const noexcept {
#ifndef SIMDJSON_CLANG_VISUAL_STUDIO
		// Under Visual Studio, the next SIMDJSON_ASSUME fails with: the argument
		// has side effects that will be discarded.
#endif
	}

	inline void json_iterator::assert_more_tokens(uint32_t required_tokens) const noexcept {
		assert_valid_position(token._position + required_tokens - 1);
	}

	inline void json_iterator::assert_valid_position(token_position position) const noexcept {
	}

	inline bool json_iterator::at_end() const noexcept {
		return position() == end_position();
	}
	inline token_position json_iterator::end_position() const noexcept {
		size_t n_structural_indexes{ parser->getTapeLength() };
		return &parser->getTape()[n_structural_indexes];
	}

	inline std::string json_iterator::to_string() const noexcept {
		if (!is_alive()) {
			return "dead json_iterator instance";
		}
		const char* current_structural = reinterpret_cast<const char*>(token.peek());
		return std::string("json_iterator [ depth : ") + std::to_string(_depth) + std::string(", structural : '") +
			std::string(current_structural, 1) + std::string("', offset : ") + std::to_string(token.current_offset()) + std::string("', error : ") +
			std::to_string(( int32_t )error) + std::string(" ]");
	}

	inline const char* json_iterator::current_location() noexcept {
		if (!is_alive()) {// Unrecoverable error
			if (!at_root()) {
				return reinterpret_cast<const char*>(token.peek(-1));
			} else {
				return reinterpret_cast<const char*>(token.peek());
			}
		}
		if (at_end()) {
			return nullptr;
		}
		return reinterpret_cast<const char*>(token.peek());
	}

	inline bool json_iterator::is_alive() const noexcept {
		return parser;
	}

	inline void json_iterator::abandon() noexcept {
		parser = nullptr;
		_depth = 0;
	}

	inline const uint8_t* json_iterator::return_current_and_advance() noexcept {
#if SIMDJSON_CHECK_EOF
		assert_more_tokens();
#endif// SIMDJSON_CHECK_EOF
		return token.return_current_and_advance();
	}

	inline const uint8_t* json_iterator::unsafe_pointer() const noexcept {
		// deliberately done without safety guard:
		return token.peek(0);
	}

	inline const uint8_t* json_iterator::peek(int32_t delta) const noexcept {
#if SIMDJSON_CHECK_EOF
		assert_more_tokens(delta + 1);
#endif// SIMDJSON_CHECK_EOF
		return token.peek(delta);
	}

	inline uint32_t json_iterator::peek_length(int32_t delta) const noexcept {
#if SIMDJSON_CHECK_EOF
		assert_more_tokens(delta + 1);
#endif// #if SIMDJSON_CHECK_EOF
		return token.peek_length(delta);
	}

	inline const uint8_t* json_iterator::peek(token_position position) const noexcept {
		// todo: currently we require end-of-string buffering, but the following
		// assert_valid_position should be turned on if/when we lift that condition.
		// assert_valid_position(position);
		// This is almost surely related to SIMDJSON_CHECK_EOF but given that SIMDJSON_CHECK_EOF
		// is ON by default, we have no choice but to disable it for real with a comment.
		return token.peek(position);
	}

	inline uint32_t json_iterator::peek_length(token_position position) const noexcept {
#if SIMDJSON_CHECK_EOF
		assert_valid_position(position);
#endif// SIMDJSON_CHECK_EOF
		return token.peek_length(position);
	}

	inline token_position json_iterator::last_position() const noexcept {
		// The following line fails under some compilers...
		// SIMDJSON_ASSUME(parser->implementation->n_structural_indexes > 0);
		// since it has side-effects.
		size_t n_structural_indexes{ parser->getTapeLength() };
		return &parser->getTape()[n_structural_indexes - 1];
	}
	inline const uint8_t* json_iterator::peek_last() const noexcept {
		return token.peek(last_position());
	}

	inline void json_iterator::ascend_to(depth_t parent_depth) noexcept {
		parent_depth >= 0 && parent_depth < INT32_MAX - 1;
		_depth = parent_depth;
	}

	inline void json_iterator::descend_to(depth_t child_depth) noexcept {
		child_depth >= 1 && child_depth < INT32_MAX;
		_depth = child_depth;
	}

	inline depth_t json_iterator::depth() const noexcept {
		return _depth;
	}

	inline uint8_t*& json_iterator::string_buf_loc() noexcept {
		return _string_buf_loc;
	}

	inline ErrorCode json_iterator::report_error(ErrorCode _error, const char* message) noexcept {
		error = _error;
		return error;
	}

	inline token_position json_iterator::position() const noexcept {
		return token.position();
	}

	inline void json_iterator::reenter_child(token_position position, depth_t child_depth) noexcept {
		child_depth >= 1 && child_depth < INT32_MAX;
		token.set_position(position);
		_depth = child_depth;
	}

#if SIMDJSON_DEVELOPMENT_CHECKS

	inline token_position json_iterator::start_position(depth_t depth) const noexcept {
		SIMDJSON_ASSUME(size_t(depth) < parser->max_depth());
		return size_t(depth) < parser->max_depth() ? parser->start_positions[depth] : 0;
	}

	inline void json_iterator::set_start_position(depth_t depth, token_position position) noexcept {
		SIMDJSON_ASSUME(size_t(depth) < parser->max_depth());
		if (size_t(depth) < parser->max_depth()) {
			parser->start_positions[depth] = position;
		}
	}

#endif


	inline ErrorCode json_iterator::optional_error(ErrorCode _error, const char* message) noexcept {
		return _error;
	}

	template<int N>
	inline bool json_iterator::copy_to_buffer(const uint8_t* json, uint32_t max_len, uint8_t (&tmpbuf)[N]) noexcept {
		// Let us guard against silly cases:
		if ((N < max_len) || (N == 0)) {
			return false;
		}
		// Truncate whitespace to fit the buffer.
		if (max_len > N - 1) {
			// if (jsoncharutils::is_not_structural_or_whitespace(json[N-1])) { return false; }
			max_len = N - 1;
		}

		// Copy to the buffer.
		std::memcpy(tmpbuf, json, max_len);
		tmpbuf[max_len] = ' ';
		return true;
	}

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
		uint32_t* nextStructural{ nullptr };
		const uint8_t* stringView{ nullptr };
		SimdJsonValue* masterParser;
		uint32_t depth{};

		inline ErrorCode walkDocument(TapeBuilder& visitor);

		inline JsonIterator(SimdJsonValue* masterParserNew, size_t start_structural_index);

		inline const uint8_t* peek() const noexcept;

		inline const uint8_t* advance() noexcept;

		inline size_t remainingLen() const noexcept;

		inline bool atEof() const noexcept;

		inline bool atBeginning() const noexcept;

		inline uint8_t lastStructural() const noexcept;

		inline ErrorCode visitRootPrimitive(TapeBuilder& visitor, const uint8_t* value);
		inline ErrorCode visitPrimitive(TapeBuilder& visitor, const uint8_t* value);
	};

	inline JsonIterator::JsonIterator(SimdJsonValue* masterParserNew, size_t start_structural_index)
		: nextStructural(masterParserNew->getStructuralIndexes()), stringView{ masterParserNew->getStringView() }, masterParser{ masterParserNew } {};

	inline const uint8_t* JsonIterator::peek() const noexcept {
		return &this->stringView[*this->nextStructural];
	}

	inline const uint8_t* JsonIterator::advance() noexcept {
		return &this->stringView[*this->nextStructural++];
	}

	inline size_t JsonIterator::remainingLen() const noexcept {
		return this->masterParser->getStructuralIndexCount() - *this->nextStructural;
	}

	inline bool JsonIterator::atEof() const noexcept {
		return this->nextStructural == &this->masterParser->getStructuralIndexes()[this->masterParser->getStructuralIndexCount() - 1];
	}

	inline bool JsonIterator::atBeginning() const noexcept {
		return this->nextStructural == this->masterParser->getStructuralIndexes();
	}

	inline uint8_t JsonIterator::lastStructural() const noexcept {
		return this->stringView[this->masterParser->getStructuralIndexes()[this->masterParser->getStructuralIndexCount() - 1]];
	}

	struct TapeWriter {
		TapeWriter(uint64_t* ptr) {
			this->nextTapeLocation = ptr;
		}
		uint64_t* nextTapeLocation;
		inline void appendS64(int64_t&& value) noexcept;
		inline void appendU64(uint64_t&& value) noexcept;
		inline void appendDouble(double&& value) noexcept;
		inline void append(uint64_t&& val, TapeType t) noexcept;
		inline void skip() noexcept;
		inline void skipLargeInteger() noexcept;
		inline void skipDouble() noexcept;
		inline static void write(uint64_t& tape_loc, uint64_t&& val, TapeType t) noexcept;

	  private:
		template<typename T> inline void append2(uint64_t&& val, T&& val2, TapeType t) noexcept;
	};

	inline void TapeWriter::appendS64(int64_t&& value) noexcept {
		append2(0, value, TapeType::Int64);
	}

	inline void TapeWriter::appendU64(uint64_t&& value) noexcept {
		append2(0, value, TapeType::Uint64);
	}

	inline void TapeWriter::appendDouble(double&& value) noexcept {
		append2(0, value, TapeType::Double);
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

	inline void TapeWriter::append(uint64_t&& val, TapeType t) noexcept {
		*this->nextTapeLocation = val | ((uint64_t(uint8_t(t))) << 56);
		this->nextTapeLocation++;
	}

	template<typename T> inline void TapeWriter::append2(uint64_t&& val, T&& val2, TapeType t) noexcept {
		append(std::move(val), std::move(t));
		static_assert(sizeof(val2) == sizeof(*this->nextTapeLocation), "Type is not 64 *theBits!");
		memcpy(this->nextTapeLocation, &val2, sizeof(val2));
		this->nextTapeLocation++;
	}

	inline void TapeWriter::write(uint64_t& tape_loc, uint64_t&& val, TapeType t) noexcept {
		tape_loc = val | ((uint64_t(uint8_t(t))) << 56);
	}

	struct TapeBuilder {
		inline static ErrorCode parseDocument(SimdJsonValue& masterParser);

		inline ErrorCode visitDocumentStart(JsonIterator& iter) noexcept;

		inline ErrorCode visitDocumentEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitArrayStart(JsonIterator& iter) noexcept;

		inline ErrorCode visitArrayEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitEmptyArray(JsonIterator& iter) noexcept;

		inline ErrorCode visitObjectStart(JsonIterator& iter) noexcept;

		inline ErrorCode visitKey(JsonIterator& iter, const uint8_t* key) noexcept;

		inline ErrorCode visitObjectEnd(JsonIterator& iter) noexcept;

		inline ErrorCode visitEmptyObject(JsonIterator& iter) noexcept;

		inline ErrorCode visitPrimitive(JsonIterator& iter, const uint8_t* value);

		inline ErrorCode visitRootPrimitive(JsonIterator& iter, const uint8_t* value);

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
		size_t& currentTapeLength;

		inline TapeBuilder(SimdJsonValue& doc) noexcept;

		inline uint32_t nextTapeIndex(JsonIterator& iter) const noexcept;
		inline ErrorCode startContainer(JsonIterator& iter) noexcept;
		inline ErrorCode endContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline ErrorCode emptyContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline uint8_t* onStartString(JsonIterator& iter) noexcept;
		inline ErrorCode onEndString(uint8_t* dst) noexcept;
	};

	inline ErrorCode TapeBuilder::parseDocument(SimdJsonValue& masterParser) {
		JsonIterator iter(&masterParser, 0);
		TapeBuilder builder(masterParser);
		return iter.walkDocument(builder);
	}

	inline ErrorCode TapeBuilder::visitRootPrimitive(JsonIterator& iter, const uint8_t* value) {
		return iter.visitRootPrimitive(*this, value);
	}
	inline ErrorCode TapeBuilder::visitPrimitive(JsonIterator& iter, const uint8_t* value) {
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
		this->tape.append(startTapeIndex, TapeType::Root);
		TapeWriter::write(iter.masterParser->getTape()[startTapeIndex], nextTapeIndex(iter), TapeType::Root);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitKey(JsonIterator& iter, const uint8_t* key) noexcept {
		return visitString(iter, key);
	}

	inline ErrorCode TapeBuilder::incrementCount(JsonIterator& iter) noexcept {
		iter.masterParser->getOpenContainers()[iter.depth].count++;
		return ErrorCode::Success;
	}

	inline TapeBuilder::TapeBuilder(SimdJsonValue& doc) noexcept
		: tape{ doc.getTape() }, currentStringBufferLocation{ doc.getStringBuffer() }, currentTapeLength(doc.getTapeLength()){};

	inline ErrorCode TapeBuilder::visitString(JsonIterator& iter, const uint8_t* value) noexcept {
		uint8_t* dst01 = reinterpret_cast<uint8_t*>(onStartString(iter));
		auto dst02 = StringParser::parseString(value + 1ull, dst01, (*iter.nextStructural + 1ull) - (*iter.nextStructural));
		if (dst02 == nullptr) {
			return ErrorCode::StringError;
		}
		onEndString(dst02);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootString(JsonIterator& iter, const uint8_t* value) noexcept {
		return visitString(iter, value);
	}

	inline ErrorCode TapeBuilder::visitNumber(JsonIterator& iter, const uint8_t* value) noexcept {
		return NumberParser::parseNumber<TapeWriter>(reinterpret_cast<const uint8_t*>(value), this->tape);
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
		if (!StringParser::isValidTrueAtom(value)) {
			return ErrorCode::TAtomError;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootTrueAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!StringParser::isValidTrueAtom(value)) {
			return ErrorCode::NAtomError;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!StringParser::isValidFalseAtom(value)) {
			return ErrorCode::FAtomError;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootFalseAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!StringParser::isValidFalseAtom(value)) {
			return ErrorCode::FAtomError;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitNullAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!StringParser::isValidNullAtom(value)) {
			return ErrorCode::NAtomError;
		}
		this->tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootNullAtom(JsonIterator& iter, const uint8_t* value) noexcept {
		if (!StringParser::isValidNullAtom(value)) {
			return ErrorCode::NAtomError;
		}
		this->tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline uint32_t TapeBuilder::nextTapeIndex(JsonIterator& iter) const noexcept {
		return uint32_t(tape.nextTapeLocation - iter.masterParser->getTape());
	}

	inline ErrorCode TapeBuilder::emptyContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept {
		auto startIndex = nextTapeIndex(iter);
		this->tape.append(startIndex + 2ull, start);
		this->tape.append(startIndex, end);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::startContainer(JsonIterator& iter) noexcept {
		iter.masterParser->getOpenContainers()[iter.depth].tapeIndex = nextTapeIndex(iter);
		iter.masterParser->getOpenContainers()[iter.depth].count = 0;
		this->tape.skip();
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::endContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept {
		const uint32_t startTapeIndex = iter.masterParser->getOpenContainers()[iter.depth].tapeIndex;
		this->tape.append(startTapeIndex, end);
		const uint32_t count = iter.masterParser->getOpenContainers()[iter.depth].count;
		const uint32_t cntsat = count > 0xFFFFFF ? 0xFFFFFF : count;
		TapeWriter::write(iter.masterParser->getTape()[startTapeIndex], nextTapeIndex(iter) | (uint64_t(cntsat) << 32), start);
		return ErrorCode::Success;
	}

	inline uint8_t* TapeBuilder::onStartString(JsonIterator& iter) noexcept {
		this->tape.append(currentStringBufferLocation - iter.masterParser->getStringBuffer(), TapeType::String);
		return this->currentStringBufferLocation + sizeof(uint32_t);
	}

	inline ErrorCode TapeBuilder::onEndString(uint8_t* dst) noexcept {
		uint32_t strLength = uint32_t(dst - (reinterpret_cast<uint8_t*>(this->currentStringBufferLocation) + sizeof(uint32_t)));
		memcpy(this->currentStringBufferLocation, &strLength, sizeof(uint32_t));
		*dst = 0;
		this->currentStringBufferLocation = dst + 1;
		return ErrorCode::Success;
	}

	class EnumStringConverter {
	  public:
		EnumStringConverter(ErrorCode error) {
			this->code = error;
		}
		operator std::string() {
			switch (this->code) {
				case ErrorCode::Empty: {
					return "Empty";
				}
				case ErrorCode::TapeError: {
					return "Tape Error";
				}
				case ErrorCode::DepthError: {
					return "Depth Error";
				}
				case ErrorCode::Success: {
					return "Success";
				}
				case ErrorCode::ParseError: {
					return "Parse Error";
				}
				case ErrorCode::StringError: {
					return "String Error";
				}
				case ErrorCode::TAtomError: {
					return "TAtom Error";
				}
				case ErrorCode::FAtomError: {
					return "FAtom Error";
				}
				case ErrorCode::NAtomError: {
					return "NAtom Error";
				}
				case ErrorCode::MemAlloc: {
					return "Mem Alloc";
				}
				case ErrorCode::InvalidNumber: {
					return "Invalid Number";
				}
				default: {
					return "Unknown Error";
				}
			}
		}

	  protected:
		ErrorCode code{};
	};

	inline ErrorCode JsonIterator::walkDocument(TapeBuilder& visitor) {
		if (atEof()) {
			return ErrorCode::Empty;
		}
		visitor.visitDocumentStart(*this);
		this->masterParser->getTapeLength()++;
		{
			auto value = this->advance();

			switch (*value) {
				case '{':
					if (*this->peek() == '}') {
						this->advance();
						visitor.visitEmptyObject(*this);
						break;
					}
					goto Object_Begin;
				case '[':
					if (*this->peek() == ']') {
						this->advance();
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
		this->depth++;
		this->masterParser->getTapeLength()++;
		if (this->depth >= masterParser->getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		this->masterParser->getIsArray()[this->depth] = false;
		visitor.visitObjectStart(*this);

		{
			auto key = this->advance();
			if (*key != '"') {
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
			}
			visitor.visitKey(*this, key);
			visitor.incrementCount(*this);
		}

	Object_Field: {
		auto newValue = *this->advance();
		if (newValue != ':') {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
				", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
		{
			auto value = this->advance();
			this->masterParser->getTapeLength()++;
			switch (*value) {
				case '{':
					if (*this->peek() == '}') {
						this->advance();
						visitor.visitEmptyObject(*this);
						break;
					}
					goto Object_Begin;
				case '[':
					if (*this->peek() == ']') {
						this->advance();
						visitor.visitEmptyArray(*this);
						break;
					}
					goto Array_Begin;
				default:
					if (auto resultCode = visitor.visitPrimitive(*this, value); resultCode != ErrorCode::Success) {
						throw JsonifierException{ "Sorry, but you've encountered the following error: " +
							std::string{ static_cast<EnumStringConverter>(resultCode) } +
							", at the following index into the string: " + std::to_string(*this->nextStructural) };
					}
					break;
			}
		}
	}
		
	Object_Continue: {
			auto newValue = *this->advance();
		this->masterParser->getTapeLength()++;
		switch (newValue) {
			case ',':
				visitor.incrementCount(*this);
				{
					auto key = this->advance();
					if (*key != '"') {
						throw JsonifierException{ "Sorry, but you've encountered the following error: " + 
							std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
							", at the following index into the string: " + std::to_string(*this->nextStructural) };
					}
					visitor.visitKey(*this, key);
				}
				goto Object_Field;
			case '}':
				visitor.visitObjectEnd(*this);
				goto Scope_End;
			default:
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}

	}
	
	Scope_End : {
		this->depth--;
		
		if (this->depth == 0) {
			goto Document_End;
		}
		if (masterParser->getIsArray()[this->depth]) {
			goto Array_Continue;
		}
		this->masterParser->getTapeLength()++;
		goto Object_Continue;
	}

	Array_Begin : {
		this->depth++;
		this->masterParser->getTapeLength()++;
		if (this->depth >= masterParser->getMaxDepth()) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::DepthError) } +
				", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
		this->masterParser->getIsArray()[this->depth] = true;

		visitor.visitArrayStart(*this);
		visitor.incrementCount(*this);
	}

	Array_Value : {
		auto value = this->advance();
		this->masterParser->getTapeLength()++;
		{
			switch (*value) {
				case '{':
					if (*this->peek() == '}') {
						this->advance();
						visitor.visitEmptyObject(*this);
						break;
					}
					goto Object_Begin;
				case '[':
					if (*this->peek() == ']') {
						this->advance();
						visitor.visitEmptyArray(*this);
						break;
					}
					goto Array_Begin;
				default:
					if (auto resultCode = visitor.visitPrimitive(*this, value); resultCode != ErrorCode::Success) {
						throw JsonifierException{ "Sorry, but you've encountered the following error: " +
							std::string{ static_cast<EnumStringConverter>(resultCode) } +
							", at the following index into the string: " + std::to_string(*this->nextStructural) };
					}
					break;
			}
		}
	}

	Array_Continue : {
		auto newValue = *this->advance();
		this->masterParser->getTapeLength()++;
		{
			switch (newValue) {
				case ',':
					visitor.incrementCount(*this);
					goto Array_Value;
				case ']':
					visitor.visitArrayEnd(*this);
					goto Scope_End;
				default:
					return ErrorCode::TapeError;
			}
		}
	}

	Document_End: {
		visitor.visitDocumentEnd(*this);
		this->masterParser->getTapeLength()++;

		auto nextStructuralIndex = uint32_t(this->nextStructural - &this->masterParser->getStructuralIndexes()[0]);

		if (nextStructuralIndex != this->masterParser->getStructuralIndexCount()) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
				", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
	}
		return ErrorCode::Success;
	}

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
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
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
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
	}
	
	JsonParser SimdJsonValue::getJsonData(std::string& string) {
		this->generateJsonEvents(reinterpret_cast<uint8_t*>(string.data()), string.size());
		if (TapeBuilder::parseDocument(*this) != ErrorCode::Success) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } + ", at the following index into the string: " };
		}

		return JsonParser{ this->getTape(), this->getTapeLength(), this->stringBuffer.get() };
	}

};
