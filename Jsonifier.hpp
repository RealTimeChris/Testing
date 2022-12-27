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
					std::memcpy(&string_length, stringBuffer + (tape_val & JSON_VALUE_MASK), sizeof(uint32_t));
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

		inline uint8_t advance() noexcept {
			auto returnValue = (*(tapePosition++));
			for (size_t x = 0; x < this->currentStructuralCount; ++x) {
				std::cout << "RETURN VALUE: " << (tapePosition[x]>>56)<< std::endl;
			}
			this->rewind();
			return (returnValue >> 56);
		}

		inline void rewind() noexcept {
			tapePosition = this->initialTapePosition;
		}

		inline uint32_t currentOffset() noexcept {
			return *(tapePosition);
		}

		inline const uint8_t* returnCurrentAndAdvance() noexcept {
			return &stringBuffer[*(tapePosition++)];
		}

		inline const uint8_t* peek(uint64_t* position) noexcept {
			return &stringBuffer[*position];
		}

		inline uint32_t peekIndex(uint32_t* position) noexcept {
			return *position;
		}

		inline uint32_t peekLength(uint64_t* position) noexcept {
			return *(position + 1) - *position;
		}

		inline const uint8_t* peek(int32_t delta) noexcept {
			return &stringBuffer[*(tapePosition + delta)];
		}

		inline uint32_t peekIndex(int32_t delta) noexcept {
			return *(tapePosition + delta);
		}

		inline uint32_t peekLength(int32_t delta) noexcept {
			return *(tapePosition + delta + 1) - *(tapePosition + delta);
		}

		inline uint64_t* position() noexcept {
			return tapePosition;
		}

		inline void setPosition(uint64_t* target_position) noexcept {
			tapePosition = target_position;
		}


		inline bool atEof() {
			if ((this->tapePosition - this->initialTapePosition) >= this->currentStructuralCount) {
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
			if (*this->peek() == '[' || *this->peek() == '{' || *this->peek() == 'r') {
				return (((*tapePosition) & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK;

			} else if (*this->peek() == '"') {
				size_t stringLength{};
				std::memcpy(&stringLength, stringBuffer + ((*tapePosition) & JSON_VALUE_MASK), sizeof(uint32_t));
				return stringLength;
			} else {
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

	class SimdJsonValue;

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

		JsonParser() noexcept = default;

		inline JsonParser(uint8_t* buf, SimdJsonValue* _parser) noexcept;

		inline bool isSingleToken() const noexcept;

		inline void rewind() noexcept;

		inline bool balanced() noexcept {
			TapeIterator ti(tapeIter);
			int32_t count{ 0 };
			ti.setPosition(rootPosition());
			while (ti.peek() <= peekLast()) {
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
			if (depth() <= parent_depth) {
				return ErrorCode::Success;
			}
			switch (*returnCurrentAndAdvance()) {
				case '[':
				case '{':
				case ':':
					break;
				case ',':
					break;
				case ']':
				case '}':
					currentDepth--;
					if (depth() <= parent_depth) {
						return ErrorCode::Success;
					}
					break;
				case '"':
					if (*peek() == static_cast<uint8_t>(':')) {
						returnCurrentAndAdvance();
						break;
					}
					[[fallthrough]];
				default:
					currentDepth--;
					if (depth() <= parent_depth) {
						return ErrorCode::Success;
					}
					break;
			}

			while (position() < endPosition()) {
				switch (*returnCurrentAndAdvance()) {
					case '[':
					case '{':
						currentDepth++;
						break;
					case ']':
					case '}':
						currentDepth--;
						if (depth() <= parent_depth) {
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

		inline bool atRoot() noexcept;

		inline uint64_t* rootPosition() noexcept;

		inline void assertAtRoot() noexcept {
			assert(tapeIter.position() == root);
		}

		inline void assertMoreTokens(uint32_t required_tokens) noexcept {
			assert(tapeIter.position() + required_tokens - 1);
		}

		inline bool atEnd() noexcept {
			return position() == endPosition();
		}

		inline uint64_t* endPosition() noexcept {
			size_t n_structural_indexes{ tapeIter.getStructuralCount() };
			return &tapeIter.getTape()[n_structural_indexes];
		}

		inline std::string toString() noexcept {
			if (!isAlive()) {
				return "dead JsonParser instance";
			}
			const char* current_structural = reinterpret_cast<const char*>(tapeIter.peek());
			return std::string("JsonParser [ depth : ") + std::to_string(currentDepth) + std::string(", structural : '") +
				std::string(current_structural, 1) + std::string("', offset : ") + std::to_string(tapeIter.currentOffset()) +
				std::string("', error : ") + std::string(" ]");
		}

		inline const char* currentLocation() noexcept {
			if (!isAlive()) {
				if (!atRoot()) {
					return reinterpret_cast<const char*>(tapeIter.peek(-1));
				} else {
					return reinterpret_cast<const char*>(tapeIter.peek());
				}
			}
			if (atEnd()) {
				return nullptr;
			}
			return reinterpret_cast<const char*>(tapeIter.peek());
		}

		inline void abandon() noexcept {
			currentDepth = 0;
		}

		inline const uint8_t* returnCurrentAndAdvance() noexcept {
			return tapeIter.returnCurrentAndAdvance();
		}

		inline const uint8_t* unsafePointer() noexcept {
			return tapeIter.peek(0);
		}

		inline const uint8_t* peek(int32_t delta = 0) noexcept {
			return tapeIter.peek(delta);
		}

		inline uint32_t peekLength(int32_t delta) noexcept {
			return tapeIter.peekLength(delta);
		}

		inline const uint8_t* peek(uint64_t* position) noexcept {
			return tapeIter.peek(position);
		}

		inline uint32_t peekLength(uint64_t* position) noexcept {
			return tapeIter.peekLength(position);
		}

		inline uint64_t* lastPosition() noexcept {
			size_t n_structural_indexes{ tapeIter.getStructuralCount() };
			assert(n_structural_indexes > 0);
			return &tapeIter.getTape()[n_structural_indexes - 1];
		}

		inline const uint8_t* peekLast() noexcept {
			return tapeIter.peek(lastPosition());
		}

		inline void ascendTo(size_t parent_depth) noexcept {
			currentDepth = parent_depth;
		}

		inline void descendTo(size_t child_depth) noexcept {
			child_depth >= 1 && child_depth < INT32_MAX;
			//assert(currentDepth == child_depth - 1);
			currentDepth = child_depth;
		}

		inline bool isAlive() const noexcept {
			return parser;
		}

		inline size_t depth() const noexcept {
			return currentDepth;
		}

		inline uint8_t*& stringBufLoc() noexcept {
			return stringBufferLocation;
		}

		inline TapeIterator& getTapeIterator() {
			return this->tapeIter;
		}

		inline uint64_t* position() noexcept {
			return tapeIter.position();
		}

		inline void reenterChild(uint64_t* position, size_t child_depth) noexcept {
			tapeIter.setPosition(position);
			currentDepth = child_depth;
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

		inline JsonParser(uint64_t* tapePtrsNew, size_t count, uint8_t* stringBufferNew, SimdJsonValue* parserNew)
			: tapeIter{ stringBufferNew, tapePtrsNew, count } {
			this->stringBufferLocation = stringBufferNew;
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

		inline int64_t getInt64() {
			int64_t answer{};
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

		inline std::string_view getString() {
			std::string_view returnValue{};
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

		inline bool findField(const char* keyToField) {
			for (size_t x = 0; x < this->tapeIter.getStructuralCount(); ++x) {
				if (this->tapeIter.advance() == '"') {
					std::cout << "THE STRING: " << this->getString() << std::endl;
					if (this->getString() == keyToField) {
						return true;
					}
				}
			}
			return false;
		}

		inline JsonType getType() {
			JsonType returnValue{};
			return returnValue;
		}

		template<> inline JsonParser getValue() {
			return std::move(*this);
		}

		inline JsonParser& operator[](const std::string& key) {
			this->findField(key.data());
			return *this;
		};

		inline JsonParser(ErrorCode error) : tapeIter{ nullptr, nullptr, 0 } {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " + std::to_string(( int32_t )error) };
		}

	  protected:
		uint8_t* stringBufferLocation{};
		SimdJsonValue* parser{};
		TapeIterator tapeIter;
		size_t currentDepth{};
		uint64_t* root{};
	};
	
	class ArrayIterator;
	class SimdJsonValue;
	class JsonParser;
	class Array;

	class RawJsonString {
	  public:
		inline RawJsonString() noexcept = default;
		inline RawJsonString(const uint8_t* _buf) noexcept;
		inline const char* raw() const noexcept;
		inline bool unsafeIsEqual(size_t length, std::string_view target) const noexcept;
		inline bool unsafeIsEqual(std::string_view target) const noexcept;
		inline bool unsafeIsEqual(const char* target) const noexcept;
		inline bool isEqual(std::string_view target) const noexcept;
		inline bool isEqual(const char* target) const noexcept;
		static inline bool isFreeFromUnescapedQuote(std::string_view target) noexcept;
		static inline bool isFreeFromUnescapedQuote(const char* target) noexcept;

	  private:
		inline void consume() noexcept {
			buf = nullptr;
		}
		inline bool alive() const noexcept {
			return buf != nullptr;
		}
		inline std::string_view unescape(JsonParser& iter) const noexcept;

		const uint8_t* buf{};
		friend class object;
		friend class field;
		friend class parser;
	};

	class ValueIterator {
	  protected:
		uint64_t* rootPosition{};
		JsonParser* jsonIter{};
		size_t currentDepth{};

	  public:
		ValueIterator() noexcept = default;
		inline void advanceRootScalar(const char* type) noexcept;
		inline const uint8_t* peekNonRootScalar(const char* type) noexcept;
		inline void advanceNonRootScalar(const char* type) noexcept;
		inline bool startedArray() noexcept;
		inline bool atEnd() noexcept;
		inline bool atStart() noexcept;
		inline bool isAtStart() noexcept;
		inline bool isOpen() noexcept;
		inline bool atRoot() noexcept;
		inline bool atFirstField() noexcept;
		inline void abandon() noexcept;
		inline size_t depth() noexcept;
		inline JsonType type() noexcept;
		inline ValueIterator child() noexcept;
		inline uint32_t peekStartLength() noexcept;
		inline bool startValue() noexcept;
		inline RawJsonString fieldKey() noexcept;
		inline const uint8_t* peekStart() noexcept;
		inline bool findFieldRaw(const std::string_view key) noexcept;
		inline ErrorCode skipChild() noexcept;
		inline bool startedValue() noexcept;
		inline bool hasNextField() noexcept;
		inline ErrorCode fieldValue() noexcept;
		inline bool startArray() noexcept;
		inline const uint8_t* peekRootScalar(const char* type) noexcept;
		inline bool hasNextElement() noexcept;
		inline ErrorCode endContainer() noexcept;
		inline bool isRootNull() noexcept;
		inline ErrorCode startContainer(uint8_t start_char, const char* incorrect_type_message, const char* type) noexcept;
		
		inline std::string_view getString() noexcept;
		inline uint64_t getUint64() noexcept;
		inline int64_t getInt64() noexcept;
		inline double getDouble() noexcept;
		inline bool getBool() noexcept;
		inline bool isNull() noexcept;

		inline ErrorCode error() noexcept;
		inline uint8_t*& stringBufLoc() noexcept;

		inline ValueIterator(JsonParser* json_iter, size_t depth, uint64_t* start_position) noexcept;

	  protected:
		inline uint64_t* startPosition() noexcept;
		inline uint64_t* lastPosition() noexcept;
		inline uint64_t* endPosition() noexcept;
		inline uint64_t* position() noexcept;
	};

	class Value {
	  public:
		Value() noexcept = default; 
		template<typename T> inline T get() noexcept {
			static_assert(!sizeof(T), "The get method with given type is not implemented by the simdjson library.");
		}
		template<typename T> inline ErrorCode get(T& out) noexcept;
		inline Array getArray() noexcept;
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
		inline Value getValue() noexcept;
		inline ValueIterator resumeValueIterator() noexcept;
		inline Value start(ValueIterator& iter) noexcept;
		inline Value at(size_t index) noexcept;
		inline Value findField(std::string_view key) noexcept;
		inline Value findField(const char* key) noexcept;
		inline Value findFieldUnordered(std::string_view key) noexcept;
		inline Value findFieldUnordered(const char* key) noexcept;
		inline Value startOrResumeValue() noexcept;
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
		ValueIterator iter;
	};
	
	class ArrayIterator {
	  public:
		inline ArrayIterator() noexcept = default;
		inline Value operator*() noexcept;
		inline bool operator==(const ArrayIterator&) noexcept;
		inline bool operator!=(const ArrayIterator&) noexcept;
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
		inline Array(const ValueIterator& iter) noexcept;
		inline ArrayIterator begin() noexcept;
		inline ArrayIterator end() noexcept;
		inline size_t countElements() & noexcept;
		inline bool isEmpty() & noexcept;
		inline static Array start(ValueIterator&) noexcept;
		inline bool reset() & noexcept;
		inline Value at(size_t index) noexcept;
		ArrayIterator iter;

		friend class Value;
		friend class Document;
		friend struct Value;
		friend struct Array;
		friend class ArrayIterator;
	};

	class Document {
	  public:
		inline Document() noexcept {};
		inline Document(const Document& other) noexcept = delete;
		inline Document(Document&& other) noexcept = default;
		inline Document& operator=(const Document& other) noexcept = delete;
		inline Document& operator=(Document&& other) noexcept = default;
		inline ValueIterator getRootValueIterator() noexcept;
		inline ValueIterator resumeValueIterator() noexcept;
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

		template<size_t amount> inline SimdBase256 shl(SimdBase256 previousValue) {
			SimdBase256 returnValue{};
			this->printBits("PRE-SHIFT(LEFT): ");
			auto newValue01 = SimdBase256{ _mm256_srli_epi64(*this, amount) };
			newValue01.printBits("POST-SHIFT 01(LEFT): ");

			auto newValue02 = SimdBase256{ _mm256_srli_epi64(_mm256_slli_si256(previousValue, 7), 64 - amount) };
			auto newValue03 = SimdBase256{ _mm256_slli_epi64(_mm256_srli_si256(*this, 7), 64 - amount) };
			newValue02.printBits("POST-SHIFT 02(LEFT): ");
			newValue03.printBits("POST-SHIFT 03(LEFT): ");
			returnValue = newValue01 | newValue02 | newValue03;
			returnValue.printBits("POST-SHIFT(LEFT): ");
			return returnValue;
		}

		template<size_t amount> inline SimdBase256 shr(SimdBase256 previousValue) {
			SimdBase256 returnValue{};
			this->printBits("PRE-SHIFT(RIGHT): ");

			auto newValue01 = SimdBase256{ _mm256_slli_epi64(*this, 64 - amount) };
			newValue01.printBits("POST-SHIFT 01(RIGHT): ");
			auto newValue02 = SimdBase256{ _mm256_slli_epi64(_mm256_srli_si256(previousValue, 1), amount) };
			auto newValue03 = SimdBase256{ _mm256_srli_epi64(_mm256_slli_si256(*this, 1), amount) };
			
			newValue02.printBits("POST-SHIFT 02(RIGHT): ");
			newValue03.printBits("POST-SHIFT 03(RIGHT): ");
			returnValue = newValue01 | newValue02 | newValue03;
			returnValue.printBits("POST-SHIFT(RIGHT): ");
			return returnValue;
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
			for (int i = 0; i < cnt; i++) {
				newValue = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + currentIndexIntoString;

				if (newValue >= stringLength) {
					currentIndexIntoTape += cnt;
					return cnt;

				} else {
					tapePtrs[i + currentIndexIntoTape] = newValue;
					*theBits = _blsr_u64(*theBits);
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

		inline SimdBase256 follows(SimdBase256 match, SimdBase256& overflow) {
			auto newMatch = match.shl<1>(this->previousMatch);
			SimdBase256 result = newMatch | overflow;
			overflow = newMatch.shr<63>(this->previousMatch);
			this->previousMatch = newMatch;
			this->previousMatch.printBits("PREVIOUS MATCH: ");
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

		inline SimdBase256 collectQuotedRange() {
			SimdBase256 backslashes = _mm256_set1_epi8('\\');
			SimdBase256 backslashesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				backslashesReal[x] = this->values[x] == backslashes;
			}

			auto B256 = convertSimdBytesToBits(backslashesReal);

			SimdBase256 E{ _mm256_set1_epi8(0b01010101) };
			SimdBase256 O{ _mm256_set1_epi8(0b10101010) };
			this->S256 = B256.bitAndNot(B256.shl<1>(B256));
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
			return this->Q256.carrylessMultiplication(this->prevInString);
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
			auto scalar = ~this->S256 | this->W256;
			auto stringTail = this->R256 ^ this->Q256;
			SimdBase256 nonquoteScalar = scalar.bitAndNot(this->Q256);
			this->followsPotentialNonquoteScalar = follows(nonquoteScalar, this->prevInScalar);
			this->followsPotentialNonquoteScalar.printBits("NONQUOTE SCALAR: ");
			return this->S256 | (~S256 | this->W256).bitAndNot(this->followsPotentialNonquoteScalar).bitAndNot(stringTail);
		}

		void submitDataForProcessing(const uint8_t* valueNew) {
			this->packStringIntoValue(&this->values[0], valueNew);
			this->packStringIntoValue(&this->values[1], valueNew + 32);
			this->packStringIntoValue(&this->values[2], valueNew + 64);
			this->packStringIntoValue(&this->values[3], valueNew + 96);
			this->packStringIntoValue(&this->values[4], valueNew + 128);
			this->packStringIntoValue(&this->values[5], valueNew + 160);
			this->packStringIntoValue(&this->values[6], valueNew + 192);
			this->packStringIntoValue(&this->values[7], valueNew + 224);
			this->Q256 = this->collectQuotes();
			this->R256 = this->collectQuotedRange();
			this->W256 = this->collectWhiteSpace();
			this->S256 = this->collectStructuralCharacters();
			this->S256 = this->collectFinalStructurals();
			this->S256.printBits("FINAL BITS: ");
		}

	  protected:
		SimdBase256 followsPotentialNonquoteScalar{};
		size_t currentIndexIntoString{};
		SimdBase256 previousMatch{};
		SimdBase256 prevInScalar{};
		SimdBase256 values[8]{};
		int64_t prevInString{};
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
		SimdJsonValue& operator=(SimdJsonValue&&) = default;
		SimdJsonValue(SimdJsonValue&&) = default;
		inline SimdJsonValue() {
		}

		int64_t round(int64_t a, int64_t n) {
			return (((a) + (( n )-1)) & ~(( n )-1));
		}

		Document& getDocument() {
			return this->document;
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
				StringBlockReader<256> stringReader{ this->stringView, this->stringLengthRaw };
				while (stringReader.hasFullBlock()) {
					this->section.submitDataForProcessing(stringReader.fullBlock());
					auto indexCount = section.getStructuralIndices(this->structuralIndexes.get(), tapeCurrentIndex, this->stringLengthRaw);
					this->nStructuralIndexes += indexCount;
					stringReader.advance();
				}
				uint8_t block[256];
				stringReader.getRemainder(block);
				this->section.submitDataForProcessing(block);
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
		Document document;
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

		inline const uint8_t* peek() noexcept;

		inline const uint8_t* advance() noexcept;

		inline size_t remainingLen() noexcept;

		inline bool atEof() noexcept;

		inline bool atBeginning() noexcept;

		inline uint8_t lastStructural() noexcept;

		inline ErrorCode visitRootPrimitive(TapeBuilder& visitor, const uint8_t* Value);
		inline ErrorCode visitPrimitive(TapeBuilder& visitor, const uint8_t* Value);
	};

	inline JsonIterator::JsonIterator(SimdJsonValue* masterParserNew, size_t start_structural_index)
		: nextStructural(masterParserNew->getStructuralIndexes()), stringView{ masterParserNew->getStringView() }, masterParser{ masterParserNew } {};

	inline const uint8_t* JsonIterator::peek() noexcept {
		return &this->stringView[*this->nextStructural];
	}

	inline const uint8_t* JsonIterator::advance() noexcept {
		return &this->stringView[*this->nextStructural++];
	}

	inline size_t JsonIterator::remainingLen() noexcept {
		return this->masterParser->getStructuralIndexCount() - *this->nextStructural;
	}

	inline bool JsonIterator::atEof() noexcept {
		return this->nextStructural == &this->masterParser->getStructuralIndexes()[this->masterParser->getStructuralIndexCount() - 1];
	}

	inline bool JsonIterator::atBeginning() noexcept {
		return this->nextStructural == this->masterParser->getStructuralIndexes();
	}

	inline uint8_t JsonIterator::lastStructural() noexcept {
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
		uint8_t* currentStringBufferLocation{};
		size_t& currentTapeLength;

		inline TapeBuilder(SimdJsonValue& doc) noexcept;

		inline uint32_t nextTapeIndex(JsonIterator& iter) noexcept;
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
		: tape{ doc.getTape() }, currentStringBufferLocation{ doc.getStringBuffer() },
		  currentTapeLength(doc.getTapeLength()){};

	inline ErrorCode TapeBuilder::visitString(JsonIterator& iter, const uint8_t* Value) noexcept {
		uint8_t* dst01 = onStartString(iter);
		dst01 = StringParser::parseString(reinterpret_cast<const uint8_t*>(Value) + 1ull, reinterpret_cast<uint8_t*>(dst01),
			(*iter.nextStructural + 1ull) - (*iter.nextStructural));
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
		std::unique_ptr<uint8_t[]> copy(new (std::nothrow) uint8_t[iter.remainingLen() + 256]);
		if (copy.get() == nullptr) {
			return ErrorCode::MemAlloc;
		}
		std::memcpy(copy.get(), Value, iter.remainingLen());
		std::memset(copy.get() + iter.remainingLen(), ' ', 256);
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

	inline uint32_t TapeBuilder::nextTapeIndex(JsonIterator& iter) noexcept {
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
		uint32_t strLength = uint32_t(dst - (this->currentStringBufferLocation + sizeof(uint32_t)));
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
		visitor.visitDocumentStart(*this);
		{
			auto Value = this->advance();

			switch (*Value) {
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
		if (this->masterParser->getIsArray()[this->depth]) {
			goto Array_Continue;
		}
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
	
	JsonParser SimdJsonValue::getJsonData(std::string& string) {
		this->generateJsonEvents(reinterpret_cast<uint8_t*>(string.data()), string.size());
		if (TapeBuilder::parseDocument(*this) != ErrorCode::Success) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } + ", at the following index into the string: " };
		}
		this->getTapeLength() = (this->getTape()[0] & JSON_VALUE_MASK);
		for (size_t x = 0; x < this->getTapeLength(); ++x) {
			std::cout << "CURRENT INDEX (VALUE): " << (this->getTape()[x] >> 56) << std::endl;
			std::cout << "CURRENT INDEX (COUNT): " << (this->getTape()[x] & JSON_COUNT_MASK) << std::endl;
		}		
		//dumpRawTape(std::cout, this->getTape(), this->getStringBuffer());
		//std::cout << "TAPE LENGTH: " << this->getTapeLength() << std::endl;
		
		return JsonParser{ this->getTape(), this->getTapeLength(), this->getStringBuffer(), this };
	}

	inline Document::Document(JsonParser&& _iter) noexcept : iter{ _iter } {
	}

	inline Document Document::start(JsonParser&& iter) noexcept {
		return Document(std::move(iter));
	}

	inline Value Document::getValue() noexcept {
		switch (*iter.peek()) {
			case '[':
			case '{':
				return Value(getRootValueIterator());
			default:
				return Value(getRootValueIterator());
		}
	}

	inline ValueIterator Document::resumeValueIterator() noexcept {
		return ValueIterator(&iter, 1, iter.rootPosition());
	}

	inline ValueIterator Document::getRootValueIterator() noexcept {
		return resumeValueIterator();
	}

	inline ValueIterator::ValueIterator(JsonParser* json_iter, size_t depth, uint64_t* start_position) noexcept {
		this->rootPosition = start_position;
		this->currentDepth = depth;
		this->jsonIter = json_iter;
	}

	inline Value::Value(const ValueIterator& iter) noexcept : iter{ iter } {
		this->iter = iter;
	}

	inline Value Value::operator[](const char* key) noexcept {
		return Value::resume(resumeValueIterator());
	}

	inline Value Value::startOrResumeValue() noexcept {
		if (iter.atRoot()) {
			return getValue();
		} else {
			return Value::resume(resumeValueIterator());
		}
	}

	inline Value Value::getValue() noexcept {
		return Value::start(iter);
	}

	inline ValueIterator Value::resumeValueIterator() noexcept {
		return ValueIterator(iter);
	}

	inline bool ValueIterator::startValue() noexcept {
		startContainer('{', "Not an object", "object");
		return startedValue();
	}

	inline ErrorCode ValueIterator::endContainer() noexcept {
		jsonIter->ascendTo(depth() - 1);
		return ErrorCode::Success;
	}

	inline bool ValueIterator::startedValue() noexcept {
		if (*jsonIter->peek() == '}') {
			jsonIter->returnCurrentAndAdvance();
			endContainer();
			return false;
		}
		return true;
	}

	inline Value Value::start(ValueIterator& iter) noexcept {
		iter.startValue();
		return Value(iter);
	}

	inline bool ValueIterator::isAtStart() noexcept {
		return position() == startPosition();
	}

	inline const uint8_t* ValueIterator::peekStart() noexcept {
		return jsonIter->peek(startPosition());
	}

	inline ErrorCode ValueIterator::startContainer(uint8_t start_char, const char* incorrect_type_message, const char* type) noexcept {
		const uint8_t* json{};
		if (!isAtStart()) {
			json = peekStart();
			if (*json != start_char) {
				return ErrorCode::Incorrect_Type;
			}
		} else {
			json = jsonIter->peek();
			if (*json != start_char) {
				return ErrorCode::Incorrect_Type;
			}
			jsonIter->returnCurrentAndAdvance();
		}

		return ErrorCode::Success;
	}

	inline uint64_t* ValueIterator::startPosition() noexcept {
		return rootPosition;
	}

	inline uint64_t* ValueIterator::position() noexcept {
		return this->rootPosition;
	}

	inline uint64_t* ValueIterator::endPosition() noexcept {
		return jsonIter->endPosition();
	}

	inline uint64_t* ValueIterator::lastPosition() noexcept {
		return jsonIter->lastPosition();
	}

	inline bool ValueIterator::atRoot() noexcept {
		return position() == startPosition();
	}

	inline size_t ValueIterator::depth() noexcept {
		return currentDepth;
	}

	inline Value Value::resume(const ValueIterator& iter) noexcept {
		return iter;
	}

	inline bool Document::isNull() noexcept {
		return getRootValueIterator().isRootNull();
	}

	inline const uint8_t* ValueIterator::peekNonRootScalar(const char* type) noexcept {
		if (!isAtStart()) {
			return peekStart();
		}
		return jsonIter->peek();
	}

	inline const uint8_t* ValueIterator::peekRootScalar(const char* type) noexcept {
		if (!isAtStart()) {
			return peekStart();
		}

		return jsonIter->peek();
	}

	inline uint32_t ValueIterator::peekStartLength() noexcept {
		return jsonIter->peekLength(startPosition());
	}

	inline bool ValueIterator::isRootNull() noexcept {
		if (!jsonIter->isSingleToken()) {
			return false;
		}
		auto max_len = peekStartLength();
		auto json = peekRootScalar("null");
		bool result = (max_len >= 4 && !StringParser::str4ncmp(json, "null") && (max_len == 4 || NumberParser::isNotStructuralOrWhitespace(json[5])));
		if (result) {
			advanceRootScalar("null");
		}
		return result;
	}

	inline bool Value::isNull() noexcept {
		return iter.isNull();
	}

	inline void ValueIterator::advanceRootScalar(const char* type) noexcept {
		if (!isAtStart()) {
			return;
		}
		jsonIter->returnCurrentAndAdvance();
		jsonIter->ascendTo(depth() - 1);
	}

	inline ValueIterator ValueIterator ::child() noexcept {
		return { jsonIter, depth() + 1, jsonIter->getTapeIterator().position() };
	}

	inline Value Value::findField(const char* key) noexcept {
		if (!iter.findFieldRaw(key)) {
			return Value(iter.child());
		}
		return Value(iter.child());
	}

	inline bool ValueIterator::isNull() noexcept {
		bool is_null_value;
		return is_null_value;
	}

	inline ErrorCode ValueIterator::skipChild() noexcept {
		assert(jsonIter->getTapeIterator().position() > rootPosition);
		assert(jsonIter->depth() >= currentDepth);
		return jsonIter->skipChild(depth());
	}

	inline RawJsonString::RawJsonString(const uint8_t* _buf) noexcept : buf{ _buf } {
	}

	inline const char* RawJsonString::raw() const noexcept {
		return reinterpret_cast<const char*>(buf);
	}


	inline bool RawJsonString::isFreeFromUnescapedQuote(std::string_view target) noexcept {
		size_t pos{ 0 };
		for (; pos < target.size() && target[pos] != '\\'; pos++) {}
		bool escaping{ false };
		for (; pos < target.size(); pos++) {
			if ((target[pos] == '"') && !escaping) {
				return false;
			} else if (target[pos] == '\\') {
				escaping = !escaping;
			} else {
				escaping = false;
			}
		}
		return true;
	}

	inline bool RawJsonString::isFreeFromUnescapedQuote(const char* target) noexcept {
		size_t pos{ 0 };
		for (; target[pos] && target[pos] != '\\'; pos++) {}
		bool escaping{ false };
		for (; target[pos]; pos++) {
			if ((target[pos] == '"') && !escaping) {
				return false;
			} else if (target[pos] == '\\') {
				escaping = !escaping;
			} else {
				escaping = false;
			}
		}
		return true;
	}

	inline bool RawJsonString::unsafeIsEqual(size_t length, std::string_view target) const noexcept {
		return (length >= target.size()) && (raw()[target.size()] == '"') && !memcmp(raw(), target.data(), target.size());
	}

	inline bool RawJsonString::unsafeIsEqual(std::string_view target) const noexcept {
		if (target.size() <= 256) {
			return (raw()[target.size()] == '"') && !memcmp(raw(), target.data(), target.size());
		}
		const char* r{ raw() };
		size_t pos{ 0 };
		for (; pos < target.size(); pos++) {
			if (r[pos] != target[pos]) {
				return false;
			}
		}
		if (r[pos] != '"') {
			return false;
		}
		return true;
	}

	inline bool RawJsonString::isEqual(std::string_view target) const noexcept {
		const char* r{ raw() };
		size_t pos{ 0 };
		bool escaping{ false };
		for (; pos < target.size(); pos++) {
			if (r[pos] != target[pos]) {
				return false;
			}
			if ((target[pos] == '"') && !escaping) {
				return false;
			} else if (target[pos] == '\\') {
				escaping = !escaping;
			} else {
				escaping = false;
			}
		}
		if (r[pos] != '"') {
			return false;
		}
		return true;
	}

	inline JsonType Value::type() noexcept {
		return this->iter.type();
	}

	inline JsonType ValueIterator::type() noexcept {
		std::cout << "PEEK START: " << +*this->peekStart() << std::endl;
		switch (*peekStart()) {
			case '{':
				return JsonType::Object;
			case '[':
				return JsonType::Array;
			case '"':
				return JsonType::String;
			case 'n':
				return JsonType::Null;
			case 't':
			case 'f':
				return JsonType::Bool;
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
				return JsonType::Int64;
			default:
				return static_cast<JsonType>(ErrorCode::Incorrect_Type);
		}
	}

	inline bool RawJsonString::unsafeIsEqual(const char* target) const noexcept {
		const char* r{ raw() };
		size_t pos{ 0 };
		for (; target[pos]; pos++) {
			if (r[pos] != target[pos]) {
				return false;
			}
		}
		if (r[pos] != '"') {
			return false;
		}
		return true;
	}

	inline bool RawJsonString::isEqual(const char* target) const noexcept {
		const char* r{ raw() };
		size_t pos{ 0 };
		bool escaping{ false };
		for (; target[pos]; pos++) {
			if (r[pos] != target[pos]) {
				return false;
			}
			if ((target[pos] == '"') && !escaping) {
				return false;
			} else if (target[pos] == '\\') {
				escaping = !escaping;
			} else {
				escaping = false;
			}
		}
		if (r[pos] != '"') {
			return false;
		}
		return true;
	}

	inline std::string_view RawJsonString::unescape(JsonParser& iter) const noexcept {
		return std::string_view{};
	}

	inline RawJsonString ValueIterator::fieldKey() noexcept {

		const uint8_t* key = jsonIter->returnCurrentAndAdvance();
		if (*(key++) != '"') {
			return RawJsonString{};
			
		}
		return RawJsonString(key);
	}

	inline bool ValueIterator::findFieldRaw(const std::string_view key) noexcept {
		ErrorCode error{};
		bool has_value{};
		if (atFirstField()) {
			has_value = true;

		} else if (!isOpen()) {
			return false;
		} else {
			if (skipChild() != ErrorCode::Success) {
				abandon();
				return false;
			}
			if (!hasNextField()) {
				abandon();
				return false;
			}
		}
		while (has_value) {
			RawJsonString actual_key{};
			if (fieldKey().isEqual("")) {
				abandon();
				return false;
			};
			if (fieldValue() != ErrorCode::Success) {
				abandon();
				return false;
			}
			if (actual_key.unsafeIsEqual(key)) {
				return true;
			}
			skipChild();
			if (!hasNextField()) {
				abandon();
				return false;
			}
		}
		return false;
	}

	inline void ValueIterator::abandon() noexcept {
		jsonIter->abandon();
	}

	inline bool ValueIterator::isOpen()  noexcept {
		return jsonIter->depth() >= depth();
	}

	inline bool ValueIterator::atFirstField() noexcept {
		assert(jsonIter->getTapeIterator().position()  > rootPosition);
		return jsonIter->getTapeIterator().position() == startPosition() + 1;
	}

	inline bool ValueIterator::hasNextField() noexcept {
		switch (*jsonIter->returnCurrentAndAdvance()) {
			case '}':
				endContainer();
				return false;
			case ',':
				return true;
			default:
				return false;
		}
	}

	inline Array::Array(const ValueIterator& iterator) noexcept : iter{ iterator } {
		this->iter = iterator;
	}

	inline Array Value::getArray() noexcept {
		return Array::start(iter);
	}

	inline Array Array::start(ValueIterator& iter) noexcept {
		bool has_value{};
		iter.startArray();
		return Array(iter);
	}

	inline bool ValueIterator::startedArray() noexcept {
		if (*jsonIter->peek() == ']') {
			jsonIter->returnCurrentAndAdvance();
			endContainer();
			return false;
		}
		jsonIter->descendTo(depth() + 1);
		return true;
	}

	inline std::string_view Value::getString() noexcept {
		return iter.getString();
	}

	inline double Value::getDouble() noexcept {
		return iter.getDouble();
	}

	inline uint64_t Value::getUint64() noexcept {
		return iter.getUint64();
	}

	inline int64_t Value::getInt64() noexcept {
		return iter.getInt64();
	}

	inline bool Value::getBool() noexcept {
		return iter.getBool();
	}

	inline bool ValueIterator::startArray() noexcept {
		startContainer('[', "Not an array", "array");
		return startedArray();
	}

	inline ErrorCode ValueIterator::fieldValue() noexcept {

		if (*jsonIter->returnCurrentAndAdvance() != ':') {
			return ErrorCode::TapeError;
		}
		jsonIter->descendTo(depth() + 1);
		return ErrorCode::Success;
	}

	inline bool JsonParser::isSingleToken() const noexcept {
		return parser->getStructuralIndexCount() == 1;
	}

	inline void ValueIterator::advanceNonRootScalar(const char* type) noexcept {
		if (!isAtStart()) {
			return;
		}

		jsonIter->returnCurrentAndAdvance();
		jsonIter->ascendTo(depth() - 1);
	}

	inline std::string_view ValueIterator::getString() noexcept {
		return std::string_view{};
		//get_raw_json_string().unescape(json_iter());
	}
	inline uint64_t ValueIterator::getUint64() noexcept {
		auto result = NumberParser::parseNumber<uint64_t>(peekNonRootScalar("uint64"));
		 advanceNonRootScalar("uint64");
		return result;
	}
	 inline int64_t ValueIterator::getInt64() noexcept {
		auto result = NumberParser::parseNumber<int64_t>(peekNonRootScalar("int64"));
		 advanceNonRootScalar("int64");
		return result;
	}
	 inline double ValueIterator::getDouble() noexcept {
		auto result = NumberParser ::parseDouble(peekNonRootScalar("double"));
		 advanceNonRootScalar("double");
		return result;
	}

	 inline bool ValueIterator::getBool() noexcept {
		auto result = *peekNonRootScalar("bool");
		if (result == 'f') {
			advanceNonRootScalar("bool");
			return false;

		} else {
			return true;
		}
	}

	 inline ArrayIterator::ArrayIterator(const ValueIterator& other) noexcept : iter{ other } {
		this->iter = other;
	 }

	 inline ArrayIterator& ArrayIterator::operator++() noexcept {
		ErrorCode error{};
		// PERF NOTE this is a safety rail ... users should exit loops as soon as they receive an error, so we'll never get here.
		// However, it does not seem to make a perf difference, so we add it out of an abundance of caution.
		if (iter.error() != ErrorCode::Success) {
			return *this;
		}
		if (iter.skipChild() != ErrorCode::Success) {
			return *this;
		}
		if (iter.hasNextElement()) {
			return *this;
		}
		return *this;
	}

	 inline bool ArrayIterator::operator!=(const ArrayIterator&) noexcept {
		return iter.isOpen();
	}

	 inline ErrorCode ValueIterator::error() noexcept {
		return ErrorCode{};
	 }

	 inline Value ArrayIterator::operator*() noexcept {
		if (iter.error()!=ErrorCode::Success) {
			iter.abandon();
			return Value(iter.child());
		}
		return Value(iter.child());
	}

	 inline ArrayIterator Array::begin() noexcept {
		return ArrayIterator(iter);
	}
	 inline ArrayIterator  Array::end() noexcept {
		return ArrayIterator(iter);
	}

	 inline bool ValueIterator::hasNextElement() noexcept {

		switch (*jsonIter->returnCurrentAndAdvance()) {
			case ']':
				endContainer();
				return false;
			case ',':
				jsonIter->descendTo(depth() + 1);
				return true;
			default:
				return false;
		}
	}

	 inline uint64_t* JsonParser::rootPosition() noexcept {
		return this->tapeIter.getTape();
	}

	 inline ArrayIterator Value::begin() & noexcept {
		return getArray().begin();
	}
	inline ArrayIterator Value::end() & noexcept {
		return {};
	}

	inline size_t Array::countElements() & noexcept {
		size_t count{ 0 };
		// Important: we do not consume any of the values.
		for (Value v: *this) {
			count++;
		}
		return count;
	}

	inline Value Array::at(size_t index) noexcept {
		size_t i = 0;
		for (auto value: *this) {
			if (i == index) {
				return value;
			}
			i++;
		}
		return Value{};
	}

};