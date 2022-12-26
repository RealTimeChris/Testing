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

	constexpr int64_t JSON_VALUE_MASK{ 0x00FFFFFFFFFFFFFF };
	constexpr uint32_t JSON_COUNT_MASK{ 0xFFFFFF };

	template<typename RTy> void reverseByteOrder(RTy& net) {
		if constexpr (std::endian::native == std::endian::little) {
			switch (sizeof(RTy)) {
				case 1: {
					return;
				}
				case 2: {
					net =
						_mm256_extract_epi16(_mm256_shuffle_epi8(_mm256_insert_epi16(__m256i{}, net, 0), _mm256_insert_epi16(__m256i{}, 0x01, 0)), 0);
					break;
				}
				case 4: {
					net = _mm256_extract_epi32(
						_mm256_shuffle_epi8(_mm256_insert_epi32(__m256i{}, net, 0), _mm256_insert_epi32(__m256i{}, 0x10203, 0)), 0);
					break;
				}
				case 8: {
					net = _mm256_extract_epi64(
						_mm256_shuffle_epi8(_mm256_insert_epi64(__m256i{}, net, 0), _mm256_insert_epi64(__m256i{}, 0x102030405060708, 0)), 0);
					break;
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
	concept IsEnum = std::is_enum<Ty>::Value;

	struct EnumConverter {
		template<IsEnum EnumType> EnumConverter& operator=(const std::vector<EnumType>& data) {
			for (auto& Value: data) {
				this->vector.emplace_back(std::move(static_cast<uint64_t>(Value)));
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
			for (auto& Value: data) {
				this->jsonValue.array->push_back(std::move(Value));
			}
			return *this;
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier(std::vector<OTy>&& data) noexcept {
			*this = std::move(data);
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::vector<OTy>& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& Value: data) {
				this->jsonValue.array->push_back(Value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier OTy> Jsonifier(std::vector<OTy>& data) noexcept {
			*this = data;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::unordered_map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::object);
			for (auto& [key, Value]: data) {
				(*this->jsonValue.object)[key] = std::move(Value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::unordered_map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::unordered_map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::object);
			for (auto& [key, Value]: data) {
				(*this->jsonValue.object)[key] = Value;
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::unordered_map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::object);
			for (auto& [key, Value]: data) {
				(*this->jsonValue.object)[key] = std::move(Value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier(std::map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> Jsonifier& operator=(std::map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::object);
			for (auto& [key, Value]: data) {
				(*this->jsonValue.object)[key] = Value;
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

		void appendUint64(const uint64_t Value);

		void appendUint32(const uint32_t Value);

		void appendInt64(const int64_t Value);

		void appendInt32(const int32_t Value);

		void appendUint8(const uint8_t Value);

		void appendInt8(const int8_t Value);

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

		TapeIterator(uint8_t* stringBufferNew, uint32_t* tapePositionNew, size_t currentStructuralCountNew) {
			this->currentStructuralCount = currentStructuralCountNew;
			this->initialTapePosition = tapePositionNew;
			this->tapePosition = tapePositionNew;
			this->stringBuffer = stringBufferNew;
		}

		inline uint8_t advance() noexcept {
			return (*(this->tapePosition++));
		}

		inline void ascendTo(size_t parent_depth) noexcept {
			this->currentDepth = parent_depth;
		}

		inline void descendTo(size_t child_depth) noexcept {
			this->currentDepth = child_depth;
		}

		inline void rewind() noexcept {
			this->tapePosition = this->initialTapePosition;
		}

		inline uint32_t currentOffset() noexcept {
			return *(this->tapePosition);
		}

		inline const uint8_t* returnCurrentAndAdvance() noexcept {
			return &this->stringBuffer[*(this->tapePosition++)];
		}

		inline const uint8_t* peek(uint32_t* position) noexcept {
			return &this->stringBuffer[*position];
		}

		inline uint32_t peekIndex(uint32_t* position) noexcept {
			return *position;
		}

		inline uint32_t peekLength(uint32_t* position) noexcept {
			return *(position + 1) - *position;
		}

		inline const uint8_t* peek(int32_t delta) noexcept {
			std::cout << "THE PEEEKED VALUE: " << this->stringBuffer[*(this->tapePosition + delta)] << std::endl;
			return &this->stringBuffer[*(this->tapePosition + delta)];
		}

		inline uint32_t peekIndex(int32_t delta) noexcept {
			return *(this->tapePosition + delta);
		}

		inline uint32_t peekLength(int32_t delta) noexcept {
			return *(this->tapePosition + delta + 1) - *(this->tapePosition + delta);
		}

		inline uint32_t* position() noexcept {
			return this->tapePosition;
		}

		inline void setPosition(uint32_t* target_position) noexcept {
			this->tapePosition = target_position;
		}

		inline bool atEof() {
			if((this->tapePosition - this->initialTapePosition) >= this->currentStructuralCount){
				return true;
			} else {
				return false;
			}
		}

		inline size_t getStructuralCount() {
			return this->currentStructuralCount;
		}

		inline uint8_t* peek(uint32_t position = 0) noexcept {
			if (position) {
				return &this->stringBuffer[position];
			} else {
				return this->stringBuffer;
			}
		}

		inline uint32_t peekIndex() noexcept {
			return (tapePosition - this->initialTapePosition);
		}

		inline size_t peekLengthOrSize() noexcept {
			if ( *this->peek() == '[' || *this->peek() == '{' || *this->peek() == 'r') {
				return (((*tapePosition) & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK;
				
			} else if (*this->peek() == '"') {
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

		inline uint32_t* getTapePosition() {
			return this->tapePosition;
		}

		inline uint32_t* getTape() {
			return this->initialTapePosition;
		}

	  protected:
		size_t currentStructuralCount{};
		uint32_t* initialTapePosition{};
		uint32_t* tapePosition{};
		uint8_t* stringBuffer{};
		size_t currentDepth{};
	};
	
	class ArrayIterator;
	class SimdJsonValue;
	class JsonParser;

	class ValueIterator {
	  protected:
		JsonParser* jsonIter{};
		size_t _depth{};
		uint32_t* _start_position{};

	  public:
		inline ValueIterator() noexcept = default;
		inline ErrorCode skip_child() noexcept;
		inline bool at_end() const noexcept;
		inline bool at_start() const noexcept;
		inline bool is_open() const noexcept;
		inline bool at_first_field() const noexcept;
		inline void abandon() noexcept;
		inline size_t depth() const noexcept;
		inline JsonType type() const noexcept;
		inline bool start_object() noexcept;
		inline bool start_root_object() noexcept;
		inline bool has_next_field() noexcept;
		inline ErrorCode field_value() noexcept;
		inline bool find_field_raw(const std::string_view key) noexcept;
		inline bool find_field_unordered_raw(const std::string_view key) noexcept;
		inline bool start_array() noexcept;
		inline bool start_root_array() noexcept;
		inline bool has_next_element() noexcept;

		inline std::string_view get_string() noexcept;
		inline uint64_t get_uint64() noexcept;
		inline uint64_t get_uint64_in_string() noexcept;
		inline int64_t get_int64() noexcept;
		inline int64_t get_int64_in_string() noexcept;
		inline double get_double() noexcept;
		inline double get_double_in_string() noexcept;
		inline bool get_bool() noexcept;
		inline bool is_null() noexcept;
		inline bool is_negative() noexcept;
		inline bool is_integer() noexcept;

		inline std::string_view get_root_string() noexcept;
		inline uint64_t get_root_uint64() noexcept;
		inline uint64_t get_root_uint64_in_string() noexcept;
		inline int64_t get_root_int64() noexcept;
		inline int64_t get_root_int64_in_string() noexcept;
		inline double get_root_double() noexcept;
		inline double get_root_double_in_string() noexcept;
		inline bool get_root_bool() noexcept;
		inline bool is_root_negative() noexcept;
		inline bool is_root_integer() noexcept;
		inline bool is_root_null() noexcept;

		inline ErrorCode error() const noexcept;
		inline uint8_t*& string_buf_loc() noexcept;
		inline const JsonParser& json_iter() const noexcept;
		inline JsonParser& json_iter() noexcept;

		inline void assert_is_valid() const noexcept;
		inline bool is_valid() const noexcept;
		inline ValueIterator(JsonParser* json_iter, size_t depth, uint32_t* start_position,
			std::source_location location = std::source_location::current()) noexcept;

	  protected:
		inline bool reset_array() noexcept;
		inline bool reset_object() noexcept;
		inline void move_at_start() noexcept;
		inline void move_at_container_start() noexcept;
		inline std::string to_string() const noexcept;
		

		inline bool parse_null(const uint8_t* json) const noexcept;
		inline bool parse_bool(const uint8_t* json) const noexcept;
		inline const uint8_t* peek_start() const noexcept;
		inline uint32_t peek_start_length() const noexcept;

		inline void advance_scalar(const char* type) noexcept;
		inline void advance_root_scalar(const char* type) noexcept;
		inline void advance_non_root_scalar(const char* type) noexcept;

		inline const uint8_t* peek_scalar(const char* type) noexcept;
		inline const uint8_t* peek_root_scalar(const char* type) noexcept;
		inline const uint8_t* peek_non_root_scalar(const char* type) noexcept;


		inline ErrorCode start_container(uint8_t start_char, const char* incorrect_type_message, const char* type) noexcept;
		inline ErrorCode end_container() noexcept;
		inline const uint8_t* advance_to_value() noexcept;

		inline ErrorCode incorrect_type_error(const char* message) const noexcept;
		inline ErrorCode error_unless_more_tokens(uint32_t tokens = 1) const noexcept;
		inline bool is_at_start() const noexcept;
		inline bool is_at_iterator_start() const noexcept;
		inline bool is_at_key() const noexcept;
		inline void assert_at_start() const noexcept;
		inline void assert_at_container_start() const noexcept;
		inline void assert_at_root() const noexcept;
		inline void assert_at_child() const noexcept;
		inline void assert_at_next() const noexcept;
		inline void assert_at_non_root_start() const noexcept;
		inline uint32_t* start_position() const noexcept;
		inline uint32_t* position() const noexcept;
		inline uint32_t* last_position() const noexcept;
		inline uint32_t* end_position() const noexcept;
		inline ErrorCode report_error(ErrorCode error, const char* message) noexcept;

		friend class Document;
		friend class Object;
		friend class Array;
		friend class Value;
	};

	class Value {
	  public:
		Value() noexcept = default;
		template<typename T> inline T get() noexcept {
			static_assert(!sizeof(T), "The get method with given type is not implemented by the simdjson library.");
		}
		template<typename T> inline ErrorCode get(T& out) noexcept;
		inline Array getArray() noexcept;
		inline Object getObject() noexcept;
		inline uint64_t getUint64() noexcept;
		inline int64_t getInt64() noexcept;
		inline double getDouble() noexcept;
		inline std::string_view getString() noexcept;
		inline bool getBool() noexcept;
		inline bool isNull() noexcept;
		inline ArrayIterator begin() & noexcept;
		inline ArrayIterator end() & noexcept;
		inline size_t countElements() & noexcept;
		inline size_t countFields() & noexcept;
		inline Value at(size_t index) noexcept;
		inline Value findField(std::string_view key) noexcept;
		inline Value findField(const char* key) noexcept;
		inline Value findFieldUnordered(std::string_view key) noexcept;
		inline Value findFieldUnordered(const char* key) noexcept;
		inline Value operator[](std::string_view key) noexcept;
		inline Value operator[](const char* key) noexcept;
		inline JsonType type() noexcept;
		inline bool isScalar() noexcept;
		inline bool isNegative() noexcept;
		inline bool isInteger() noexcept;
		inline const char* currentLocation() noexcept;
		inline int32_t currentDepth() noexcept;

		inline Value(const ValueIterator& iter) noexcept;
	  protected:
		inline void skip() noexcept;
		static inline Value start(const ValueIterator& iter) noexcept;
		static inline Value resume(const ValueIterator& iter) noexcept;
		inline Object start_or_resume_object() noexcept;
		ValueIterator iter;

		friend class Document;
		friend class ArrayIterator;
		friend class field;
		friend class Object;
		friend struct Value;
		friend struct field;
	};

	class raw_json_string {
	  public:
		inline raw_json_string() noexcept = default;
		inline raw_json_string(const uint8_t* _buf) noexcept;
		inline const char* raw() const noexcept;
		inline bool unsafe_is_equal(size_t length, std::string_view target) const noexcept;
		inline bool unsafe_is_equal(std::string_view target) const noexcept;
		inline bool unsafe_is_equal(const char* target) const noexcept;
		inline bool is_equal(std::string_view target) const noexcept;
		inline bool is_equal(const char* target) const noexcept;
		static inline bool is_free_from_unescaped_quote(std::string_view target) noexcept;
		static inline bool is_free_from_unescaped_quote(const char* target) noexcept;

	  private:
		inline void consume() noexcept {
			buf = nullptr;
		}
		inline bool alive() const noexcept {
			return buf != nullptr;
		}
		inline std::string_view unescape(JsonParser& iter) const noexcept;

		const uint8_t* buf{};
		friend class Object;
		friend class field;
		friend class ValueIterator;
		friend class parser;
	};

	class field : public std::pair<raw_json_string, Value> {
	  public:
		inline field() noexcept;
		inline std::string_view unescaped_key() noexcept;
		inline Value& Value() & noexcept;

	  protected:
		static inline field start(ValueIterator& parent_iter) noexcept;
		static inline field start(const ValueIterator& parent_iter, raw_json_string key) noexcept;
		friend struct field;
		friend class object_iterator;
	};

	class object_iterator {
	  public:
		inline object_iterator() noexcept = default;
		inline field operator*() noexcept;
		inline bool operator==(const object_iterator&) const noexcept;
		inline bool operator!=(const object_iterator&) const noexcept;
		inline object_iterator& operator++() noexcept;

	  private:
		ValueIterator iter;

		inline object_iterator(const ValueIterator& iter) noexcept;
		friend class Object;
	};

	class Object {
	  public:
		inline Object() noexcept = default;

		inline object_iterator begin() noexcept;
		inline object_iterator end() noexcept;
		inline Value find_field(std::string_view key) & noexcept;
		inline Value find_field(std::string_view key) && noexcept;
		inline Value find_field_unordered(std::string_view key) & noexcept;
		inline Value find_field_unordered(std::string_view key) && noexcept;
		inline Value operator[](std::string_view key) & noexcept;
		inline Value operator[](std::string_view key) && noexcept;
		inline Value at_pointer(std::string_view json_pointer) noexcept;
		inline bool reset() & noexcept;
		inline bool is_empty() & noexcept;
		inline size_t count_fields() & noexcept;
		inline std::string_view raw_json() noexcept;

	  protected:
		inline ErrorCode consume() noexcept;
		static inline Object start(ValueIterator& iter) noexcept;
		static inline Object start_root(ValueIterator& iter) noexcept;
		static inline Object started(ValueIterator& iter) noexcept;
		static inline Object resume(const ValueIterator& iter) noexcept;
		inline Object(const ValueIterator& iter) noexcept;

		inline ErrorCode find_field_raw(const std::string_view key) noexcept;

		ValueIterator iter;

		friend class Value;
		friend class Document;
		friend struct Object;
	};
	
	class ArrayIterator {
	  public:
		inline ArrayIterator() noexcept = default;
		inline Value operator*() noexcept;
		inline bool operator==(const ArrayIterator&) const noexcept;
		inline bool operator!=(const ArrayIterator&) const noexcept;
		inline ArrayIterator& operator++() noexcept;

	  private:
		ValueIterator iter;

		inline ArrayIterator(const ValueIterator& iter) noexcept;

		friend class Array;
		friend class Value;
		friend struct ArrayIterator;
	};

	class Array {
	  public:
		inline Array() noexcept = default;
		inline Array(const ValueIterator& iter) noexcept;
		inline ArrayIterator begin() noexcept;
		inline ArrayIterator end() noexcept;
		inline size_t countElements() & noexcept;
		inline bool isEmpty() & noexcept;
		inline bool reset() & noexcept;
		inline Value at(size_t index) noexcept;
		ArrayIterator iter;

		friend class Value;
		friend class Document;
		friend struct Value;
		friend struct Array;
		friend class ArrayIterator;
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

		inline JsonParser(JsonParser&& other) noexcept : tapeIter{ other.tapeIter } {
			*this = std::move(other);
		}

		inline bool atRoot() noexcept {
			return position() == rootPosition();
		}

		inline uint32_t* rootPosition() noexcept {
			return this->tapeIter.getTape();
		}

		inline JsonParser& operator=(JsonParser&& other) noexcept {
			this->stringBufferLocation = other.stringBufferLocation;
			this->startPositionVal = other.startPositionVal;
			this->tapePosition = other.tapePosition;
			this->currentDepth = other.currentDepth;
			this->tapeIter = other.tapeIter;
			this->parser = other.parser;
			return *this;
		}

		inline uint32_t* startPosition() noexcept {
			return this->startPositionVal;
		}

		inline const uint8_t* peekStart() noexcept {
			return tapeIter.peek(startPosition());
		}

		inline void advanceNonRootScalar(const char* type) noexcept {
			if (!isAtStart()) {
				return;
			}
			tapeIter.returnCurrentAndAdvance();
			tapeIter.ascendTo(depth() - 1);
		}

		inline const uint8_t* peekNonRootScalar(const char* type) noexcept {
			if (!isAtStart()) {
				return peekStart();
			}
			return tapeIter.peek();
		}

		inline int64_t getInt64() noexcept {
			auto result = NumberParser::parseInteger(peekNonRootScalar("int64"));
			advanceNonRootScalar("int64");
			return result;
		}

		inline JsonParser(uint8_t* buf, SimdJsonValue* _parser) noexcept;

		inline void rewind() noexcept;

		inline bool balanced() noexcept {
			TapeIterator ti(this->tapeIter);
			int32_t count{ 0 };
			ti.setPosition(this->rootPosition());
			while (ti.peek() <= this->peekLast()) {
				switch (*ti.returnCurrentAndAdvance()) {
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

		inline ErrorCode skipChild(size_t parent_depth) noexcept {
			if (this->depth() <= parent_depth) {
				return ErrorCode::Success;
			}
			switch (*this->returnCurrentAndAdvance()) {
				case '[':
				case '{':
				case ':':
					break;
				case ',':
					break;
				case ']':
				case '}':
					this->currentDepth--;
					if (this->depth() <= parent_depth) {
						return ErrorCode::Success;
					}
					break;
				case '"':
					if (*this->peek() == static_cast<uint8_t>(':')) {
						this->returnCurrentAndAdvance();
						break;
					}
					[[fallthrough]];
				default:
					this->currentDepth--;
					if (this->depth() <= parent_depth) {
						return ErrorCode::Success;
					}
					break;
			}

			while (this->position() < this->endPosition()) {
				switch (*this->returnCurrentAndAdvance()) {
					case '[':
					case '{':
						this->currentDepth++;
						break;
					case ']':
					case '}':
						this->currentDepth--;
						if (this->depth() <= parent_depth) {
							return ErrorCode::Success;
						}
						break;
					default:
						break;
				}
			}

			return ErrorCode::TapeError;
		}

		inline bool isSingleToken() noexcept;

		inline bool isAtStart() {
			return this->tapeIter.getTapePosition() == this->tapeIter.getTape();
		}


		inline void assertAtRoot() noexcept {
			assert(this->tapeIter.position() == this->startPositionVal);
		}

		inline void assertMoreTokens(uint32_t required_tokens) noexcept {
			assert(this->tapeIter.position() + required_tokens - 1);
		}

		inline bool atEnd() noexcept {
			return this->position() == this->endPosition();
		}

		inline uint32_t* endPosition() noexcept {
			size_t n_structural_indexes{ this->tapeIter.getStructuralCount() };
			return &this->tapeIter.getTape()[n_structural_indexes];
		}

		inline std::string toString() noexcept {
			if (!this->isAlive()) {
				return "dead JsonParser instance";
			}
			const char* current_structural = reinterpret_cast<const char*>(this->tapeIter.peek());
			return std::string("JsonParser [ depth : ") + std::to_string(this->currentDepth) + std::string(", structural : '") +
				std::string(current_structural, 1) + std::string("', offset : ") + std::to_string(this->tapeIter.currentOffset()) +
				std::string("', error : ") + std::string(" ]");
		}

		inline const char* currentLocation() noexcept {
			if (!this->isAlive()) {
				if (!this->atRoot()) {
					return reinterpret_cast<const char*>(this->tapeIter.peek(-1));
				} else {
					return reinterpret_cast<const char*>(this->tapeIter.peek());
				}
			}
			if (this->atEnd()) {
				return nullptr;
			}
			return reinterpret_cast<const char*>(this->tapeIter.peek());
		}

		inline void abandon() noexcept {
			this->currentDepth = 0;
		}

		inline const uint8_t* returnCurrentAndAdvance() noexcept {
			return this->tapeIter.returnCurrentAndAdvance();
		}

		inline const uint8_t* unsafePointer() noexcept {
			return this->tapeIter.peek(0);
		}

		inline const uint8_t* peek(int32_t delta = 0) noexcept {
			return this->tapeIter.peek(delta);
		}

		inline uint32_t peekLength(int32_t delta) noexcept {
			return this->tapeIter.peekLength(delta);
		}

		inline const uint8_t* peek(uint32_t* position) noexcept {
			return this->tapeIter.peek(position);
		}

		inline uint32_t peekLength(uint32_t* position) noexcept {
			return this->tapeIter.peekLength(position);
		}

		inline uint32_t* lastPosition() noexcept {
			size_t n_structural_indexes{ this->tapeIter.getStructuralCount() };
			assert(n_structural_indexes > 0);
			return &this->tapeIter.getTape()[n_structural_indexes - 1];
		}
		inline const uint8_t* peekLast() noexcept {
			return this->tapeIter.peek(lastPosition());
		}

		inline void ascendTo(size_t parent_depth) noexcept {
			this->currentDepth = parent_depth;
		}

		inline void descendTo(size_t child_depth) noexcept {
			child_depth >= 1 && child_depth < INT32_MAX;
			assert(this->currentDepth == child_depth - 1);
			this->currentDepth = child_depth;
		}

		inline bool isAlive() const noexcept {
			return this->parser;
		}

		inline size_t& depth() noexcept {
			return this->currentDepth;
		}

		inline uint8_t*& stringBufLoc() noexcept {
			return this->stringBufferLocation;
		}

		inline uint32_t* position() noexcept {
			return this->tapeIter.position();
		}

		inline void reenterChild(uint32_t* position, size_t child_depth) noexcept {
			this->tapeIter.setPosition(position);
			this->currentDepth = child_depth;
		}

		inline ErrorCode optionalError(ErrorCode _error, const char* message) noexcept {
			return _error;
		}

		template<int N> inline bool copyToBuffer(const uint8_t* json, uint32_t max_len, uint8_t (&tmpbuf)[N]) noexcept {
			if ((N < max_len) || (N == 0)) {
				return false;
			}
			if (max_len > N - 1) {
				max_len = N - 1;
			}
			std::memcpy(tmpbuf, json, max_len);
			tmpbuf[max_len] = ' ';
			return true;
		}

		union JsonValue {
			JsonValue() noexcept = default;
			JsonValue& operator=(const JsonValue&) noexcept = delete;
			JsonValue(const JsonValue&) noexcept = delete;
			ObjectType* Object;
			StringType* string;
			ArrayType* Array;
			FloatType numberDouble;
			UintType numberUint;
			IntType numberInt;
			BoolType boolean;
		};

		inline uint64_t parseJsonUint() {
			return uint64_t{};
		}

		inline int64_t parseJsonInt() {
			int64_t returnData{};
			return returnData;
		}

		inline std::string_view parseJsonString() {
			std::string_view returnData{};
			return returnData;
		}

		inline double parseJsonFloat() {
			double returnData{};
			return returnData;
		}

		inline bool parseJsonBool() {
			bool returnData{};
			return returnData;
		}

		inline nullptr_t parseJsonNull() {
			nullptr_t returnData{};
			return returnData;
		}

		inline size_t size() {
			size_t returnValue{};
			return returnValue;
		}

		inline JsonParser(uint32_t* tapePtrsNew, size_t count, uint8_t* stringBufferNew, SimdJsonValue* parserNew)
			: tapeIter{ stringBufferNew, tapePtrsNew, count } {
			this->stringBufferLocation = stringBufferNew;
			this->startPositionVal = tapePtrsNew;
			this->tapePosition = tapePtrsNew;
			this->parser = parserNew;
		};

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
			return this->getInt64();
		}

		template<> inline int32_t getValue() {
			return this->getInt64();
		}

		template<> inline int16_t getValue() {
			return this->getInt64();
		}

		template<> inline int8_t getValue() {
			return this->getInt64();
		}

		template<> inline std::string getValue() {
			return static_cast<std::string>(this->parseJsonString());
		}

		template<> inline std::string_view getValue() {
			return this->parseJsonString();
		}

		inline bool getBool() {
			if (*this->tapeIter.peek() == 'f') {
				return false;
			} else {
				return true;
			}
		}

		inline uint64_t getUint64() {
			uint64_t answer{};
			auto ptr = this->tapeIter.getTapePosition();
			std::memcpy(&answer, ++ptr, sizeof(answer));
			return answer;
		}

		inline float getFloat() {
			double answer{};
			auto ptr = this->tapeIter.getTapePosition();
			std::memcpy(&answer, ++ptr, sizeof(answer));
			return answer;
		}

		inline const char* getString() {
			const char* returnValue{};
			if (*this->tapeIter.peek() == '"') {
				size_t stringLength{};
				std::memcpy(&stringLength, this->tapeIter.getStringBuffer() + (*this->tapeIter.getTapePosition() & JSON_VALUE_MASK),
					sizeof(uint32_t));
				returnValue = reinterpret_cast<const char*>(
					this->tapeIter.getStringBuffer() + (*this->tapeIter.getTapePosition() & JSON_VALUE_MASK) + sizeof(uint32_t));
			}
			return returnValue;
		}

		inline std::string_view getKey() {
			auto returnValue = this->getString();
			this->tapeIter.advance();
			return returnValue;
		}

		inline JsonType getType() {
			JsonType returnValue{};
			return returnValue;
		}

		template<> inline JsonParser getValue() {
			return std::move(*this);
		}

		TapeIterator& getTapeIterator() {
			return this->tapeIter;
		}

		inline JsonParser& operator[](const std::string& key) {
			return *this;
		};

		inline JsonParser(ErrorCode error) : tapeIter{ nullptr, nullptr, 0 } {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " + std::to_string(( int32_t )error) };
		}

	  protected:
		uint8_t* stringBufferLocation{};
		uint32_t* startPositionVal{};
		uint32_t* tapePosition{};
		SimdJsonValue* parser{};
		TapeIterator tapeIter;
		size_t currentDepth{};
	};

	class Document {
	  public:
		inline Document(const Document& other) noexcept = delete;
		inline Document(Document&& other) noexcept = default;
		inline Document& operator=(const Document& other) noexcept = delete;
		inline Document& operator=(Document&& other) noexcept = default;
		inline Value getValue() noexcept;
		inline bool isNull() noexcept;

	  protected:

		inline Document(JsonParser&& iter) noexcept;
		static inline Document start(JsonParser&& iter) noexcept;
		JsonParser iter;

		friend class ArrayIterator;
		friend class SimdJsonValue;
		friend class Value;
		friend class Object;
		friend class Array;
		friend class field;
		friend class token;
		friend class document_stream;
	};

	class SimdBase256;

	class SimdBase128 {
	  public:
		inline SimdBase128() noexcept = default;

		inline SimdBase128& operator=(char other) {
			this->Value = _mm_set1_epi8(other);
			return *this;
		}

		inline SimdBase128(char other) {
			*this = other;
		}

		inline operator __m128i() {
			return this->Value;
		}

	  protected:
		__m128i Value{};
	};

	class SimdBase256 {
	  public:
		inline SimdBase256() noexcept {
			this->Value = _mm256_set1_epi8(0x00);
		};

		inline explicit operator bool() {
			for (size_t x = 0; x < 4; ++x) {
				if (this->getUint64(x) != 0) {
					return true;
				}
			}
			return false;
		}

		inline SimdBase256& operator=(uint8_t other) {
			this->Value = _mm256_set1_epi8(other);
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
			this->Value = _mm256_set_epi64x(value03, value02, value01, value00);
		}

		inline SimdBase256(uint64_t value00, uint64_t value01, uint64_t value02, uint64_t value03) {
			this->Value = _mm256_set_epi64x(static_cast<int64_t>(value03), static_cast<int64_t>(value02), static_cast<int64_t>(value01),
				static_cast<int64_t>(value00));
		}

		inline SimdBase256& operator=(__m256i other) {
			this->Value = other;
			return *this;
		}

		inline SimdBase256(__m256i other) {
			*this = other;
		}

		inline void store(uint8_t dst[32]) const {
			return _mm256_storeu_epi8(dst, this->Value);
		}

		inline uint64_t getUint64(size_t index) {
			switch (index) {
				case 0: {
					return static_cast<size_t>(_mm256_extract_epi64(this->Value, 0));
				}
				case 1: {
					return static_cast<size_t>(_mm256_extract_epi64(this->Value, 1));
				}
				case 2: {
					return static_cast<size_t>(_mm256_extract_epi64(this->Value, 2));
				}
				case 3: {
					return static_cast<size_t>(_mm256_extract_epi64(this->Value, 3));
				}
				default: {
					return static_cast<size_t>(_mm256_extract_epi64(this->Value, 0));
				}
			}
		}

		inline int64_t getInt64(size_t index) {
			switch (index) {
				case 0: {
					return _mm256_extract_epi64(this->Value, 0);
				}
				case 1: {
					return _mm256_extract_epi64(this->Value, 1);
				}
				case 2: {
					return _mm256_extract_epi64(this->Value, 2);
				}
				case 3: {
					return _mm256_extract_epi64(this->Value, 3);
				}
				default: {
					return _mm256_extract_epi64(this->Value, 0);
				}
			}
		}

		inline void insertInt64(int64_t Value, size_t index) {
			switch (index) {
				case 0: {
					this->Value = _mm256_insert_epi64(this->Value, Value, 0);
					break;
				}
				case 1: {
					this->Value = _mm256_insert_epi64(this->Value, Value, 1);
					break;
				}
				case 2: {
					this->Value = _mm256_insert_epi64(this->Value, Value, 2);
					break;
				}
				case 3: {
					this->Value = _mm256_insert_epi64(this->Value, Value, 3);
					break;
				}
				default: {
					this->Value = _mm256_insert_epi64(this->Value, Value, 0);
					break;
				}
			}
		}

		inline operator __m256i&() {
			return this->Value;
		}

		inline SimdBase256 operator|(SimdBase256 other) {
			return _mm256_or_si256(this->Value, other);
		}

		inline SimdBase256 operator&(SimdBase256 other) {
			return _mm256_and_si256(this->Value, other);
		}

		inline SimdBase256 operator^(SimdBase256 other) {
			return _mm256_xor_si256(this->Value, other);
		}

		inline SimdBase256 operator+(SimdBase256 other) {
			return _mm256_add_epi8(this->Value, other);
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

		inline SimdBase256 operator-(int32_t other) {
			auto currentValue = *this;
			if (other == 1) {
				currentValue = ~currentValue & 1;
			}
			return currentValue;
		}

		inline SimdBase256 operator==(SimdBase256 other) {
			return _mm256_cmpeq_epi8(this->Value, other);
		}

		inline SimdBase256 operator==(uint8_t other) {
			return _mm256_cmpeq_epi8(this->Value, _mm256_set1_epi8(other));
		}

		inline SimdBase256 operator<<(size_t amount) {
			uint64_t values[4]{};
			values[0] = _mm256_extract_epi64(this->Value, 0);
			values[1] = _mm256_extract_epi64(this->Value, 1);
			values[2] = _mm256_extract_epi64(this->Value, 2);
			values[3] = _mm256_extract_epi64(this->Value, 3);
			SimdBase256 newValues{};
			newValues = _mm256_insert_epi64(newValues, (values[0] << (amount % 64)), 0);
			newValues = _mm256_insert_epi64(newValues, (values[1] << (amount % 64)) | ((values[0]) >> 64 - amount % 64), 1);
			newValues = _mm256_insert_epi64(newValues, (values[2] << (amount % 64)) | ((values[1]) >> 64 - amount % 64), 2);
			newValues = _mm256_insert_epi64(newValues, (values[3] << (amount % 64)) | ((values[2]) >> 64 - amount % 64), 3);
			return newValues;
		}

		inline SimdBase256 operator~() {
			SimdBase256 newValues{};
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->Value, 0), 0);
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->Value, 1), 1);
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->Value, 2), 2);
			newValues = _mm256_insert_epi64(newValues, ~_mm256_extract_epi64(this->Value, 3), 3);
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
				if (_addcarry_u64(0, this->getUint64(x), other1.getUint64(x), &returnValue64)) {
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
					cout << std::bitset<1>{ static_cast<size_t>(*(reinterpret_cast<int8_t*>(&this->Value) + x)) >> y };
				}
			}
			cout << std::endl;
		}

		inline SimdBase256 bitAndNot(SimdBase256 other) {
			return _mm256_andnot_si256(other, this->Value);
		}

		inline SimdBase256 shuffle(SimdBase256 other) {
			return _mm256_shuffle_epi8(other, this->Value);
		}

	  protected:
		__m256i Value{};
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
		const uint8_t* stringBuffer;
		const size_t len;
		const size_t lenminusstep;
		size_t idx;
	};

	template<size_t StepSize>
	inline StringBlockReader<StepSize>::StringBlockReader(const uint8_t* _buf, size_t _len)
		: stringBuffer{ _buf }, len{ _len }, lenminusstep{ len < StepSize ? 0 : len - StepSize }, idx{ 0 } {
	}

	template<size_t StepSize> inline size_t StringBlockReader<StepSize>::blockIndex() {
		return idx;
	}

	template<size_t StepSize> inline bool StringBlockReader<StepSize>::hasFullBlock() const {
		return idx < lenminusstep;
	}

	template<size_t StepSize> inline const uint8_t* StringBlockReader<StepSize>::fullBlock() const {
		return &stringBuffer[idx];
	}

	template<size_t StepSize> inline size_t StringBlockReader<StepSize>::getRemainder(uint8_t* dst) const {
		if (len == idx) {
			return 0;
		}
		std::memset(dst, 0x20, StepSize);
		std::memcpy(dst, stringBuffer + idx, len - idx);
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

	inline int64_t totalTimePassed{};
	inline int64_t totalTimePassed02{};
	inline int64_t iterationCount{};

	class SimdJsonValue {
	  public:
		inline SimdJsonValue() {
		}

		int64_t round(int64_t a, int64_t n) {
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

			size_t tapeCapacity = round(this->stringLengthRaw + 3, 64);
			size_t stringCapacity = round(5 * this->stringLengthRaw / 3 + 64, 64);
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
				this->nStructuralIndexes = 0;
				size_t tapeCurrentIndex{ 0 };
				SimdBase256 prevInScalar{};
				SimdBase256 followsPotentialNonquoteScalar{};
				int64_t prevInString{};
				StringBlockReader<256> stringReader{ this->stringView, this->stringLengthRaw };
				while (stringReader.hasFullBlock()) {
					this->section.submitDataForProcessing(stringReader.fullBlock(), prevInString, prevInScalar, followsPotentialNonquoteScalar);
					auto indexCount = section.getStructuralIndices(this->structuralIndexes.get(), tapeCurrentIndex, this->stringLengthRaw);
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
			--this->nStructuralIndexes;
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

		inline Document getJsonData(std::string& string);

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
		std::unique_ptr<OpenContainer[]> openContainers{};
		std::unique_ptr<uint32_t[]> structuralIndexes{};
		std::unique_ptr<uint8_t[]> stringBuffer{};
		std::unique_ptr<uint64_t[]> tape{};
		std::unique_ptr<bool[]> isArray{};
		size_t nStructuralIndexes{ 0 };
		SimdStringSection section{};
		size_t allocatedCapacity{};
		uint32_t maxDepth{ 512 };
		size_t stringLengthRaw{};
		size_t tapeLength{ 0 };
		uint8_t* stringView{};
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
		SimdJsonValue* masterParser{};
		const uint8_t* stringView{};
		uint32_t* nextStructural{};
		uint32_t depth{};

		inline ErrorCode walkDocument(TapeBuilder& visitor);

		inline JsonIterator(SimdJsonValue* masterParserNew, size_t start_structural_index);

		inline const uint8_t* peek() const noexcept;

		inline const uint8_t* advance() noexcept;

		inline size_t remainingLen() const noexcept;

		inline bool atEof() const noexcept;

		inline bool atBeginning() const noexcept;

		inline uint8_t lastStructural() const noexcept;

		inline ErrorCode visitRootPrimitive(TapeBuilder& visitor, const uint8_t* Value);
		inline ErrorCode visitPrimitive(TapeBuilder& visitor, const uint8_t* Value);
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
		inline static void write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept;
		inline void append(uint64_t val, TapeType t) noexcept;
		inline void appendDouble(double Value) noexcept;
		inline void appendU64(uint64_t Value) noexcept;
		inline void appendS64(int64_t Value) noexcept;
		inline void skipLargeInteger() noexcept;
		inline void skipDouble() noexcept;
		inline void skip() noexcept;

	  private:
		template<typename T> inline void append2(uint64_t val, T val2, TapeType t) noexcept;
	};

	inline void TapeWriter::appendS64(int64_t Value) noexcept {
		append2(0, Value, TapeType::Int64);
	}

	inline void TapeWriter::appendU64(uint64_t Value) noexcept {
		append2(0, Value, TapeType::Uint64);
	}

	inline void TapeWriter::appendDouble(double Value) noexcept {
		append2(0, Value, TapeType::Double);
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
		*this->nextTapeLocation = val | ((uint64_t(uint8_t(t))) << 56);
		this->nextTapeLocation++;
	}

	template<typename T> inline void TapeWriter::append2(uint64_t val, T val2, TapeType t) noexcept {
		append(std::move(val), std::move(t));
		static_assert(sizeof(val2) == sizeof(*this->nextTapeLocation), "Type is not 64 *theBits!");
		memcpy(this->nextTapeLocation, &val2, sizeof(val2));
		this->nextTapeLocation++;
	}

	inline void TapeWriter::write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept {
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

		inline ErrorCode visitPrimitive(JsonIterator& iter, const uint8_t* Value);

		inline ErrorCode visitRootPrimitive(JsonIterator& iter, const uint8_t* Value);

		inline ErrorCode visitString(JsonIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitNumber(JsonIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitTrueAtom(JsonIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitFalseAtom(JsonIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitNullAtom(JsonIterator& iter, const uint8_t* Value) noexcept;

		inline ErrorCode visitRootString(JsonIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitRootNumber(JsonIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitRootTrueAtom(JsonIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitRootFalseAtom(JsonIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitRootNullAtom(JsonIterator& iter, const uint8_t* Value) noexcept;

		inline ErrorCode incrementCount(JsonIterator& iter) noexcept;

		TapeWriter tape;

	  private:
		char* currentStringBufferLocation{};
		size_t& currentTapeLength;

		inline TapeBuilder(SimdJsonValue& doc) noexcept;

		inline uint32_t nextTapeIndex(JsonIterator& iter) const noexcept;
		inline ErrorCode startContainer(JsonIterator& iter) noexcept;
		inline ErrorCode endContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline ErrorCode emptyContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline char* onStartString(JsonIterator& iter) noexcept;
		inline ErrorCode onEndString(char* dst) noexcept;
	};

	inline ErrorCode TapeBuilder::parseDocument(SimdJsonValue& masterParser) {
		JsonIterator iter(&masterParser, 0);
		TapeBuilder builder(masterParser);
		return iter.walkDocument(builder);
	}

	inline ErrorCode TapeBuilder::visitRootPrimitive(JsonIterator& iter, const uint8_t* Value) {
		return iter.visitRootPrimitive(*this, Value);
	}
	inline ErrorCode TapeBuilder::visitPrimitive(JsonIterator& iter, const uint8_t* Value) {
		return iter.visitPrimitive(*this, Value);
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
		: tape{ doc.getTape() }, currentStringBufferLocation{ reinterpret_cast<char*>(doc.getStringBuffer()) },
		  currentTapeLength(doc.getTapeLength()){};

	inline ErrorCode TapeBuilder::visitString(JsonIterator& iter, const uint8_t* Value) noexcept {
		char* dst01 = onStartString(iter);
		dst01 = StringParser::parseString(reinterpret_cast<const char*>(Value) + 1ull, dst01, (*iter.nextStructural + 1ull) - (*iter.nextStructural));
		if (dst01 == nullptr) {
			return ErrorCode::StringError;
		}
		onEndString(dst01);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootString(JsonIterator& iter, const uint8_t* Value) noexcept {
		return visitString(iter, Value);
	}

	inline ErrorCode TapeBuilder::visitNumber(JsonIterator& iter, const uint8_t* Value) noexcept {
		return NumberParser::parseNumber<TapeWriter>(Value, this->tape);
	}

	inline ErrorCode TapeBuilder::visitRootNumber(JsonIterator& iter, const uint8_t* Value) noexcept {
		std::unique_ptr<uint8_t[]> copy(new (std::nothrow) uint8_t[iter.remainingLen() + 20]);
		if (copy.get() == nullptr) {
			return ErrorCode::MemAlloc;
		}
		std::memcpy(copy.get(), Value, iter.remainingLen());
		std::memset(copy.get() + iter.remainingLen(), ' ', 20);
		return visitNumber(iter, copy.get());
	}

	inline ErrorCode TapeBuilder::visitTrueAtom(JsonIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidTrueAtom(Value)) {
			return ErrorCode::TAtomError;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootTrueAtom(JsonIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidTrueAtom(Value)) {
			return ErrorCode::NAtomError;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitFalseAtom(JsonIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidFalseAtom(Value)) {
			return ErrorCode::FAtomError;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootFalseAtom(JsonIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidFalseAtom(Value)) {
			return ErrorCode::FAtomError;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitNullAtom(JsonIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidNullAtom(Value)) {
			return ErrorCode::NAtomError;
		}
		this->tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootNullAtom(JsonIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidNullAtom(Value)) {
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

	inline char* TapeBuilder::onStartString(JsonIterator& iter) noexcept {
		this->tape.append(currentStringBufferLocation - reinterpret_cast<char*>(iter.masterParser->getStringBuffer()), TapeType::String);
		return this->currentStringBufferLocation + sizeof(uint32_t);
	}

	inline ErrorCode TapeBuilder::onEndString(char* dst) noexcept {
		uint32_t strLength = uint32_t(dst - this->currentStringBufferLocation + sizeof(uint32_t));
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
		this->masterParser->getTapeLength() = 0;
		if (atEof()) {
			return ErrorCode::Empty;
		}
		this->masterParser->getTapeLength()++;
		visitor.visitDocumentStart(*this);
		{
			auto Value = this->advance();

			switch (*Value) {
				case '{':
					if (*this->peek() == '}') {
						this->advance();
						visitor.visitEmptyObject(*this);
						this->masterParser->getTapeLength()++;
						break;
					}
					goto Object_Begin;
				case '[':
					if (*this->peek() == ']') {
						this->advance();
						visitor.visitEmptyArray(*this);
						this->masterParser->getTapeLength()++;
						break;
					}
					goto Array_Begin;
				default:
					this->masterParser->getTapeLength()++;
					visitor.visitRootPrimitive(*this, Value);
					break;
			}
		}
		goto Document_End;

	Object_Begin:
		this->depth++;
		if (this->depth >= masterParser->getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		this->masterParser->getIsArray()[this->depth] = false;
		this->masterParser->getTapeLength()++;
		visitor.visitObjectStart(*this);
		{
			auto key = this->advance();
			if (*key != '"') {
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
			}
			this->masterParser->getTapeLength()++;
			visitor.visitKey(*this, key);
			visitor.incrementCount(*this);
		}

	Object_Field : {
		auto newValue = *this->advance();
		if (newValue != ':') {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
				", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
		{
			auto Value = this->advance();
			switch (*Value) {
				case '{':
					if (*this->peek() == '}') {
						this->advance();
						visitor.visitEmptyObject(*this);
						this->masterParser->getTapeLength()++;
						break;
					}
					goto Object_Begin;
				case '[':
					if (*this->peek() == ']') {
						this->advance();
						visitor.visitEmptyArray(*this);
						this->masterParser->getTapeLength()++;
						break;
					}
					goto Array_Begin;
				default:
					this->masterParser->getTapeLength()++;
					if (auto resultCode = visitor.visitPrimitive(*this, Value); resultCode != ErrorCode::Success) {
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
					this->masterParser->getTapeLength()++;
					visitor.visitKey(*this, key);
				}
				goto Object_Field;
			case '}':
				this->masterParser->getTapeLength()++;
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
			this->masterParser->getTapeLength()++;
			this->masterParser->getTapeLength()++;
			goto Document_End;
		}
		if (this->masterParser->getIsArray()[this->depth]) {
			this->masterParser->getTapeLength()++;
			goto Array_Continue;
		}
		this->masterParser->getTapeLength()++;
		goto Object_Continue;
	}

	Array_Begin : {
		this->depth++;
		if (this->depth >= masterParser->getMaxDepth()) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::DepthError) } +
				", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
		this->masterParser->getIsArray()[this->depth] = true;
		this->masterParser->getTapeLength()++;
		visitor.visitArrayStart(*this);
		visitor.incrementCount(*this);
	}

	Array_Value : {
		auto Value = this->advance();
		{
			switch (*Value) {
				case '{':
					if (*this->peek() == '}') {
						this->advance();
						visitor.visitEmptyObject(*this);
						this->masterParser->getTapeLength()++;
						break;
					}
					
					goto Object_Begin;
				case '[':
					if (*this->peek() == ']') {
						this->advance();
						visitor.visitEmptyArray(*this);
						this->masterParser->getTapeLength()++;
						break;
					}
					goto Array_Begin;
				default:
					this->masterParser->getTapeLength()++;
					if (auto resultCode = visitor.visitPrimitive(*this, Value); resultCode != ErrorCode::Success) {
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
		{
			switch (newValue) {
				case ',':
					this->masterParser->getTapeLength()++;
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

	Document_End : {
		this->masterParser->getTapeLength()++;
		visitor.visitDocumentEnd(*this);

		auto nextStructuralIndex = uint32_t(this->nextStructural - &this->masterParser->getStructuralIndexes()[0]);

		if (nextStructuralIndex != this->masterParser->getStructuralIndexCount()) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
				", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
	}
		return ErrorCode::Success;
	}

	inline ErrorCode JsonIterator::visitRootPrimitive(TapeBuilder& visitor, const uint8_t* Value) {
		switch (*Value) {
			case '"':
				return visitor.visitRootString(*this, Value);
			case 't':
				return visitor.visitRootTrueAtom(*this, Value);
			case 'f':
				return visitor.visitRootFalseAtom(*this, Value);
			case 'n':
				return visitor.visitRootNullAtom(*this, Value);
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
				return visitor.visitRootNumber(*this, Value);
			default:
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
	}

	inline ErrorCode JsonIterator::visitPrimitive(TapeBuilder& visitor, const uint8_t* Value) {
		switch (*Value) {
			case '"':
				return visitor.visitString(*this, Value);
			case 't':
				return visitor.visitTrueAtom(*this, Value);
			case 'f':
				return visitor.visitFalseAtom(*this, Value);
			case 'n':
				return visitor.visitNullAtom(*this, Value);
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
				return visitor.visitNumber(*this, Value);
			default:
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
	}
	
	Document SimdJsonValue::getJsonData(std::string& string) {
		this->generateJsonEvents(reinterpret_cast<uint8_t*>(string.data()), string.size());
		if (TapeBuilder::parseDocument(*this) != ErrorCode::Success) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } + ", at the following index into the string: " };
		}
		//for (size_t x = 0; x < this->getTapeLength(); ++x) {
		//std::cout << "CURRENT INDEX: " << (this->getTape()[x] >> 56) << std::endl;
		//}
		//std::cout << "TAPE LENGTH: " << this->getTapeLength() << std::endl;
		auto jsonParser =
			JsonParser{ reinterpret_cast<uint32_t*>(this->getStructuralIndexes()), this->getTapeLength(), this->stringBuffer.get(), this };
		return Document{ std::move(jsonParser) };
	}

};
