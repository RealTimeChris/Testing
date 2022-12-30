#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)

#include "NumberParsingUtils.hpp"
#include "StringParsingUtils.hpp"
#include <iterator>

namespace Jsonifier {

	class EnumStringConverter {
	  public:
		inline EnumStringConverter(ErrorCode error) {
			this->code = error;
		}
		inline operator std::string() {
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

	struct JsonifierException : public std::runtime_error, std::string {
		JsonifierException(const std::string&, std::source_location = std::source_location::current()) noexcept;
	};

	template<typename OTy> class ObjectBuffer {
	  public:
		template<typename OTy> class ObjectAllocator {
		  public:
			using value_type = OTy;
			using pointer = OTy*;
			using const_pointer = const OTy*;
			using reference = OTy&;
			using const_reference = const OTy&;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using propagate_on_container_move_assignment = std::true_type;
			using is_always_equal = std::true_type;

			inline ObjectAllocator() noexcept = default;

			inline OTy* allocate(size_t count) {
				return static_cast<OTy*>(malloc(sizeof(OTy) * count));
			}

			template<typename OTy> inline void deallocate(OTy* ptr, size_t count) {
				free(ptr);
			}
		};

		using AllocatorTraits = std::allocator_traits<ObjectAllocator<OTy>>;

		inline ObjectBuffer& operator=(ObjectBuffer&&) = delete;
		inline ObjectBuffer(ObjectBuffer&&) = delete;

		inline ObjectBuffer& operator=(const ObjectBuffer&) = delete;
		inline ObjectBuffer(const ObjectBuffer&) = delete;

		inline ObjectBuffer() noexcept = default;

		inline OTy& operator[](size_t index) noexcept {
			return this->objects[index];
		}

		inline void deallocate(size_t currentSize) {
			if (currentSize > 0 && this->objects) {
				ObjectAllocator<OTy> allocator{};
				AllocatorTraits::deallocate(allocator, this->objects, currentSize);
				this->objects = nullptr;
			}
		}

		inline void allocate(size_t newSize, size_t oldSize) noexcept {
			if (this->objects) {
				this->deallocate(oldSize);
			}
			if (newSize != 0) {
				ObjectAllocator<OTy> allocator{};
				this->objects = AllocatorTraits::allocate(allocator, newSize);
			}
		}

		inline OTy* get() noexcept {
			return this->objects;
		}

	  protected:
		OTy* objects{};
	};

	constexpr int64_t JSON_VALUE_MASK{ 0x00FFFFFFFFFFFFFF };
	constexpr uint32_t JSON_COUNT_MASK{ 0xFFFFFF };

	template<typename RTy> inline void reverseByteOrder(RTy& net) {
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

	template<typename RTy> inline void storeBits(char* to, RTy num) {
		uint8_t byteSize{ 8 };
		reverseByteOrder<RTy>(num);
		for (uint32_t x = 0; x < sizeof(RTy); ++x) {
			to[x] = static_cast<uint8_t>(num >> (byteSize * x));
		}
	}

	template<typename TTy> class StopWatch {
	  public:
		using HRClock = std::chrono::high_resolution_clock;

		inline StopWatch() = delete;

		inline StopWatch<TTy>& operator=(const StopWatch<TTy>& data) {
			this->maxNumberOfMs.store(data.maxNumberOfMs.load());
			this->startTime.store(data.startTime.load());
			return *this;
		}

		inline StopWatch(const StopWatch<TTy>& data) {
			*this = data;
		}

		inline StopWatch(TTy maxNumberOfMsNew) {
			this->maxNumberOfMs.store(maxNumberOfMsNew);
			this->startTime.store(std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch()));
		}

		inline TTy totalTimePassed() {
			TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
			TTy elapsedTime = currentTime - this->startTime.load();
			return elapsedTime;
		}

		inline TTy getTotalWaitTime() {
			return this->maxNumberOfMs.load();
		}

		inline bool hasTimePassed() {
			TTy currentTime = std::chrono::duration_cast<TTy>(HRClock::now().time_since_epoch());
			TTy elapsedTime = currentTime - this->startTime.load();
			if (elapsedTime >= this->maxNumberOfMs.load()) {
				return true;
			} else {
				return false;
			}
		}

		inline void resetTimer() {
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
		template<IsEnum EnumType> inline EnumConverter& operator=(const std::vector<EnumType>& data) {
			for (auto& Value: data) {
				this->vector.emplace_back(std::move(static_cast<uint64_t>(Value)));
			}
			return *this;
		};

		template<IsEnum EnumType> inline EnumConverter(const std::vector<EnumType>& data) {
			*this = data;
		};

		template<IsEnum EnumType> inline EnumConverter& operator=(EnumType data) {
			this->integer = static_cast<uint64_t>(data);
			return *this;
		};

		template<IsEnum EnumType> inline EnumConverter(EnumType data) {
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
			inline JsonValue() noexcept = default;
			inline JsonValue& operator=(JsonValue&&) noexcept = delete;
			inline JsonValue(JsonValue&&) noexcept = delete;
			inline JsonValue& operator=(const JsonValue&) noexcept = delete;
			inline JsonValue(const JsonValue&) noexcept = delete;
			ObjectType* object;
			StringType* string;
			ArrayType* array;
			FloatType numberDouble;
			UintType numberUint;
			IntType numberInt;
			BoolType boolean;
		};
		
		Jsonifier() noexcept = default;

		template<IsConvertibleToJsonifier OTy> inline Jsonifier& operator=(std::vector<OTy>&& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& Value: data) {
				this->jsonValue.array->push_back(std::move(Value));
			}
			return *this;
		}

		template<IsConvertibleToJsonifier OTy> inline Jsonifier(std::vector<OTy>&& data) noexcept {
			*this = std::move(data);
		}

		template<IsConvertibleToJsonifier OTy> inline Jsonifier& operator=(std::vector<OTy>& data) noexcept {
			this->setValue(JsonType::Array);
			for (auto& Value: data) {
				this->jsonValue.array->push_back(Value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier OTy> inline Jsonifier(std::vector<OTy>& data) noexcept {
			*this = data;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy>
		inline Jsonifier& operator=(std::unordered_map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::object);
			for (auto& [key, Value]: data) {
				(*this->jsonValue.object)[key] = std::move(Value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> inline Jsonifier(std::unordered_map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy>
		inline Jsonifier& operator=(std::unordered_map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::object);
			for (auto& [key, Value]: data) {
				(*this->jsonValue.object)[key] = Value;
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> inline Jsonifier(std::unordered_map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> inline Jsonifier& operator=(std::map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::object);
			for (auto& [key, Value]: data) {
				(*this->jsonValue.object)[key] = std::move(Value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> inline Jsonifier(std::map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> inline Jsonifier& operator=(std::map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::object);
			for (auto& [key, Value]: data) {
				(*this->jsonValue.object)[key] = Value;
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> inline Jsonifier(std::map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsEnum Ty> inline Jsonifier& operator=(Ty data) noexcept {
			this->jsonValue.numberUint = static_cast<uint64_t>(data);
			this->type = JsonType::Uint64;
			return *this;
		}

		template<IsEnum Ty> inline Jsonifier(Ty data) noexcept {
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

		size_t size() noexcept;

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
		inline EscapeJsonString(std::string_view _str) noexcept : str{ _str } {
		}
		inline operator std::string() noexcept {
			std::stringstream s;
			s << *this;
			return s.str();
		}

	  protected:
		std::string_view str;
		inline friend std::ostream& operator<<(std::ostream& out, const EscapeJsonString& unescaped);
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

	inline bool dumpRawTape(std::ostream& os, uint64_t* tape, const uint8_t* stringBuffer) noexcept {
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
		cout << "\t// pointing to " << how_many << " (right after last node)\n";
		for (; tape_idx < how_many; tape_idx++) {
			cout << tape_idx << " : ";
			tape_val = tape[tape_idx];
			type = uint8_t(tape_val >> 56);
			switch (type) {
				case '"':
					cout << "string \"";
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
					cout << "integer " << static_cast<int64_t>(tape[++tape_idx]) << "\n";
					break;
				case 'u':
					if (tape_idx + 1 >= how_many) {
						return false;
					}
					cout << "unsigned integer " << tape[++tape_idx] << "\n";
					break;
				case 'd':
					cout << "float ";
					if (tape_idx + 1 >= how_many) {
						return false;
					}
					double answer;
					std::memcpy(&answer, &tape[++tape_idx], sizeof(answer));
					cout << answer << '\n';
					break;
				case 'n':
					cout << "null\n";
					break;
				case 't':
					cout << "true\n";
					break;
				case 'f':
					cout << "false\n";
					break;
				case '{':
					cout << "{\t// pointing to next tape location " << uint32_t((tape_val & JSON_VALUE_MASK)) << " (first node after the scope), "
						 << " saturated count " << (((tape_val & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK) << "\n";
					break;
				case '}':
					cout << "}\t// pointing to previous tape location " << uint32_t((tape_val & JSON_VALUE_MASK)) << " (start of the scope)\n";
					break;
				case '[':
					cout << "[\t// pointing to next tape location " << uint32_t((tape_val & JSON_VALUE_MASK)) << " (first node after the scope), "
						 << " saturated count " << (((tape_val & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK) << "\n";
					break;
				case ']':
					cout << "]\t// pointing to previous tape location " << uint32_t((tape_val & JSON_VALUE_MASK)) << " (start of the scope)\n";
					break;
				case 'r':
					return false;
				default:
					return false;
			}
		}
		tape_val = tape[tape_idx];
		type = uint8_t(tape_val >> 56);
		cout << tape_idx << " : " << type << "\t// pointing to " << (tape_val & JSON_VALUE_MASK) << " (start root)\n";
		return true;
	}

	class Document;
	class Object;
	class Array;
	class Object;
	class Field;

	template<typename OTy> class JsonifierResult : protected std::pair<OTy, ErrorCode> {
	  public:
		template<typename OTy>
		inline JsonifierResult(OTy&& other, ErrorCode&& error) noexcept : std::pair<OTy, ErrorCode>{ std::move(other), std::move(error) } {};

		template<typename OTy> inline ErrorCode get(OTy& value) noexcept {
			value = std::move(this->first);
			return this->second;
		}

		inline OTy getValue() {
			if (this->second != ErrorCode::Success) {
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(this->second) } };
			}
			return std::move(this->first);
		}
	};

	class JsonValueBase {
	  public:
		template<typename OTy> inline JsonifierResult<OTy> get() noexcept;

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
				case 'l': {
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

		inline JsonValueBase(uint8_t* stringBufferNew, uint64_t* tapeRootPositionNew, uint64_t* currentTapePositionNew) {
			this->localTapeRootPosition = currentTapePositionNew;
			this->tapeRootPosition = tapeRootPositionNew;
			this->stringBuffer = stringBufferNew;
		}

		inline Field operator[](const char* keyNew);

		inline Field findField(const char* keyNew) noexcept;

		inline Object parseJsonObject() noexcept;

		inline Field parseJsonField(const char* fieldKey) noexcept;

		inline Array parseJsonArray() noexcept;

		inline std::string_view parseJsonString() noexcept {
			assert(this->peek() == '"');
			std::string_view returnValue{};
			if (this->peek() == '"') {
				size_t stringLength{};
				std::memcpy(&stringLength, this->stringBuffer + (uint32_t(*(this->localTapeRootPosition + this->currentIndex) & JSON_VALUE_MASK)),
					sizeof(uint32_t));
				returnValue =
					std::string_view{ reinterpret_cast<const char*>(this->stringBuffer +
										  (uint32_t(*(this->localTapeRootPosition + this->currentIndex) & JSON_VALUE_MASK)) + sizeof(uint32_t)),
						stringLength };
				this->advance();
			} else {
				this->error = ErrorCode::ParseError;
			}
			return returnValue;
		}

		inline double parseJsonFloat() noexcept {
			assert(this->peek(0) == 'd');
			double returnValue{};
			if (this->peek() == 'd') {
				this->advance();
				std::memcpy(&returnValue, &this->localTapeRootPosition[this->currentIndex], sizeof(returnValue));
				this->advance();
			} else {
				this->error = ErrorCode::ParseError;
			}
			return returnValue;
		}

		inline uint64_t parseJsonUint() noexcept {
			assert(this->peek(0) == 'u');
			uint64_t returnValue{};
			if (this->peek() == 'u') {
				this->advance();
				std::memcpy(&returnValue, &this->localTapeRootPosition[this->currentIndex], sizeof(returnValue));
				this->advance();
			} else {
				this->error = ErrorCode::ParseError;
			}
		}

		inline int64_t parseJsonInt() noexcept {
			assert(this->peek(0) == 'l');
			int64_t returnValue{};
			if (this->peek() == 'l') {
				this->advance();
				std::memcpy(&returnValue, &this->localTapeRootPosition[this->currentIndex], sizeof(returnValue));
				this->advance();
			} else {
				this->error = ErrorCode::ParseError;
			}
			return returnValue;
		}

		inline bool parseJsonBool() noexcept {
			assert(this->peek() == 'f' || this->peek() == 't');
			if (this->peek() == 'f') {
				this->advance();
				return false;
			} else if (this->peek() == 't') {
				this->advance();
				return true;
			} else {
				this->error = ErrorCode::ParseError;
				return false;
			}
		}

		inline nullptr_t parseJsonNull() noexcept {
			assert(this->peek() == 'n');
			if (this->peek() == 'n') {
				this->advance();
				return nullptr_t{};
			} else {
				this->error = ErrorCode::ParseError;
				return nullptr_t{};
			}
		}

		inline uint64_t* advance(uint32_t value = 1) noexcept {
			auto returnValue = &this->localTapeRootPosition[this->currentIndex];
			++this->currentIndex;
			return returnValue;
		}

		inline void setPosition(int32_t delta = 0) noexcept {
			auto newValue = static_cast<int32_t>(this->currentIndex);
			newValue += delta;
			this->currentIndex += static_cast<uint32_t>(newValue);
		}

		inline size_t getOffset() noexcept {
			return this->currentIndex + 1;
		}

		inline JsonValueBase& getCurrentIterator() noexcept {
			return *this;
		}

		inline JsonValueBase& advanceIteratorAndReturn(size_t value) noexcept {
			this->currentIndex += value;
			return *this;
		}

		inline void asserAtFieldStart(size_t amountToOffset) noexcept {
			assert(this->peek(amountToOffset) == '"');
		}

		inline void assertAtObjectStart(size_t amountToOffset = 0) noexcept {
			assert(this->peek(amountToOffset) == '{');
		}

		inline void assertAtArrayStart(size_t amountToOffset = 0) noexcept {
			assert(this->peek(amountToOffset) == '[');
		}

		inline void assertAtStringStart(size_t amountToOffset = 0) noexcept {
			assert(this->peek(amountToOffset) == '"');
		}

		inline const uint8_t peek(uint32_t index = 0) noexcept {
			return (*(this->localTapeRootPosition + this->currentIndex + index)) >> 56;
		}

		inline size_t getCurrentCount() noexcept {
			return uint32_t((*this->tapeRootPosition & JSON_VALUE_MASK)) - (this->localTapeRootPosition - this->tapeRootPosition) - 2;
		}

		inline uint8_t getRootKey() noexcept {
			return (*this->localTapeRootPosition >> 56);
		}

		inline size_t size() noexcept {
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

		inline JsonType type() noexcept {
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
		uint64_t* tapeRootPosition{};
		uint8_t* stringBuffer{};
		size_t currentIndex{};
		ErrorCode error{};
	};

	class Field : public JsonValueBase, protected std::pair<std::string_view, JsonValueBase> {
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

			inline friend bool operator==(const FieldIterator& lhs, const FieldIterator& rhs) noexcept {
				return lhs.ptr->getOffset() >= lhs.ptr->getCurrentCount();
			};

		  protected:
			Pointer ptr{};
		};

		inline Field() noexcept
			: JsonValueBase{ nullptr, nullptr, nullptr }, std::pair<std::string_view, JsonValueBase>{ std::string_view{},
				  JsonValueBase{ nullptr, nullptr, nullptr } } {};

		inline auto begin() noexcept {
			return FieldIterator{ this };
		}

		inline auto end() noexcept {
			return FieldIterator{ this };
		}

		inline std::string_view getKey() {
			return this->first;
		}

		inline Field(std::string_view&& key, JsonValueBase& value)
			: JsonValueBase{ value }, std::pair<std::string_view, JsonValueBase>{ std::move(key), std::move(value) } {
			if (this->peek() != '"') {
				throw JsonifierException{ "Sorry, but this item's type is not field." };
			}
			this->advance();
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

			inline friend bool operator==(ArrayIterator& lhs, const ArrayIterator& rhs) noexcept {
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

		inline Array(JsonValueBase& other) : JsonValueBase{ other } {
			if (this->peek() != '[') {
				throw JsonifierException{ "Sorry, but this item's type is not array." };
			}
			this->advance();
		}
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

			inline friend bool operator==(const ObjectIterator& lhs, const ObjectIterator& rhs) noexcept {
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

		inline Object(JsonValueBase& other) : JsonValueBase{ other } {
			if (this->peek() != '{') {
				throw JsonifierException{ "Sorry, but this item's type is not object." };
			}
		};
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

		void fromUint64(uint64_t data) {
			this->Value = _mm256_set1_epi64x(static_cast<int64_t>(data));
		}

		explicit inline SimdBase256(int64_t other) {
			this->fromUint64(other);
		}

		inline SimdBase256& operator=(uint8_t other) {
			this->Value = _mm256_set1_epi8(other);
			return *this;
		}

		inline SimdBase256& operator=(const uint8_t* values) {
			*this = _mm256_loadu_epi8(values);
			return *this;
		}

		explicit inline SimdBase256(const uint8_t* values) {
			*this = values;
		}

		explicit inline SimdBase256(int64_t value00, int64_t value01, int64_t value02, int64_t value03) {
			this->Value = _mm256_set_epi64x(value03, value02, value01, value00);
		}

		explicit inline SimdBase256(uint64_t value00, uint64_t value01, uint64_t value02, uint64_t value03) {
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
			returnValue |= _mm256_slli_epi64(*this, 64 - (amount % 64));
			returnValue |= _mm256_srli_epi64(_mm256_slli_si256(*this, (amount % 64) / 8), (amount % 64));
			return returnValue;
		}

		template<size_t amount> inline SimdBase256 shr() {
			SimdBase256 returnValue{};
			returnValue |= _mm256_srli_epi64(*this, 64 - (amount % 64));
			returnValue |= _mm256_slli_epi64(_mm256_srli_si256(*this, (amount % 64) / 8), (amount % 64));
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

		inline void printBits(uint64_t values, const std::string& valuesTitle) {
			using std::cout;
			cout << valuesTitle;
			cout << std::bitset<64>{ values };
			cout << std::endl;
		}

		inline SimdBase256 printBits(const std::string& valuesTitle) {
			using std::cout;
			cout << valuesTitle;
			for (size_t x = 0; x < 32; ++x) {
				for (size_t y = 0; y < 8; ++y) {
					cout << std::bitset<1>{ static_cast<size_t>(*(reinterpret_cast<int8_t*>(&this->Value) + x)) >> y };
				}
			}
			cout << std::endl;
			return *this;
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

	class SimdStringSection {
	  public:
		inline SimdStringSection() noexcept = default;

		inline void packStringIntoValue(SimdBase256* theValue, const uint8_t string[32]) {
			*theValue = string;
		}

		inline uint64_t addTapeValues(uint32_t* tapePtrs, uint64_t* theBits, size_t currentIndexNew, size_t& currentIndexIntoTape,
			size_t stringLength) {
			uint64_t cnt = __popcnt64(*theBits);
			int64_t newValue{};
			for (int i = 0; i < cnt; i++) {
				newValue = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + this->currentIndexIntoString;

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

		inline size_t getStructuralIndices(uint32_t* currentPtr, size_t& currentIndexIntoTape, size_t stringLength) {
			size_t returnValue{};
			for (size_t x = 0; x < 4; ++x) {
				auto newValue = this->S256.getUint64(x);
				returnValue += this->addTapeValues(currentPtr, &newValue, x, currentIndexIntoTape, stringLength);
			}
			this->currentIndexIntoString += 256;
			return returnValue;
		}

		inline SimdBase256 follows(SimdBase256 match, SimdBase256& overflow) {
			SimdBase256 result = match.shl<1>() | overflow;
			overflow = match.shr<63>();
			return result;
		}

		inline SimdBase256 collectWhiteSpace() {
			uint8_t valuesNew[32]{ ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113,
				2, 100, '\t', '\n', 112, 100, '\r', 100, 100 };
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
			this->S256 = B256.bitAndNot(B256.shl<1>());
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
			this->S256 = this->S256.bitAndNot(this->R256);
			this->S256 = this->S256 | this->Q256;
			auto P = this->S256 | this->W256;
			P = P.shl<1>();
			P &= ~this->W256.bitAndNot(this->R256);
			this->S256 = this->S256 | P;
			this->S256 = this->S256.bitAndNot(this->Q256.bitAndNot(this->R256));
			return this->S256;
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

	class JsonifierCore;

	class Document : public JsonValueBase {
	  public:
		inline Document() noexcept;
		inline Document(JsonifierCore* value) noexcept;

	  protected:
		JsonifierCore* parser{};
	};

	inline int64_t totalTimePassed{};
	inline int64_t totalTimePassed02{};
	inline int64_t iterationCount{};

	class JsonifierCore {
	  public:
		inline Document getDocument() {
			Document returnValue{ this };
			returnValue.advance();
			return returnValue;
		}

		inline JsonifierCore& operator=(JsonifierCore&&) = default;
		inline JsonifierCore(JsonifierCore&&) = default;
		inline JsonifierCore(){};

		inline int64_t round(int64_t a, int64_t n) {
			return (((a) + (( n )-1)) & ~(( n )-1));
		}

		inline ErrorCode allocate(uint8_t* stringViewNew) noexcept {
			if (this->stringLengthRaw == 0) {
				return ErrorCode::Success;
			}

			this->stringCapacity = round(5 * this->stringLengthRaw / 3 + 256, 256);
			this->tapeCapacity = round(this->stringLengthRaw + 3, 256);
			this->structuralIndexes.allocate(this->tapeCapacity, this->tapeCapacity);
			this->stringBuffer.allocate(this->stringCapacity, this->stringCapacity);
			this->isArray.allocate(this->tapeCapacity, this->tapeCapacity);
			this->openContainers.allocate(this->maxDepth, this->maxDepth);
			this->tape.allocate(this->tapeCapacity, this->tapeCapacity);
			this->stringView = stringViewNew;
			if (!(this->tape.get() && this->structuralIndexes.get() && this->stringBuffer.get() && this->isArray.get() &&
					this->openContainers.get())) {
				this->structuralIndexes.deallocate(this->tapeCapacity);
				this->stringBuffer.deallocate(this->stringCapacity);
				this->openContainers.deallocate(this->maxDepth);
				this->isArray.deallocate(this->tapeCapacity);
				this->tape.deallocate(this->tapeCapacity);
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
				StringBlockReader<256> stringReader{ this->stringView, this->stringLengthRaw };
				StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
				size_t tapeCurrentIndex{ 0 };
				while (stringReader.hasFullBlock()) {
					this->section.submitDataForProcessing(stringReader.fullBlock());
					auto indexCount = section.getStructuralIndices(this->structuralIndexes.get(), tapeCurrentIndex, this->stringLengthRaw);
					stringReader.advance();
				}
				uint8_t block[256];
				stringReader.getRemainder(block);
				this->section.submitDataForProcessing(block);
				auto indexCount = section.getStructuralIndices(this->structuralIndexes.get(), tapeCurrentIndex, this->stringLengthRaw);
				totalTimePassed += stopWatch.totalTimePassed().count();
				for (size_t x = 0; x < indexCount; ++x) {
					std::cout << "THE INDEX: " << this->structuralIndexes[x]
							  << ", THE VALUE AT THAT INDEX: " << this->stringView[this->structuralIndexes[x]] << std::endl;
				}
				std::cout << "TIME FOR STAGE1: " << totalTimePassed / iterationCount << std::endl;
			}
		}

		inline uint8_t* getStringView() {
			return this->stringView;
		}

		inline uint8_t* getStringBuffer() {
			return this->stringBuffer.get();
		}

		inline OpenContainer* getOpenContainers() {
			return this->openContainers.get();
		}

		inline uint32_t* getStructuralIndexes() {
			return this->structuralIndexes.get();
		}

		inline size_t getStructuralIndexCount() {
			return this->lastStructural - this->getStructuralIndexes();
		}

		inline uint64_t* getTape() {
			return this->tape.get();
		}

		static inline JsonifierResult<Document> getJsonData(std::string& string);

		inline uint32_t getMaxDepth() {
			return this->maxDepth;
		}

		inline size_t& getTapeLength() {
			return this->tapeLength;
		}

		inline bool* getIsArray() {
			return this->isArray.get();
		}

		~JsonifierCore() {
			this->structuralIndexes.deallocate(this->tapeCapacity);
			this->stringBuffer.deallocate(this->stringCapacity);
			this->openContainers.deallocate(this->maxDepth);
			this->isArray.deallocate(this->tapeCapacity);
			this->tape.deallocate(this->tapeCapacity);
		}

	  protected:
		ObjectBuffer<OpenContainer> openContainers{};
		ObjectBuffer<uint32_t> structuralIndexes{};
		ObjectBuffer<uint8_t> stringBuffer{};
		ObjectBuffer<uint64_t> tape{};
		ObjectBuffer<bool> isArray{};
		SimdStringSection section{};
		uint32_t* lastStructural{};
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

	class StructuralIterator {
	  public:
		JsonifierCore* masterParser{};
		const uint8_t* stringView{};
		uint32_t* nextStructural{};
		uint32_t depth{};

		inline StructuralIterator(JsonifierCore* masterParserNew, size_t start_structural_index);

		inline ErrorCode visitRootPrimitive(TapeBuilder& visitor, const uint8_t* Value);

		inline ErrorCode visitPrimitive(TapeBuilder& visitor, const uint8_t* Value);

		inline ErrorCode walkDocument(TapeBuilder& visitor);

		inline const uint8_t* advance() noexcept;

		inline uint8_t lastStructural() noexcept;

		inline size_t remainingLen() noexcept;

		inline const uint8_t* peek() noexcept;

		inline bool atBeginning() noexcept;

		inline bool atEof() noexcept;
	};

	inline StructuralIterator::StructuralIterator(JsonifierCore* masterParserNew, size_t start_structural_index)
		: nextStructural(masterParserNew->getStructuralIndexes()), stringView{ masterParserNew->getStringView() }, masterParser{ masterParserNew } {};

	inline const uint8_t* StructuralIterator::peek() noexcept {
		return &this->stringView[*this->nextStructural];
	}

	inline const uint8_t* StructuralIterator::advance() noexcept {
		return &this->stringView[*this->nextStructural++];
	}

	inline size_t StructuralIterator::remainingLen() noexcept {
		return this->masterParser->getStructuralIndexCount() - *this->nextStructural;
	}

	inline bool StructuralIterator::atEof() noexcept {
		return this->nextStructural == &this->masterParser->getStructuralIndexes()[this->masterParser->getStructuralIndexCount() - 1];
	}

	inline bool StructuralIterator::atBeginning() noexcept {
		return this->nextStructural == this->masterParser->getStructuralIndexes();
	}

	inline uint8_t StructuralIterator::lastStructural() noexcept {
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
		inline static ErrorCode parseDocument(JsonifierCore& masterParser);

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

	  protected:
		uint8_t* currentStringBufferLocation{};
		size_t& currentTapeLength;

		inline TapeBuilder(JsonifierCore& doc) noexcept;

		inline uint32_t nextTapeIndex(StructuralIterator& iter) noexcept;
		inline ErrorCode startContainer(StructuralIterator& iter) noexcept;
		inline ErrorCode endContainer(StructuralIterator& iter, TapeType start, TapeType end) noexcept;
		inline ErrorCode emptyContainer(StructuralIterator& iter, TapeType start, TapeType end) noexcept;
		inline uint8_t* onStartString(StructuralIterator& iter) noexcept;
		inline ErrorCode onEndString(uint8_t* dst) noexcept;
	};

	inline ErrorCode TapeBuilder::parseDocument(JsonifierCore& masterParser) {
		StructuralIterator iter(&masterParser, 0);
		TapeBuilder builder(masterParser);
		return iter.walkDocument(builder);
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

	inline TapeBuilder::TapeBuilder(JsonifierCore& doc) noexcept
		: tape{ doc.getTape() }, currentStringBufferLocation{ doc.getStringBuffer() }, currentTapeLength(doc.getTapeLength()){};

	inline ErrorCode TapeBuilder::visitString(StructuralIterator& iter, const uint8_t* Value) noexcept {
		uint8_t* dst01 = onStartString(iter);
		dst01 = StringParser::parseString(reinterpret_cast<const uint8_t*>(Value) + 1ull, reinterpret_cast<uint8_t*>(dst01),
			(*iter.nextStructural + 1ull) - (*iter.nextStructural));
		if (dst01 == nullptr) {
			return ErrorCode::StringError;
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
		std::unique_ptr<uint8_t[]> copy(new (std::nothrow) uint8_t[iter.remainingLen() + 256]);
		if (copy.get() == nullptr) {
			return ErrorCode::MemAlloc;
		}
		std::memcpy(copy.get(), Value, iter.remainingLen());
		std::memset(copy.get() + iter.remainingLen(), ' ', 256);
		return visitNumber(iter, copy.get());
	}

	inline ErrorCode TapeBuilder::visitTrueAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidTrueAtom(Value)) {
			return ErrorCode::TAtomError;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootTrueAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidTrueAtom(Value)) {
			return ErrorCode::NAtomError;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitFalseAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidFalseAtom(Value)) {
			return ErrorCode::FAtomError;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootFalseAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidFalseAtom(Value)) {
			return ErrorCode::FAtomError;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitNullAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidNullAtom(Value)) {
			return ErrorCode::NAtomError;
		}
		this->tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootNullAtom(StructuralIterator& iter, const uint8_t* Value) noexcept {
		if (!StringParser::isValidNullAtom(Value)) {
			return ErrorCode::NAtomError;
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
	}

	inline ErrorCode StructuralIterator::walkDocument(TapeBuilder& visitor) {
		this->masterParser->getTapeLength() = 0;
		if (atEof()) {
			return ErrorCode::Empty;
		}
		visitor.visitDocumentStart(*this);
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
		goto Document_End;

	Object_Begin : {
		this->depth++;
		if (this->depth >= masterParser->getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		this->masterParser->getIsArray()[this->depth] = false;
		visitor.visitObjectStart(*this);
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

	Object_Continue : {
		auto newValue = *this->advance();
		switch (newValue) {
			case ',':
				visitor.incrementCount(*this);
				{
					auto key = this->advance();
					if (*key != '"') {
						throw JsonifierException{ "Sorry, but you've encountered the following error: " +
							std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
							", at the following index into the string: " + std::to_string(*this->nextStructural) +
							", and the key was: " + std::string{ *reinterpret_cast<const char*>(this->peek() - 1) } };
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

	Array_Continue : {
		auto newValue = *this->advance();
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

	inline ErrorCode StructuralIterator::visitRootPrimitive(TapeBuilder& visitor, const uint8_t* Value) {
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

	inline ErrorCode StructuralIterator::visitPrimitive(TapeBuilder& visitor, const uint8_t* Value) {
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

	JsonifierResult<Document> JsonifierCore::getJsonData(std::string& string) {
		JsonifierCore returnValueFirst{};
		returnValueFirst.generateJsonEvents(reinterpret_cast<uint8_t*>(string.data()), string.size());
		auto errorCode = TapeBuilder::parseDocument(returnValueFirst);
		returnValueFirst.getTapeLength() = (returnValueFirst.getTape()[0] & JSON_VALUE_MASK);
		JsonifierResult<Document> returnValue{ Document{ returnValueFirst.getDocument() }, std::move(errorCode) };
		return returnValue;
	}

	inline Field JsonValueBase::parseJsonField(const char* fieldKey) noexcept {
		return this->findField(fieldKey);
	}

	inline Object JsonValueBase::parseJsonObject() noexcept {
		this->assertAtObjectStart();
		if (this->peek() == '{') {
			return Object{ this->getCurrentIterator() };
		} else {
			return Object{};
		}
	}

	inline Array JsonValueBase::parseJsonArray() noexcept {
		this->assertAtArrayStart();
		if (this->peek() == '[') {
			return Array{ this->getCurrentIterator() };

		} else {
			return Array{};
		}
	}

	inline Field JsonValueBase::operator[](const char* keyNew) {
		return this->findField(keyNew);
	}

	template<> inline JsonifierResult<Array> JsonValueBase::get<Array>() noexcept {
		return { this->parseJsonArray(), std::move(this->error) };
	}

	template<> inline JsonifierResult<Object> JsonValueBase::get<Object>() noexcept {
		return { this->parseJsonObject(), std::move(this->error) };
	}

	template<> inline JsonifierResult<const char*> JsonValueBase::get<const char*>() noexcept {
		return { this->parseJsonString().data(), std::move(this->error) };
	}

	template<> inline JsonifierResult<std::string_view> JsonValueBase::get<std::string_view>() noexcept {
		return { this->parseJsonString(), std::move(this->error) };
	}

	template<> inline JsonifierResult<int64_t> JsonValueBase::get<int64_t>() noexcept {
		return { this->parseJsonInt(), std::move(this->error) };
	}

	template<> inline JsonifierResult<uint64_t> JsonValueBase::get<uint64_t>() noexcept {
		return { this->parseJsonUint(), std::move(this->error) };
	}

	template<> inline JsonifierResult<double> JsonValueBase::get<double>() noexcept {
		return { this->parseJsonFloat(), std::move(this->error) };
	}

	template<> inline JsonifierResult<bool> JsonValueBase::get<bool>() noexcept {
		return { this->parseJsonBool(), std::move(this->error) };
	}

	inline Document::Document() noexcept : parser{ nullptr }, JsonValueBase{ nullptr, nullptr, nullptr } {};

	inline Document::Document(JsonifierCore* value) noexcept
		: parser{ value }, JsonValueBase{ value->getStringBuffer(), value->getTape(), value->getTape() } {};

	inline Field JsonValueBase::findField(const char* keyNew) noexcept {
		int32_t index{};
		std::string_view newString{};
		if (this->peek() == '"') {
			newString = this->parseJsonString();
		}
		while (this->peek(index) != '"' && newString != keyNew) {
			if (this->peek(index) == '"') {
				newString = this->parseJsonString();
			}
			++index;
		}
		this->advance(index);
		this->asserAtFieldStart(0);
		return Field{ std::move(newString), this->getCurrentIterator() };
	}

};