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

		uint64_t parseJsonUint() {
			return uint64_t{};
		}

		uint64_t* findKey(const char* keyToFind) {

		}

		int64_t parseJsonInt() {
			int64_t returnData{};
			return returnData;
		}

		JsonParser() noexcept = default; 

		std::string_view parseJsonString() {
			std::string_view  returnData{};
			return returnData;
		}

		double parseJsonFloat() {
			double returnData{};
			return returnData;
		}

		bool parseJsonBool() {
			bool returnData{};
			return returnData;
		}

		nullptr_t parseJsonNull() {
			nullptr_t returnData{};
			return returnData;
		}

		size_t size() {
			size_t returnValue{};
			return returnValue;
		}

		JsonParser(uint64_t* tapePtrsNew, size_t count, uint8_t* stringBufferNew) {
			this->tapeIter = TapeIterator{ stringBufferNew, tapePtrsNew, count };
		}

		template<typename OTy> inline OTy getValue();

		template<> inline double getValue() {
			return this->parseJsonFloat();
		}

		template<> inline float getValue() {
			return this->parseJsonFloat();
		}

		template<> inline bool getValue() {
			return this->parseJsonBool();
		}

		template<> inline uint64_t getValue() {
			return this->parseJsonUint();
		}

		template<> inline uint32_t getValue() {
			return this->parseJsonUint();
		}

		template<> inline uint16_t getValue() {
			return this->parseJsonUint();
		}

		template<> inline uint8_t getValue() {
			return this->parseJsonUint();
		}

		template<> inline int64_t getValue() {
			return this->parseJsonInt();
		}

		template<> inline int32_t getValue() {
			return this->parseJsonInt();
		}

		template<> inline int16_t getValue() {
			return this->parseJsonInt();
		}

		template<> inline int8_t getValue() {
			return this->parseJsonInt();
		}

		template<> inline std::string getValue() {
			return static_cast<std::string>(this->parseJsonString());
		}

		template<> inline std::string_view getValue() {
			return this->parseJsonString();
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

		JsonType getType() {
			JsonType returnValue{};
			return returnValue;
		}

		template<> inline JsonParser getValue() {
			return std::move(*this);
		}

		inline JsonParser& operator[](const std::string& key) {			
			return *this;
		};

		inline JsonParser(ErrorCode error) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " + std::to_string(( int32_t )error) };
		}

	  protected:
		TapeIterator tapeIter{};
		size_t currentDepth{};
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

	class json_iterator;

	enum class number_type {
		floating_point_number = 1,/// a binary64 number
		signed_integer,/// a signed integer that fits in a 64-bit word using two's complement
		unsigned_integer/// a positive integer larger or equal to 1<<63
	};

	struct number {
		/**
   * return the automatically determined type of
   * the number: number_type::floating_point_number,
   * number_type::signed_integer or number_type::unsigned_integer.
   *
   *    enum class number_type {
   *        floating_point_number=1, /// a binary64 number
   *        signed_integer,          /// a signed integer that fits in a 64-bit word using two's complement
   *        unsigned_integer         /// a positive integer larger or equal to 1<<63
   *    };
   */
		inline number_type get_number_type() const noexcept;
		/**
   * return true if the automatically determined type of
   * the number is number_type::unsigned_integer.
   */
		inline bool is_uint64() const noexcept;
		/**
   * return the value as a uint64_t, only valid if is_uint64() is true.
   */
		inline uint64_t get_uint64() const noexcept;
		inline operator uint64_t() const noexcept;

		/**
   * return true if the automatically determined type of
   * the number is number_type::signed_integer.
   */
		inline bool is_int64() const noexcept;
		/**
   * return the value as a int64_t, only valid if is_int64() is true.
   */
		inline int64_t get_int64() const noexcept;
		inline operator int64_t() const noexcept;


		/**
   * return true if the automatically determined type of
   * the number is number_type::floating_point_number.
   */
		inline bool is_double() const noexcept;
		/**
   * return the value as a double, only valid if is_double() is true.
   */
		inline double get_double() const noexcept;
		inline operator double() const noexcept;

		/**
   * Convert the number to a double. Though it always succeed, the conversion
   * may be lossy if the number cannot be represented exactly.
   */
		inline double as_double() const noexcept;


	  protected:
		/**
   * The next block of declaration is designed so that we can call the number parsing
   * functions on a number type. They are protected and should never be used outside
   * of the core simdjson library.
   */
		friend class value_iterator;
		/** Store a signed 64-bit value to the number. */
		inline void append_s64(int64_t value) noexcept;
		/** Store an unsigned 64-bit value to the number. */
		inline void append_u64(uint64_t value) noexcept;
		/** Store a double value to the number. */
		inline void append_double(double value) noexcept;
		/** Specifies that the value is a double, but leave it undefined. */
		inline void skip_double() noexcept;
		/**
   * End of friend declarations.
   */

		/**
   * Our attributes are a union type (size = 64 bits)
   * followed by a type indicator.
   */
		union {
			double floating_point_number;
			int64_t signed_integer;
			uint64_t unsigned_integer;
		} payload{ 0 };
		number_type type{ number_type::signed_integer };
	};

	template<typename T> struct simdjson_result_base : protected std::pair<T, ErrorCode> {
		/**
   * Create a new empty result with error = UNINITIALIZED.
   */
		inline simdjson_result_base() noexcept;

		/**
   * Create a new error result.
   */
		inline simdjson_result_base(ErrorCode error) noexcept;

		/**
   * Create a new successful result.
   */
		inline simdjson_result_base(T&& value) noexcept;

		/**
   * Create a new result with both things (use if you don't want to branch when creating the result).
   */
		inline simdjson_result_base(T&& value, ErrorCode error) noexcept;

		/**
   * Move the value and the error to the provided variables.
   *
   * @param value The variable to assign the value to. May not be set if there is an error.
   * @param error The variable to assign the error to. Set to SUCCESS if there is no error.
   */
		inline void tie(T& value, ErrorCode& error) && noexcept;

		/**
   * Move the value to the provided variable.
   *
   * @param value The variable to assign the value to. May not be set if there is an error.
   */
		inline ErrorCode get(T& value) && noexcept;

		/**
   * The error.
   */
		inline ErrorCode error() const noexcept;

#if SIMDJSON_EXCEPTIONS

		/**
   * Get the result value.
   *
   * @throw simdjson_error if there was an error.
   */
		inline T& value() & noexcept(false);

		/**
   * Take the result value (move it).
   *
   * @throw simdjson_error if there was an error.
   */
		inline T&& value() && noexcept(false);

		/**
   * Take the result value (move it).
   *
   * @throw simdjson_error if there was an error.
   */
		inline T&& take_value() && noexcept(false);

		/**
   * Cast to the value (will throw on error).
   *
   * @throw simdjson_error if there was an error.
   */
		inline operator T&&() && noexcept(false);
#endif// SIMDJSON_EXCEPTIONS

		/**
   * Get the result value. This function is safe if and only
   * the error() method returns a value that evaluates to false.
   */
		inline const T& value_unsafe() const& noexcept;

		/**
   * Take the result value (move it). This function is safe if and only
   * the error() method returns a value that evaluates to false.
   */
		inline T&& value_unsafe() && noexcept;

	};// struct simdjson_result_base

// namespace internal

/**
 * The result of a simdjson operation that could fail.
 *
 * Gives the option of reading error codes, or throwing an exception by casting to the desired result.
 */
template<typename T> struct simdjson_result : public simdjson_result_base<T> {
	/**
   * @private Create a new empty result with error = UNINITIALIZED.
   */
	inline simdjson_result() noexcept;
	/**
   * @private Create a new error result.
   */
	inline simdjson_result(T&& value) noexcept;
	/**
   * @private Create a new successful result.
   */
	inline simdjson_result(ErrorCode ErrorCode) noexcept;
	/**
   * @private Create a new result with both things (use if you don't want to branch when creating the result).
   */
	inline simdjson_result(T&& value, ErrorCode error) noexcept;

	/**
   * Move the value and the error to the provided variables.
   *
   * @param value The variable to assign the value to. May not be set if there is an error.
   * @param error The variable to assign the error to. Set to SUCCESS if there is no error.
   */
	inline void tie(T& value, ErrorCode& error) && noexcept;

	/**
   * Move the value to the provided variable.
   *
   * @param value The variable to assign the value to. May not be set if there is an error.
   */
	inline ErrorCode get(T& value) && noexcept;

	/**
   * The error.
   */
	inline ErrorCode error() const noexcept;

#if SIMDJSON_EXCEPTIONS

	/**
   * Get the result value.
   *
   * @throw simdjson_error if there was an error.
   */
	inline T& value() & noexcept(false);

	/**
   * Take the result value (move it).
   *
   * @throw simdjson_error if there was an error.
   */
	inline T&& value() && noexcept(false);

	/**
   * Take the result value (move it).
   *
   * @throw simdjson_error if there was an error.
   */
	inline T&& take_value() && noexcept(false);

	/**
   * Cast to the value (will throw on error).
   *
   * @throw simdjson_error if there was an error.
   */
	inline operator T&&() && noexcept(false);
#endif// SIMDJSON_EXCEPTIONS

	/**
   * Get the result value. This function is safe if and only
   * the error() method returns a value that evaluates to false.
   */
	inline const T& value_unsafe() const& noexcept;

	/**
   * Take the result value (move it). This function is safe if and only
   * the error() method returns a value that evaluates to false.
   */
	inline T&& value_unsafe() && noexcept;
};
using token_position = uint32_t *;
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
   * Normally we have root == parser->implementation->structural_indexes.get()
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

	template<int N>  inline bool copy_to_buffer(const uint8_t* json, uint32_t max_len, uint8_t (&tmpbuf)[N]) noexcept;

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
	inline simdjson_result<const char*> current_location() noexcept;

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

class value_iterator {
  protected:
	/** The underlying JSON iterator */
	json_iterator* _json_iter{};
	/** The depth of this value */
	depth_t _depth{};
	/**
   * The starting token index for this value
   */
	token_position _start_position{};

  public:
	inline value_iterator() noexcept = default;

	/**
   * Denote that we're starting a document.
   */
	inline void start_document() noexcept;

	/**
   * Skips a non-iterated or partially-iterated JSON value, whether it is a scalar, array or object.
   *
   * Optimized for scalars.
   */
	inline ErrorCode skip_child() noexcept;

	/**
   * Tell whether the iterator is at the EOF mark
   */
	inline bool at_end() const noexcept;

	/**
   * Tell whether the iterator is at the start of the value
   */
	inline bool at_start() const noexcept;

	/**
   * Tell whether the value is open--if the value has not been used, or the array/object is still open.
   */
	inline bool is_open() const noexcept;

	/**
   * Tell whether the value is at an object's first field (just after the {).
   */
	inline bool at_first_field() const noexcept;

	/**
   * Abandon all iteration.
   */
	inline void abandon() noexcept;

	/**
   * Get the child value as a value_iterator.
   */
	inline value_iterator child_value() const noexcept;

	/**
   * Get the depth of this value.
   */
	inline int32_t depth() const noexcept;

	/**
   * Get the JSON type of this value.
   *
   * @error TAPE_ERROR when the JSON value is a bad token like "}" "," or "alse".
   */
	inline simdjson_result<JsonType> type() const noexcept;

	/**
   * @addtogroup object Object iteration
   *
   * Methods to iterate and find object fields. These methods generally *assume* the value is
   * actually an object; the caller is responsible for keeping track of that fact.
   *
   * @{
   */

	/**
   * Start an object iteration.
   *
   * @returns Whether the object had any fields (returns false for empty).
   * @error INCORRECT_TYPE if there is no opening {
   */
	inline simdjson_result<bool> start_object() noexcept;
	/**
   * Start an object iteration from the root.
   *
   * @returns Whether the object had any fields (returns false for empty).
   * @error INCORRECT_TYPE if there is no opening {
   * @error TAPE_ERROR if there is no matching } at end of document
   */
	inline simdjson_result<bool> start_root_object() noexcept;

	/**
   * Start an object iteration after the user has already checked and moved past the {.
   *
   * Does not move the iterator unless the object is empty ({}).
   *
   * @returns Whether the object had any fields (returns false for empty).
   * @error INCOMPLETE_ARRAY_OR_OBJECT If there are no more tokens (implying the *parent*
   *        array or object is incomplete).
   */
	inline simdjson_result<bool> started_object() noexcept;
	/**
   * Start an object iteration from the root, after the user has already checked and moved past the {.
   *
   * Does not move the iterator unless the object is empty ({}).
   *
   * @returns Whether the object had any fields (returns false for empty).
   * @error INCOMPLETE_ARRAY_OR_OBJECT If there are no more tokens (implying the *parent*
   *        array or object is incomplete).
   */
	inline simdjson_result<bool> started_root_object() noexcept;

	/**
   * Moves to the next field in an object.
   *
   * Looks for , and }. If } is found, the object is finished and the iterator advances past it.
   * Otherwise, it advances to the next value.
   *
   * @return whether there is another field in the object.
   * @error TAPE_ERROR If there is a comma missing between fields.
   * @error TAPE_ERROR If there is a comma, but not enough tokens remaining to have a key, :, and value.
   */
	inline simdjson_result<bool> has_next_field() noexcept;

	/**
   * Get the current field's key.
   */

	/**
   * Pass the : in the field and move to its value.
   */
	inline ErrorCode field_value() noexcept;

	/**
   * Find the next field with the given key.
   *
   * Assumes you have called next_field() or otherwise matched the previous value.
   *
   * This means the iterator must be sitting at the next key:
   *
   * ```
   * { "a": 1, "b": 2 }
   *           ^
   * ```
   *
   * Key is *raw JSON,* meaning it will be matched against the verbatim JSON without attempting to
   * unescape it. This works well for typical ASCII and UTF-8 keys (almost all of them), but may
   * fail to match some keys with escapes (\u, \n, etc.).
   */
	inline ErrorCode find_field(const std::string_view key) noexcept;

	/**
   * Find the next field with the given key, *without* unescaping. This assumes object order: it
   * will not find the field if it was already passed when looking for some *other* field.
   *
   * Assumes you have called next_field() or otherwise matched the previous value.
   *
   * This means the iterator must be sitting at the next key:
   *
   * ```
   * { "a": 1, "b": 2 }
   *           ^
   * ```
   *
   * Key is *raw JSON,* meaning it will be matched against the verbatim JSON without attempting to
   * unescape it. This works well for typical ASCII and UTF-8 keys (almost all of them), but may
   * fail to match some keys with escapes (\u, \n, etc.).
   */
	inline simdjson_result<bool> find_field_raw(const std::string_view key) noexcept;

	/**
   * Find the field with the given key without regard to order, and *without* unescaping.
   *
   * This is an unordered object lookup: if the field is not found initially, it will cycle around and scan from the beginning.
   *
   * Assumes you have called next_field() or otherwise matched the previous value.
   *
   * This means the iterator must be sitting at the next key:
   *
   * ```
   * { "a": 1, "b": 2 }
   *           ^
   * ```
   *
   * Key is *raw JSON,* meaning it will be matched against the verbatim JSON without attempting to
   * unescape it. This works well for typical ASCII and UTF-8 keys (almost all of them), but may
   * fail to match some keys with escapes (\u, \n, etc.).
   */
	inline simdjson_result<bool> find_field_unordered_raw(const std::string_view key) noexcept;

	/** @} */

	/**
   * @addtogroup array Array iteration
   * Methods to iterate over array elements. These methods generally *assume* the value is actually
   * an object; the caller is responsible for keeping track of that fact.
   * @{
   */

	/**
   * Check for an opening [ and start an array iteration.
   *
   * @returns Whether the array had any elements (returns false for empty).
   * @error INCORRECT_TYPE If there is no [.
   */
	inline simdjson_result<bool> start_array() noexcept;
	/**
   * Check for an opening [ and start an array iteration while at the root.
   *
   * @returns Whether the array had any elements (returns false for empty).
   * @error INCORRECT_TYPE If there is no [.
   * @error TAPE_ERROR if there is no matching ] at end of document
   */
	inline simdjson_result<bool> start_root_array() noexcept;

	/**
   * Start an array iteration, after the user has already checked and moved past the [.
   *
   * Does not move the iterator unless the array is empty ([]).
   *
   * @returns Whether the array had any elements (returns false for empty).
   * @error INCOMPLETE_ARRAY_OR_OBJECT If there are no more tokens (implying the *parent*
   *        array or object is incomplete).
   */
	inline simdjson_result<bool> started_array() noexcept;
	/**
   * Start an array iteration from the root, after the user has already checked and moved past the [.
   *
   * Does not move the iterator unless the array is empty ([]).
   *
   * @returns Whether the array had any elements (returns false for empty).
   * @error INCOMPLETE_ARRAY_OR_OBJECT If there are no more tokens (implying the *parent*
   *        array or object is incomplete).
   */
	inline simdjson_result<bool> started_root_array() noexcept;

	/**
   * Moves to the next element in an array.
   *
   * Looks for , and ]. If ] is found, the array is finished and the iterator advances past it.
   * Otherwise, it advances to the next value.
   *
   * @return Whether there is another element in the array.
   * @error TAPE_ERROR If there is a comma missing between elements.
   */
	inline simdjson_result<bool> has_next_element() noexcept;

	/**
   * Get a child value iterator.
   */
	inline value_iterator child() const noexcept;

	/** @} */

	/**
   * @defgroup scalar Scalar values
   * @addtogroup scalar
   * @{
   */

	inline simdjson_result<std::string_view> get_string() noexcept;
	inline simdjson_result<uint64_t> get_uint64() noexcept;
	inline simdjson_result<uint64_t> get_uint64_in_string() noexcept;
	inline simdjson_result<int64_t> get_int64() noexcept;
	inline simdjson_result<int64_t> get_int64_in_string() noexcept;
	inline simdjson_result<double> get_double() noexcept;
	inline simdjson_result<double> get_double_in_string() noexcept;
	inline simdjson_result<bool> get_bool() noexcept;
	inline simdjson_result<bool> is_null() noexcept;
	inline bool is_negative() noexcept;
	inline simdjson_result<bool> is_integer() noexcept;
	inline simdjson_result<number_type> get_number_type() noexcept;
	inline simdjson_result<number> get_number() noexcept;

	inline simdjson_result<std::string_view> get_root_string() noexcept;
	inline simdjson_result<uint64_t> get_root_uint64() noexcept;
	inline simdjson_result<uint64_t> get_root_uint64_in_string() noexcept;
	inline simdjson_result<int64_t> get_root_int64() noexcept;
	inline simdjson_result<int64_t> get_root_int64_in_string() noexcept;
	inline simdjson_result<double> get_root_double() noexcept;
	inline simdjson_result<double> get_root_double_in_string() noexcept;
	inline simdjson_result<bool> get_root_bool() noexcept;
	inline bool is_root_negative() noexcept;
	inline simdjson_result<bool> is_root_integer() noexcept;
	inline simdjson_result<number_type> get_root_number_type() noexcept;
	inline simdjson_result<number> get_root_number() noexcept;
	inline bool is_root_null() noexcept;

	inline ErrorCode error() const noexcept;
	inline uint8_t*& string_buf_loc() noexcept;
	inline const json_iterator& json_iter() const noexcept;
	inline json_iterator& json_iter() noexcept;

	inline void assert_is_valid() const noexcept;
	inline bool is_valid() const noexcept;

	/** @} */
  protected:
	/**
   * Restarts an array iteration.
   * @returns Whether the array has any elements (returns false for empty).
   */
	inline simdjson_result<bool> reset_array() noexcept;
	/**
   * Restarts an object iteration.
   * @returns Whether the object has any fields (returns false for empty).
   */
	inline simdjson_result<bool> reset_object() noexcept;
	/**
   * move_at_start(): moves us so that we are pointing at the beginning of
   * the container. It updates the index so that at_start() is true and it
   * syncs the depth. The user can then create a new container instance.
   *
   * Usage: used with value::count_elements().
   **/
	inline void move_at_start() noexcept;

	/**
   * move_at_container_start(): moves us so that we are pointing at the beginning of
   * the container so that assert_at_container_start() passes.
   *
   * Usage: used with reset_array() and reset_object().
   **/
	inline void move_at_container_start() noexcept;
	/* Useful for debugging and logging purposes. */
	inline std::string to_string() const noexcept;
	inline value_iterator(json_iterator* json_iter, depth_t depth, token_position start_index) noexcept;

	inline simdjson_result<bool> parse_null(const uint8_t* json) const noexcept;
	inline simdjson_result<bool> parse_bool(const uint8_t* json) const noexcept;
	inline const uint8_t* peek_start() const noexcept;
	inline uint32_t peek_start_length() const noexcept;

	/**
   * The general idea of the advance_... methods and the peek_* methods
   * is that you first peek and check that you have desired type. If you do,
   * and only if you do, then you advance.
   *
   * We used to unconditionally advance. But this made reasoning about our
   * current state difficult.
   * Suppose you always advance. Look at the 'value' matching the key
   * "shadowable" in the following example...
   *
   * ({"globals":{"a":{"shadowable":[}}}})
   *
   * If the user thinks it is a Boolean and asks for it, then we check the '[',
   * decide it is not a Boolean, but still move into the next character ('}'). Now
   * we are left pointing at '}' right after a '['. And we have not yet reported
   * an error, only that we do not have a Boolean.
   *
   * If, instead, you just stand your ground until it is content that you know, then
   * you will only even move beyond the '[' if the user tells you that you have an
   * array. So you will be at the '}' character inside the array and, hopefully, you
   * will then catch the error because an array cannot start with '}', but the code
   * processing Boolean values does not know this.
   *
   * So the contract is: first call 'peek_...' and then call 'advance_...' only
   * if you have determined that it is a type you can handle.
   *
   * Unfortunately, it makes the code more verbose, longer and maybe more error prone.
   */

	inline void advance_scalar(const char* type) noexcept;
	inline void advance_root_scalar(const char* type) noexcept;
	inline void advance_non_root_scalar(const char* type) noexcept;

	inline const uint8_t* peek_scalar(const char* type) noexcept;
	inline const uint8_t* peek_root_scalar(const char* type) noexcept;
	inline const uint8_t* peek_non_root_scalar(const char* type) noexcept;


	inline ErrorCode start_container(uint8_t start_char, const char* incorrect_type_message, const char* type) noexcept;
	inline ErrorCode end_container() noexcept;

	/**
   * Advance to a place expecting a value (increasing depth).
   *
   * @return The current token (the one left behind).
   * @error TAPE_ERROR If the document ended early.
   */
	inline simdjson_result<const uint8_t*> advance_to_value() noexcept;

	inline ErrorCode incorrect_type_error(const char* message) const noexcept;
	inline ErrorCode error_unless_more_tokens(uint32_t tokens = 1) const noexcept;

	inline bool is_at_start() const noexcept;
	/**
   * is_at_iterator_start() returns true on an array or object after it has just been
   * created, whether the instance is empty or not.
   *
   * Usage: used by array::begin() in debug mode (SIMDJSON_DEVELOPMENT_CHECKS)
   */
	inline bool is_at_iterator_start() const noexcept;

	/**
   * Assuming that we are within an object, this returns true if we
   * are pointing at a key.
   *
   * Usage: the skip_child() method should never be used while we are pointing
   * at a key inside an object.
   */
	inline bool is_at_key() const noexcept;

	inline void assert_at_start() const noexcept;
	inline void assert_at_container_start() const noexcept;
	inline void assert_at_root() const noexcept;
	inline void assert_at_child() const noexcept;
	inline void assert_at_next() const noexcept;
	inline void assert_at_non_root_start() const noexcept;

	/** Get the starting position of this value */
	inline token_position start_position() const noexcept;

	/** @copydoc ErrorCode json_iterator::position() const noexcept; */
	inline token_position position() const noexcept;
	/** @copydoc ErrorCode json_iterator::end_position() const noexcept; */
	inline token_position last_position() const noexcept;
	/** @copydoc ErrorCode json_iterator::end_position() const noexcept; */
	inline token_position end_position() const noexcept;
	/** @copydoc ErrorCode json_iterator::report_error(ErrorCode error, const char *message) noexcept; */
	inline ErrorCode report_error(ErrorCode error, const char* message) noexcept;

	friend class document;
	friend class object;
	friend class array;
	friend class value;
};

class object_iterator {
  public:
	/**
   * Create a new invalid object_iterator.
   *
   * Exists so you can declare a variable and later assign to it before use.
   */
	inline object_iterator() noexcept = default;

	//
	// Iterator interface
	//

	// Reads key and value, yielding them to the user.
	// MUST ONLY BE CALLED ONCE PER ITERATION.
	// Assumes it's being compared with the end. true if depth < iter->depth.
	inline bool operator==(const object_iterator&) const noexcept;
	// Assumes it's being compared with the end. true if depth >= iter->depth.
	inline bool operator!=(const object_iterator&) const noexcept;
	// Checks for ']' and ','
	inline object_iterator& operator++() noexcept;

  private:
	/**
   * The underlying JSON iterator.
   *
   * PERF NOTE: expected to be elided in favor of the parent document: this is set when the object
   * is first used, and never changes afterwards.
   */
	value_iterator iter{};

	inline object_iterator(const value_iterator& iter) noexcept;
	friend struct simdjson_result<object_iterator>;
	friend class object;
};

class object {
  public:
	/**
   * Create a new invalid object.
   *
   * Exists so you can declare a variable and later assign to it before use.
   */
	inline object() noexcept = default;

	inline simdjson_result<object_iterator> begin() noexcept;
	inline simdjson_result<object_iterator> end() noexcept;
	/**
   * Look up a field by name on an object (order-sensitive).
   *
   * The following code reads z, then y, then x, and thus will not retrieve x or y if fed the
   * JSON `{ "x": 1, "y": 2, "z": 3 }`:
   *
   * ```c++
   * simdjson::ondemand::parser parser;
   * auto obj = parser.parse(R"( { "x": 1, "y": 2, "z": 3 } )"_padded);
   * double z = obj.find_field("z");
   * double y = obj.find_field("y");
   * double x = obj.find_field("x");
   * ```
   * If you have multiple fields with a matching key ({"x": 1,  "x": 1}) be mindful
   * that only one field is returned.
   *
   * **Raw Keys:** The lookup will be done against the *raw* key, and will not unescape keys.
   * e.g. `object["a"]` will match `{ "a": 1 }`, but will *not* match `{ "\u0061": 1 }`.
   *
   * You must consume the fields on an object one at a time. A request for a new key
   * invalidates previous field values: it makes them unsafe. The value instance you get
   * from  `content["bids"]` becomes invalid when you call `content["asks"]`. The array
   * given by content["bids"].get_array() should not be accessed after you have called
   * content["asks"].get_array(). You can detect such mistakes by first compiling and running
   * the code in Debug mode (or with the macro `SIMDJSON_DEVELOPMENT_CHECKS` set to 1): an
   * OUT_OF_ORDER_ITERATION error is generated.
   *
   * You are expected to access keys only once. You should access the value corresponding to a
   * key a single time. Doing object["mykey"].to_string() and then again object["mykey"].to_string()
   * is an error.
   *
   * @param key The key to look up.
   * @returns The value of the field, or NO_SUCH_FIELD if the field is not in the object.
   */
	inline simdjson_result<value> find_field(std::string_view key) & noexcept;
	/** @overload inline simdjson_result<value> find_field(std::string_view key) & noexcept; */
	inline simdjson_result<value> find_field(std::string_view key) && noexcept;

	/**
   * Look up a field by name on an object, without regard to key order.
   *
   * **Performance Notes:** This is a bit less performant than find_field(), though its effect varies
   * and often appears negligible. It starts out normally, starting out at the last field; but if
   * the field is not found, it scans from the beginning of the object to see if it missed it. That
   * missing case has a non-cache-friendly bump and lots of extra scanning, especially if the object
   * in question is large. The fact that the extra code is there also bumps the executable size.
   *
   * It is the default, however, because it would be highly surprising (and hard to debug) if the
   * default behavior failed to look up a field just because it was in the wrong order--and many
   * APIs assume this. Therefore, you must be explicit if you want to treat objects as out of order.
   *
   * Use find_field() if you are sure fields will be in order (or are willing to treat it as if the
   * field wasn't there when they aren't).
   *
   * If you have multiple fields with a matching key ({"x": 1,  "x": 1}) be mindful
   * that only one field is returned.
   *
   * You must consume the fields on an object one at a time. A request for a new key
   * invalidates previous field values: it makes them unsafe. The value instance you get
   * from  `content["bids"]` becomes invalid when you call `content["asks"]`. The array
   * given by content["bids"].get_array() should not be accessed after you have called
   * content["asks"].get_array(). You can detect such mistakes by first compiling and running
   * the code in Debug mode (or with the macro `SIMDJSON_DEVELOPMENT_CHECKS` set to 1): an
   * OUT_OF_ORDER_ITERATION error is generated.
   *
   * You are expected to access keys only once. You should access the value corresponding to a key
   * a single time. Doing object["mykey"].to_string() and then again object["mykey"].to_string() is an error.
   *
   * @param key The key to look up.
   * @returns The value of the field, or NO_SUCH_FIELD if the field is not in the object.
   */
	inline simdjson_result<value> find_field_unordered(std::string_view key) & noexcept;
	/** @overload inline simdjson_result<value> find_field_unordered(std::string_view key) & noexcept; */
	inline simdjson_result<value> find_field_unordered(std::string_view key) && noexcept;
	/** @overload inline simdjson_result<value> find_field_unordered(std::string_view key) & noexcept; */
	inline simdjson_result<value> operator[](std::string_view key) & noexcept;
	/** @overload inline simdjson_result<value> find_field_unordered(std::string_view key) & noexcept; */
	inline simdjson_result<value> operator[](std::string_view key) && noexcept;

	/**
   * Get the value associated with the given JSON pointer. We use the RFC 6901
   * https://tools.ietf.org/html/rfc6901 standard, interpreting the current node
   * as the root of its own JSON document.
   *
   *   ondemand::parser parser;
   *   auto json = R"({ "foo": { "a": [ 10, 20, 30 ] }})"_padded;
   *   auto doc = parser.iterate(json);
   *   doc.at_pointer("/foo/a/1") == 20
   *
   * It is allowed for a key to be the empty string:
   *
   *   ondemand::parser parser;
   *   auto json = R"({ "": { "a": [ 10, 20, 30 ] }})"_padded;
   *   auto doc = parser.iterate(json);
   *   doc.at_pointer("//a/1") == 20
   *
   * Note that at_pointer() called on the document automatically calls the document's rewind
   * method between each call. It invalidates all previously accessed arrays, objects and values
   * that have not been consumed. Yet it is not the case when calling at_pointer on an object
   * instance: there is no rewind and no invalidation.
   *
   * You may call at_pointer more than once on an object, but each time the pointer is advanced
   * to be within the value matched by the key indicated by the JSON pointer query. Thus any preceding
   * key (as well as the current key) can no longer be used with following JSON pointer calls.
   *
   * Also note that at_pointer() relies on find_field() which implies that we do not unescape keys when matching.
   *
   * @return The value associated with the given JSON pointer, or:
   *         - NO_SUCH_FIELD if a field does not exist in an object
   *         - INDEX_OUT_OF_BOUNDS if an array index is larger than an array length
   *         - INCORRECT_TYPE if a non-integer is used to access an array
   *         - INVALID_JSON_POINTER if the JSON pointer is invalid and cannot be parsed
   */
	inline simdjson_result<value> at_pointer(std::string_view json_pointer) noexcept;

	/**
   * Reset the iterator so that we are pointing back at the
   * beginning of the object. You should still consume values only once even if you
   * can iterate through the object more than once. If you unescape a string within
   * the object more than once, you have unsafe code. Note that rewinding an object
   * means that you may need to reparse it anew: it is not a free operation.
   *
   * @returns true if the object contains some elements (not empty)
   */
	inline simdjson_result<bool> reset() & noexcept;
	/**
   * This method scans the beginning of the object and checks whether the
   * object is empty.
   * The runtime complexity is constant time. After
   * calling this function, if successful, the object is 'rewinded' at its
   * beginning as if it had never been accessed. If the JSON is malformed (e.g.,
   * there is a missing comma), then an error is returned and it is no longer
   * safe to continue.
   */
	inline simdjson_result<bool> is_empty() & noexcept;
	/**
   * This method scans the object and counts the number of key-value pairs.
   * The count_fields method should always be called before you have begun
   * iterating through the object: it is expected that you are pointing at
   * the beginning of the object.
   * The runtime complexity is linear in the size of the object. After
   * calling this function, if successful, the object is 'rewinded' at its
   * beginning as if it had never been accessed. If the JSON is malformed (e.g.,
   * there is a missing comma), then an error is returned and it is no longer
   * safe to continue.
   *
   * To check that an object is empty, it is more performant to use
   * the is_empty() method.
   *
   * Performance hint: You should only call count_fields() as a last
   * resort as it may require scanning the document twice or more.
   */
	inline simdjson_result<size_t> count_fields() & noexcept;
	/**
   * Consumes the object and returns a string_view instance corresponding to the
   * object as represented in JSON. It points inside the original byte array containing
   * the JSON document.
   */
	inline simdjson_result<std::string_view> raw_json() noexcept;

  protected:
	/**
   * Go to the end of the object, no matter where you are right now.
   */
	inline ErrorCode consume() noexcept;
	static inline simdjson_result<object> start(value_iterator& iter) noexcept;
	static inline simdjson_result<object> start_root(value_iterator& iter) noexcept;
	static inline simdjson_result<object> started(value_iterator& iter) noexcept;
	static inline object resume(const value_iterator& iter) noexcept;
	inline object(const value_iterator& iter) noexcept;

	inline ErrorCode find_field_raw(const std::string_view key) noexcept;

	value_iterator iter{};

	friend class value;
	friend class document;
	friend struct simdjson_result<object>;
};

class array_iterator;
class value {
  public:
	/**
   * Create a new invalid value.
   *
   * Exists so you can declare a variable and later assign to it before use.
   */
	inline value() noexcept = default;

	/**
   * Get this value as the given type.
   *
   * Supported types: object, array, raw_json_string, string_view, uint64_t, int64_t, double, bool
   *
   * You may use get_double(), get_bool(), get_uint64(), get_int64(),
   * get_object(), get_array(), get_raw_json_string(), or get_string() instead.
   *
   * @returns A value of the given type, parsed from the JSON.
   * @returns INCORRECT_TYPE If the JSON value is not the given type.
   */
	template<typename T> inline simdjson_result<T> get() noexcept {
		// Unless the simdjson library provides an inline implementation, calling this method should
		// immediately fail.
		static_assert(!sizeof(T), "The get method with given type is not implemented by the simdjson library.");
	}

	/**
   * Get this value as the given type.
   *
   * Supported types: object, array, raw_json_string, string_view, uint64_t, int64_t, double, bool
   *
   * @param out This is set to a value of the given type, parsed from the JSON. If there is an error, this may not be initialized.
   * @returns INCORRECT_TYPE If the JSON value is not an object.
   * @returns SUCCESS If the parse succeeded and the out parameter was set to the value.
   */
	template<typename T> inline ErrorCode get(T& out) noexcept;

	/**
   * Cast this JSON value to an array.
   *
   * @returns An object that can be used to iterate the array.
   * @returns INCORRECT_TYPE If the JSON value is not an array.
   */
	inline simdjson_result<array> get_array() noexcept;

	/**
   * Cast this JSON value to an object.
   *
   * @returns An object that can be used to look up or iterate fields.
   * @returns INCORRECT_TYPE If the JSON value is not an object.
   */
	inline simdjson_result<object> get_object() noexcept;

	/**
   * Cast this JSON value to an unsigned integer.
   *
   * @returns A unsigned 64-bit integer.
   * @returns INCORRECT_TYPE If the JSON value is not a 64-bit unsigned integer.
   */
	inline simdjson_result<uint64_t> get_uint64() noexcept;

	/**
   * Cast this JSON value (inside string) to a unsigned integer.
   *
   * @returns A unsigned 64-bit integer.
   * @returns INCORRECT_TYPE If the JSON value is not a 64-bit unsigned integer.
   */
	inline simdjson_result<uint64_t> get_uint64_in_string() noexcept;

	/**
   * Cast this JSON value to a signed integer.
   *
   * @returns A signed 64-bit integer.
   * @returns INCORRECT_TYPE If the JSON value is not a 64-bit integer.
   */
	inline simdjson_result<int64_t> get_int64() noexcept;

	/**
   * Cast this JSON value (inside string) to a signed integer.
   *
   * @returns A signed 64-bit integer.
   * @returns INCORRECT_TYPE If the JSON value is not a 64-bit integer.
   */
	inline simdjson_result<int64_t> get_int64_in_string() noexcept;

	/**
   * Cast this JSON value to a double.
   *
   * @returns A double.
   * @returns INCORRECT_TYPE If the JSON value is not a valid floating-point number.
   */
	inline simdjson_result<double> get_double() noexcept;

	/**
   * Cast this JSON value (inside string) to a double
   *
   * @returns A double.
   * @returns INCORRECT_TYPE If the JSON value is not a valid floating-point number.
   */
	inline simdjson_result<double> get_double_in_string() noexcept;

	/**
   * Cast this JSON value to a string.
   *
   * The string is guaranteed to be valid UTF-8.
   *
   * Equivalent to get<std::string_view>().
   *
   * Important: a value should be consumed once. Calling get_string() twice on the same value
   * is an error.
   *
   * @returns An UTF-8 string. The string is stored in the parser and will be invalidated the next
   *          time it parses a document or when it is destroyed.
   * @returns INCORRECT_TYPE if the JSON value is not a string.
   */
	inline simdjson_result<std::string_view> get_string() noexcept;

	/**
   * Cast this JSON value to a raw_json_string.
   *
   * The string is guaranteed to be valid UTF-8, and may have escapes in it (e.g. \\ or \n).
   *
   * @returns A pointer to the raw JSON for the given string.
   * @returns INCORRECT_TYPE if the JSON value is not a string.
   */

	/**
   * Cast this JSON value to a bool.
   *
   * @returns A bool value.
   * @returns INCORRECT_TYPE if the JSON value is not true or false.
   */
	inline simdjson_result<bool> get_bool() noexcept;

	/**
   * Checks if this JSON value is null. If and only if the value is
   * null, then it is consumed (we advance). If we find a token that
   * begins with 'n' but is not 'null', then an error is returned.
   *
   * @returns Whether the value is null.
   * @returns INCORRECT_TYPE If the JSON value begins with 'n' and is not 'null'.
   */
	inline simdjson_result<bool> is_null() noexcept;

#if SIMDJSON_EXCEPTIONS
	/**
   * Cast this JSON value to an array.
   *
   * @returns An object that can be used to iterate the array.
   * @exception simdjson_error(INCORRECT_TYPE) If the JSON value is not an array.
   */
	inline operator array() noexcept(false);
	/**
   * Cast this JSON value to an object.
   *
   * @returns An object that can be used to look up or iterate fields.
   * @exception simdjson_error(INCORRECT_TYPE) If the JSON value is not an object.
   */
	inline operator object() noexcept(false);
	/**
   * Cast this JSON value to an unsigned integer.
   *
   * @returns A signed 64-bit integer.
   * @exception simdjson_error(INCORRECT_TYPE) If the JSON value is not a 64-bit unsigned integer.
   */
	inline operator uint64_t() noexcept(false);
	/**
   * Cast this JSON value to a signed integer.
   *
   * @returns A signed 64-bit integer.
   * @exception simdjson_error(INCORRECT_TYPE) If the JSON value is not a 64-bit integer.
   */
	inline operator int64_t() noexcept(false);
	/**
   * Cast this JSON value to a double.
   *
   * @returns A double.
   * @exception simdjson_error(INCORRECT_TYPE) If the JSON value is not a valid floating-point number.
   */
	inline operator double() noexcept(false);
	/**
   * Cast this JSON value to a string.
   *
   * The string is guaranteed to be valid UTF-8.
   *
   * Equivalent to get<std::string_view>().
   *
   * @returns An UTF-8 string. The string is stored in the parser and will be invalidated the next
   *          time it parses a document or when it is destroyed.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON value is not a string.
   */
	inline operator std::string_view() noexcept(false);
	/**
   * Cast this JSON value to a raw_json_string.
   *
   * The string is guaranteed to be valid UTF-8, and may have escapes in it (e.g. \\ or \n).
   *
   * @returns A pointer to the raw JSON for the given string.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON value is not a string.
   */
	inline operator raw_json_string() noexcept(false);
	/**
   * Cast this JSON value to a bool.
   *
   * @returns A bool value.
   * @exception simdjson_error(INCORRECT_TYPE) if the JSON value is not true or false.
   */
	inline operator bool() noexcept(false);
#endif

	/**
   * Begin array iteration.
   *
   * Part of the std::iterable interface.
   *
   * @returns INCORRECT_TYPE If the JSON value is not an array.
   */
	inline simdjson_result<array_iterator> begin() & noexcept;
	/**
   * Sentinel representing the end of the array.
   *
   * Part of the std::iterable interface.
   */
	inline simdjson_result<array_iterator> end() & noexcept;
	/**
   * This method scans the array and counts the number of elements.
   * The count_elements method should always be called before you have begun
   * iterating through the array: it is expected that you are pointing at
   * the beginning of the array.
   * The runtime complexity is linear in the size of the array. After
   * calling this function, if successful, the array is 'rewinded' at its
   * beginning as if it had never been accessed. If the JSON is malformed (e.g.,
   * there is a missing comma), then an error is returned and it is no longer
   * safe to continue.
   *
   * Performance hint: You should only call count_elements() as a last
   * resort as it may require scanning the document twice or more.
   */
	inline simdjson_result<size_t> count_elements() & noexcept;
	/**
   * This method scans the object and counts the number of key-value pairs.
   * The count_fields method should always be called before you have begun
   * iterating through the object: it is expected that you are pointing at
   * the beginning of the object.
   * The runtime complexity is linear in the size of the object. After
   * calling this function, if successful, the object is 'rewinded' at its
   * beginning as if it had never been accessed. If the JSON is malformed (e.g.,
   * there is a missing comma), then an error is returned and it is no longer
   * safe to continue.
   *
   * To check that an object is empty, it is more performant to use
   * the is_empty() method on the object instance.
   *
   * Performance hint: You should only call count_fields() as a last
   * resort as it may require scanning the document twice or more.
   */
	inline simdjson_result<size_t> count_fields() & noexcept;
	/**
   * Get the value at the given index in the array. This function has linear-time complexity.
   * This function should only be called once on an array instance since the array iterator is not reset between each call.
   *
   * @return The value at the given index, or:
   *         - INDEX_OUT_OF_BOUNDS if the array index is larger than an array length
   */
	inline simdjson_result<value> at(size_t index) noexcept;
	/**
   * Look up a field by name on an object (order-sensitive).
   *
   * The following code reads z, then y, then x, and thus will not retrieve x or y if fed the
   * JSON `{ "x": 1, "y": 2, "z": 3 }`:
   *
   * ```c++
   * simdjson::ondemand::parser parser;
   * auto obj = parser.parse(R"( { "x": 1, "y": 2, "z": 3 } )"_padded);
   * double z = obj.find_field("z");
   * double y = obj.find_field("y");
   * double x = obj.find_field("x");
   * ```
   * If you have multiple fields with a matching key ({"x": 1,  "x": 1}) be mindful
   * that only one field is returned.

   * **Raw Keys:** The lookup will be done against the *raw* key, and will not unescape keys.
   * e.g. `object["a"]` will match `{ "a": 1 }`, but will *not* match `{ "\u0061": 1 }`.
   *
   * @param key The key to look up.
   * @returns The value of the field, or NO_SUCH_FIELD if the field is not in the object.
   */
	inline simdjson_result<value> find_field(std::string_view key) noexcept;
	/** @overload inline simdjson_result<value> find_field(std::string_view key) noexcept; */
	inline simdjson_result<value> find_field(const char* key) noexcept;

	/**
   * Look up a field by name on an object, without regard to key order.
   *
   * **Performance Notes:** This is a bit less performant than find_field(), though its effect varies
   * and often appears negligible. It starts out normally, starting out at the last field; but if
   * the field is not found, it scans from the beginning of the object to see if it missed it. That
   * missing case has a non-cache-friendly bump and lots of extra scanning, especially if the object
   * in question is large. The fact that the extra code is there also bumps the executable size.
   *
   * It is the default, however, because it would be highly surprising (and hard to debug) if the
   * default behavior failed to look up a field just because it was in the wrong order--and many
   * APIs assume this. Therefore, you must be explicit if you want to treat objects as out of order.
   *
   * If you have multiple fields with a matching key ({"x": 1,  "x": 1}) be mindful
   * that only one field is returned.
   *
   * Use find_field() if you are sure fields will be in order (or are willing to treat it as if the
   * field wasn't there when they aren't).
   *
   * @param key The key to look up.
   * @returns The value of the field, or NO_SUCH_FIELD if the field is not in the object.
   */
	inline simdjson_result<value> find_field_unordered(std::string_view key) noexcept;
	/** @overload inline simdjson_result<value> find_field_unordered(std::string_view key) noexcept; */
	inline simdjson_result<value> find_field_unordered(const char* key) noexcept;
	/** @overload inline simdjson_result<value> find_field_unordered(std::string_view key) noexcept; */
	inline simdjson_result<value> operator[](std::string_view key) noexcept;
	/** @overload inline simdjson_result<value> find_field_unordered(std::string_view key) noexcept; */
	inline simdjson_result<value> operator[](const char* key) noexcept;

	/**
   * Get the type of this JSON value. It does not validate or consume the value.
   * E.g., you must still call "is_null()" to check that a value is null even if
   * "type()" returns JsonType::null.
   *
   * NOTE: If you're only expecting a value to be one type (a typical case), it's generally
   * better to just call .get_double, .get_string, etc. and check for INCORRECT_TYPE (or just
   * let it throw an exception).
   *
   * @return The type of JSON value (JsonType::array, JsonType::object, JsonType::string,
   *     JsonType::number, JsonType::boolean, or JsonType::null).
   * @error TAPE_ERROR when the JSON value is a bad token like "}" "," or "alse".
   */
	inline simdjson_result<JsonType> type() noexcept;

	/**
   * Checks whether the value is a scalar (string, number, null, Boolean).
   * Returns false when there it is an array or object.
   *
   * @returns true if the type is string, number, null, Boolean
   * @error TAPE_ERROR when the JSON value is a bad token like "}" "," or "alse".
   */
	inline simdjson_result<bool> is_scalar() noexcept;

	/**
   * Checks whether the value is a negative number.
   *
   * @returns true if the number if negative.
   */
	inline bool is_negative() noexcept;
	/**
   * Checks whether the value is an integer number. Note that
   * this requires to partially parse the number string. If
   * the value is determined to be an integer, it may still
   * not parse properly as an integer in subsequent steps
   * (e.g., it might overflow).
   *
   * Performance note: if you call this function systematically
   * before parsing a number, you may have fallen for a performance
   * anti-pattern.
   *
   * @returns true if the number if negative.
   */
	inline simdjson_result<bool> is_integer() noexcept;
	/**
   * Determine the number type (integer or floating-point number) as quickly
   * as possible. This function does not fully validate the input. It is
   * useful when you only need to classify the numbers, without parsing them.
   *
   * If you are planning to retrieve the value or you need full validation,
   * consider using the get_number() method instead: it will fully parse
   * and validate the input, and give you access to the type:
   * get_number().get_number_type().
   *
   * get_number_type() is number_type::unsigned_integer if we have
   * an integer greater or equal to 9223372036854775808
   * get_number_type() is number_type::signed_integer if we have an
   * integer that is less than 9223372036854775808
   * Otherwise, get_number_type() has value number_type::floating_point_number
   *
   * This function requires processing the number string, but it is expected
   * to be faster than get_number().get_number_type() because it is does not
   * parse the number value.
   *
   * @returns the type of the number
   */
	inline simdjson_result<number_type> get_number_type() noexcept;

	/**
   * Attempt to parse an ondemand::number. An ondemand::number may
   * contain an integer value or a floating-point value, the simdjson
   * library will autodetect the type. Thus it is a dynamically typed
   * number. Before accessing the value, you must determine the detected
   * type.
   *
   * number.get_number_type() is number_type::signed_integer if we have
   * an integer in [-9223372036854775808,9223372036854775808)
   * You can recover the value by calling number.get_int64() and you
   * have that number.is_int64() is true.
   *
   * number.get_number_type() is number_type::unsigned_integer if we have
   * an integer in [9223372036854775808,18446744073709551616)
   * You can recover the value by calling number.get_uint64() and you
   * have that number.is_uint64() is true.
   *
   * Otherwise, number.get_number_type() has value number_type::floating_point_number
   * and we have a binary64 number.
   * You can recover the value by calling number.get_double() and you
   * have that number.is_double() is true.
   *
   * You must check the type before accessing the value: it is an error
   * to call "get_int64()" when number.get_number_type() is not
   * number_type::signed_integer and when number.is_int64() is false.
   *
   * Performance note: this is designed with performance in mind. When
   * calling 'get_number()', you scan the number string only once, determining
   * efficiently the type and storing it in an efficient manner.
   */
	 inline simdjson_result<number> get_number() noexcept;


	/**
   * Get the raw JSON for this token.
   *
   * The string_view will always point into the input buffer.
   *
   * The string_view will start at the beginning of the token, and include the entire token
   * *as well as all spaces until the next token (or EOF).* This means, for example, that a
   * string token always begins with a " and is always terminated by the final ", possibly
   * followed by a number of spaces.
   *
   * The string_view is *not* null-terminated. However, if this is a scalar (string, number,
   * boolean, or null), the character after the end of the string_view is guaranteed to be
   * a non-space token.
   *
   * Tokens include:
   * - {
   * - [
   * - "a string (possibly with UTF-8 or backslashed characters like \\\")".
   * - -1.2e-100
   * - true
   * - false
   * - null
   */
	inline std::string_view raw_json_token() noexcept;

	/**
   * Returns the current location in the document if in bounds.
   */
	inline simdjson_result<const char*> current_location() noexcept;

	/**
   * Returns the current depth in the document if in bounds.
   *
   * E.g.,
   *  0 = finished with document
   *  1 = document root value (could be [ or {, not yet known)
   *  2 = , or } inside root array/object
   *  3 = key or value inside root array/object.
   */
	inline int32_t current_depth() const noexcept;

	/**
   * Get the value associated with the given JSON pointer.  We use the RFC 6901
   * https://tools.ietf.org/html/rfc6901 standard.
   *
   *   ondemand::parser parser;
   *   auto json = R"({ "foo": { "a": [ 10, 20, 30 ] }})"_padded;
   *   auto doc = parser.iterate(json);
   *   doc.at_pointer("/foo/a/1") == 20
   *
   * It is allowed for a key to be the empty string:
   *
   *   ondemand::parser parser;
   *   auto json = R"({ "": { "a": [ 10, 20, 30 ] }})"_padded;
   *   auto doc = parser.iterate(json);
   *   doc.at_pointer("//a/1") == 20
   *
   * Note that at_pointer() called on the document automatically calls the document's rewind
   * method between each call. It invalidates all previously accessed arrays, objects and values
   * that have not been consumed.
   *
   * Calling at_pointer() on non-document instances (e.g., arrays and objects) is not
   * standardized (by RFC 6901). We provide some experimental support for JSON pointers
   * on non-document instances.  Yet it is not the case when calling at_pointer on an array
   * or an object instance: there is no rewind and no invalidation.
   *
   * You may only call at_pointer on an array after it has been created, but before it has
   * been first accessed. When calling at_pointer on an array, the pointer is advanced to
   * the location indicated by the JSON pointer (in case of success). It is no longer possible
   * to call at_pointer on the same array.
   *
   * You may call at_pointer more than once on an object, but each time the pointer is advanced
   * to be within the value matched by the key indicated by the JSON pointer query. Thus any preceding
   * key (as well as the current key) can no longer be used with following JSON pointer calls.
   *
   * Also note that at_pointer() relies on find_field() which implies that we do not unescape keys when matching
   *
   * @return The value associated with the given JSON pointer, or:
   *         - NO_SUCH_FIELD if a field does not exist in an object
   *         - INDEX_OUT_OF_BOUNDS if an array index is larger than an array length
   *         - INCORRECT_TYPE if a non-integer is used to access an array
   *         - INVALID_JSON_POINTER if the JSON pointer is invalid and cannot be parsed
   */
	inline simdjson_result<value> at_pointer(std::string_view json_pointer) noexcept;

  protected:
	/**
   * Create a value.
   */
	inline value(const value_iterator& iter) noexcept;

	/**
   * Skip this value, allowing iteration to continue.
   */
	inline void skip() noexcept;

	/**
   * Start a value at the current position.
   *
   * (It should already be started; this is just a self-documentation method.)
   */
	static inline value start(const value_iterator& iter) noexcept;

	/**
   * Resume a value.
   */
	static inline value resume(const value_iterator& iter) noexcept;

	/**
   * Get the object, starting or resuming it as necessary
   */
	inline simdjson_result<object> start_or_resume_object() noexcept;

	// inline void log_value(const char *type) const noexcept;
	// inline void log_error(const char *message) const noexcept;

	value_iterator iter{};

	friend class document;
	friend class array_iterator;
	friend class field;
	friend class object;
	friend struct simdjson_result<value>;
};

	class array_iterator {
	  public:
		/** Create a new, invalid array iterator. */
		inline array_iterator() noexcept = default;

		//
		// Iterator interface
		//

		/**
   * Get the current element.
   *
   * Part of the std::iterator interface.
   */
		inline simdjson_result<value> operator*() noexcept;// MUST ONLY BE CALLED ONCE PER ITERATION.
		/**
   * Check if we are at the end of the JSON.
   *
   * Part of the std::iterator interface.
   *
   * @return true if there are no more elements in the JSON array.
   */
		inline bool operator==(const array_iterator&) const noexcept;
		/**
   * Check if there are more elements in the JSON array.
   *
   * Part of the std::iterator interface.
   *
   * @return true if there are more elements in the JSON array.
   */
		inline bool operator!=(const array_iterator&) const noexcept;
		/**
   * Move to the next element.
   *
   * Part of the std::iterator interface.
   */
		inline array_iterator& operator++() noexcept;

	  private:
		value_iterator iter{};

		inline array_iterator(const value_iterator& iter) noexcept;

		friend class array;
		friend class value;
		friend struct simdjson_result<array_iterator>;
	};

	class array {
	  public:
		/**
   * Create a new invalid array.
   *
   * Exists so you can declare a variable and later assign to it before use.
   */
		inline array() noexcept = default;

		/**
   * Begin array iteration.
   *
   * Part of the std::iterable interface.
   */
		inline simdjson_result<array_iterator> begin() noexcept;
		/**
   * Sentinel representing the end of the array.
   *
   * Part of the std::iterable interface.
   */
		inline simdjson_result<array_iterator> end() noexcept;
		/**
   * This method scans the array and counts the number of elements.
   * The count_elements method should always be called before you have begun
   * iterating through the array: it is expected that you are pointing at
   * the beginning of the array.
   * The runtime complexity is linear in the size of the array. After
   * calling this function, if successful, the array is 'rewinded' at its
   * beginning as if it had never been accessed. If the JSON is malformed (e.g.,
   * there is a missing comma), then an error is returned and it is no longer
   * safe to continue.
   *
   * To check that an array is empty, it is more performant to use
   * the is_empty() method.
   */
		inline simdjson_result<size_t> count_elements() & noexcept;
		/**
   * This method scans the beginning of the array and checks whether the
   * array is empty.
   * The runtime complexity is constant time. After
   * calling this function, if successful, the array is 'rewinded' at its
   * beginning as if it had never been accessed. If the JSON is malformed (e.g.,
   * there is a missing comma), then an error is returned and it is no longer
   * safe to continue.
   */
		inline simdjson_result<bool> is_empty() & noexcept;
		/**
   * Reset the iterator so that we are pointing back at the
   * beginning of the array. You should still consume values only once even if you
   * can iterate through the array more than once. If you unescape a string
   * within the array more than once, you have unsafe code. Note that rewinding
   * an array means that you may need to reparse it anew: it is not a free
   * operation.
   *
   * @returns true if the array contains some elements (not empty)
   */
		inline simdjson_result<bool> reset() & noexcept;
		/**
   * Get the value associated with the given JSON pointer.  We use the RFC 6901
   * https://tools.ietf.org/html/rfc6901 standard, interpreting the current node
   * as the root of its own JSON document.
   *
   *   ondemand::parser parser;
   *   auto json = R"([ { "foo": { "a": [ 10, 20, 30 ] }} ])"_padded;
   *   auto doc = parser.iterate(json);
   *   doc.at_pointer("/0/foo/a/1") == 20
   *
   * Note that at_pointer() called on the document automatically calls the document's rewind
   * method between each call. It invalidates all previously accessed arrays, objects and values
   * that have not been consumed. Yet it is not the case when calling at_pointer on an array
   * instance: there is no rewind and no invalidation.
   *
   * You may only call at_pointer on an array after it has been created, but before it has
   * been first accessed. When calling at_pointer on an array, the pointer is advanced to
   * the location indicated by the JSON pointer (in case of success). It is no longer possible
   * to call at_pointer on the same array.
   *
   * Also note that at_pointer() relies on find_field() which implies that we do not unescape keys when matching.
   *
   * @return The value associated with the given JSON pointer, or:
   *         - NO_SUCH_FIELD if a field does not exist in an object
   *         - INDEX_OUT_OF_BOUNDS if an array index is larger than an array length
   *         - INCORRECT_TYPE if a non-integer is used to access an array
   *         - INVALID_JSON_POINTER if the JSON pointer is invalid and cannot be parsed
   */
		inline simdjson_result<value> at_pointer(std::string_view json_pointer) noexcept;
		/**
   * Consumes the array and returns a string_view instance corresponding to the
   * array as represented in JSON. It points inside the original document.
   */
		inline simdjson_result<std::string_view> raw_json() noexcept;

		/**
   * Get the value at the given index. This function has linear-time complexity.
   * This function should only be called once on an array instance since the array iterator is not reset between each call.
   *
   * @return The value at the given index, or:
   *         - INDEX_OUT_OF_BOUNDS if the array index is larger than an array length
   */
		inline simdjson_result<value> at(size_t index) noexcept;

	  protected:
		/**
   * Go to the end of the array, no matter where you are right now.
   */
		inline ErrorCode consume() noexcept;

		/**
   * Begin array iteration.
   *
   * @param iter The iterator. Must be where the initial [ is expected. Will be *moved* into the
   *        resulting array.
   * @error INCORRECT_TYPE if the iterator is not at [.
   */
		static inline simdjson_result<array> start(value_iterator& iter) noexcept;
		/**
   * Begin array iteration from the root.
   *
   * @param iter The iterator. Must be where the initial [ is expected. Will be *moved* into the
   *        resulting array.
   * @error INCORRECT_TYPE if the iterator is not at [.
   * @error TAPE_ERROR if there is no closing ] at the end of the document.
   */
		static inline simdjson_result<array> start_root(value_iterator& iter) noexcept;
		/**
   * Begin array iteration.
   *
   * This version of the method should be called after the initial [ has been verified, and is
   * intended for use by switch statements that check the type of a value.
   *
   * @param iter The iterator. Must be after the initial [. Will be *moved* into the resulting array.
   */
		static inline simdjson_result<array> started(value_iterator& iter) noexcept;

		/**
   * Create an array at the given Internal array creation. Call array::start() or array::started() instead of this.
   *
   * @param iter The iterator. Must either be at the start of the first element with iter.is_alive()
   *        == true, or past the [] with is_alive() == false if the array is empty. Will be *moved*
   *        into the resulting array.
   */
		inline array(const value_iterator& iter) noexcept;

		/**
   * Iterator marking current position.
   *
   * iter.is_alive() == false indicates iteration is complete.
   */
		value_iterator iter{};

		friend class value;
		friend class document;
		friend struct simdjson_result<value>;
		friend struct simdjson_result<array>;
		friend class array_iterator;
	};

	class document {
	  public:
		/**
   * Create a new invalid document.
   *
   * Exists so you can declare a variable and later assign to it before use.
   */
		inline document() noexcept = default;
		inline document(const document& other) noexcept = delete;// pass your documents by reference, not by copy
		inline document(document&& other) noexcept = default;
		inline document& operator=(const document& other) noexcept = delete;
		inline document& operator=(document&& other) noexcept = default;

		/**
   * Cast this JSON value to an array.
   *
   * @returns An object that can be used to iterate the array.
   * @returns INCORRECT_TYPE If the JSON value is not an array.
   */
		inline array get_array() & noexcept;
		/**
   * Cast this JSON value to an object.
   *
   * @returns An object that can be used to look up or iterate fields.
   * @returns INCORRECT_TYPE If the JSON value is not an object.
   */
		inline object get_object() & noexcept;
		/**
   * Cast this JSON value to an unsigned integer.
   *
   * @returns A signed 64-bit integer.
   * @returns INCORRECT_TYPE If the JSON value is not a 64-bit unsigned integer.
   */
		inline uint64_t get_uint64() noexcept;
		/**
   * Cast this JSON value (inside string) to an unsigned integer.
   *
   * @returns A signed 64-bit integer.
   * @returns INCORRECT_TYPE If the JSON value is not a 64-bit unsigned integer.
   */
		inline uint64_t get_uint64_in_string() noexcept;
		/**
   * Cast this JSON value to a signed integer.
   *
   * @returns A signed 64-bit integer.
   * @returns INCORRECT_TYPE If the JSON value is not a 64-bit integer.
   */
		inline int64_t get_int64() noexcept;
		/**
   * Cast this JSON value (inside string) to a signed integer.
   *
   * @returns A signed 64-bit integer.
   * @returns INCORRECT_TYPE If the JSON value is not a 64-bit integer.
   */
		inline int64_t get_int64_in_string() noexcept;
		/**
   * Cast this JSON value to a double.
   *
   * @returns A double.
   * @returns INCORRECT_TYPE If the JSON value is not a valid floating-point number.
   */
		inline double get_double() noexcept;

		/**
   * Cast this JSON value (inside string) to a double.
   *
   * @returns A double.
   * @returns INCORRECT_TYPE If the JSON value is not a valid floating-point number.
   */
		inline double get_double_in_string() noexcept;
		/**
   * Cast this JSON value to a string.
   *
   * The string is guaranteed to be valid UTF-8.
   *
   * Important: Calling get_string() twice on the same document is an error.
   *
   * @returns An UTF-8 string. The string is stored in the parser and will be invalidated the next
   *          time it parses a document or when it is destroyed.
   * @returns INCORRECT_TYPE if the JSON value is not a string.
   */
		inline std::string_view get_string() noexcept;
		/**
   * Cast this JSON value to a raw_json_string.
   *
   * The string is guaranteed to be valid UTF-8, and may have escapes in it (e.g. \\ or \n).
   *
   * @returns A pointer to the raw JSON for the given string.
   * @returns INCORRECT_TYPE if the JSON value is not a string.
   */
		//inline raw_json_string get_raw_json_string() noexcept;
		/**
   * Cast this JSON value to a bool.
   *
   * @returns A bool value.
   * @returns INCORRECT_TYPE if the JSON value is not true or false.
   */
		inline bool get_bool() noexcept;
		/**
   * Cast this JSON value to a value when the document is an object or an array.
   *
   * @returns A value if a JSON array or object cannot be found.
   * @returns SCALAR_DOCUMENT_AS_VALUE error is the document is a scalar (see is_scalar() function).
   */
		inline value get_value() noexcept;

		/**
   * Checks if this JSON value is null.  If and only if the value is
   * null, then it is consumed (we advance). If we find a token that
   * begins with 'n' but is not 'null', then an error is returned.
   *
   * @returns Whether the value is null.
   * @returns INCORRECT_TYPE If the JSON value begins with 'n' and is not 'null'.
   */
		inline bool is_null() noexcept;

		/**
   * Get this value as the given type.
   *
   * Supported types: object, array, raw_json_string, string_view, uint64_t, int64_t, double, bool
   *
   * You may use get_double(), get_bool(), get_uint64(), get_int64(),
   * get_object(), get_array(), get_raw_json_string(), or get_string() instead.
   *
   * @returns A value of the given type, parsed from the JSON.
   * @returns INCORRECT_TYPE If the JSON value is not the given type.
   */
		template<typename T> inline T get() & noexcept {
			// Unless the simdjson library provides an inline implementation, calling this method should
			// immediately fail.
			static_assert(!sizeof(T), "The get method with given type is not implemented by the simdjson library.");
		}
		/** @overload template<typename T> T> get() & noexcept */
		template<typename T> inline T get() && noexcept {
			// Unless the simdjson library provides an inline implementation, calling this method should
			// immediately fail.
			static_assert(!sizeof(T), "The get method with given type is not implemented by the simdjson library.");
		}

		/**
   * Get this value as the given type.
   *
   * Supported types: object, array, raw_json_string, string_view, uint64_t, int64_t, double, bool, value
   *
   * Be mindful that the document instance must remain in scope while you are accessing object, array and value instances.
   *
   * @param out This is set to a value of the given type, parsed from the JSON. If there is an error, this may not be initialized.
   * @returns INCORRECT_TYPE If the JSON value is not an object.
   * @returns SUCCESS If the parse succeeded and the out parameter was set to the value.
   */
		template<typename T> inline ErrorCode get(T& out) & noexcept;
		/** @overload template<typename T> ErrorCode get(T &out) & noexcept */
		template<typename T> inline ErrorCode get(T& out) && noexcept;


		/**
   * This method scans the array and counts the number of elements.
   * The count_elements method should always be called before you have begun
   * iterating through the array: it is expected that you are pointing at
   * the beginning of the array.
   * The runtime complexity is linear in the size of the array. After
   * calling this function, if successful, the array is 'rewinded' at its
   * beginning as if it had never been accessed. If the JSON is malformed (e.g.,
   * there is a missing comma), then an error is returned and it is no longer
   * safe to continue.
   */
		inline size_t count_elements() & noexcept;
		/**
   * This method scans the object and counts the number of key-value pairs.
   * The count_fields method should always be called before you have begun
   * iterating through the object: it is expected that you are pointing at
   * the beginning of the object.
   * The runtime complexity is linear in the size of the object. After
   * calling this function, if successful, the object is 'rewinded' at its
   * beginning as if it had never been accessed. If the JSON is malformed (e.g.,
   * there is a missing comma), then an error is returned and it is no longer
   * safe to continue.
   *
   * To check that an object is empty, it is more performant to use
   * the is_empty() method.
   */
		inline size_t count_fields() & noexcept;
		/**
   * Get the value at the given index in the array. This function has linear-time complexity.
   * This function should only be called once on an array instance since the array iterator is not reset between each call.
   *
   * @return The value at the given index, or:
   *         - INDEX_OUT_OF_BOUNDS if the array index is larger than an array length
   */
		inline value at(size_t index) & noexcept;
		/**
   * Begin array iteration.
   *
   * Part of the std::iterable interface.
   */
		inline array_iterator begin() & noexcept;
		/**
   * Sentinel representing the end of the array.
   *
   * Part of the std::iterable interface.
   */
		inline array_iterator end() & noexcept;

		/**
   * Look up a field by name on an object (order-sensitive).
   *
   * The following code reads z, then y, then x, and thus will not retrieve x or y if fed the
   * JSON `{ "x": 1, "y": 2, "z": 3 }`:
   *
   * ```c++
   * simdjson::ondemand::parser parser;
   * auto obj = parser.parse(R"( { "x": 1, "y": 2, "z": 3 } )"_padded);
   * double z = obj.find_field("z");
   * double y = obj.find_field("y");
   * double x = obj.find_field("x");
   * ```
   *
   * **Raw Keys:** The lookup will be done against the *raw* key, and will not unescape keys.
   * e.g. `object["a"]` will match `{ "a": 1 }`, but will *not* match `{ "\u0061": 1 }`.
   *
   *
   * You must consume the fields on an object one at a time. A request for a new key
   * invalidates previous field values: it makes them unsafe. E.g., the array
   * given by content["bids"].get_array() should not be accessed after you have called
   * content["asks"].get_array(). You can detect such mistakes by first compiling and running
   * the code in Debug mode (or with the macro `SIMDJSON_DEVELOPMENT_CHECKS` set to 1): an
   * OUT_OF_ORDER_ITERATION error is generated.
   *
   * You are expected to access keys only once. You should access the value corresponding to
   * a key a single time. Doing object["mykey"].to_string()and then again object["mykey"].to_string()
   * is an error.
   *
   * @param key The key to look up.
   * @returns The value of the field, or NO_SUCH_FIELD if the field is not in the object.
   */
		inline value find_field(std::string_view key) & noexcept;
		/** @overload inline value> find_field(std::string_view key) & noexcept; */
		inline value find_field(const char* key) & noexcept;

		/**
   * Look up a field by name on an object, without regard to key order.
   *
   * **Performance Notes:** This is a bit less performant than find_field(), though its effect varies
   * and often appears negligible. It starts out normally, starting out at the last field; but if
   * the field is not found, it scans from the beginning of the object to see if it missed it. That
   * missing case has a non-cache-friendly bump and lots of extra scanning, especially if the object
   * in question is large. The fact that the extra code is there also bumps the executable size.
   *
   * It is the default, however, because it would be highly surprising (and hard to debug) if the
   * default behavior failed to look up a field just because it was in the wrong order--and many
   * APIs assume this. Therefore, you must be explicit if you want to treat objects as out of order.
   *
   * Use find_field() if you are sure fields will be in order (or are willing to treat it as if the
   * field wasn't there when they aren't).
   *
   * You must consume the fields on an object one at a time. A request for a new key
   * invalidates previous field values: it makes them unsafe. E.g., the array
   * given by content["bids"].get_array() should not be accessed after you have called
   * content["asks"].get_array(). You can detect such mistakes by first compiling and running
   * the code in Debug mode (or with the macro `SIMDJSON_DEVELOPMENT_CHECKS` set to 1): an
   * OUT_OF_ORDER_ITERATION error is generated.
   *
   * You are expected to access keys only once. You should access the value corresponding to a key
   * a single time. Doing object["mykey"].to_string() and then again object["mykey"].to_string()
   * is an error.
   *
   * @param key The key to look up.
   * @returns The value of the field, or NO_SUCH_FIELD if the field is not in the object.
   */
		inline value find_field_unordered(std::string_view key) & noexcept;
		/** @overload inline value> find_field_unordered(std::string_view key) & noexcept; */
		inline value find_field_unordered(const char* key) & noexcept;
		/** @overload inline value> find_field_unordered(std::string_view key) & noexcept; */
		inline value operator[](std::string_view key) & noexcept;
		/** @overload inline value> find_field_unordered(std::string_view key) & noexcept; */
		inline value operator[](const char* key) & noexcept;

		/**
   * Get the type of this JSON value. It does not validate or consume the value.
   * E.g., you must still call "is_null()" to check that a value is null even if
   * "type()" returns JsonType::null.
   *
   * NOTE: If you're only expecting a value to be one type (a typical case), it's generally
   * better to just call .get_double, .get_string, etc. and check for INCORRECT_TYPE (or just
   * let it throw an exception).
   *
   * @error TAPE_ERROR when the JSON value is a bad token like "}" "," or "alse".
   */
		inline JsonType type() noexcept;

		/**
   * Checks whether the document is a scalar (string, number, null, Boolean).
   * Returns false when there it is an array or object.
   *
   * @returns true if the type is string, number, null, Boolean
   * @error TAPE_ERROR when the JSON value is a bad token like "}" "," or "alse".
   */
		inline bool is_scalar() noexcept;

		/**
   * Checks whether the document is a negative number.
   *
   * @returns true if the number if negative.
   */
		inline bool is_negative() noexcept;
		/**
   * Checks whether the document is an integer number. Note that
   * this requires to partially parse the number string. If
   * the value is determined to be an integer, it may still
   * not parse properly as an integer in subsequent steps
   * (e.g., it might overflow).
   *
   * @returns true if the number if negative.
   */
		inline bool is_integer() noexcept;
		/**
   * Determine the number type (integer or floating-point number) as quickly
   * as possible. This function does not fully validate the input. It is
   * useful when you only need to classify the numbers, without parsing them.
   *
   * If you are planning to retrieve the value or you need full validation,
   * consider using the get_number() method instead: it will fully parse
   * and validate the input, and give you access to the type:
   * get_number().get_number_type().
   *
   * get_number_type() is number_type::unsigned_integer if we have
   * an integer greater or equal to 9223372036854775808
   * get_number_type() is number_type::signed_integer if we have an
   * integer that is less than 9223372036854775808
   * Otherwise, get_number_type() has value number_type::floating_point_number
   *
   * This function requires processing the number string, but it is expected
   * to be faster than get_number().get_number_type() because it is does not
   * parse the number value.
   *
   * @returns the type of the number
   */
		//inline number_type get_number_type() noexcept;

		/**
   * Attempt to parse an ondemand::number. An ondemand::number may
   * contain an integer value or a floating-point value, the simdjson
   * library will autodetect the type. Thus it is a dynamically typed
   * number. Before accessing the value, you must determine the detected
   * type.
   *
   * number.get_number_type() is number_type::signed_integer if we have
   * an integer in [-9223372036854775808,9223372036854775808)
   * You can recover the value by calling number.get_int64() and you
   * have that number.is_int64() is true.
   *
   * number.get_number_type() is number_type::unsigned_integer if we have
   * an integer in [9223372036854775808,18446744073709551616)
   * You can recover the value by calling number.get_uint64() and you
   * have that number.is_uint64() is true.
   *
   * Otherwise, number.get_number_type() has value number_type::floating_point_number
   * and we have a binary64 number.
   * You can recover the value by calling number.get_double() and you
   * have that number.is_double() is true.
   *
   * You must check the type before accessing the value: it is an error
   * to call "get_int64()" when number.get_number_type() is not
   * number_type::signed_integer and when number.is_int64() is false.
   */
		//inline number get_number() noexcept;

		/**
   * Get the raw JSON for this token.
   *
   * The string_view will always point into the input buffer.
   *
   * The string_view will start at the beginning of the token, and include the entire token
   * *as well as all spaces until the next token (or EOF).* This means, for example, that a
   * string token always begins with a " and is always terminated by the final ", possibly
   * followed by a number of spaces.
   *
   * The string_view is *not* null-terminated. If this is a scalar (string, number,
   * boolean, or null), the character after the end of the string_view may be the padded buffer.
   *
   * Tokens include:
   * - {
   * - [
   * - "a string (possibly with UTF-8 or backslashed characters like \\\")".
   * - -1.2e-100
   * - true
   * - false
   * - null
   */
		inline std::string_view raw_json_token() noexcept;

		/**
   * Reset the iterator inside the document instance so we are pointing back at the
   * beginning of the document, as if it had just been created. It invalidates all
   * values, objects and arrays that you have created so far (including unescaped strings).
   */
		inline void rewind() noexcept;
		/**
   * Returns debugging information.
   */
		inline std::string to_debug_string() noexcept;
		/**
   * Some unrecoverable error conditions may render the document instance unusable.
   * The is_alive() method returns true when the document is still suitable.
   */
		inline bool is_alive() noexcept;

		/**
   * Returns the current location in the document if in bounds.
   */
		inline const char* current_location() noexcept;

		/**
   * Returns the current depth in the document if in bounds.
   *
   * E.g.,
   *  0 = finished with document
   *  1 = document root value (could be [ or {, not yet known)
   *  2 = , or } inside root array/object
   *  3 = key or value inside root array/object.
   */
		inline int32_t current_depth() const noexcept;

		/**
   * Get the value associated with the given JSON pointer.  We use the RFC 6901
   * https://tools.ietf.org/html/rfc6901 standard.
   *
   *   ondemand::parser parser;
   *   auto json = R"({ "foo": { "a": [ 10, 20, 30 ] }})"_padded;
   *   auto doc = parser.iterate(json);
   *   doc.at_pointer("/foo/a/1") == 20
   *
   * It is allowed for a key to be the empty string:
   *
   *   ondemand::parser parser;
   *   auto json = R"({ "": { "a": [ 10, 20, 30 ] }})"_padded;
   *   auto doc = parser.iterate(json);
   *   doc.at_pointer("//a/1") == 20
   *
   * Note that at_pointer() automatically calls rewind between each call. Thus
   * all values, objects and arrays that you have created so far (including unescaped strings)
   * are invalidated. After calling at_pointer, you need to consume the result: string values
   * should be stored in your own variables, arrays should be decoded and stored in your own array-like
   * structures and so forth.
   *
   * Also note that at_pointer() relies on find_field() which implies that we do not unescape keys when matching
   *
   * @return The value associated with the given JSON pointer, or:
   *         - NO_SUCH_FIELD if a field does not exist in an object
   *         - INDEX_OUT_OF_BOUNDS if an array index is larger than an array length
   *         - INCORRECT_TYPE if a non-integer is used to access an array
   *         - INVALID_JSON_POINTER if the JSON pointer is invalid and cannot be parsed
   *         - SCALAR_DOCUMENT_AS_VALUE if the json_pointer is empty and the document is not a scalar (see is_scalar() function).
   */
		inline value at_pointer(std::string_view json_pointer) noexcept;
		/**
   * Consumes the document and returns a string_view instance corresponding to the
   * document as represented in JSON. It points inside the original byte array containing
   * the JSON document.
   */
		inline std::string_view raw_json() noexcept;

	  protected:
		/**
   * Consumes the document.
   */
		inline ErrorCode consume() noexcept;

		inline document(json_iterator&& iter) noexcept;
		inline const uint8_t* text(uint32_t idx) const noexcept;

		inline value_iterator resume_value_iterator() noexcept;
		inline value_iterator get_root_value_iterator() noexcept;
		inline object start_or_resume_object() noexcept;
		static inline document start(json_iterator&& iter) noexcept;

		//
		// Fields
		//
		json_iterator iter{};///< Current position in the document
		static constexpr depth_t DOCUMENT_DEPTH = 0;///< document depth is always 0

		friend class array_iterator;
		friend class value;
		friend class object;
		friend class array;
		friend class field;
		friend class token;
		friend class document_stream;
	};


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
				//std::cout << "TIME FOR STAGE1: " << totalTimePassed / iterationCount << std::endl;
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
		: token(buf, reinterpret_cast<uint32_t*>( & _parser->getTape()[0])), parser{ _parser },
		  _string_buf_loc{ parser->getStringBuffer() }, _depth{ 1 }, _root{ reinterpret_cast<uint32_t*>(parser->getTape()) }, _streaming{ false }

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
		return reinterpret_cast<uint32_t*>(&parser->getTape()[n_structural_indexes]);
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
		return reinterpret_cast<uint32_t*>(&parser->getTape()[n_structural_indexes - 1]);
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
