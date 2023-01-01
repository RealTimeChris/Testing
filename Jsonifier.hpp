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

		inline void allocate(size_t newSize) noexcept {
			this->deallocate();
			if (newSize != 0) {
				ObjectAllocator<OTy> allocator{};
				this->objects = AllocatorTraits::allocate(allocator, newSize);
				this->currentSize = newSize;
			}
		}

		inline void deallocate() {
			if (this->currentSize > 0 && this->objects) {
				ObjectAllocator<OTy> allocator{};
				AllocatorTraits::deallocate(allocator, this->objects, this->currentSize);
				this->objects = nullptr;
			}
		}

		inline OTy* get() noexcept {
			return this->objects;
		}

		inline size_t size() noexcept {
			return this->currentSize;
		}

		inline ~ObjectBuffer() noexcept {
			this->deallocate();
		}

	  protected:
		size_t currentSize{};
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
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = std::move(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> inline Jsonifier(std::unordered_map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy>
		inline Jsonifier& operator=(std::unordered_map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = value;
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> inline Jsonifier(std::unordered_map<KTy, OTy>& data) noexcept {
			*this = data;
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> inline Jsonifier& operator=(std::map<KTy, OTy>&& data) noexcept {
			this->setValue(JsonType::object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = std::move(value);
			}
			return *this;
		}

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> inline Jsonifier(std::map<KTy, OTy>&& data) noexcept {
			*this = std::move(data);
		};

		template<IsConvertibleToJsonifier KTy, IsConvertibleToJsonifier OTy> inline Jsonifier& operator=(std::map<KTy, OTy>& data) noexcept {
			this->setValue(JsonType::object);
			for (auto& [key, value]: data) {
				(*this->jsonValue.object)[key] = value;
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

		template<typename Ty> inline const Ty& getValue() const {
			return Ty{};
		}

		template<typename Ty> inline Ty& getValue() {
			return Ty{};
		}

		size_t size() noexcept;

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
		inline void writeJsonInt(NumberType Int) {
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

	class JsonifierCore;
	class JsonValueBase;
	class Object;
	class Array;
	class Field;

	template<typename OTy> class JsonifierResult : protected std::pair<OTy, ErrorCode> {
	  public:
		template<typename OTy> inline JsonifierResult() noexcept : std::pair<OTy, ErrorCode>{ OTy{}, ErrorCode::Success } {};
		template<typename OTy>
		inline JsonifierResult(OTy&& other, ErrorCode&& error) noexcept : std::pair<OTy, ErrorCode>{ std::move(other), std::move(error) } {};

		template<typename OTy> inline ErrorCode get(OTy& value) noexcept {
			value = std::move(this->first);
			return std::move(this->second);
		}

		inline OTy getValue() {
			if (this->second != ErrorCode::Success) {
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(this->second) } };
			}
			return std::move(this->first);
		}
	};

	class RawJsonString {
	  public:
		inline RawJsonString() noexcept = default;
		inline RawJsonString(const uint8_t* _buf) noexcept;
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
		inline std::string_view unescape(JsonValueBase& iter) const noexcept;

		const uint8_t* buf{};
		friend class object;
		friend class field;
		friend class parser;
		friend class JsonValueBase;
	};

	class JsonValueBase {
	  public:
		inline JsonValueBase() noexcept = default;
		inline JsonValueBase& operator=(const JsonValueBase& other) noexcept;
		inline JsonValueBase(const JsonValueBase& other) noexcept;
		inline JsonValueBase& operator=(JsonValueBase&& other) noexcept;
		inline JsonValueBase(JsonValueBase&& other) noexcept;

		inline JsonValueBase(JsonifierCore* other) noexcept;

		inline Field operator[](const char* keyNew) noexcept;
		
		template<typename OTy> inline ErrorCode get(OTy&) noexcept;

		template<typename OTy> inline JsonifierResult<OTy> get() noexcept;

		template<typename OTy> inline JsonifierResult<OTy> get(const char*) noexcept;

		inline JsonValueBase resume_value_iterator() noexcept;

		inline JsonValueBase get_root_value_iterator() noexcept;

		inline Field findField(const char* keyNew) noexcept;

		inline JsonifierCore* getCore() noexcept;

		inline Object get_object() & noexcept;

		inline Object parseJsonObject() noexcept;

		inline Field parseJsonField(const char* fieldKey = nullptr) noexcept;

		inline Array parseJsonArray() noexcept;

		inline std::string_view parseJsonString() noexcept;
		
		inline double parseJsonFloat() noexcept;
		
		inline uint64_t parseJsonUint() noexcept;
		
		inline int64_t parseJsonInt() noexcept;
		
		inline bool parseJsonBool() noexcept;
		
		inline nullptr_t parseJsonNull() noexcept;
		
		inline uint64_t* advance() noexcept;
		
		inline void setPosition(int32_t delta = 0) noexcept;
		
		inline size_t getOffset() noexcept;
		
		inline JsonValueBase& getCurrentIterator() noexcept;
		
		inline void asserAtFieldStart(size_t amountToOffset) noexcept;
		
		inline void assertAtObjectStart(size_t amountToOffset = 0) noexcept;
		
		inline void assertAtArrayStart(size_t amountToOffset = 0) noexcept;
		
		inline void assertAtStringStart(size_t amountToOffset = 0) noexcept;
		
		inline size_t getCurrentCount() noexcept;
		
		inline uint8_t getRootKey() noexcept;
		
		inline size_t size() noexcept;
		inline std::string_view unescape(RawJsonString in, uint8_t*& dst) const noexcept;
		
		inline JsonType type() noexcept;
		inline const uint8_t* return_current_and_advance() noexcept;
		inline uint32_t current_offset() const noexcept;
		inline uint32_t peek_length(int32_t delta = 0) const noexcept;
		inline const uint8_t* peek() const noexcept;
		inline uint32_t peek_length(uint32_t* position) const noexcept;
		inline void set_position(uint32_t* target_position) noexcept;
		inline bool operator==(const JsonValueBase& other) const noexcept;
		inline bool operator!=(const JsonValueBase& other) const noexcept;
		inline bool operator>(const JsonValueBase& other) const noexcept;
		inline bool operator>=(const JsonValueBase& other) const noexcept;
		inline bool operator<(const JsonValueBase& other) const noexcept;
		inline bool operator<=(const JsonValueBase& other) const noexcept;
		inline ErrorCode skip_child(size_t parent_depth) noexcept;
		inline bool at_root() const noexcept;
		inline bool streaming() const noexcept;
		inline uint32_t* root_position() const noexcept;
		inline void assert_at_document_depth() const noexcept;
		inline void assert_at_root() const noexcept;
		inline bool at_end() const noexcept;
		inline bool is_alive() const noexcept;
		inline void abandon() noexcept;
		inline bool is_single_token() const noexcept;
		inline void assert_more_tokens(uint32_t required_tokens = 1) const noexcept;
		inline void assert_valid_position(uint32_t* position) const noexcept;
		inline const uint8_t* unsafe_pointer() const noexcept;
		inline const uint8_t* peek_last() const noexcept;
		inline void ascend_to(size_t parent_depth) noexcept;
		inline void descend_to(size_t child_depth) noexcept;
		inline void descend_to(size_t child_depth, int32_t delta) noexcept;
		inline size_t depth() const noexcept;
		inline uint8_t*& string_buf_loc() noexcept;
		inline ErrorCode report_error(ErrorCode error, const char* message) noexcept;
		inline ErrorCode optional_error(ErrorCode error, const char* message) noexcept;

		template<int N>
		inline bool copy_to_buffer(const uint8_t* json, uint32_t max_len, uint8_t (&tmpbuf)[N]) noexcept;

		inline uint32_t* positionVal() const noexcept;
		inline void reenter_child(uint32_t* position, size_t child_depth) noexcept;
		inline std::string to_string() const noexcept;
		inline JsonifierResult<const char*> current_location() noexcept;
		inline void rewind() noexcept;
		inline bool balanced() const noexcept;
		inline uint32_t* last_position() const noexcept;
		inline uint32_t* end_position() const noexcept;
		inline uint32_t* end() const noexcept;
		inline uint32_t peek_index(int32_t delta = 0) const noexcept;
		inline uint32_t peek_index(uint32_t* position) const noexcept;
		inline void start_document() noexcept;
		inline bool is_open() const noexcept;
		inline bool at_first_field() const noexcept;
		inline JsonValueBase child_value() const noexcept;
		inline JsonType type() const noexcept;
		inline bool start_object() noexcept;
		inline bool start_root_object() noexcept;
		inline bool started_object() noexcept;
		inline bool started_root_object() noexcept;
		inline bool has_next_field() noexcept;
		inline RawJsonString field_key() noexcept;
		inline ErrorCode field_value() noexcept;
		inline ErrorCode find_field(const std::string_view key) noexcept;
		inline bool find_field_raw(const std::string_view key) noexcept;
		inline bool find_field_unordered_raw(const std::string_view key) noexcept;
		inline bool start_array() noexcept;
		inline bool start_root_array() noexcept;
		inline bool started_array() noexcept;
		inline bool started_root_array() noexcept;
		inline bool has_next_element() noexcept;
		inline JsonValueBase child() noexcept;

		inline std::string_view get_string() noexcept;
		inline RawJsonString get_raw_json_string() noexcept;
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
		inline RawJsonString get_root_raw_json_string() noexcept;
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

		inline ErrorCode getError()  noexcept;
		inline const JsonValueBase& json_iter() const noexcept;
		inline JsonValueBase& json_iter() noexcept;

		inline void assert_is_valid() const noexcept;
		inline bool is_valid() const noexcept;
	  protected:
		inline bool reset_array() noexcept;
		inline bool reset_object() noexcept;
		inline void move_at_start() noexcept;
		inline void move_at_container_start() noexcept;

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
		inline const uint8_t*advance_to_value() noexcept;

		inline ErrorCode incorrect_type_error(const char* message) const noexcept;
		inline ErrorCode error_unless_more_tokens(uint32_t tokens = 1) const noexcept;

		inline bool is_at_start() const noexcept;
		inline bool is_at_iterator_start() const noexcept;
		inline bool is_at_key() const noexcept;

		inline void assert_at_start() const noexcept;
		inline void assert_at_container_start() const noexcept;
		inline void assert_at_child() const noexcept;
		inline void assert_at_next() const noexcept;
		inline void assert_at_non_root_start() const noexcept;
		inline uint32_t* start_position() const noexcept;


		const uint8_t* stringView{};
		uint32_t* position{};

		friend class RawJsonString;

	  protected:
		ErrorCode error{ ErrorCode::Success };
		uint8_t* stringBufferLocation{};
		JsonifierCore* parser{};
		size_t currentIndex{};
		uint32_t* root{};
		size_t currentDepth{};
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

		inline auto begin() noexcept {
			return FieldIterator{ this };
		}

		static inline Field start(JsonValueBase& parent_iter) noexcept {
			RawJsonString key{};
			key = parent_iter.field_key();
			std::cout << "CURRENT KEY: " << key.raw() << std::endl;
			parent_iter.field_value();
			return Field::start(parent_iter, key);
		}

		static inline Field start(JsonValueBase& parent_iter, RawJsonString key) noexcept {
			return Field(key, parent_iter.child());
		}

		inline auto end() noexcept {
			return FieldIterator{ this };
		}

		inline std::string_view getKey() {
			return this->first;
		}

		inline Field() noexcept = default;

		inline Field(RawJsonString& key, JsonValueBase&& value)
			: JsonValueBase{ value }, std::pair<std::string_view, JsonValueBase>{ std::move(key.raw()), value } {
		};

		inline Field(std::string_view&& key, JsonValueBase& value)
			: JsonValueBase{ value }, std::pair<std::string_view, JsonValueBase>{ std::move(key), value } {
		};
	};

	class ArrayIterator {
	  public:
		inline ArrayIterator() noexcept = default;
		inline JsonifierResult<Object> operator*() noexcept;
		inline bool operator==(const ArrayIterator&) const noexcept;
		inline bool operator!=(const ArrayIterator&) const noexcept;
		inline ArrayIterator& operator++() noexcept;

	  private:
		JsonValueBase iter{};

		inline ArrayIterator(const JsonValueBase& iter) noexcept;

		friend class Array;
		friend struct JsonifierResult<ArrayIterator>;
	};

	class field;

	class ObjectIterator {
	  public:
		inline ObjectIterator(const JsonValueBase& _iter) noexcept : iter{ _iter } {
		}

		inline Field operator*() noexcept {
			ErrorCode error = iter.getError();
			if (error!=ErrorCode::Success) {
				iter.abandon();
				return Field{};
			}
			auto result = Field::start(iter);
			// TODO this is a safety rail ... users should exit loops as soon as they receive an error.
			// Nonetheless, let's see if performance is OK with this if statement--the compiler may give it to us for free.
			if (result.getError() != ErrorCode::Success) {
				iter.abandon();
			}
			return result;
		}

		inline bool operator==(const ObjectIterator& other) const noexcept {
			return !(*this != other);
		}

		inline bool operator!=(const ObjectIterator&) const noexcept {
			return iter.is_open();
		}

		inline ObjectIterator& operator++() noexcept {
			// TODO this is a safety rail ... users should exit loops as soon as they receive an error.
			// Nonetheless, let's see if performance is OK with this if statement--the compiler may give it to us for free.
			if (!iter.is_open()) {
				return *this;
			}// Iterator will be released if there is an error

			ErrorCode error;
			if (error = iter.skip_child(iter.depth()); error != ErrorCode::Success) {
				return *this;
			}

			bool has_value;
			if (!iter.has_next_field()) {
				return *this;
			};
			return *this;
		}

	  private:
		JsonValueBase iter{};
		friend struct JsonifierResult<ObjectIterator>;
		friend class object;
	};

	class Array {
	  public:
		inline Array() noexcept = default;
		inline JsonifierResult<ArrayIterator> begin() noexcept;
		inline JsonifierResult<ArrayIterator> end() noexcept;
		inline JsonifierResult<size_t> count_elements() & noexcept;
		inline JsonifierResult<bool> is_empty() & noexcept;
		inline JsonifierResult<bool> reset() & noexcept;
		inline JsonifierResult<Object> at_pointer(std::string_view json_pointer) noexcept;
		inline JsonifierResult<std::string_view> raw_json() noexcept;
		inline JsonifierResult<Object> at(size_t index) noexcept;
		inline Array(const JsonValueBase& iter) noexcept;

	  protected:
		inline ErrorCode consume() noexcept;
		static inline JsonifierResult<Array> start(JsonValueBase& iter) noexcept;
		static inline JsonifierResult<Array> start_root(JsonValueBase& iter) noexcept;
		static inline JsonifierResult<Array> started(JsonValueBase& iter) noexcept;
		
		JsonValueBase iter{};

		friend class value;
		friend class document;
		friend struct JsonifierResult<Object>;
		friend struct JsonifierResult<Array>;
		friend class ArrayIterator;
	};

	class Object : public JsonValueBase {
	  public:

		inline auto begin() noexcept {
			return ObjectIterator{ *this };
		}

		inline auto end() noexcept {
			return ObjectIterator{ *this };
		}
		inline size_t count_fields() noexcept;
		inline Object() noexcept = default;

		static inline Object start(JsonValueBase& iter) noexcept {
			iter.start_object();
			return Object(iter);
		}

		static inline Object start_root(JsonValueBase& iter) noexcept {
			iter.start_root_object();
			return Object(iter);
		}

		inline Object(JsonValueBase& other) : JsonValueBase{ std::move(other) } {
		};
	};

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
			this->value = _mm256_set1_epi64x(static_cast<int64_t>(data));
		}

		explicit inline SimdBase256(int64_t other) {
			this->fromUint64(other);
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

		explicit inline SimdBase256(const char* values) {
			*this = values;
		}

		explicit inline SimdBase256(int64_t value00, int64_t value01, int64_t value02, int64_t value03) {
			this->value = _mm256_set_epi64x(value03, value02, value01, value00);
		}

		explicit inline SimdBase256(uint64_t value00, uint64_t value01, uint64_t value02, uint64_t value03) {
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

		inline SimdBase256 operator|(SimdBase256& other) {
			return _mm256_or_si256(this->value, other);
		}

		inline SimdBase256 operator&(SimdBase256& other) {
			return _mm256_and_si256(this->value, other);
		}

		inline SimdBase256 operator^(SimdBase256& other) {
			return _mm256_xor_si256(this->value, other);
		}

		inline SimdBase256 operator+(SimdBase256& other) {
			return _mm256_add_epi8(this->value, other);
		}

		inline SimdBase256& operator|=(SimdBase256& other) {
			*this = *this | other;
			return *this;
		}

		inline SimdBase256& operator&=(SimdBase256& other) {
			*this = *this & other;
			return *this;
		}

		inline SimdBase256& operator^=(SimdBase256& other) {
			*this = *this ^ other;
			return *this;
		}

		inline SimdBase256 operator==(SimdBase256& other) {
			return _mm256_cmpeq_epi8(this->value, other);
		}

		inline SimdBase256 operator|(SimdBase256&& other) {
			return _mm256_or_si256(this->value, other);
		}

		inline SimdBase256 operator&(SimdBase256&& other) {
			return _mm256_and_si256(this->value, other);
		}

		inline SimdBase256 operator^(SimdBase256&& other) {
			return _mm256_xor_si256(this->value, other);
		}

		inline SimdBase256 operator+(SimdBase256&& other) {
			return _mm256_add_epi8(this->value, other);
		}

		inline SimdBase256& operator|=(SimdBase256&& other) {
			*this = *this | other;
			return *this;
		}

		inline SimdBase256& operator&=(SimdBase256&& other) {
			*this = *this & other;
			return *this;
		}

		inline SimdBase256& operator^=(SimdBase256&& other) {
			*this = *this ^ other;
			return *this;
		}

		inline SimdBase256 operator==(SimdBase256&& other) {
			return _mm256_cmpeq_epi8(this->value, other);
		}

		inline SimdBase256 operator==(uint8_t other) {
			return _mm256_cmpeq_epi8(this->value, _mm256_set1_epi8(other));
		}

		template<size_t amount> inline SimdBase256 shl() {
			SimdBase256 returnValueReal{};
			SimdBase256 returnValue{};
			returnValue = _mm256_slli_epi64(*this, (amount % 64));
			returnValueReal |= returnValue;
			returnValue = _mm256_permute4x64_epi64(*this, 0b10010011);
			returnValue = _mm256_srli_epi64(returnValue, 64 - amount);
			returnValueReal |= returnValue;
			returnValue = _mm256_set_epi64x(0, 0, 0, (1ll << amount) - (1ll << 0));
			returnValueReal &= ~returnValue;
			return returnValueReal;
		}

		template<size_t amount> inline SimdBase256 shr() {
			SimdBase256 returnValueReal{};
			SimdBase256 returnValue{};
			returnValue = _mm256_srli_epi64(*this, (amount % 64));
			returnValueReal |= returnValue;
			returnValue = _mm256_permute4x64_epi64(*this, 0b00111001);
			returnValue = _mm256_slli_epi64(returnValue, 64 - amount);
			returnValueReal |= returnValue;
			returnValue = _mm256_set_epi64x(0, 0, 0, (1ull << 64-amount) - (1ull << 0));
			returnValueReal &= returnValue;
			return returnValueReal;
		}

		inline SimdBase256 operator~() {
			SimdBase256 newValues{};
			newValues = _mm256_xor_si256(*this, _mm256_set1_epi64x(-1ll));
			return newValues;
		}

		inline SimdBase256 carrylessMultiplication(int64_t& prevInString) {
			SimdBase128 allOnes{ '\xFF' };
			auto inString00 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(0)), allOnes, 0)) ^ prevInString;
			prevInString = static_cast<uint64_t>(inString00 >> 63);
			auto inString01 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(1)), allOnes, 0)) ^ prevInString;
			prevInString = static_cast<uint64_t>(inString01 >> 63);
			auto inString02 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(2)), allOnes, 0)) ^ prevInString;
			prevInString = static_cast<uint64_t>(inString02 >> 63);
			auto inString03 = _mm_cvtsi128_si64(_mm_clmulepi64_si128(_mm_set_epi64x(0ULL, this->getInt64(3)), allOnes, 0)) ^ prevInString;
			prevInString = static_cast<uint64_t>(inString03 >> 63);
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
		inline size_t getRemainder(char* dst) const;
		inline const char* fullBlock() const;
		inline bool hasFullBlock() const;
		inline size_t blockIndex();
		inline void advance();

	  private:
		const uint8_t* stringBuffer{};
		const size_t length{};
		const size_t lengthMinusStep{};
		size_t index{};
	};

	template<size_t StepSize>
	inline StringBlockReader<StepSize>::StringBlockReader(const uint8_t* _buf, size_t _len)
		: stringBuffer{ _buf }, length{ _len }, lengthMinusStep{ length < StepSize ? 0 : length - StepSize }, index{ 0 } {
	}

	template<size_t StepSize> inline size_t StringBlockReader<StepSize>::blockIndex() {
		return index;
	}

	template<size_t StepSize> inline bool StringBlockReader<StepSize>::hasFullBlock() const {
		return index < lengthMinusStep;
	}

	template<size_t StepSize> inline const char* StringBlockReader<StepSize>::fullBlock() const {
		return reinterpret_cast<const char*>(&stringBuffer[index]);
	}

	template<size_t StepSize> inline size_t StringBlockReader<StepSize>::getRemainder(char* dst) const {
		if (length == index) {
			return 0;
		}
		std::memset(dst, 0x20, StepSize);
		std::memcpy(dst, stringBuffer + index, length - index);
		return length - index;
	}

	template<size_t StepSize> inline void StringBlockReader<StepSize>::advance() {
		index += StepSize;
	}


	class SimdStringSection {
	  public:
		inline SimdStringSection() noexcept = default;

		inline void packStringIntoValue(SimdBase256* theValue, const char string[32]) {
			*theValue = string;
		}

		inline uint64_t addTapeValues(uint32_t* tapePtrs, uint64_t* theBits, size_t currentIndexNew, size_t& currentIndexIntoTape,
			size_t stringLength) {
			int cnt = static_cast<int>(__popcnt64(*theBits));
			int64_t newValue{};
			for (int i = 0; i < cnt; i++) {
				newValue = _tzcnt_u64(*theBits) + (currentIndexNew * 64) + currentIndexIntoString;

				if (newValue > stringLength) {
					currentIndexIntoTape += i;
					return i;

				} else {
					tapePtrs[i + currentIndexIntoTape] = newValue;
					*theBits = _blsr_u64(*theBits);
				}
			}
			currentIndexIntoTape += cnt;
			return cnt;
		}

		inline SimdBase256 follows(SimdBase256 match, SimdBase256 overflow) {
			SimdBase256 result = match.shl<1>() | overflow;
			overflow = match.shr<63>();
			return result;
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

		inline SimdBase256 collectWhiteSpace() {
			char valuesNew[32]{ ' ', 100, 100, 100, 17, 100, 113, 2, 100, '\t', '\n', 112, 100, '\r', 100, 100, ' ', 100, 100, 100, 17, 100, 113,
				2, 100, '\t', '\n', 112, 100, '\r', 100, 100 };
			SimdBase256 whitespaceTable{ valuesNew };
			SimdBase256 whiteSpaceReal[8]{};
			for (size_t x = 0; x < 8; ++x) {
				whiteSpaceReal[x] = this->values[x].shuffle(whitespaceTable) == this->values[x];
			}
			return convertSimdBytesToBits(whiteSpaceReal);
		}

		inline SimdBase256 collectStructuralCharacters() {
			char newValues[32]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ':', '{', ',', '}', 0, 0 };
			SimdBase256 opTable{ newValues };
			SimdBase256 structural[8]{};
			for (size_t x = 0; x < 8; ++x) {
				auto valuesNew00 = this->values[x] | char{ 0x20 };
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

			auto S = B256 & ~(B256.shl<1>());
			auto ES = S & E;
			SimdBase256 EC{};
			B256.collectCarries(ES, &EC);
			auto ECE = EC.bitAndNot(B256);
			auto OD1 = ECE & ~E;
			auto OS = S & O;
			auto OC = B256 + OS;
			auto OCE = OC & ~B256;
			auto OD2 = OCE & E;
			auto OD = OD1 | OD2;
			auto R = Q256.bitAndNot(OD);
			return R.carrylessMultiplication(this->prevInString);
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
			auto nonquoteScalar = ~(this->S256 | this->W256).bitAndNot(this->Q256);
			this->followsPotentialNonquoteScalar = follows(nonquoteScalar, this->prevInScalar);
			auto string_tail = this->R256 ^ this->Q256;
			auto scalar = ~(this->S256 | this->W256);
			auto potential_scalar_start = scalar & ~this->followsPotentialNonquoteScalar;
			auto op = this->S256;
			auto potential_structural_start = op | potential_scalar_start;
			auto structuralStart = potential_structural_start & ~string_tail;
			return structuralStart;
		}

		void submitDataForProcessing(const char* valueNew) {
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
			//this->S256.printBits("FINAL BITS: ");
		}

	  protected:
		SimdBase256 followsPotentialNonquoteScalar{};
		size_t currentIndexIntoString{};
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

	class JsonifierCore;

	class Document : public JsonValueBase {
	  public:
		inline Document() noexcept;
		inline Document(JsonifierCore* value) noexcept;

	  protected:
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

			this->stringBuffer.allocate(round(5 * this->stringLengthRaw / 3 + 256, 256));
			this->structuralIndexes.allocate(round(this->stringLengthRaw + 3, 256));
			this->isArray.allocate(this->structuralIndexes.size());
			this->tape.allocate(this->structuralIndexes.size());
			this->openContainers.allocate(this->maxDepth);
			this->stringView = stringViewNew;
			if (!(this->tape.get() && this->structuralIndexes.get() && this->stringBuffer.get() && this->isArray.get() &&
					this->openContainers.get())) {
				this->structuralIndexes.deallocate();
				this->openContainers.deallocate();
				this->stringBuffer.deallocate();
				this->isArray.deallocate();
				this->tape.deallocate();
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

				//iterationCount++;
				StringBlockReader<256> stringReader{ this->stringView, this->stringLengthRaw };
				//StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
				size_t tapeCurrentIndex{ 0 };
				while (stringReader.hasFullBlock()) {
					this->section.submitDataForProcessing(stringReader.fullBlock());
					auto indexCount = section.getStructuralIndices(this->structuralIndexes.get(), tapeCurrentIndex, this->stringLengthRaw);
					stringReader.advance();
				}
				char block[256];
				stringReader.getRemainder(block);
				this->section.submitDataForProcessing(block);
				auto indexCount = section.getStructuralIndices(this->structuralIndexes.get(), tapeCurrentIndex, this->stringLengthRaw);
				//totalTimePassed += stopWatch.totalTimePassed().count();
				this->getTapeLength() = tapeCurrentIndex;
				//std::cout << "TIME FOR STAGE1: " << totalTimePassed / iterationCount << std::endl;
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

		inline uint64_t* getTape() {
			return this->tape.get();
		}

		inline Document parseJson(std::string& string);

		inline uint32_t getMaxDepth() {
			return this->maxDepth;
		}

		inline size_t& getTapeLength() {
			return this->tapeLength;
		}

		inline bool* getIsArray() {
			return this->isArray.get();
		}

	  protected:
		ObjectBuffer<OpenContainer> openContainers{};
		ObjectBuffer<uint32_t> structuralIndexes{};
		ObjectBuffer<uint8_t> stringBuffer{};
		ObjectBuffer<uint64_t> tape{};
		ObjectBuffer<bool> isArray{};
		SimdStringSection section{};
		uint32_t maxDepth{ 512 };
		size_t stringLengthRaw{};
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

	struct TapeWriter {
		TapeWriter(uint64_t* ptr) {
			this->nextTapeLocation = ptr;
		}
		uint64_t* nextTapeLocation;
		static inline void write(uint64_t& tape_loc, uint64_t val, TapeType t) noexcept;
		inline void append(uint64_t val, TapeType t) noexcept;
		inline void appendDouble(double value) noexcept;
		inline void appendU64(uint64_t value) noexcept;
		inline void appendS64(int64_t value) noexcept;
		inline void skipLargeInteger() noexcept;
		inline void skipDouble() noexcept;
		inline void skip() noexcept;

	  protected:
		template<typename OTy> inline void append2(uint64_t val, OTy val2, TapeType t) noexcept;
	};

	inline void TapeWriter::appendS64(int64_t value) noexcept {
		append2(0, value, TapeType::Int64);
	}

	inline void TapeWriter::appendU64(uint64_t value) noexcept {
		append2(0, value, TapeType::Uint64);
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

		inline TapeBuilder(JsonifierCore* doc) noexcept;

		inline ErrorCode walkDocument();

	  protected:
		uint8_t* currentStringBufferLocation{};
		JsonifierCore* masterParser{};
		uint32_t* nextStructural{};
		TapeWriter tape{ nullptr };
		uint32_t depth{};

		inline ErrorCode visitRootPrimitive(const uint8_t* value);

		inline ErrorCode visitPrimitive(const uint8_t* value);

		inline const uint8_t* advance() noexcept;

		inline uint8_t lastStructural() noexcept;

		inline size_t remainingLen() noexcept;

		inline const uint8_t* peek() noexcept;

		inline bool atBeginning() noexcept;

		inline bool atEof() noexcept;

		inline uint32_t nextTapeIndex() noexcept;
		inline ErrorCode startContainer() noexcept;
		inline ErrorCode endContainer(TapeType start, TapeType end) noexcept;
		inline ErrorCode emptyContainer(TapeType start, TapeType end) noexcept;
		inline uint8_t* onStartString() noexcept;
		inline ErrorCode onEndString(uint8_t* dst) noexcept;
		inline ErrorCode incrementCount() noexcept;

		inline ErrorCode visitDocumentStart() noexcept;

		inline ErrorCode visitDocumentEnd() noexcept;

		inline ErrorCode visitArrayStart() noexcept;

		inline ErrorCode visitArrayEnd() noexcept;

		inline ErrorCode visitEmptyArray() noexcept;

		inline ErrorCode visitObjectStart() noexcept;

		inline ErrorCode visitKey(const uint8_t* key) noexcept;

		inline ErrorCode visitObjectEnd() noexcept;

		inline ErrorCode visitEmptyObject() noexcept;

		inline ErrorCode visitString(const uint8_t* value) noexcept;
		inline ErrorCode visitNumber(const uint8_t* value) noexcept;
		inline ErrorCode visitTrueAtom(const uint8_t* value) noexcept;
		inline ErrorCode visitFalseAtom(const uint8_t* value) noexcept;
		inline ErrorCode visitNullAtom(const uint8_t* value) noexcept;

		inline ErrorCode visitRootString(const uint8_t* value) noexcept;
		inline ErrorCode visitRootNumber(const uint8_t* value) noexcept;
		inline ErrorCode visitRootTrueAtom(const uint8_t* value) noexcept;
		inline ErrorCode visitRootFalseAtom(const uint8_t* value) noexcept;
		inline ErrorCode visitRootNullAtom(const uint8_t* value) noexcept;
	};
	
	inline TapeBuilder::TapeBuilder(JsonifierCore* masterParserNew) noexcept
		: nextStructural(masterParserNew->getStructuralIndexes()), masterParser{ masterParserNew }, tape{ masterParserNew->getTape() },
		  currentStringBufferLocation{ masterParserNew->getStringBuffer() } {};

	inline const uint8_t* TapeBuilder::peek() noexcept {
		return &this->masterParser->getStringView()[*this->nextStructural];
	}

	inline const uint8_t* TapeBuilder::advance() noexcept {
		return &this->masterParser->getStringView()[*this->nextStructural++];
	}

	inline size_t TapeBuilder::remainingLen() noexcept {
		return this->masterParser->getTapeLength() - *this->nextStructural;
	}

	inline bool TapeBuilder::atEof() noexcept {
		return this->nextStructural == &this->masterParser->getStructuralIndexes()[this->masterParser->getTapeLength() - 1];
	}

	inline bool TapeBuilder::atBeginning() noexcept {
		return this->nextStructural == this->masterParser->getStructuralIndexes();
	}

	inline uint8_t TapeBuilder::lastStructural() noexcept {
		return this->masterParser->getStringView()[this->masterParser->getStructuralIndexes()[this->masterParser->getTapeLength() - 1]];
	}

	inline ErrorCode TapeBuilder::visitEmptyObject() noexcept {
		return emptyContainer(TapeType::Start_Object, TapeType::End_Object);
	}

	inline ErrorCode TapeBuilder::visitEmptyArray() noexcept {
		return emptyContainer(TapeType::Start_Array, TapeType::End_Array);
	}

	inline ErrorCode TapeBuilder::visitDocumentStart() noexcept {
		return startContainer();
	}

	inline ErrorCode TapeBuilder::visitObjectStart() noexcept {
		return startContainer();
	}

	inline ErrorCode TapeBuilder::visitArrayStart() noexcept {
		return startContainer();
	}

	inline ErrorCode TapeBuilder::visitObjectEnd() noexcept {
		return endContainer(TapeType::Start_Object, TapeType::End_Object);
	}

	inline ErrorCode TapeBuilder::visitArrayEnd() noexcept {
		return endContainer(TapeType::Start_Array, TapeType::End_Array);
	}

	inline ErrorCode TapeBuilder::visitDocumentEnd() noexcept {
		constexpr uint32_t startTapeIndex = 0;
		this->tape.append(startTapeIndex, TapeType::Root);
		TapeWriter::write(this->masterParser->getTape()[startTapeIndex], nextTapeIndex(), TapeType::Root);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitKey(const uint8_t* key) noexcept {
		return visitString(key);
	}

	inline ErrorCode TapeBuilder::incrementCount() noexcept {
		this->masterParser->getOpenContainers()[this->depth].count++;
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitString(const uint8_t* value) noexcept {
		uint8_t* dst01 = onStartString();
		dst01 = StringParser::parseString(reinterpret_cast<const uint8_t*>(value) + 1ull, reinterpret_cast<uint8_t*>(dst01),
			(*this->nextStructural + 1ull) - (*this->nextStructural));
		if (dst01 == nullptr) {
			return ErrorCode::StringError;
		}
		return onEndString(dst01);
	}

	inline ErrorCode TapeBuilder::visitRootString(const uint8_t* value) noexcept {
		return visitString(value);
	}

	inline ErrorCode TapeBuilder::visitNumber(const uint8_t* value) noexcept {
		return NumberParser::parseNumber<TapeWriter>(value, this->tape);
	}

	inline ErrorCode TapeBuilder::visitRootNumber(const uint8_t* value) noexcept {
		std::unique_ptr<uint8_t[]> copy(new (std::nothrow) uint8_t[this->remainingLen() + 256]);
		if (copy.get() == nullptr) {
			return ErrorCode::MemAlloc;
		}
		std::memcpy(copy.get(), value, this->remainingLen());
		std::memset(copy.get() + this->remainingLen(), ' ', 256);
		return visitNumber(copy.get());
	}

	inline ErrorCode TapeBuilder::visitTrueAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidTrueAtom(value)) {
			return ErrorCode::TAtomError;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootTrueAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidTrueAtom(value)) {
			return ErrorCode::NAtomError;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitFalseAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidFalseAtom(value)) {
			return ErrorCode::FAtomError;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootFalseAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidFalseAtom(value)) {
			return ErrorCode::FAtomError;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitNullAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidNullAtom(value)) {
			return ErrorCode::NAtomError;
		}
		this->tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootNullAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidNullAtom(value)) {
			return ErrorCode::NAtomError;
		}
		this->tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline uint32_t TapeBuilder::nextTapeIndex() noexcept {
		return uint32_t(tape.nextTapeLocation - this->masterParser->getTape());
	}

	inline ErrorCode TapeBuilder::emptyContainer(TapeType start, TapeType end) noexcept {
		auto startIndex = nextTapeIndex();
		this->tape.append(startIndex + 2ull, start);
		this->tape.append(startIndex, end);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::startContainer() noexcept {
		this->masterParser->getOpenContainers()[this->depth].tapeIndex = nextTapeIndex();
		this->masterParser->getOpenContainers()[this->depth].count = 0;
		this->tape.skip();
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::endContainer(TapeType start, TapeType end) noexcept {
		const uint32_t startTapeIndex = this->masterParser->getOpenContainers()[this->depth].tapeIndex;
		this->tape.append(startTapeIndex, end);
		const uint32_t count = this->masterParser->getOpenContainers()[this->depth].count;
		const uint32_t cntsat = count > 0xFFFFFF ? 0xFFFFFF : count;
		TapeWriter::write(this->masterParser->getTape()[startTapeIndex], nextTapeIndex() | (uint64_t(cntsat) << 32), start);
		return ErrorCode::Success;
	}

	inline uint8_t* TapeBuilder::onStartString() noexcept {
		this->tape.append(currentStringBufferLocation - this->masterParser->getStringBuffer(), TapeType::String);
		return this->currentStringBufferLocation + sizeof(uint32_t);
	}

	inline ErrorCode TapeBuilder::onEndString(uint8_t* dst) noexcept {
		uint32_t strLength = uint32_t(dst - (this->currentStringBufferLocation + sizeof(uint32_t)));
		memcpy(this->currentStringBufferLocation, &strLength, sizeof(uint32_t));
		*dst = 0;
		this->currentStringBufferLocation = dst + 1;
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::walkDocument() {
		if (this->atEof()) {
			return ErrorCode::Empty;
		}
		this->visitDocumentStart();
		auto value = this->advance();

		switch (*value) {
			case '{':
				if (*this->peek() == '}') {
					this->advance();
					this->visitEmptyObject();
					break;
				}
				goto Object_Begin;
			case '[':
				if (*this->peek() == ']') {
					this->advance();
					this->visitEmptyArray();
					break;
				}
				goto Array_Begin;
			default:
				this->visitRootPrimitive(value);
				break;
		}
		goto Document_End;

	Object_Begin : {
		this->depth++;
		if (this->depth >= masterParser->getMaxDepth()) {
			return ErrorCode::DepthError;
		}
		this->masterParser->getIsArray()[this->depth] = false;
		this->visitObjectStart();
		auto key = this->advance();
		if (*key != '"') {
			return ErrorCode::TapeError;
		}
		this->visitKey(key);
		this->incrementCount();
	}

	Object_Field : {
		auto newValue = *this->advance();
		if (newValue != ':') {
			return ErrorCode::TapeError;
		}
		auto value = this->advance();
		switch (*value) {
			case '{':
				if (*this->peek() == '}') {
					this->advance();
					this->visitEmptyObject();
					break;
				}
				goto Object_Begin;
			case '[':
				if (*this->peek() == ']') {
					this->advance();
					this->visitEmptyArray();
					break;
				}
				goto Array_Begin;
			default:
				if (auto resultCode = this->visitPrimitive(value); resultCode != ErrorCode::Success) {
					return resultCode;
				}
				break;
		}
	}

	Object_Continue : {
		auto newValue = *this->advance();
		switch (newValue) {
			case ',':
				this->incrementCount();
				{
					auto key = this->advance();
					if (*key != '"') {
						return ErrorCode::TapeError;
					}
					this->visitKey(key);
				}
				goto Object_Field;
			case '}':
				this->visitObjectEnd();
				goto Scope_End;
			default:
				return ErrorCode::TapeError;
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
			return ErrorCode::DepthError;
		}
		this->masterParser->getIsArray()[this->depth] = true;
		this->visitArrayStart();
		this->incrementCount();
	}

	Array_Value : {
		auto value = this->advance();
		switch (*value) {
			case '{':
				if (*this->peek() == '}') {
					this->advance();
					this->visitEmptyObject();
					break;
				}

				goto Object_Begin;
			case '[':
				if (*this->peek() == ']') {
					this->advance();
					this->visitEmptyArray();
					break;
				}
				goto Array_Begin;
			default:
				if (auto resultCode = this->visitPrimitive(value); resultCode != ErrorCode::Success) {
					return resultCode;
				}
				break;
		}
	}

	Array_Continue : {
		auto newValue = *this->advance();
		switch (newValue) {
			case ',':
				this->incrementCount();
				goto Array_Value;
			case ']':
				this->visitArrayEnd();
				goto Scope_End;
			default:
				return ErrorCode::TapeError;
		}
	}

	Document_End : {
		this->visitDocumentEnd();

		auto nextStructuralIndex = uint32_t(this->nextStructural - &this->masterParser->getStructuralIndexes()[0]);

		if (nextStructuralIndex != this->masterParser->getTapeLength()) {
			return ErrorCode::TapeError;
		}
	}
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootPrimitive(const uint8_t* value) {
		switch (*value) {
			case '"':
				return this->visitRootString(value);
			case 't':
				return this->visitRootTrueAtom(value);
			case 'f':
				return this->visitRootFalseAtom(value);
			case 'n':
				return this->visitRootNullAtom(value);
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
				return this->visitRootNumber(value);
			default:
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
	}

	inline ErrorCode TapeBuilder::visitPrimitive(const uint8_t* value) {
		switch (*value) {
			case '"':
				return this->visitString(value);
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
				throw JsonifierException{ "Sorry, but you've encountered the following error: " +
					std::string{ static_cast<EnumStringConverter>(ErrorCode::TapeError) } +
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
	}

	Document JsonifierCore::parseJson(std::string& string) {
		this->generateJsonEvents(reinterpret_cast<uint8_t*>(string.data()), string.size());
		TapeBuilder tapeBuilder{ this };
		auto errorCode = tapeBuilder.walkDocument();
		this->getTapeLength() = (this->getTape()[0] & JSON_VALUE_MASK);
		return this->getDocument();
	}

	inline Field JsonValueBase::parseJsonField(const char* fieldKey) noexcept {
		return this->findField(fieldKey);
	}

	inline Object JsonValueBase::parseJsonObject() noexcept {
		this->assertAtObjectStart();
		if (*this->peek() == '{') {
			//std::cout << "PEEKING THE JSONOBJECT:!" << std::endl;
			this->error = ErrorCode::Success;
			return Object{ this->getCurrentIterator() };
		} else {
			return Object{ this->getCurrentIterator() };
		}
	}

	inline Array JsonValueBase::parseJsonArray() noexcept {
		//std::cout << "PEEKING THE JSONARRAY!" << this->peek() << std::endl;
		this->assertAtArrayStart();
		if (*this->peek() == '[') {
			//			std::cout << "PEEKING THE JSONARRAY!" << std::endl;
			this->error = ErrorCode::Success;
			return Array{ this->getCurrentIterator() };

		} else {
			return Array{};
		}
	}

	inline Field JsonValueBase::operator[](const char* keyNew) noexcept {
		return this->findField(keyNew);
	}

	template<> inline JsonifierResult<Array> JsonValueBase::get<Array>() noexcept {
		return { this->parseJsonArray(), std::move(this->error) };
	}

	template<> inline JsonifierResult<Field> JsonValueBase::get<Field>(const char*keyNew) noexcept {
		return { this->parseJsonField(keyNew), std::move(this->error) };
	}

	template<> inline JsonifierResult<Object> JsonValueBase::get<Object>() noexcept {
		return { this->get_object(), std::move(this->error) };
	}

	template<> inline JsonifierResult<const char*> JsonValueBase::get<const char*>() noexcept {
		return { this->get_string().data(), std::move(this->error) };
	}

	template<> inline JsonifierResult<std::string_view> JsonValueBase::get<std::string_view>() noexcept {
		return { this->get_string(), std::move(this->error) };
	}

	template<> inline JsonifierResult<int64_t> JsonValueBase::get<int64_t>() noexcept {
		return { this->get_int64(), std::move(this->error) };
	}

	template<> inline JsonifierResult<uint64_t> JsonValueBase::get<uint64_t>() noexcept {
		return { this->get_uint64(), std::move(this->error) };
	}

	template<> inline JsonifierResult<double> JsonValueBase::get<double>() noexcept {
		return { this->get_double(), std::move(this->error) };
	}

	template<> inline JsonifierResult<bool> JsonValueBase::get<bool>() noexcept {
		return { this->get_bool(), std::move(this->error) };
	}

	template<> inline ErrorCode JsonValueBase::get<Object>(Object& value) noexcept {
		value = this->get_object();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<Array>(Array& value) noexcept {
		value = this->parseJsonArray();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<double>(double& value) noexcept {
		value = this->get_double();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<bool>(bool& value) noexcept {
		value = this->get_bool();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<int64_t>(int64_t& value) noexcept {
		value = this->get_int64();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<uint64_t>(uint64_t& value) noexcept {
		value = this->get_uint64();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<std::string>(std::string& value) noexcept {
		value = this->get_string();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<std::string_view>(std::string_view& value) noexcept {
		value = this->get_string();
		return std::move(this->error);
	}

	inline Document::Document() noexcept : JsonValueBase{} {};

	inline Document::Document(JsonifierCore* value) noexcept : JsonValueBase{ value } {};

	inline JsonValueBase::JsonValueBase(JsonifierCore* other) noexcept
		: root(other->getStructuralIndexes()), stringView(other->getStringView()), stringBufferLocation(other->getStringBuffer()), position(other->getStructuralIndexes()) {
		this->parser = other;
	}

	inline JsonValueBase& JsonValueBase::operator=(const JsonValueBase& other) noexcept {
		this->stringBufferLocation = other.stringBufferLocation;
		this->currentIndex = other.currentIndex;
		this->stringView = other.stringView;
		this->position = other.position;
		this->parser = other.parser;
		this->root = other.root;
		return *this;
	}

	inline JsonValueBase::JsonValueBase(const JsonValueBase& other) noexcept {
		*this = other;
	}

	inline JsonValueBase& JsonValueBase::operator=(JsonValueBase && other) noexcept {
		this->stringBufferLocation = other.stringBufferLocation;
		this->currentIndex = other.currentIndex;
		this->stringView = other.stringView;
		this->position = other.position;
		this->parser = other.parser;
		this->root = other.root;
		return *this;
	}

	inline JsonValueBase::JsonValueBase(JsonValueBase&& other) noexcept {
		*this = std::move(other);
	}

	inline std::string_view JsonValueBase::parseJsonString() noexcept {
		assert(*this->peek() == '"');
		std::string_view returnValue{};
		if (*this->peek() == '"') {
			size_t stringLength{};
			std::memcpy(&stringLength, this->parser->getStringBuffer() + (uint32_t(*(this->parser->getTape() + this->currentIndex) & JSON_VALUE_MASK)),
				sizeof(uint32_t));
			returnValue = std::string_view{ reinterpret_cast<const char*>(this->parser->getStringBuffer() +
												(uint32_t(*(this->parser->getTape() + this->currentIndex) & JSON_VALUE_MASK)) + sizeof(uint32_t)),
				stringLength };
			this->error = ErrorCode::Success;
		} else {
			this->error = ErrorCode::ParseError;
		}
		return returnValue;
	}

	inline double JsonValueBase::parseJsonFloat() noexcept {
		assert(*this->peek() == 'd');
		double returnValue{};
		if (*this->peek() == 'd') {
			this->advance();
			this->error = ErrorCode::Success;
			std::memcpy(&returnValue, &this->parser->getTape()[this->currentIndex], sizeof(returnValue));
			this->advance();
		} else {
			this->error = ErrorCode::ParseError;
		}
		return returnValue;
	}

	inline uint64_t JsonValueBase::parseJsonUint() noexcept {
		assert(*this->peek() == 'u');
		uint64_t returnValue{};
		if (*this->peek() == 'u') {
			this->advance();
			this->error = ErrorCode::Success;
			std::memcpy(&returnValue, &this->parser->getTape()[this->currentIndex], sizeof(returnValue));
			this->advance();
		} else {
			this->error = ErrorCode::ParseError;
		}
		return returnValue;
	}

	inline int64_t JsonValueBase::parseJsonInt() noexcept {
		assert(*this->peek() == 'l');
		int64_t returnValue{};
		if (*this->peek() == 'l') {
			this->advance();
			this->error = ErrorCode::Success;
			std::memcpy(&returnValue, &this->parser->getTape()[this->currentIndex], sizeof(returnValue));
			this->advance();
		} else {
			this->error = ErrorCode::ParseError;
		}
		return returnValue;
	}

	inline bool JsonValueBase::parseJsonBool() noexcept {
		assert(*this->peek() == 'f' || *this->peek() == 't');
		if (*this->peek() == 'f') {
			this->advance();
			this->error = ErrorCode::Success;
			return false;
		} else if (*this->peek() == 't') {
			this->advance();
			this->error = ErrorCode::Success;
			return true;
		} else {
			this->error = ErrorCode::ParseError;
			return false;
		}
	}

	inline nullptr_t JsonValueBase::parseJsonNull() noexcept {
		assert(*this->peek() == 'n');
		if (*this->peek() == 'n') {
			this->advance();
			this->error = ErrorCode::Success;
			return nullptr_t{};
		} else {
			this->error = ErrorCode::ParseError;
			return nullptr_t{};
		}
	}

	inline uint64_t* JsonValueBase::advance() noexcept {
		auto returnValue = &this->parser->getTape()[this->currentIndex];
		++this->currentIndex;
		return returnValue;
	}

	inline void JsonValueBase::setPosition(int32_t delta) noexcept {
		auto newValue = static_cast<int32_t>(this->currentIndex);
		newValue += delta;
		this->currentIndex += static_cast<uint32_t>(newValue);
	}

	inline size_t JsonValueBase::getOffset() noexcept {
		return this->currentIndex + 1;
	}

	inline JsonValueBase& JsonValueBase::getCurrentIterator() noexcept {
		return *this;
	}

	inline void JsonValueBase::asserAtFieldStart(size_t amountToOffset) noexcept {
		assert(*this->peek() == '"');
	}

	inline void JsonValueBase::assertAtObjectStart(size_t amountToOffset) noexcept {
		assert(*this->peek() == '{');
	}

	inline void JsonValueBase::assertAtArrayStart(size_t amountToOffset) noexcept {
		assert(*this->peek() == '[');
	}

	inline void JsonValueBase::assertAtStringStart(size_t amountToOffset) noexcept {
		assert(*this->peek() == '"');
	}

	inline size_t JsonValueBase::getCurrentCount() noexcept {
		return uint32_t((*this->parser->getTape() & JSON_VALUE_MASK)) - (this->parser->getTape() - this->parser->getTape()) - 2;
	}

	inline uint8_t JsonValueBase::getRootKey() noexcept {
		return (*this->parser->getTape() >> 56);
	}

	inline size_t JsonValueBase::size() noexcept {
		switch (this->getRootKey()) {
			case 'r': {
				[[fallthrough]];
			}
			case '[': {
				[[fallthrough]];
			}
			case '{': {
				return (((*(this->parser->getTape()) & JSON_VALUE_MASK) >> 32) & JSON_COUNT_MASK);
			}
			case '"': {
				size_t stringLength{};
				std::memcpy(&stringLength, this->parser->getStringBuffer() + ((*this->parser->getTape()) & JSON_VALUE_MASK), sizeof(uint32_t));
				return stringLength;
			}
			default: {
				return 1;
			}
		}
	}

	inline JsonType JsonValueBase::type() noexcept {
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

	inline Field JsonValueBase::findField(const char* keyNew) noexcept {
		std::string_view newString{};
		if (*this->peek() == '"') {
			this->error = ErrorCode::Success;
			newString = this->parseJsonString();
		}
		//std::cout << "NEW STRING: " << newString << std::endl;
		//std::cout << "KEY NEW: " << keyNew << std::endl;
		while (newString != keyNew) {
			//			std::cout << "NEW STRING: " << newString << std::endl;
			if (*this->peek() == '"') {
				
				this->error = ErrorCode::Success;
				newString = this->parseJsonString();
				//WERE				std::cout << "NEW STRING: " << newString << std::endl;
			} else {
				this->advance();
			}
		}
		this->asserAtFieldStart(0);
		return Field{ std::move(newString), this->getCurrentIterator() };
	}

	JsonifierCore* JsonValueBase::getCore() noexcept {
		return this->parser;
	}

	inline uint64_t JsonValueBase::get_uint64() noexcept {
		auto result = NumberParser::parseUnsigned(peek_non_root_scalar("uint64"));
		advance_non_root_scalar("uint64");
		return result;
	}

	inline const uint8_t* JsonValueBase::peek_non_root_scalar(const char* type) noexcept {
		if (!is_at_start()) {
			return peek_start();
		}

		assert_at_non_root_start();
		return this->peek();
	}

	inline const uint8_t* JsonValueBase::peek() const noexcept {
		return &stringView[*this->position];
	}

	inline int64_t JsonValueBase::get_int64() noexcept {
		auto result = NumberParser::parseInteger(peek_non_root_scalar("int64"));
		advance_non_root_scalar("int64");
		return result;
	}

	inline bool JsonValueBase::get_bool() noexcept {
		auto result = parse_bool(peek_non_root_scalar("bool"));
		advance_non_root_scalar("bool");
		return result;
	}

	inline bool JsonValueBase::parse_bool(const uint8_t* json) const noexcept {
		auto not_true = StringParser::str4ncmp(json, "true");
		auto not_false = StringParser::str4ncmp(json, "fals") | (json[4] ^ 'e');
		bool error = (not_true && not_false) || NumberParser::isNotStructuralOrWhitespace(json[not_true ? 5 : 4]);
		if (error) {
			return false;
		}
		return bool{ !not_true };
	}

	inline void JsonValueBase::advance_non_root_scalar(const char* type) noexcept {
		if (!is_at_start()) {
			return;
		}

		assert_at_non_root_start();
		this->return_current_and_advance();
		this->ascend_to(depth() - 1);
	}

	inline void JsonValueBase::assert_at_non_root_start() const noexcept {
		assert_at_start();
		assert(currentDepth > 1);
	}
		
	inline void JsonValueBase::assert_at_next() const noexcept {
		assert(position > root);
	}

	inline void JsonValueBase::assert_at_start() const noexcept {
		assert(position == root);
	}

	inline const uint8_t* JsonValueBase::return_current_and_advance() noexcept {
		return &stringView[*(position++)];
	}

	inline bool JsonValueBase::is_at_start() const noexcept {
		return positionVal() == start_position();
	}

	inline uint32_t* JsonValueBase::start_position() const noexcept {
		return root;
	}

	inline const uint8_t* JsonValueBase::peek_start() const noexcept {
		return &this->stringView[*start_position()];
	}

	inline uint32_t* JsonValueBase::positionVal() const noexcept {
		return this->position;
	}

	inline size_t JsonValueBase::depth() const noexcept {
		return currentDepth;
	}

	inline void JsonValueBase::ascend_to(size_t parent_depth) noexcept {
		assert(parent_depth >= 0 && parent_depth < INT32_MAX - 1);
		assert(currentDepth == parent_depth + 1);
		currentDepth = parent_depth;
	}

	inline double JsonValueBase::get_double() noexcept {
		auto result = NumberParser::parseDouble(peek_non_root_scalar("double"));
		advance_non_root_scalar("double");
		return result;
	}

	inline Array::Array(const JsonValueBase& iterNew) noexcept {
		this->iter = iterNew;
	}

	inline std::string_view JsonValueBase::get_string() noexcept {
		return get_raw_json_string().unescape(json_iter());
	}

	inline RawJsonString JsonValueBase::get_raw_json_string() noexcept {
		auto json = peek_scalar("string");
		if (*json != '"') {
			return RawJsonString{};
		}
		advance_scalar("string");
		return RawJsonString(json + 1);
	}

	inline std::string_view RawJsonString::unescape(JsonValueBase& iter) const noexcept {
		auto newValue = ( uint8_t* )(iter.stringView);
		return iter.unescape(*this, newValue);
	}

	inline std::string_view JsonValueBase::unescape(RawJsonString in, uint8_t*& dst) const noexcept {
		uint8_t* end = StringParser::parseString(in.buf, dst);
		if (!end) {
			return "";
		}
		std::string_view result(reinterpret_cast<const char*>(dst), end - dst);
		dst = end;
		return result;
	}

	inline const uint8_t* JsonValueBase::peek_scalar(const char* type) noexcept {
		if (!is_at_start()) {
			return peek_start();
		}

		assert_at_start();
		return this->peek();
	}

	inline void JsonValueBase::advance_scalar(const char* type) noexcept {
		if (!is_at_start()) {
			return;
		}

		assert_at_start();
		this->return_current_and_advance();
		this->ascend_to(depth() - 1);
	}

	inline JsonValueBase& JsonValueBase::json_iter() noexcept {
		return *this;
	}

	inline RawJsonString::RawJsonString(const uint8_t* bufNew) noexcept {
		this->buf = bufNew;
	}
	
	inline bool JsonValueBase::start_object() noexcept {
		start_container('{', "Not an object", "object");
		return started_object();
	}

	inline bool JsonValueBase::start_root_object() noexcept {
		start_container('{', "Not an object", "object");
		return started_root_object();
	}

	inline Object JsonValueBase::get_object() & noexcept {
		auto value = get_root_value_iterator();
		return Object::start_root(value);
	}

	inline JsonValueBase JsonValueBase::resume_value_iterator() noexcept {
		return JsonValueBase(*this);
	}
	inline JsonValueBase JsonValueBase::get_root_value_iterator() noexcept {
		return resume_value_iterator();
	}

	inline uint32_t* JsonValueBase::root_position() const noexcept {
		return root;
	}

	inline bool JsonValueBase::started_root_object() noexcept {
		return started_object();
	}

	inline bool JsonValueBase::started_object() noexcept {
		assert_at_container_start();
		if (*this->peek() == '}') {
			this->return_current_and_advance();
			end_container();
			return false;
		}
		return true;
	}
	
	inline ErrorCode JsonValueBase::start_container(uint8_t start_char, const char* incorrect_type_message, const char* type) noexcept {
		const uint8_t* json;
		if (!is_at_start()) {
			json = peek_start();
			if (*json != start_char) {
				return incorrect_type_error(incorrect_type_message);
			}
		} else {
			assert_at_start();
			json = this->peek();
			if (*json != start_char) {
				return incorrect_type_error(incorrect_type_message);
			}
			this->return_current_and_advance();
		}


		return ErrorCode::Success;
	}

	inline void JsonValueBase::assert_at_container_start() const noexcept {
		assert(position == root+ 1);
	}

	inline ErrorCode JsonValueBase::end_container() noexcept {
		this->ascend_to(depth() - 1);
		return ErrorCode::Success;
	}

	inline ErrorCode JsonValueBase::incorrect_type_error(const char* message) const noexcept {
		return ErrorCode::Incorrect_Type;
	}

	inline bool JsonValueBase::reset_object() noexcept {
		move_at_container_start();
		return started_object();
	}

	inline ErrorCode JsonValueBase::getError() noexcept {
		return this->error;
	}

	inline size_t Object::count_fields()  noexcept {
		size_t count{ 0 };
		// Important: we do not consume any of the values.
		for ( auto v: *this) {
			count++;
		}
		// The above loop will always succeed, but we want to report errors.
		if (getError()!=ErrorCode::Success) {
			return static_cast<size_t>(getError());
		}
		// We need to move back at the start because we expect users to iterate through
		// the object after counting the number of elements.
		this->reset_object();
		return count;
	}

	inline void JsonValueBase::move_at_container_start() noexcept {
		this->currentDepth =currentDepth;
		this->set_position(start_position() + 1);
	}

	inline void JsonValueBase::set_position(uint32_t* target_position) noexcept {
		position = target_position;
	}

	inline void JsonValueBase::abandon() noexcept {
		parser = nullptr;
		currentDepth = 0;
	}
	inline ErrorCode JsonValueBase::skip_child(size_t parent_depth) noexcept {
		if (depth() <= parent_depth) {
			return ErrorCode::Success;
		}
		switch (*return_current_and_advance()) {
			case '[':
			case '{':
			case ':':
				break;
			case ',':
				break;
			case ']':
			case '}':
				--currentDepth;
				if (depth() <= parent_depth) {
					return ErrorCode::Success;
				}
				break;
			case '"':
				if (*peek() == ':') {
					return_current_and_advance();
					break;
				}
				[[fallthrough]];
			default:
				--currentDepth;
				if (depth() <= parent_depth) {
					return ErrorCode::Success;
				}
				break;
		}

		while (position < end_position()) {
			switch (*return_current_and_advance()) {
				case '[':
				case '{':
					++currentDepth;
					break;
				case ']':
				case '}':
					--currentDepth;
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

	inline uint32_t* JsonValueBase::end_position() const noexcept {
		size_t n_structural_indexes{ parser->getTapeLength() };
		return &parser->getStructuralIndexes()[n_structural_indexes];
	}

	inline ErrorCode JsonValueBase::field_value() noexcept {
		assert_at_next();

		if (*this->return_current_and_advance() != ':') {
			return ErrorCode::TapeError;
		}
		this->descend_to(depth() + 1);
		return ErrorCode::Success;
	}
		
	inline void JsonValueBase::descend_to(size_t child_depth) noexcept {
		assert(child_depth >= 1 && child_depth < INT32_MAX);
		assert(currentDepth == child_depth - 1);
		currentDepth = child_depth;
	}

	inline RawJsonString JsonValueBase::field_key() noexcept {
		assert_at_next();

		const uint8_t* key = this->return_current_and_advance();
		if (*(key++) != '"') {
			return RawJsonString{};
		}
		return RawJsonString{ key };
	}

	inline JsonValueBase JsonValueBase::child() noexcept {
		assert_at_child();
		++this->currentDepth;
		return { *this };
	}

	inline const char* RawJsonString::raw() const noexcept {
		return reinterpret_cast<const char*>(buf);
	}

	inline bool JsonValueBase::is_open() const noexcept {
		return depth() >= depth();
	}

	inline bool JsonValueBase::has_next_field() noexcept {
		assert_at_next();

		// It's illegal to call this unless there are more tokens: anything that ends in } or ] is
		// obligated to verify there are more tokens if they are not the top level.
		switch (*this->return_current_and_advance()) {
			case '}':
				end_container();
				return false;
			case ',':
				return true;
			default:
				return false;
		}
	}

	inline void JsonValueBase::assert_at_child() const noexcept {
		assert(position > start_position());
		assert(currentDepth > 0);
	}

};
