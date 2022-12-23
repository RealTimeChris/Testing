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
		template<IsEnum EnumType> EnumConverter& operator=(const std::vector<EnumType>& data) {
			for (auto& value: data) {
				this->vector.emplace_back(std::move(static_cast<size_t>(value)));
			}
			return *this;
		};

		template<IsEnum EnumType> EnumConverter(const std::vector<EnumType>& data) {
			*this = data;
		};

		template<IsEnum EnumType> EnumConverter& operator=(EnumType data) {
			this->integer = static_cast<size_t>(data);
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
			this->jsonValue.numberUint = static_cast<size_t>(data);
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
		uint32_t string_length;
		size_t tape_idx = 0;
		uint64_t tape_val = tape[tape_idx];
		uint8_t type = uint8_t(tape_val >> 56);
		std::cout  << tape_idx << " : " << type;
		tape_idx++;
		size_t how_many = 0;
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
		TapeIterator(uint8_t* bufferNew, uint64_t* tapePositionNew) {
			this->initialTapePosition = tapePositionNew;
			this->tapePosition = tapePositionNew;
			this->buffer = bufferNew;
		}

		inline uint64_t currentOffset() const noexcept {
			return tapePosition - initialTapePosition;
		}

		inline uint8_t* advance() noexcept {
			return &this->buffer[*(tapePosition++) & JSON_VALUE_MASK];
		}

		inline uint8_t* peek() const noexcept {
			return &this->buffer[*( tapePosition )&JSON_VALUE_MASK];
		}

		inline uint32_t peekIndex() const noexcept {
			return (tapePosition - this->initialTapePosition);
		}

		inline size_t peekLengthOrSize() const noexcept {
			switch (*this->peek()) {
				case 'r': {
					return ((*tapePosition) & JSON_VALUE_MASK);
				}
			}
			return (*(tapePosition + 1) & JSON_VALUE_MASK) - ((*tapePosition) & JSON_VALUE_MASK);
		}

		inline uint32_t position() const noexcept {
			return *tapePosition;
		}

		inline void setPosition(uint64_t target_position) noexcept {
			tapePosition = this->initialTapePosition + target_position;
		}

		inline bool operator==(const TapeIterator& other) const noexcept {
			return tapePosition == other.tapePosition;
		}

		inline bool operator!=(const TapeIterator& other) const noexcept {
			return tapePosition != other.tapePosition;
		}

		inline bool operator>(const TapeIterator& other) const noexcept {
			return tapePosition > other.tapePosition;
		}

		inline bool operator>=(const TapeIterator& other) const noexcept {
			return tapePosition >= other.tapePosition;
		}

		inline bool operator<(const TapeIterator& other) const noexcept {
			return tapePosition < other.tapePosition;
		}

		inline bool operator<=(const TapeIterator& other) const noexcept {
			return tapePosition <= other.tapePosition;
		}

	protected:
		uint64_t* initialTapePosition{};
		uint64_t* tapePosition{};
		uint8_t* buffer{};
	};
	
	class JsonParser {
	  public:
		JsonParser& operator=(JsonParser&& other) noexcept {
			//this->currentStructuralCount = other.currentStructuralCount;
			//other.currentStructuralCount = 0;
			//this->stringBuffer = other.stringBuffer;
			//this->stringBuffer = std::move(other.stringBuffer);
			this->tapeIter = other.tapeIter;
			return *this;
		}

		JsonParser& operator=(const JsonParser&) = delete;
		JsonParser(const JsonParser&) = delete;

		JsonParser(JsonParser&& other) noexcept  {
			*this = std::move(other);
		}

		JsonParser(uint64_t* tapePtrsNew, size_t count, uint8_t* stringBufferNew) {
			//this->stringBuffer = stringBufferNew;
			//this->currentStructuralCount = count;
			this->tapeIter = TapeIterator{ stringBufferNew, tapePtrsNew };
		}

		template<typename OTy> inline OTy getValue();

		template<> inline double getValue() {
			double returnValue{};
			return returnValue;
		}

		template<> inline bool getValue() {
			bool returnValue{};
			return returnValue;
		}

		template<> inline int64_t getValue() {
			int64_t returnValue{};
			return returnValue;
		}

		template<> inline int32_t getValue() {
			int32_t returnValue{};
			return returnValue;
		}

		template<> inline int8_t getValue() {
			int8_t returnValue{};
			return returnValue;
		}

		template<> inline std::string getValue() {
			std::string returnValue{};
			return returnValue;
		}

		inline std::string_view getString() {
			return std::string_view{};
		}

		template<> inline std::vector<JsonParser> getValue() {
			std::vector<JsonParser> returnValue{};
			return std::vector<JsonParser>{};
		}

		inline JsonParser& operator[](const std::string& key) {
			/*
			for (size_t x = 0; x < this->currentStructuralCount; ++x) {
				std::cout << "CURRENT INDEX NEW: " << (x) << std::endl;
				std::cout << "CURRENT VALUE NEW: " << ( uint8_t )(this->tapePtrs[x] >> 56) << std::endl;
				std::cout << "CURRENT VALUE NEW: " << (this->tapePtrs[x] & JSON_VALUE_MASK) << std::endl;
				std::cout << "CURRENT COUNT NEWER: " << (((this->tapePtrs[x] & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK) << std::endl;
			}
			
			uint32_t string_length;
			size_t tape_idx = this->currenPositionInTape;
			uint64_t tape_val = this->tapePtrs[tape_idx];
			uint8_t type = uint8_t(tape_val >> 56);
			std::cout << tape_idx << " : " << type;
			tape_idx++;
			size_t how_many = 0;
			if (type == 'r') {
				how_many = size_t(tape_val & JSON_VALUE_MASK);
			}
			std::cout << "\t// pointing to " << how_many << " (right after last node)\n";
			for (; tape_idx < how_many; tape_idx++) {
				std::cout  << tape_idx << " : ";
				tape_val = tapePtrs[tape_idx];
				type = uint8_t(tape_val >> 56);
				switch (type) {
					case '"':
						std::cout << "string \"";
						std::memcpy(&string_length, stringBuffer + (tape_val & JSON_VALUE_MASK), sizeof(uint32_t));
						std::cout  << EscapeJsonString(std::string_view(
							reinterpret_cast<const uint8_t*>(stringBuffer + (tape_val & JSON_VALUE_MASK) + sizeof(uint32_t)), string_length));
						std::cout  << '"';
						std::cout  << '\n';
						break;
					case 'l':
						if (tape_idx + 1 >= how_many) {
							return *this;
						}
						std::cout  << "integer " << static_cast<int64_t>(tapePtrs[++tape_idx]) << "\n";
						break;
					case 'u':
						if (tape_idx + 1 >= how_many) {
							return *this;
						}
						std::cout  << "unsigned integer " << tapePtrs[++tape_idx] << "\n";
						break;
					case 'd':
						std::cout  << "float ";
						if (tape_idx + 1 >= how_many) {
							return *this;
						}
						double answer;
						std::memcpy(&answer, &tapePtrs[++tape_idx], sizeof(answer));
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
						std::cout  << "{\t// pointing to next tape location " << uint32_t((tape_val & JSON_VALUE_MASK)) << " (first node after the scope), "
						   << " saturated count " << (((tape_val & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK) << "\n";
						break;
					case '}':
						std::cout  << "}\t// pointing to previous tape location " << uint32_t((tape_val & JSON_VALUE_MASK)) << " (start of the scope)\n";
						break;
					case '[':
						std::cout  << "[\t// pointing to next tape location " << uint32_t((tape_val & JSON_VALUE_MASK)) << " (first node after the scope), "
						   << " saturated count " << (((tape_val & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK) << "\n";
						break;
					case ']':
						std::cout  << "]\t// pointing to previous tape location " << uint32_t((tape_val & JSON_VALUE_MASK)) << " (start of the scope)\n";
						break;
					case 'r': {
					}
					default: {
					}
				}
			}
			tape_val = tapePtrs[tape_idx];
			type = uint8_t(tape_val >> 56);
			std::cout << tape_idx << " : " << type << "\t// pointing to " << (tape_val & JSON_VALUE_MASK) << " (start root)\n";
			
			
			dumpRawTape(std::cout, this->tapePtrs, this->stringBuffer);
			
			auto newValue = *this->tapeIter.peek();
			
			if (newValue != 'r' && newValue !='"') {
				//throw JsonifierException{ "Sorry, but this is not the correct type for that: IT IS: " };
				this->type = JsonType::Object;
				return *this;
			} else if (newValue =='r') {
				this->type = JsonType::Object;
				return *this;
			}
			else if (newValue == '"') {
				return *this;
			}
			if (newValue == '[') {
				this->type = JsonType::Array;
				return *this;
			}
			if (newValue == '{') {
				this->type = JsonType::Object;
				return this->operator[](key);
			}
			if (newValue == '\"') {
				auto theString = this->getString();
				if (key == theString) {
					this->type = JsonType::String;
					return *this;

				} else {
					return *this;
				}
			}
			if (newValue == 's') {
				this->type = JsonType::Int64;
				return *this;
			}
			if (newValue == 'd') {
				this->type = JsonType::Float;
				return *this;
			}
			if (newValue == 'l') {
				this->type = JsonType::Uint64;
				return *this;
			}
			if (newValue == 't' || newValue == 'f') {
				this->type = JsonType::Bool;
				return *this;
			}
			if (newValue == 'n') {
				this->type = JsonType::Null;
				return *this;
			}*/
			return *this;
		};

		inline JsonParser(ErrorCode error) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " + std::to_string(( int32_t )error) };
		}

	  protected:
		//uint8_t* stringBuffer{ nullptr };
		//size_t currentStructuralCount{};
		TapeIterator tapeIter{};
		//JsonType type{};
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
		explicit operator bool() noexcept {
			for (size_t x = 0; x < 4; ++x) {
				if (this->getUint64(x) != 0) {
					return true;
				}
			}
			return false;
		}

		inline SimdBase256& operator=(uint8_t other) noexcept {
			this->value = _mm256_set1_epi8(other);
			return *this;
		}

		inline SimdBase256(uint8_t other) noexcept {
			*this = other;
		}

		inline SimdBase256& operator=(uint64_t* other) noexcept {
			*this = _mm256_set_epi64x(other[0], other[1], other[2], other[3]);
			return *this;
		}

		inline SimdBase256(uint64_t* other) noexcept {
			*this = other;
		}

		inline SimdBase256& operator=(const uint8_t* values) noexcept {
			*this = _mm256_load_si256(reinterpret_cast<const __m256i*>(values));
			return *this;
		}

		inline SimdBase256(const uint8_t* values) noexcept {
			*this = values;
		}

		inline SimdBase256(int64_t value00, int64_t value01, int64_t value02, int64_t value03) noexcept {
			this->value = _mm256_set_epi64x(value03, value02, value01, value00);
		}

		inline SimdBase256(uint64_t value00, uint64_t value01, uint64_t value02, uint64_t value03) noexcept {
			this->value = _mm256_set_epi64x(static_cast<int64_t>(value03), static_cast<int64_t>(value02), static_cast<int64_t>(value01),
				static_cast<int64_t>(value00));
		}

		inline void store(uint8_t dst[32]) const noexcept {
			return _mm256_storeu_epi8(dst, this->value);
		}

		inline uint64_t getUint64(size_t index) noexcept {
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

		inline int64_t getInt64(size_t index) noexcept {
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

		inline void insertInt64(int64_t value, size_t index) noexcept {
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

		inline SimdBase256() noexcept : value{ __m256i() } {
		}

		inline SimdBase256(const __m256i _value) noexcept : value(_value) {
		}

		inline operator const __m256i&() const noexcept {
			return this->value;
		}

		inline operator __m256i&() {
			return this->value;
		}

		inline SimdBase256 operator+(const SimdBase256 other) const noexcept {
			return _mm256_add_epi8(*this, other);
		}

		inline SimdBase256 operator-(const SimdBase256 other) const noexcept {
			return _mm256_sub_epi8(*this, other);
		}

		inline SimdBase256& operator+=(const SimdBase256 other) noexcept {
			*this = *this + other;
			  return *static_cast<SimdBase256*>(this);
		}

		inline SimdBase256& operator-=(const SimdBase256 other) noexcept {
			*this = *this - other;
			  return *static_cast<SimdBase256*>(this);
		}

		inline SimdBase256 operator|(const SimdBase256 other) const noexcept {
			return _mm256_or_si256(*this, other);
		}
		inline SimdBase256 operator&(const SimdBase256 other) const noexcept {
			return _mm256_and_si256(*this, other);
		}
		inline SimdBase256 operator^(const SimdBase256 other) const noexcept {
			return _mm256_xor_si256(*this, other);
		}

		inline SimdBase256& operator|=(const SimdBase256 other) noexcept {
			auto this_cast = static_cast<SimdBase256*>(this);
			*this_cast = *this_cast | other;
			return *this_cast;
		}

		inline SimdBase256& operator&=(const SimdBase256 other) noexcept {
			auto this_cast = static_cast<SimdBase256*>(this);
			*this_cast = *this_cast & other;
			return *this_cast;
		}

		inline SimdBase256& operator^=(const SimdBase256 other) noexcept {
			 auto this_cast = static_cast<SimdBase256*>(this);
			 *this_cast = *this_cast ^ other;
			 return *this_cast;
		}

		inline SimdBase256 operator==(SimdBase256 other) noexcept {
			return _mm256_cmpeq_epi8(this->value, other);
		}

		inline SimdBase256 operator==(uint8_t other) noexcept {
			return _mm256_cmpeq_epi8(this->value, _mm256_set1_epi8(other));
		}

		inline SimdBase256 operator<<(size_t amount) noexcept {
			uint64_t values[4]{};
			values[0] = this->getUint64(0);
			values[1] = this->getUint64(1);
			values[2] = this->getUint64(2);
			values[3] = this->getUint64(3);
			SimdBase256 newValues{ *this };
			newValues.insertInt64(values[0] << (amount % 64), 0);
			newValues.insertInt64(values[1] << (amount % 64) | ((values[0]) >> 64 - amount % 64), 1);
			newValues.insertInt64(values[2] << (amount % 64) | ((values[1]) >> 64 - amount % 64), 2);
			newValues.insertInt64(values[3] << (amount % 64) | ((values[2]) >> 64 - amount % 64), 3);
			return newValues;
		}

		inline SimdBase256 operator~() noexcept {
			SimdBase256 newValues{};
			newValues.insertInt64(~this->getInt64(0), 0);
			newValues.insertInt64(~this->getInt64(1), 1);
			newValues.insertInt64(~this->getInt64(2), 2);
			newValues.insertInt64(~this->getInt64(3), 3);
			return newValues;
		}

		inline SimdBase256 carrylessMultiplication(int64_t& prevInString) noexcept {
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

		inline bool collectCarries(SimdBase256 other1, SimdBase256* result) noexcept {
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

		inline void printBits(const std::string& valuesTitle) noexcept {
		using std::cout;
			cout << valuesTitle;
			for (size_t x = 0; x < 32; ++x) {
				for (size_t y = 0; y < 8; ++y) {
					cout << std::bitset<1>{ static_cast<size_t>(*(reinterpret_cast<int8_t*>(&this->value) + x)) >> y };
				}
			}
			cout << std::endl;
		}

		inline SimdBase256 bitAndNot(SimdBase256 other) noexcept {
			return _mm256_andnot_si256(other, this->value);
		}

		inline SimdBase256 shuffle(SimdBase256 other) noexcept {
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

		inline void packStringIntoValue(SimdBase256* theValue, const uint8_t string[32]) {
			*theValue = string;
		}

		inline uint64_t addTapeValues(uint32_t* tapePtrs, uint64_t* theBits, size_t currentIndexNew, size_t currentIndexIntoString,
			size_t& currentIndexIntoTape, size_t stringLength) {
			uint64_t value = static_cast<size_t>(__popcnt64(*theBits));
			for (int i = 0; i < value; i++) {
				auto valueNew = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + currentIndexIntoString;
				if (valueNew < stringLength) {
					tapePtrs[currentIndexIntoTape++] = valueNew;
					*theBits = _blsr_u64(*theBits);
				} else {
					return valueNew;
				}
			}
			return value;
		}

		inline SimdBase256 follows(SimdBase256& match, SimdBase256& overflow) {
			SimdBase256 result = match << 1 | overflow;
			overflow.insertInt64(match.getUint64(3) >> 63, 0);
			return result;
		}

		inline size_t getStructuralIndices(uint32_t* currentPtr, size_t currentIndex, size_t& currentIndexIntoTape, size_t stringLength) {
			size_t returnValue{};
			for (size_t x = 0; x < 4; ++x) {
				auto newValue = this->S256.getUint64(x);
				returnValue += this->addTapeValues(currentPtr, &newValue, x, currentIndex, currentIndexIntoTape, stringLength);
			}
			return returnValue;
		}

		inline void collectQuotedRange() {
			SimdBase256 E{ _mm256_set1_epi8(0b01010101) };
			SimdBase256 O{ _mm256_set1_epi8(0b10101010) };
			this->Op256 = this->B256.bitAndNot(this->B256 << 1);
			auto ES = E & this->Op256;
			SimdBase256 EC{};
			this->B256.collectCarries(ES, &EC);
			auto ECE = EC.bitAndNot(this->B256);
			auto OD1 = ECE.bitAndNot(E);
			auto OS = this->Op256 & O;
			auto OC = this->B256 + OS;
			auto OCE = OC.bitAndNot(this->B256);
			auto OD2 = OCE & E;
			auto OD = OD1 | OD2;
			this->Q256 = this->Q256.bitAndNot(OD);
			this->R256 = this->Q256.carrylessMultiplication(this->prevInString);
		}

		inline void collectFinalStructurals() {
			auto scalar = ~this->S256 | this->W256;
			auto stringTail = this->R256 ^ this->Q256;
			SimdBase256 nonquoteScalar = scalar.bitAndNot(this->Q256);
			this->followsPotentialNonQuoteScalar = follows(nonquoteScalar, this->prevScalar);
			this->S256 = this->S256 | (~this->S256 | this->W256).bitAndNot(this->followsPotentialNonQuoteScalar).bitAndNot(stringTail);
		}

		inline void collapseValues(const uint8_t* valueNew) {
			SimdBase256 values[8]{};
			this->packStringIntoValue(&values[0], valueNew);
			this->packStringIntoValue(&values[1], valueNew + 32);
			this->packStringIntoValue(&values[2], valueNew + 64);
			this->packStringIntoValue(&values[3], valueNew + 96);
			this->packStringIntoValue(&values[4], valueNew + 128);
			this->packStringIntoValue(&values[5], valueNew + 160);
			this->packStringIntoValue(&values[6], valueNew + 192);
			this->packStringIntoValue(&values[7], valueNew + 224);
			SimdBase256 quotes = _mm256_set1_epi8('"');
			SimdBase256 quotesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				quotesReal[x] = (values[x] == quotes);
			}

			this->Q256 = convertSimdBytesToBits(quotesReal);

			SimdBase256 whitespaceTable{ this->wsValues };
			SimdBase256 whiteSpaceReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				whiteSpaceReal[x] = values[x].shuffle(whitespaceTable) == values[x];
			}
			this->W256 = convertSimdBytesToBits(whiteSpaceReal);


			SimdBase256 backslashes = _mm256_set1_epi8('\\');
			SimdBase256 backslashesReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				backslashesReal[x] = values[x] == backslashes;
			}

			this->B256 = convertSimdBytesToBits(backslashesReal);

			uint8_t newValues[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
			SimdBase256 opTable{ newValues };
			SimdBase256 structural[8]{};
			for (size_t x = 0; x < 8; ++x) {
				auto valuesNew00 = values[x] | SimdBase256{ 0x20 };
				structural[x] = values[x].shuffle(opTable) == valuesNew00;
			}

			this->S256 = convertSimdBytesToBits(structural);
		}


		inline void submitDataForParsing(const uint8_t* valueNew) {
			this->collapseValues(valueNew);
			this->collectQuotedRange();
			this->collectFinalStructurals();
		}

	  protected:
		uint8_t wsValues[32]{ ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113, 2,
			100, '\t', '\n', 112, 100, '\r', 100, 100 };
		uint8_t opValues[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
		SimdBase256 followsPotentialNonQuoteScalar{};
		SimdBase256 prevScalar{};
		int64_t prevInString{};
		SimdBase256 Op256{};
		SimdBase256 Q256{};
		SimdBase256 W256{};
		SimdBase256 R256{};
		SimdBase256 S256{};
		SimdBase256 B256{};
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

		inline ErrorCode allocate(uint8_t* stringViewNew) noexcept {
			if (this->stringLengthRaw == 0) {
				this->tape.reset(nullptr);
				this->structuralIndexes.reset(nullptr);
				this->allocatedCapacity = 0;
				return ErrorCode::Success;
			}

			size_t tapeCapacity = round(this->stringLengthRaw + 3, 64);
			this->stringView = stringViewNew;
			size_t stringCapacity = round(5 * this->stringLengthRaw / 3 + 256, 64);
			this->stringBuffer.reset(new (std::nothrow) uint8_t[stringCapacity]);
			this->structuralIndexes.reset(new (std::nothrow) uint32_t[stringCapacity]);
			this->tapeLength = 0;
			this->tape.reset(new (std::nothrow) uint64_t[tapeCapacity]);
			this->isArray.reset(new (std::nothrow) bool[this->maxDepth]);

			if (!(this->getStringViewNew() && this->tape.get())) {
				this->allocatedCapacity = 0;
				this->tape.reset(nullptr);
				this->structuralIndexes.reset(nullptr);
				return ErrorCode::MemAlloc;
			}
			this->openContainers.reset(new (std::nothrow) OpenContainer[this->maxDepth]);
			this->allocatedCapacity = stringCapacity;
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
				int64_t stringSize = this->stringLengthRaw;
				this->tapeLength = 0;
				uint32_t collectedSize{};
				size_t tapeCurrentIndex{ 0 };
				while (stringSize > 0) {
					this->section.submitDataForParsing(this->getStringView() + collectedSize);
					auto indexCount =
						this->section.getStructuralIndices(this->structuralIndexes.get(), collectedSize, tapeCurrentIndex, this->stringLengthRaw);
					this->tapeLength += indexCount;
					stringSize -= 256;
					collectedSize += 256;
				}
				this->tapeLength -= 1;
			}
		}

		inline ~SimdJsonValue() noexcept {};

		inline uint8_t* getStringView() {
			return this->stringView;
		}

		inline uint8_t* getStringViewNew() {
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

		inline size_t getTapeLength() {
			return this->tapeLength;
		}

		inline bool* getIsArray() {
			return this->isArray.get();
		}

	  protected:
		std::unique_ptr<OpenContainer[]> openContainers{ nullptr };
		std::unique_ptr<uint32_t[]> structuralIndexes{ nullptr };
		std::unique_ptr<uint8_t[]> stringBuffer{ nullptr };
		std::unique_ptr<uint64_t[]> tape{ nullptr };
		std::unique_ptr<bool[]> isArray{ nullptr };
		const uint32_t maxDepth{ 512 };
		SimdStringSection section{};
		size_t allocatedCapacity{};
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
		uint32_t* nextStructural{ nullptr };
		const uint8_t* buffer{ nullptr };
		SimdJsonValue* masterParser;
		uint32_t depth{};

		inline ErrorCode walkDocument(TapeBuilder& visitor);

		inline JsonIterator(SimdJsonValue* masterParserNew);

		inline const uint8_t* peek() const noexcept;

		inline const uint8_t* advance() noexcept;

		inline size_t remainingLen() const noexcept;

		inline bool atEof() const noexcept;

		inline bool atBeginning() const noexcept;

		inline uint8_t lastStructural() const noexcept;

		inline ErrorCode visitRootPrimitive(TapeBuilder& visitor, const uint8_t* value);
		inline ErrorCode visitPrimitive(TapeBuilder& visitor, const uint8_t* value);
	};

	inline JsonIterator::JsonIterator(SimdJsonValue* masterParserNew)
		: nextStructural(masterParserNew->getStructuralIndexes()), buffer{ masterParserNew->getStringView() }, masterParser{ masterParserNew } {};

	inline const uint8_t* JsonIterator::peek() const noexcept {
		return &this->buffer[*this->nextStructural];
	}

	inline const uint8_t* JsonIterator::advance() noexcept {
		return &this->buffer[*this->nextStructural++];
	}

	inline size_t JsonIterator::remainingLen() const noexcept {
		return this->masterParser->getTapeLength() - *this->nextStructural;
	}

	inline bool JsonIterator::atEof() const noexcept {
		return this->nextStructural == &this->masterParser->getStructuralIndexes()[this->masterParser->getTapeLength() - 1];
	}

	inline bool JsonIterator::atBeginning() const noexcept {
		return this->nextStructural == this->masterParser->getStructuralIndexes();
	}

	inline uint8_t JsonIterator::lastStructural() const noexcept {
		return this->buffer[this->masterParser->getStructuralIndexes()[this->masterParser->getTapeLength() - 1]];
	}

	struct TapeWriter {
		uint64_t* nextTapeLocation{ nullptr };

		TapeWriter(uint64_t* ptr) {
			this->nextTapeLocation = ptr;
		}
		
		inline static void write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept;

		inline void append(uint64_t val, TapeType t) noexcept;

		inline void appendDouble(double value) noexcept;

		inline void appendU64(uint64_t value) noexcept;

		inline void appendS64(int64_t value) noexcept;

		inline void skipLargeInteger() noexcept;

		inline void skipDouble() noexcept;

		inline void skip() noexcept;

	  private:
		template<typename T> inline void append2(uint64_t val, T val2, TapeType t) noexcept;
	};

	inline void TapeWriter::appendS64(int64_t value) noexcept {
		append2(0, value, TapeType::Int64);
	}

	inline void TapeWriter::appendU64(uint64_t value) noexcept {
		append(0, TapeType::Uint64);
		*this->nextTapeLocation = value;
		this->nextTapeLocation++;
	}

	inline void TapeWriter::appendDouble(double value) noexcept {
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

	inline void TapeWriter::append(uint64_t val, TapeType t) noexcept {
		*this->nextTapeLocation = val | ((uint64_t(uint8_t(t))) << 56);
		this->nextTapeLocation++;
	}

	template<typename T> inline void TapeWriter::append2(uint64_t val, T val2, TapeType t) noexcept {
		append(val, t);
		static_assert(sizeof(val2) == sizeof(*this->nextTapeLocation), "Type is not 64 bits!");
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

		inline TapeBuilder(SimdJsonValue& doc) noexcept;

		inline uint32_t nextTapeIndex(JsonIterator& iter) const noexcept;
		inline ErrorCode startContainer(JsonIterator& iter) noexcept;
		inline ErrorCode endContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline ErrorCode emptyContainer(JsonIterator& iter, TapeType start, TapeType end) noexcept;
		inline uint8_t* onStartString(JsonIterator& iter) noexcept;
		inline ErrorCode onEndString(uint8_t* dst) noexcept;
	};

	inline ErrorCode TapeBuilder::parseDocument(SimdJsonValue& masterParser) {
		JsonIterator iter(&masterParser);
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

	inline TapeBuilder::TapeBuilder(SimdJsonValue& doc) noexcept : tape{ doc.getTape() }, currentStringBufferLocation{ doc.getStringViewNew() } {};

	inline ErrorCode TapeBuilder::visitString(JsonIterator& iter, const uint8_t* value) noexcept {
		uint8_t* dst01 = reinterpret_cast<uint8_t*>(onStartString(iter));
		dst01 = StringParser::parseString(value + 1ull, dst01, (*iter.nextStructural + 1ull) - (*iter.nextStructural));
		if (dst01 == nullptr) {
			return ErrorCode::StringError;
		}
		onEndString(reinterpret_cast<uint8_t*>(dst01));
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootString(JsonIterator& iter, const uint8_t* value) noexcept {
		return visitString(iter, value);
	}

	inline ErrorCode TapeBuilder::visitNumber(JsonIterator& iter, const uint8_t* value) noexcept {
		return NumberParser::parseNumber<TapeWriter>(reinterpret_cast<const uint8_t*>(value), this->tape);
	}

	inline ErrorCode TapeBuilder::visitRootNumber(JsonIterator& iter, const uint8_t* value) noexcept {
		std::unique_ptr<uint8_t[]> copy(new (std::nothrow) uint8_t[iter.remainingLen() + 64]);
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
		this->tape.append(currentStringBufferLocation - iter.masterParser->getStringViewNew(), TapeType::String);
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
		

	Scope_End:
		this->depth--;
		if (this->depth == 0) {
			goto Document_End;
		}
		if (masterParser->getIsArray()[this->depth]) {
			goto Array_Continue;
		}
		goto Object_Continue;

	Array_Begin:
		this->depth++;
		if (this->depth >= masterParser->getMaxDepth()) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::DepthError) } +
				", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
		this->masterParser->getIsArray()[this->depth] = true;
		
		visitor.visitArrayStart(*this);
		visitor.incrementCount(*this);

	Array_Value: {
		auto value = this->advance();
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
		

	Array_Continue: {
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
		
		

	Document_End:
		visitor.visitDocumentEnd(*this);

		auto nextStructuralIndex = uint32_t(this->nextStructural - &this->masterParser->getStructuralIndexes()[0]);

		if (nextStructuralIndex != this->masterParser->getTapeLength()) {
			throw JsonifierException{ "Sorry, but you've encountered the following error: " +
				std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
				", at the following index into the string: " + std::to_string(*this->nextStructural) };
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
		this->tapeLength = (this->getTape()[0] & JSON_VALUE_MASK);
		return JsonParser{ this->getTape(), this->tapeLength, reinterpret_cast<uint8_t*>(this->getStringViewNew()) };
	}

};
