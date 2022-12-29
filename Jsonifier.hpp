#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)

#include "NumberParsingUtils.hpp"
#include "StringParsingUtils.hpp"
#include <iterator>

namespace Jsonifier {

	struct JsonifierException : public std::runtime_error, std::string {
		inline JsonifierException(const std::string&, std::source_location = std::source_location::current()) noexcept;
	};

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
				case ErrorCode::Tape_Error: {
					return "Tape Error";
				}
				case ErrorCode::Depth_Error: {
					return "Depth Error";
				}
				case ErrorCode::Success: {
					return "Success";
				}
				case ErrorCode::Parse_Error: {
					return "Parse Error";
				}
				case ErrorCode::String_Error: {
					return "String Error";
				}
				case ErrorCode::T_Atom_Error: {
					return "TAtom Error";
				}
				case ErrorCode::F_Atom_Error: {
					return "FAtom Error";
				}
				case ErrorCode::N_Atom_Error: {
					return "NAtom Error";
				}
				case ErrorCode::Mem_Alloc_Error: {
					return "Mem Alloc";
				}
				case ErrorCode::Invalid_Number_Error: {
					return "Invalid Number";
				}
				case ErrorCode::Incorrect_Type: {
					return "Incorrect Type";
				}
				default: {
					return "Unknown Error";
				}
			}
		}

	  protected:
		ErrorCode code{};
	};

	template<typename OTy>
	class ObjectBuffer {
	  public:
		using AllocatorType = std::allocator<OTy>;

		ObjectBuffer& operator=(const ObjectBuffer&) = delete;
		ObjectBuffer(const ObjectBuffer&) = delete;

		ObjectBuffer& operator=(ObjectBuffer&& other) noexcept {
			this->objects = other.objects;
			other.objects = nullptr;
			return *this;
		}

		ObjectBuffer(ObjectBuffer&& other) noexcept {
			*this = std::move(other);
		}

		ObjectBuffer() noexcept = default;

		OTy& operator[](size_t index) noexcept {
			return this->objects[index];
		}

		void deallocate(size_t currentSize) {
			if (currentSize > 0 && this->objects) {
				AllocatorType allocator{};
				allocator.deallocate(this->objects, currentSize);
			}
		}

		void allocate(size_t newSize) noexcept {
			if (newSize != 0) {
				AllocatorType allocator{};
				this->objects = allocator.allocate(newSize);
			}
		}

		OTy* get() noexcept {
			return this->objects;
		}

	  protected:
		OTy* objects{};
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

	enum class JsonType : uint8_t { Document = 0, Object = 1, Array = 2, String = 3, Float = 4, Uint64 = 5, Int64 = 6, Bool = 7, Null = 8 };

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
		size_t currentObjectDepth{};
		size_t currentArrayDepth{};
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

	  protected:
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
		using std::cout;
		uint32_t string_length{};
		size_t tape_idx{ 0 };
		uint64_t tape_val{ tape[tape_idx] };
		uint8_t type{ uint8_t(tape_val >> 56) };
		cout << tape_idx << " : " << type;
		tape_idx++;
		size_t how_many{ 0 };
		if (type == 'r') {
			how_many = size_t(tape_val & JSON_VALUE_MASK);
		} else {
			return false;
		}
		cout  << "\t// pointing to " << how_many << " (right after last node)\n";
		for (; tape_idx < how_many; tape_idx++) {
			cout  << tape_idx << " : ";
			tape_val = tape[tape_idx];
			type = uint8_t(tape_val >> 56);
			switch (type) {
				case '"':
					cout  << "string \"";
					std::memcpy(&string_length, stringBuffer + (tape_val & JSON_VALUE_MASK), sizeof(uint32_t));
					cout << EscapeJsonString(std::string_view(
						reinterpret_cast<const char*>(stringBuffer + (tape_val & JSON_VALUE_MASK) + sizeof(uint32_t)), string_length));
					cout << '"';
					cout << '\n';
					break;
				case 'l':
					if (tape_idx + 1 >= how_many) {
						return false;
					}
					cout  << "integer " << static_cast<int64_t>(tape[++tape_idx]) << "\n";
					break;
				case 'u':
					if (tape_idx + 1 >= how_many) {
						return false;
					}
					cout  << "unsigned integer " << tape[++tape_idx] << "\n";
					break;
				case 'd':
					cout  << "float ";
					if (tape_idx + 1 >= how_many) {
						return false;
					}
					double answer;
					std::memcpy(&answer, &tape[++tape_idx], sizeof(answer));
					cout  << answer << '\n';
					break;
				case 'n':
					cout  << "null\n";
					break;
				case 't':
					cout  << "true\n";
					break;
				case 'f':
					cout  << "false\n";
					break;
				case '{':
					cout  << "{\t// pointing to next tape location " << uint32_t(( tape_val & JSON_VALUE_MASK)) << " (first node after the scope), "
					   << " saturated count " << ((( tape_val & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK) << "\n";
					break;
				case '}':
					cout  << "}\t// pointing to previous tape location " << uint32_t(( tape_val & JSON_VALUE_MASK)) << " (start of the scope)\n";
					break;
				case '[':
					cout  << "[\t// pointing to next tape location " << uint32_t(( tape_val & JSON_VALUE_MASK)) << " (first node after the scope), "
						<< " saturated count " << ((( tape_val & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK) << "\n";
					break;
				case ']':
					cout  << "]\t// pointing to previous tape location " << uint32_t(( tape_val & JSON_VALUE_MASK)) << " (start of the scope)\n";
					break;
				case 'r':
					return false;
				default:
					return false;
			}
		}
		tape_val = tape[tape_idx];
		type = uint8_t(tape_val >> 56);
		cout  << tape_idx << " : " << type << "\t// pointing to " << (tape_val & JSON_VALUE_MASK) << " (start root)\n";
		return true;
	}

	class Document;
	class Object;
	class Array;
	class Object;
	class Field;

	template<typename OTy> class JsonifierResult : public std::pair<ErrorCode, OTy> { 
	  public:
		JsonifierResult(ErrorCode&&error,OTy&&value){
			this->second = std::move(value);
			this->first = error;
		}

		OTy getValueSafe() {
			if (this->first != ErrorCode::Success) {
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(this->first) } };
			}
			return std::move(this->second);
		}

		ErrorCode getError() {
			return this->first;
		}

		OTy getValueUnsafe() {
			return std::move(this->second);
		}

	  protected:
	};

	class JsonValueBase {
	  public:

		template<typename OTy> inline JsonifierResult<OTy> getValue();

		template<> inline JsonifierResult<Document> getValue<Document>();

		template<> inline JsonifierResult<Object> getValue<Object>();

		template<> inline JsonifierResult<Array> getValue<Array>();
		
		template<> inline JsonifierResult<double> getValue<double>() {
			ErrorCode error{ ErrorCode::Success };
			if (this->peek() == 'd') {
				double returnValue{};
				this->advance();
				std::memcpy(&returnValue, &this->localTapeRootPosition[this->currentIndex], sizeof(returnValue));
				this->advance();
				return JsonifierResult{ std::move(error), std::move(returnValue) };
			}
			error = ErrorCode::Incorrect_Type;
			return JsonifierResult{ std::move(error), double{} };
		}

		template<> inline JsonifierResult<float> getValue<float>() {
			ErrorCode error{ ErrorCode::Success };
			if (this->peek() == 'd') {
				float returnValue{};
				this->advance();
				std::memcpy(&returnValue, &this->localTapeRootPosition[this->currentIndex], sizeof(returnValue));
				this->advance();
				return JsonifierResult{ std::move(error), std::move(returnValue) };
				
			}
			error = ErrorCode::Incorrect_Type;
			return JsonifierResult{ std::move(error), float{} };
		}

		/*
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
			return std::forward<std::string>(static_cast<std::string>(this->parseJsonString()));
		}

		template<> inline std::string_view getValue() {
			return this->parseJsonString();
		}*/

		inline JsonValueBase& operator=(const JsonValueBase& other) noexcept {
			this->localTapeRootPosition = other.localTapeRootPosition + other.currentIndex;
			this->tapeRootPosition = other.tapeRootPosition;
			this->stringBuffer = other.stringBuffer;
			return *this;
		}

		inline JsonValueBase(const JsonValueBase& other) noexcept {
			*this = other;
		}

		inline JsonValueBase& operator=(JsonValueBase&& other) noexcept {
			this->localTapeRootPosition = other.localTapeRootPosition + other.currentIndex;
			this->tapeRootPosition = other.tapeRootPosition;
			this->stringBuffer = other.stringBuffer;
			return *this;
		}

		inline JsonValueBase collectNextIterator() {
			switch (this->peek()) {
				case 'r': {
					return JsonValueBase{ std::move(*this) };
				}
				case '{': {
					int32_t index{};
					while (this->peek(index) != '}') {
						index++;
						std::this_thread::sleep_for(std::chrono::nanoseconds{ 1 });
					}
					return JsonValueBase{ this->stringBuffer, this->tapeRootPosition, this->localTapeRootPosition + this->currentIndex + index };
				}
				case '[': {
					int32_t index{};
					while (this->peek(index) != ']') {
						index++;
						std::this_thread::sleep_for(std::chrono::nanoseconds{ 1 });
					}
					return JsonValueBase{ this->stringBuffer, this->tapeRootPosition, this->localTapeRootPosition + this->currentIndex + index };
				}
				case 'l' : {
					[[fallthrough]];
				}
				case 'u': {
					[[fallthrough]];
				}
				case 'd': {
					return JsonValueBase{ std::move(*this) };
				}
				case '"': {
					[[fallthrough]];
				}
				case 't': {
					[[fallthrough]];
				}
				case 'f': {
					[[fallthrough]];
				}
				case 'n': {
					this->advance();
					return JsonValueBase{ std::move(*this) };
				}
				default: {
					return JsonValueBase{ std::move(*this) };
				}
			}
		}

		inline JsonValueBase(JsonValueBase&& other) noexcept {
			*this = std::move(other);
		}

		inline Field operator[](const char* keyNew);
		
		inline JsonValueBase(const uint8_t* stringBufferNew, uint64_t* tapeRootPositionNew, uint64_t* currentTapePositionNew) {
			this->localTapeRootPosition = currentTapePositionNew;
			this->tapeRootPosition = tapeRootPositionNew;
			this->stringBuffer = stringBufferNew;
		}

		inline Field findField(const char* keyNew);

		inline const char* parseJsonString() {
			assert(this->peek() == '"');
			std::string_view returnValue{};
			if (this->peek() == '"') {
				size_t stringLength{};
				std::memcpy(&stringLength,
					this->stringBuffer + (uint32_t(*(this->localTapeRootPosition + this->currentIndex ) & JSON_VALUE_MASK)), sizeof(uint32_t));
				returnValue =
					std::string_view{ reinterpret_cast<const char*>(this->stringBuffer +
										  (uint32_t(*(this->localTapeRootPosition + this->currentIndex) & JSON_VALUE_MASK)) + sizeof(uint32_t)),
						stringLength };
				this->advance();
			}
			return returnValue.data();
		}
		 
		inline uint64_t parseJsonUint() {
			assert(this->peek(0) == 'u');
			this->advance();
			uint64_t returnValue{};
			std::memcpy(&returnValue, &this->localTapeRootPosition[this->currentIndex], sizeof(returnValue));
			this->advance();
			return returnValue;
		}

		inline int64_t parseJsonInt() {
			assert(this->peek(0) == 'l');
			this->advance();
			int64_t returnValue{};
			std::memcpy(&returnValue, &this->localTapeRootPosition[this->currentIndex], sizeof(returnValue));
			this->advance();
			return returnValue;
		}

		inline bool parseJsonBool() {
			assert(this->peek() == 'f' || this->peek() == 't');
			if (this->peek() == 'f') {
				this->advance();
				return false;
			} else {
				this->advance();
				return true;
			}
		}

		inline nullptr_t parseJsonNull() {
			assert(this->peek() == 'n');
			this->advance();
			return nullptr_t{};
		}

		inline uint64_t* advance(uint32_t value = 1) noexcept {
			auto returnValue = &this->localTapeRootPosition[this->currentIndex];
			++this->currentIndex;
			return returnValue;
		}

		inline size_t getOffset() {
			return this->currentIndex + 1;
		}

		JsonValueBase* getCurrentIterator() {
			return this;
		}

		inline void asserAtFieldStart(size_t amountToOffset) {
			assert(this->peek(amountToOffset) == '"');
		}

		inline void assertAtObjectStart(size_t amountToOffset = 0) {
			assert(this->peek(amountToOffset) == '{');
		}

		inline void assertAtArrayStart(size_t amountToOffset = 0) {
			assert(this->peek(amountToOffset) == '[');
		}

		inline void assertAtStringStart(size_t amountToOffset = 0) {
			assert(this->peek(amountToOffset) == '"');
		}

		inline const uint8_t peek(uint32_t index = 0) noexcept {
			return (*(this->localTapeRootPosition + this->currentIndex + index)) >> 56;
		}

		inline size_t getCurrentCount() {
			return uint32_t((*this->tapeRootPosition & JSON_VALUE_MASK)) - (this->localTapeRootPosition - this->tapeRootPosition) - 2;
		}

		inline uint8_t getRootKey() {
			return (*this->localTapeRootPosition >> 56);
		}

		inline size_t size() {
			switch (this->getRootKey()) {
				case 'r': {
					[[fallthrough]];
				}
				case '[': {
					[[fallthrough]];
				}
				case '{': {
					return (((*(this->localTapeRootPosition) & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK);
				}
				case '"': {
					size_t stringLength{};
					std::memcpy(&stringLength, this->stringBuffer + ((*this->localTapeRootPosition) & JSON_VALUE_MASK), sizeof(uint32_t));
					return stringLength;
				} 
				default: {
					return 1;
				}
			}
		}

		inline JsonType type() {
			switch (this->getRootKey()) {
				case 'r': {
					return JsonType::Document;
				}
				case '{': {
					return JsonType::Object;
				}
				case '[': {
					return JsonType::Array;
				}
				case 'l': {
					return JsonType::Int64;
				}
				case 'u': {
					return JsonType::Uint64;
				}
				case 'd': {
					return JsonType::Float;
				}
				case '"': {
					return JsonType::String;
				}
				case 't': {
					[[fallthrough]];
				}
				case 'f': {
					return JsonType::Bool;
				}
				case 'n': {
					return JsonType::Null;
				}
				default: {
					return JsonType::Null;
				}
			}
		}
		
	  protected:
		uint64_t* localTapeRootPosition{};
		const uint8_t* stringBuffer{};
		uint64_t* tapeRootPosition{};
		size_t currentIndex{};
	};

	class Field : protected std::pair<std::string_view, JsonValueBase> {
	  public:
		class FieldIterator {
		  public:
			using IteratorCategory = std::forward_iterator_tag;
			using DifferenceType = std::ptrdiff_t;
			using Reference = JsonValueBase&;
			using ValueType = JsonValueBase;
			using Pointer = JsonValueBase*;

			inline FieldIterator(Pointer ptr) noexcept : ptr(ptr) {
			}

			inline Reference operator*() noexcept {
				return *ptr;
			}

			inline Pointer operator->() noexcept {
				return ptr;
			}

			inline FieldIterator& operator++() noexcept {
				return *this;
			}

			friend inline bool operator==(const FieldIterator& lhs, const FieldIterator& rhs) noexcept {
				return lhs.ptr->getOffset() >= lhs.ptr->getCurrentCount();
			};

		  protected:
			Pointer ptr{};
		};

		inline Field() noexcept : std::pair<std::string_view, JsonValueBase>{ std::string_view{}, JsonValueBase{ nullptr, nullptr, nullptr } } {};

		inline auto begin() noexcept {
			return FieldIterator{ &this->second };
		}

		inline auto end() noexcept {
			return FieldIterator{ &this->second };
		}

		inline std::string_view getKey() {
			return this->first;
		}

		template<typename OTy> inline JsonifierResult<OTy> get() noexcept;

		template<> inline JsonifierResult<Array> get<Array>() noexcept;

		template<> inline JsonifierResult<Object> get<Object>() noexcept;

		template<> inline JsonifierResult<const char*> get<const char*>() noexcept;

		template<> inline JsonifierResult<std::string_view> get<std::string_view>() noexcept;

		template<> inline JsonifierResult<int64_t> get<int64_t>() noexcept;

		template<> inline JsonifierResult<uint64_t> get<uint64_t>() noexcept;

		template<> inline JsonifierResult<double> get<double>() noexcept;

		template<> inline JsonifierResult<bool> get<bool>() noexcept;

		inline Field(std::string_view&& key, JsonValueBase* value) 
			:  std::pair<std::string_view, JsonValueBase>{ std::move(key), *value} {
			if (this->second.peek() != '"') {
				throw JsonifierException{ "Sorry, but this item's type is not field." };
			}
			this->second.advance();
		};
	};

	class Array : public JsonValueBase {
	  public:
		class ArrayIterator {
		  public:
			using IteratorCategory = std::forward_iterator_tag;
			using DifferenceType = std::ptrdiff_t;
			using Reference = JsonValueBase&;
			using ValueType = JsonValueBase;
			using Pointer = JsonValueBase*;

			inline ArrayIterator(Pointer ptr) noexcept : ptr(ptr) {
			}

			inline Reference operator*() noexcept {
				return *ptr;
			}

			inline Pointer operator->() noexcept {
				return ptr;
			}

			inline ArrayIterator& operator++() noexcept {
				return *this;
			}

			friend inline bool operator==(ArrayIterator& lhs, const ArrayIterator& rhs) noexcept {
				return lhs.ptr->getOffset() >= lhs.ptr->getCurrentCount();
			};

		  protected:
			Pointer ptr{};
		};

		inline auto begin() noexcept {
			return ArrayIterator{ this };
		}

		inline auto end() noexcept {
			return ArrayIterator{ this };
		}

		inline Array() noexcept : JsonValueBase{ nullptr, nullptr, nullptr } {};

		inline Array(JsonValueBase* other) : JsonValueBase{ *other } {
			if (this->peek() != '[') {
				throw JsonifierException{ "Sorry, but this item's type is not array." };
			}
			this->advance();
		};

		template<typename OTy> inline JsonifierResult<OTy> get() noexcept;

		template<> inline JsonifierResult<Array> get<Array>() noexcept;

		template<> inline JsonifierResult<Object> get<Object>() noexcept;

		template<> inline JsonifierResult<const char*> get<const char*>() noexcept;

		template<> inline JsonifierResult<std::string_view> get<std::string_view>() noexcept;

		template<> inline JsonifierResult<int64_t> get<int64_t>() noexcept;

		template<> inline JsonifierResult<uint64_t> get<uint64_t>() noexcept;

		template<> inline JsonifierResult<double> get<double>() noexcept;

		template<> inline JsonifierResult<bool> get<bool>() noexcept;
	};
	
	class Object : public JsonValueBase {
	  public:
		class ObjectIterator {
		  public:
			using IteratorCategory = std::forward_iterator_tag;
			using DifferenceType = std::ptrdiff_t;
			using Reference = JsonValueBase&;
			using ValueType = JsonValueBase;
			using Pointer = JsonValueBase*;

			inline ObjectIterator(Pointer ptr) noexcept : ptr(ptr) {
			}

			inline Reference operator*() noexcept {
				return *ptr;
			}

			inline Pointer operator->() noexcept {
				return ptr;
			}

			inline ObjectIterator& operator++() noexcept {
				return *this;
			}

			friend inline bool operator==(const ObjectIterator& lhs, const ObjectIterator& rhs) noexcept {
				return lhs.ptr->getOffset() >= lhs.ptr->getCurrentCount();
			};

		  protected:
			Pointer ptr{};
		};

		inline auto begin() noexcept {
			return ObjectIterator{ this };
		}

		inline auto end() noexcept {
			return ObjectIterator{ this };
		}

		inline Object() noexcept : JsonValueBase{ nullptr, nullptr, nullptr } {};

		inline Object(JsonValueBase* other) : JsonValueBase{ *other } {};

		template<typename OTy> inline JsonifierResult<OTy> get() noexcept;

		//template<> inline Array get<Array>() noexcept;

		//template<> inline Object get<Object>() noexcept;

		//template<> inline const char* get<const char*>() noexcept;

		//template<> inline std::string_view get<std::string_view>() noexcept;

		//template<> inline int64_t get<int64_t>() noexcept;

		//template<> inline uint64_t get<uint64_t>() noexcept;

		template<> inline JsonifierResult<double> get<double>() noexcept;

		//template<> inline bool get<bool>() noexcept;
	};

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
		inline SimdBase256() noexcept {};

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

		inline SimdBase256 operator==(SimdBase256 other) {
			return _mm256_cmpeq_epi8(this->Value, other);
		}

		inline SimdBase256 operator==(uint8_t other) {
			return _mm256_cmpeq_epi8(this->Value, _mm256_set1_epi8(other));
		}

		template<size_t amount> inline SimdBase256 shl() {
			SimdBase256 returnValue{};
			auto newValue01 = SimdBase256{ _mm256_slli_epi64(*this, amount) };
			auto newValue02 = SimdBase256{ _mm256_srli_epi64(*this, 64 - amount) };
			returnValue = newValue01 | newValue02;
			return returnValue;
		}

		template<size_t amount> inline SimdBase256 shr() {
			SimdBase256 returnValue{};
			auto newValue01 = SimdBase256{ _mm256_srli_epi64(*this, amount) };
			auto newValue02 = SimdBase256{ _mm256_slli_epi64(*this, amount) };
			returnValue = newValue01 | newValue02;
			return returnValue;
		}

		inline SimdBase256 operator~() {
			SimdBase256 newValues{};
			newValues = *this ^ _mm256_set1_epi64x(-1);
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

	  protected:
		const uint8_t* stringBuffer{};
		const size_t len{};
		const size_t lenminusstep{};
		size_t idx{};
	};

	template<size_t StepSize>
	inline StringBlockReader<StepSize>::StringBlockReader(const uint8_t* _buf, size_t _len)
		: stringBuffer{ _buf }, len{ _len }, lenminusstep{ this->len < StepSize ? 0 : this->len - StepSize }, idx{ 0 } {
	}

	template<size_t StepSize> inline size_t StringBlockReader<StepSize>::blockIndex() {
		return this->idx;
	}

	template<size_t StepSize> inline bool StringBlockReader<StepSize>::hasFullBlock() const {
		return this->idx < this->lenminusstep;
	}

	template<size_t StepSize> inline const uint8_t* StringBlockReader<StepSize>::fullBlock() const {
		return &this->stringBuffer[this->idx];
	}

	template<size_t StepSize> inline size_t StringBlockReader<StepSize>::getRemainder(uint8_t* dst) const {
		if (this->len == this->idx) {
			return 0;
		}
		std::memset(dst, 0x20, StepSize);
		std::memcpy(dst, this->stringBuffer + this->idx, this->len - this->idx);
		return this->len - this->idx;
	}

	template<size_t StepSize> inline void StringBlockReader<StepSize>::advance() {
		this->idx += StepSize;
	}

	class StructuralIterator;

	class SimdStringSection {
	  public:
		inline SimdStringSection() noexcept = default;

		inline void packStringIntoValue(SimdBase256* theValue, const uint8_t string[32]) {
			*theValue = string;
		}

		inline uint64_t addTapeValues(StructuralIterator* iterator, uint64_t* tapePtrs, uint64_t* theBits, size_t currentIndexNew,
			size_t& currentIndexIntoTape, size_t stringLength, uint8_t* stringBuffer);

		inline SimdBase256 follows(SimdBase256 match, SimdBase256& overflow) {
			SimdBase256 result = match.shl<1>() | overflow;
			overflow = match.shr<63>();
			return result;
		}

		inline size_t getStructuralIndices(StructuralIterator*iterator,uint64_t* currentPtr, size_t& currentIndexIntoTape, size_t stringLength, uint8_t*stringBuffer) {
			size_t returnValue{};
			for (size_t x = 0; x < 4; ++x) {
				auto newValue = this->S256.getUint64(x);
				returnValue += this->addTapeValues(iterator, currentPtr, &newValue, x, currentIndexIntoTape, stringLength, stringBuffer);
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
			B256 &= ~this->prevEscaped;
			SimdBase256 followsEscape = B256.shl<1>() | this->prevEscaped;
			SimdBase256 E{ _mm256_set1_epi8(0b01010101) };
			SimdBase256 oddSequenceStarts = B256.bitAndNot(E).bitAndNot(followsEscape);
			SimdBase256 O{ _mm256_set1_epi8(0b10101010) };
			this->S256 = B256.bitAndNot(B256.shl<1>());
			SimdBase256 EC{};
			this->prevEscaped = B256.collectCarries(oddSequenceStarts, &EC);
			auto ECE = EC.bitAndNot(B256);
			auto OD1 = ECE.bitAndNot(E).bitAndNot(followsEscape);
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
			SimdBase256 nonQuoteScalar = scalar.bitAndNot(this->Q256);
			this->followsPotentialNonquoteScalar = follows(nonQuoteScalar, this->prevInScalar);
			return this->S256 | (~this->S256 | this->W256).bitAndNot(this->followsPotentialNonquoteScalar).bitAndNot(stringTail);
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
		}

	  protected:
		SimdBase256 followsPotentialNonquoteScalar{};
		size_t currentIndexIntoString{};
		SimdBase256 prevInScalar{};
		SimdBase256 prevEscaped{};
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

	class SimdJsonValue;

	class Document : public JsonValueBase {
	  public:
		inline Document(SimdJsonValue* value) noexcept;
	};

	inline int64_t totalTimePassed{};
	inline int64_t iterationCount{};

	class SimdJsonValue {
	  public:

		inline Document getDocument() {
			Document returnValue{ this };
			returnValue.advance();
			return returnValue;
		}

		inline SimdJsonValue& operator=(SimdJsonValue&&) = delete;
		inline SimdJsonValue(SimdJsonValue&&) = delete;

		inline SimdJsonValue& operator=(const SimdJsonValue&) = delete;
		inline SimdJsonValue(const SimdJsonValue&) = delete;
		inline SimdJsonValue(){};

		inline int64_t round(int64_t a, int64_t n) {
			return (((a) + (( n )-1)) & ~(( n )-1));
		}

		inline ErrorCode allocate(uint8_t* stringViewNew) noexcept {
			this->stringBuffer.deallocate(this->stringCapacity);
			this->openContainers.deallocate(this->maxDepth);
			this->isArray.deallocate(this->tapeCapacity);
			this->tape.deallocate(this->tapeCapacity);
			if (this->stringLengthRaw == 0) {
				return ErrorCode::Success;
			}

			this->stringCapacity = round(5 * this->stringLengthRaw / 3 + 256, 256);
			this->tapeCapacity = round(this->stringLengthRaw + 3, 256);
			this->stringBuffer.allocate(this->stringCapacity);
			this->openContainers.allocate(this->maxDepth);
			this->isArray.allocate(this->tapeCapacity);
			this->tape.allocate(this->tapeCapacity);
			this->stringView = stringViewNew;
			this->nStructuralIndexes = 0;
			if (!(this->tape.get() && this->stringBuffer.get() && this->isArray.get() &&
					this->openContainers.get())) {
				this->stringBuffer.deallocate(this->stringCapacity);
				this->openContainers.deallocate(this->maxDepth);
				this->isArray.deallocate(this->tapeCapacity);
				this->tape.deallocate(this->tapeCapacity);
				return ErrorCode::Mem_Alloc_Error;
			}
			
			return ErrorCode::Success;
		}
		
		inline void generateJsonEvents(uint8_t* stringNew, size_t stringLength);

		inline uint8_t* getStringView() {
			return this->stringView;
		}

		inline uint8_t* getStringBuffer() {
			return this->stringBuffer.get();
		}

		inline OpenContainer* getOpenContainers() {
			return this->openContainers.get();
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

		inline bool* getIsArray() {
			return this->isArray.get();
		}

		~SimdJsonValue() {
			this->stringBuffer.deallocate(this->stringCapacity);
			this->openContainers.deallocate(this->maxDepth);
			this->isArray.deallocate(this->tapeCapacity);
			this->tape.deallocate(this->tapeCapacity);
		}

	  protected:
		ObjectBuffer<OpenContainer> openContainers{};
		ObjectBuffer<uint8_t> stringBuffer{};
		ObjectBuffer<uint64_t> tape{};
		ObjectBuffer<bool> isArray{};
		size_t nStructuralIndexes{};
		SimdStringSection section{};
		uint32_t maxDepth{ 512 };
		size_t stringLengthRaw{};
		size_t stringCapacity{};
		size_t tapeCapacity{};
		uint8_t* stringView{};
		size_t tapeLength{};
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

	enum class IndexingState {
		Start_Document = 0,
		Object_Start = 1,
		Object_Field = 2,
		Object_Key = 3,
		Object_Continue_01 = 4,
		Object_Continue_02 = 5,
		Array_Start = 6,
		Array_Value = 7,
		Array_Continue_01 = 8,
		Array_Continue_02 = 9,
		Scope_End = 10,
		Document_End = 11,
		Error = 12
	};
	
	class StructuralIterator {
	  public:
		std::unique_ptr<TapeBuilder> tapeBuilder;
		SimdJsonValue* masterParser{};
		const uint8_t* stringView{};
		uint64_t* nextStructural{};
		IndexingState state{};
		uint32_t depth{};

		inline StructuralIterator(SimdJsonValue* masterParserNew);
		
		inline ErrorCode visitRootPrimitive(TapeBuilder& visitor, const uint8_t* Value);

		inline ErrorCode visitPrimitive(TapeBuilder& visitor, const uint8_t* Value);

		inline IndexingState walkDocument(const uint8_t value);

		inline const uint8_t* advance() noexcept;

		inline const uint8_t* peek() noexcept;

		inline bool atBeginning() noexcept;
	};

	inline const uint8_t* StructuralIterator::peek() noexcept {
		return &this->stringView[*this->nextStructural];
	}

	inline const uint8_t* StructuralIterator::advance() noexcept {
		return &this->stringView[*this->nextStructural++];
	}

	inline bool StructuralIterator::atBeginning() noexcept {
		return this->nextStructural == this->masterParser->getTape();
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

	  protected:
		template<typename OTy> inline void append2(uint64_t val, OTy val2, TapeType t) noexcept;
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

	template<typename OTy> inline void TapeWriter::append2(uint64_t val, OTy val2, TapeType t) noexcept {
		append(std::move(val), std::move(t));
		static_assert(sizeof(val2) == sizeof(*this->nextTapeLocation), "Type is not 64 *theBits!");
		memcpy(this->nextTapeLocation, &val2, sizeof(val2));
		this->nextTapeLocation++;
	}

	inline void TapeWriter::write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept {
		tape_loc = val | ((uint64_t(uint8_t(t))) << 56);
	}

	struct TapeBuilder {
		inline static ErrorCode parseDocument(SimdJsonValue* masterParser);

		inline ErrorCode visitDocumentStart(StructuralIterator& iter) noexcept;

		inline ErrorCode visitDocumentEnd(StructuralIterator& iter) noexcept;

		inline ErrorCode visitArrayStart(StructuralIterator& iter) noexcept;

		inline ErrorCode visitArrayEnd(StructuralIterator& iter) noexcept;

		inline ErrorCode visitEmptyArray(StructuralIterator& iter) noexcept;

		inline ErrorCode visitObjectStart(StructuralIterator& iter) noexcept;

		inline ErrorCode visitKey(StructuralIterator& iter, const uint8_t* key) noexcept;

		inline ErrorCode visitObjectEnd(StructuralIterator& iter) noexcept;

		inline ErrorCode visitEmptyObject(StructuralIterator& iter) noexcept;

		inline ErrorCode visitPrimitive(StructuralIterator& iter, const uint8_t* Value);

		inline ErrorCode visitRootPrimitive(StructuralIterator& iter, const uint8_t* Value);

		inline ErrorCode visitString(StructuralIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitNumber(StructuralIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitTrueAtom(StructuralIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitFalseAtom(StructuralIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitNullAtom(StructuralIterator& iter, const uint8_t* Value) noexcept;

		inline ErrorCode visitRootString(StructuralIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitRootNumber(StructuralIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitRootTrueAtom(StructuralIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitRootFalseAtom(StructuralIterator& iter, const uint8_t* Value) noexcept;
		inline ErrorCode visitRootNullAtom(StructuralIterator& iter, const uint8_t* Value) noexcept;

		inline ErrorCode incrementCount(StructuralIterator& iter) noexcept;

		TapeWriter tape;

		inline TapeBuilder(SimdJsonValue* doc) noexcept;

	  protected:
		uint8_t* currentStringBufferLocation{};
		size_t& currentTapeLength;		

		inline uint32_t nextTapeIndex(StructuralIterator& iter) noexcept;
		inline ErrorCode startContainer(StructuralIterator& iter) noexcept;
		inline ErrorCode endContainer(StructuralIterator& iter, TapeType start, TapeType end) noexcept;
		inline ErrorCode emptyContainer(StructuralIterator& iter, TapeType start, TapeType end) noexcept;
		inline uint8_t* onStartString(StructuralIterator& iter) noexcept;
		inline ErrorCode onEndString(uint8_t* dst) noexcept;
	};

	inline ErrorCode TapeBuilder::parseDocument(SimdJsonValue* masterParser) {
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootPrimitive(StructuralIterator& iter, const uint8_t* Value) {
		return iter.visitRootPrimitive(*this, Value);
	}

	inline ErrorCode TapeBuilder::visitPrimitive(StructuralIterator& iter, const uint8_t* Value) {
		return iter.visitPrimitive(*this, Value);
	}

	inline ErrorCode TapeBuilder::visitEmptyObject(StructuralIterator& iter) noexcept {
		return emptyContainer(iter, TapeType::Start_Object, TapeType::End_Object);
	}

	inline ErrorCode TapeBuilder::visitEmptyArray(StructuralIterator& iter) noexcept {
		return emptyContainer(iter, TapeType::Start_Array, TapeType::End_Array);
	}

	inline ErrorCode TapeBuilder::visitDocumentStart(StructuralIterator& iter) noexcept {
		startContainer(iter);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitObjectStart(StructuralIterator& iter) noexcept {
		startContainer(iter);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitArrayStart(StructuralIterator& iter) noexcept {
		startContainer(iter);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitObjectEnd(StructuralIterator& iter) noexcept {
		return endContainer(iter, TapeType::Start_Object, TapeType::End_Object);
	}

	inline ErrorCode TapeBuilder::visitArrayEnd(StructuralIterator& iter) noexcept {
		return endContainer(iter, TapeType::Start_Array, TapeType::End_Array);
	}

	inline ErrorCode TapeBuilder::visitDocumentEnd(StructuralIterator& iter) noexcept {
		constexpr uint32_t startTapeIndex = 0;
		this->tape.append(startTapeIndex, TapeType::Root);
		TapeWriter::write(iter.masterParser->getTape()[startTapeIndex], nextTapeIndex(iter), TapeType::Root);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitKey(StructuralIterator& iter, const uint8_t* key) noexcept {
		return visitString(iter, key);
	}

	inline ErrorCode TapeBuilder::incrementCount(StructuralIterator& iter) noexcept {
		iter.masterParser->getOpenContainers()[iter.depth].count++;
		return ErrorCode::Success;
	}

	inline TapeBuilder::TapeBuilder(SimdJsonValue* doc) noexcept
		: tape{ doc->getTape() }, currentStringBufferLocation{ doc->getStringBuffer() }, currentTapeLength(doc->getTapeLength()){};

	inline ErrorCode TapeBuilder::visitString(StructuralIterator& iter, const uint8_t* Value) noexcept {
		uint8_t* dst01 = onStartString(iter);
		dst01 = StringParser::parseString(reinterpret_cast<const uint8_t*>(Value) + 1ull, reinterpret_cast<uint8_t*>(dst01),
			(*iter.nextStructural + 1ull) - (*iter.nextStructural));
		if (dst01 == nullptr) {
			return ErrorCode::String_Error;
		}
		onEndString(dst01);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootString(StructuralIterator& iter, const uint8_t* Value) noexcept {
		return visitString(iter, Value);
	}

	inline ErrorCode TapeBuilder::visitNumber(StructuralIterator& iter, const uint8_t* Value) noexcept {
		return NumberParser::parseNumber<TapeWriter>(Value, this->tape);
	}

	inline ErrorCode TapeBuilder::visitRootNumber(StructuralIterator& iter, const uint8_t* Value) noexcept {
		std::unique_ptr<uint8_t[]> copy(new (std::nothrow) uint8_t[256]);
		if (copy.get() == nullptr) {
			return ErrorCode::Mem_Alloc_Error;
		}
		return visitNumber(iter, copy.get());
	}

	inline ErrorCode TapeBuilder::visitTrueAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidTrueAtom(Value)) {
			return ErrorCode::T_Atom_Error;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootTrueAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidTrueAtom(Value)) {
			return ErrorCode::N_Atom_Error;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitFalseAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidFalseAtom(Value)) {
			return ErrorCode::F_Atom_Error;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootFalseAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidFalseAtom(Value)) {
			return ErrorCode::F_Atom_Error;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitNullAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidNullAtom(Value)) {
			return ErrorCode::N_Atom_Error;
		}
		this->tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootNullAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidNullAtom(Value)) {
			return ErrorCode::N_Atom_Error;
		}
		this->tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline uint32_t TapeBuilder::nextTapeIndex(StructuralIterator& iter) noexcept {
		return uint32_t(tape.nextTapeLocation - iter.masterParser->getTape());
	}

	inline ErrorCode TapeBuilder::emptyContainer(StructuralIterator& iter, TapeType start, TapeType end) noexcept {
		auto startIndex = nextTapeIndex(iter);
		this->tape.append(startIndex + 2ull, start);
		this->tape.append(startIndex, end);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::startContainer(StructuralIterator& iter) noexcept {
		iter.masterParser->getOpenContainers()[iter.depth].tapeIndex = nextTapeIndex(iter);
		iter.masterParser->getOpenContainers()[iter.depth].count = 0;
		this->tape.skip();
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::endContainer(StructuralIterator& iter, TapeType start, TapeType end) noexcept {
		const uint32_t startTapeIndex = iter.masterParser->getOpenContainers()[iter.depth].tapeIndex;
		this->tape.append(startTapeIndex, end);
		const uint32_t count = iter.masterParser->getOpenContainers()[iter.depth].count;
		const uint32_t cntsat = count > 0xFFFFFF ? 0xFFFFFF : count;
		TapeWriter::write(iter.masterParser->getTape()[startTapeIndex], nextTapeIndex(iter) | (uint64_t(cntsat) << 32), start);
		return ErrorCode::Success;
	}

	inline uint8_t* TapeBuilder::onStartString(StructuralIterator& iter) noexcept {
		this->tape.append(currentStringBufferLocation - iter.masterParser->getStringBuffer(), TapeType::String);
		return this->currentStringBufferLocation + sizeof(uint32_t);
	}

	inline ErrorCode TapeBuilder::onEndString(uint8_t* dst) noexcept {
		uint32_t strLength = uint32_t(dst - (this->currentStringBufferLocation + sizeof(uint32_t)));
		memcpy(this->currentStringBufferLocation, &strLength, sizeof(uint32_t));
		*dst = 0;
		this->currentStringBufferLocation = dst + 1;
		return ErrorCode::Success;
	};

	inline IndexingState StructuralIterator::walkDocument(const uint8_t value) {
		this->masterParser->getTapeLength() = 0;
		std::cout << "CURRENT KEY: " << value << std::endl;
		std::cout << "CURRENT STATE: " << ( int32_t )this->state << std::endl;
		switch (this->state) {
			case IndexingState::Start_Document: {
				this->tapeBuilder->visitDocumentStart(*this);
				{
					switch (value) {
						case '{':
							return IndexingState::Object_Start;
						case '[':
							return IndexingState::Array_Start;
						default:
							this->tapeBuilder->visitRootPrimitive(*this, &value);
							return IndexingState::Object_Start;
					}
				}
				return IndexingState::Document_End;
			}
			case IndexingState::Object_Start: {
				this->depth++;
				if (value == '}') {
					this->tapeBuilder->visitEmptyObject(*this);
					return IndexingState::Scope_End;
				}
				if (this->depth >= masterParser->getMaxDepth()) {
					return IndexingState::Error;
				}
				this->masterParser->getIsArray()[this->depth] = false;
				this->tapeBuilder->visitObjectStart(*this);

				if (value != '"') {
					throw JsonifierException{ "Sorry, but you've encountered the following error: " +
						std::string{ static_cast<EnumStringConverter>(ErrorCode::Tape_Error) } +
						", at the following index into the string: " + std::to_string(*this->nextStructural) };
				}
				this->tapeBuilder->visitKey(*this, &value);
				this->tapeBuilder->incrementCount(*this);
				return IndexingState::Object_Field;
			}
			case IndexingState::Object_Key: {
				if (value!= '"') {
					throw JsonifierException{ "Sorry, but you've encountered the following error: " +
						std::string{ static_cast<EnumStringConverter>(ErrorCode::Tape_Error) } +
						", at the following index into the string: " + std::to_string(*this->nextStructural) +
						", and the key was: " + std::string{ *reinterpret_cast<const char*>(this->peek() - 1) } };
				}
				this->tapeBuilder->visitKey(*this, &value);
				return IndexingState::Object_Continue_02;
			}
			case IndexingState::Object_Field: {
				if (value != ':') {
					throw JsonifierException{ "Sorry, but you've encountered the following error: " +
						std::string{ static_cast<EnumStringConverter>(ErrorCode::Tape_Error) } +
						", at the following index into the string: " + std::to_string(*this->nextStructural) };
				}
				return IndexingState::Object_Continue_01;
			}
			case IndexingState::Object_Continue_01: {
				switch (value) {
					case '{': {
						return IndexingState::Object_Start;
					}
					case '[': {
						return IndexingState::Array_Start;
					}
					default: {
						if (auto resultCode = this->tapeBuilder->visitPrimitive(*this, &value); resultCode != ErrorCode::Success) {
							throw JsonifierException{ "Sorry, but you've encountered the following error: " +
								std::string{ static_cast<EnumStringConverter>(resultCode) } +
								", at the following index into the string: " + std::to_string(*this->nextStructural) };
						}
						break;
					}
				}
				return IndexingState::Object_Continue_02;
			}
		
			case IndexingState::Object_Continue_02: {
				if (value == '}') {
					this->tapeBuilder->visitEmptyObject(*this);
					return IndexingState::Scope_End;
				}
				if (value == ']') {
					this->tapeBuilder->visitEmptyArray(*this);
					return IndexingState::Scope_End;
				}
				switch (value) {
					case ',':
						this->tapeBuilder->incrementCount(*this);
						return IndexingState::Object_Key;
					case '}':
						this->tapeBuilder->visitObjectEnd(*this);
						return IndexingState::Scope_End;
					default:
						throw JsonifierException{ "Sorry, but you've encountered the following error: " +
							std::string{ static_cast<EnumStringConverter>(ErrorCode::Tape_Error) } +
							", at the following index into the string: " + std::to_string(*this->nextStructural) };
				}
				return IndexingState::Scope_End;
			}
			case IndexingState::Scope_End: {
				this->depth--;
				if (this->depth == 0) {
					return IndexingState::Document_End;
				}
				if (this->masterParser->getIsArray()[this->depth]) {
					return IndexingState::Array_Continue_01;
				}
				return IndexingState::Object_Continue_01;
			}
			case IndexingState::Array_Start: {
				this->depth++;
				if (value == ']') {
					this->tapeBuilder->visitEmptyArray(*this);
					return IndexingState::Scope_End;
				}
				if (this->depth >= masterParser->getMaxDepth()) {
					throw JsonifierException{ "Sorry, but you've encountered the following error: " +
						std::string{ static_cast<EnumStringConverter>(ErrorCode::Depth_Error) } +
						", at the following index into the string: " + std::to_string(*this->nextStructural) };
				}
				this->masterParser->getIsArray()[this->depth] = true;
				this->tapeBuilder->visitArrayStart(*this);
				this->tapeBuilder->incrementCount(*this);
				return IndexingState::Array_Value;
			}
			case IndexingState::Array_Value: {
				switch (value) {
					case '{':
						return IndexingState::Object_Start;
					case '[':
						return IndexingState::Array_Start;
					default:
						return IndexingState::Array_Continue_01;
				} 
			}
			case IndexingState::Array_Continue_01: {
				if (value == ']') {
					this->tapeBuilder->visitEmptyArray(*this);
					return IndexingState::Scope_End;
				}
				if (auto resultCode = this->tapeBuilder->visitPrimitive(*this, &value); resultCode != ErrorCode::Success) {
					throw JsonifierException{ "Sorry, but you've encountered the following error: " +
						std::string{ static_cast<EnumStringConverter>(resultCode) } +
						", at the following index into the string: " + std::to_string(*this->nextStructural) };
				}
				return IndexingState::Array_Continue_02;
			}
			case IndexingState::Array_Continue_02: {
				switch (value) {
					case ',':
						this->tapeBuilder->incrementCount(*this);
						return IndexingState::Array_Value;
					case ']':
						this->tapeBuilder->visitArrayEnd(*this);
						return IndexingState::Scope_End;
					default:
						return IndexingState::Error;
				}
				return IndexingState::Document_End;
			}
			case IndexingState::Document_End: {
				this->tapeBuilder->visitDocumentEnd(*this);

				auto nextStructuralIndex = uint32_t(this->nextStructural - &this->masterParser->getTape()[0]);

				return IndexingState::Document_End;
			}
		} 
	}

	inline ErrorCode StructuralIterator::visitRootPrimitive(TapeBuilder& visitor, const uint8_t* Value) {
		switch (*Value) {
			case '"':
				return this->tapeBuilder->visitRootString(*this, Value);
			case 't':
				return this->tapeBuilder->visitRootTrueAtom(*this, Value);
			case 'f':
				return this->tapeBuilder->visitRootFalseAtom(*this, Value);
			case 'n':
				return this->tapeBuilder->visitRootNullAtom(*this, Value);
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
				return this->tapeBuilder->visitRootNumber(*this, Value);
			default:
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::Tape_Error) } +
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
	}

	inline ErrorCode StructuralIterator::visitPrimitive(TapeBuilder& visitor, const uint8_t* Value) {
		switch (*Value) {
			case '"':
				return this->tapeBuilder->visitString(*this, Value);
			case 't':
				return this->tapeBuilder->visitTrueAtom(*this, Value);
			case 'f':
				return this->tapeBuilder->visitFalseAtom(*this, Value);
			case 'n':
				return this->tapeBuilder->visitNullAtom(*this, Value);
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
				return this->tapeBuilder->visitNumber(*this, Value);
			default:
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::Tape_Error) } +
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
	}
	inline int64_t iterationCount02{};
	inline int64_t totalTimePassed02{};
	
	Document SimdJsonValue::getJsonData(std::string& string) {
		this->generateJsonEvents(reinterpret_cast<uint8_t*>(string.data()), string.size());
		StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
		totalTimePassed += stopWatch.totalTimePassed().count();
		std::cout << "TIME FOR STAGE2: " << totalTimePassed / iterationCount << std::endl;
		this->getTapeLength() = (this->getTape()[0] & JSON_VALUE_MASK);
		dumpRawTape(std::cout, this->getTape(), this->getStringBuffer());
		return std::forward<Document>(this->getDocument());
	}

	inline Field JsonValueBase::operator[](const char* keyNew) {
		return this->findField(keyNew);
	}
	/*

	template<> inline Array Object::get<Array>() noexcept {
		return this->getArray();
	}

	template<> inline Object Object::get<Object>() noexcept {
		return this->getObject();
	}

	template<> inline const char* Object::get<const char*>() noexcept {
		return this->parseJsonString();
	}

	template<> inline std::string_view Object::get<std::string_view>() noexcept {
		return this->parseJsonString();
	}

	template<> inline int64_t Object::get<int64_t>() noexcept {
		return this->parseJsonInt();
	}

	template<> inline uint64_t Object::get<uint64_t>() noexcept {
		return this->parseJsonUint();
	}
	*/
	template<> inline JsonifierResult<double> Object::get<double>() noexcept {
		return this->getValue<double>();
	}
	/*
	template<> inline bool Object::get<bool>() noexcept {
		return this->parseJsonBool();
	}

	template<> inline Array Array::get<Array>() noexcept {
		return this->getArray();
	}

	template<> inline Object Array::get<Object>() noexcept {
		return this->getObject();
	}

	template<> inline const char* Array::get<const char*>() noexcept {
		return this->parseJsonString();
	}

	template<> inline std::string_view Array::get<std::string_view>() noexcept {
		return this->parseJsonString();
	}

	template<> inline int64_t Array::get<int64_t>() noexcept {
		return this->parseJsonInt();
	}

	template<> inline uint64_t Array::get<uint64_t>() noexcept {
		return this->parseJsonUint();
	}
	*/
	template<> inline JsonifierResult<double> Array::get<double>() noexcept {
		return this->getValue<double>();
	}
	/*
	template<> inline bool Array::get<bool>() noexcept {
		return this->parseJsonBool();
	}*/
	template<> inline JsonifierResult<double> Field::get<double>() noexcept {
		return this->second.getValue<double>();
	}

	template<> inline JsonifierResult<Array> JsonValueBase::getValue<Array>() {
		ErrorCode error{ ErrorCode::Success };
		//std::cout<< "CURRENT KEY: " << this->peek() << std::endl;
		assert(this->peek() == '[');
		if (this->peek() != '[') {
			error = ErrorCode::Incorrect_Type;
			return JsonifierResult{ std::move(error), Array{} };
		}
		return JsonifierResult{ std::move(error), Array{ this } };
	}

	template<> inline JsonifierResult<Object> JsonValueBase::getValue<Object>() {
		ErrorCode error{ ErrorCode::Success };
		//std::cout<< "CURRENT KEY: " << this->peek() << std::endl;
		assert(this->peek() == '{');
		if (this->peek() != '{') {
			error = ErrorCode::Incorrect_Type;
			return JsonifierResult{ std::move(error), Object{} };
		}
		return JsonifierResult{ std::move(error), Object{ this } };
	}

	template<> inline JsonifierResult<Array> Field::get<Array>() noexcept {
		return this->second.getValue<Array>();
	}
	inline Document::Document(SimdJsonValue* value) noexcept : JsonValueBase{ value->getStringBuffer(), value->getTape(), value->getTape() } {
		//std::cout<< "CURRENT KEY: " << this->peek() << std::endl;
	};

	inline Field JsonValueBase::findField(const char* keyNew) {
		int32_t index{};
		std::string_view newString{};
		if (this->peek() == '"') {
			newString = this->parseJsonString();
		}
		while (this->peek(index) != '"' && newString!= keyNew) {
			if (this->peek(index) == '"') {
				newString = this->parseJsonString();
			}
			++index;
		}
		this->advance(index);
		this->asserAtFieldStart(0);
		return Field{ std::move(newString), this->getCurrentIterator() };
	}

	inline void SimdJsonValue::generateJsonEvents(uint8_t* stringNew, size_t stringLength) {
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
			StringBlockReader<256> stringReader{ this->stringView, this->stringLengthRaw };
			//StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
			this->nStructuralIndexes = 0;
			StructuralIterator iterator{ this };
			size_t tapeCurrentIndex{ 0 };
			while (stringReader.hasFullBlock()) {
				this->section.submitDataForProcessing(stringReader.fullBlock());
				auto indexCount = section.getStructuralIndices(&iterator, this->tape.get(), tapeCurrentIndex, this->stringLengthRaw, this->stringView);
				this->nStructuralIndexes += indexCount;
				stringReader.advance();
			}
			uint8_t block[256];
			stringReader.getRemainder(block);
			this->section.submitDataForProcessing(block);
			auto indexCount = section.getStructuralIndices(&iterator, this->tape.get(), tapeCurrentIndex, this->stringLengthRaw, this->stringView);
			this->nStructuralIndexes += indexCount;//totalTimePassed += stopWatch.totalTimePassed().count();
			////std::cout<< "TIME FOR STAGE1: " << totalTimePassed / iterationCount << std::endl;
		}
		--this->nStructuralIndexes;
	}

	inline StructuralIterator::StructuralIterator(SimdJsonValue* masterParserNew)
		: nextStructural(masterParserNew->getTape()), stringView{ masterParserNew->getStringView() }, masterParser{ masterParserNew }, tapeBuilder{
			  std::make_unique<TapeBuilder>(masterParserNew)
		  } {};

	inline uint64_t SimdStringSection::addTapeValues(StructuralIterator* iterator, uint64_t* tapePtrs, uint64_t* theBits, size_t currentIndexNew,
		size_t& currentIndexIntoTape, size_t stringLength, uint8_t*stringBuffer) {
		int cnt = static_cast<int>(__popcnt64(*theBits));
		int64_t newValue{};
		for (int i = 0; i < cnt; i++) {
			newValue = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + this->currentIndexIntoString;
			auto errorCode= iterator->walkDocument(stringBuffer[newValue]);
			if (errorCode == IndexingState::Error) {
				throw JsonifierException{ "Sorry, but there was a tape error, at string index: " + std::to_string(currentIndexIntoString) };
			}
			iterator->state = errorCode;

			if (newValue >= stringLength) {
				currentIndexIntoTape += cnt;
				return cnt;

			} else {
				*theBits = _blsr_u64(*theBits);
			}
		}
		currentIndexIntoTape += cnt;
		return cnt;
	}

};