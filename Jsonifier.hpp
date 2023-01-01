#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)

#include "NumberParsingUtils.hpp"
#include "StringParsingUtils.hpp"
#include "Object.hpp"
#include "Simd.hpp"
#include <iterator>

namespace Jsonifier {

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

	inline bool dumpRawTape(uint64_t* tape, const uint8_t* stringBuffer) noexcept {
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
	class ValueIterator;
	class Object;
	class Array;
	class Field;

	

	class ObjectIterator;

	class Field : protected std::pair<std::string_view, ValueIterator> {
	  public:
		

		static inline Field start(ValueIterator& parent_iter) noexcept {
			std::string_view key{};
			key = parent_iter.field_key();
			std::cout << "CURRENT KEY: " << key << std::endl;
			parent_iter.field_value();
			return Field::start(parent_iter, RawJsonString{ reinterpret_cast<const uint8_t*>(key.data()) });
		}

		static inline Field start(ValueIterator& parent_iter, RawJsonString key) noexcept {
			return Field(key, parent_iter.child());
		}

		inline ErrorCode getError() {
			return this->second.getError();
		}

		inline ObjectIterator end() noexcept;

		inline ObjectIterator begin() noexcept;


		inline std::string_view getKey() {
			return this->first;
		}

		inline Field() noexcept = default;

		inline Field(RawJsonString& key, ValueIterator&& value) : std::pair<std::string_view, ValueIterator>{ std::move(key.raw()), Object{} } {};

		inline Field(std::string_view&& key, ValueIterator& value) : std::pair<std::string_view, ValueIterator>{ std::move(key), Object{} } {};
	};

	inline ObjectIterator Field::end() noexcept {
		return ObjectIterator{ this->second };
	}

	inline ObjectIterator Field::begin() noexcept {
		return ObjectIterator{ this->second };
	}

	class ArrayIterator {
	  public:
		inline ArrayIterator() noexcept = default;
		inline Object operator*() noexcept;
		inline bool operator==(const ArrayIterator&) const noexcept;
		inline bool operator!=(const ArrayIterator&) const noexcept;
		inline ArrayIterator& operator++() noexcept;

	  private:
		ValueIterator iter{};

		inline ArrayIterator(const ValueIterator& iter) noexcept;

		friend class Array;
		friend struct JsonifierResult<ArrayIterator>;
	};

	class field;

	class Array {
	  public:
		inline Array() noexcept = default;
		inline ArrayIterator begin() noexcept;
		inline ArrayIterator end() noexcept;
		inline size_t count_elements() & noexcept;
		inline bool is_empty() & noexcept;
		inline bool reset() & noexcept;
		inline Object at_pointer(std::string_view json_pointer) noexcept;
		inline std::string_view raw_json() noexcept;
		inline Object at(size_t index) noexcept;
		inline Array(const ValueIterator& iter) noexcept;
		static inline Array start_root(ValueIterator& iter) noexcept;
		static inline Array started(ValueIterator& iter) noexcept;

	  protected:
		inline ErrorCode consume() noexcept;
		static inline Array start(ValueIterator& iter) noexcept;

		ValueIterator iter{};

		friend class ArrayIterator;
	};

	struct OpenContainer {
		uint32_t tapeIndex{};
		uint32_t count{};
	};

	class JsonifierCore;
	class Object;

	class Document {
	  public:
		inline Document(JsonIterator&& _iter) noexcept : iter{ std::forward<JsonIterator>(_iter) } {
		}

		inline Document start(JsonIterator&& iter) noexcept {
			return Document(std::forward<JsonIterator>(iter));
		}

		inline ValueIterator get_root_value_iterator() noexcept {
			return resume_value_iterator();
		}

		inline Object get_object() & noexcept {
			auto value = get_root_value_iterator();
			return Object::start_root(value);
		}

		inline Object start_or_resume_object() noexcept {
			auto newValue = this->iter.at_root();
			std::cout << "ARE WE AT ROOT?: " << std::boolalpha << newValue << std::endl;
			if (newValue) {
				return get_object();
			} else {
				return Object::resume(resume_value_iterator());
			}
		}

		inline ValueIterator resume_value_iterator() noexcept {
			return ValueIterator(&iter, 1, iter.root_position());
		}

		inline Object find_field(std::string_view key) & noexcept {
			return start_or_resume_object().find_field(key);
		}

		inline Object find_field(const char* key) & noexcept {
			return start_or_resume_object().find_field(key);
		}

		inline Object find_field_unordered(std::string_view key) & noexcept {
			return start_or_resume_object().find_field_unordered(key);
		}

		inline Object find_field_unordered(const char* key) & noexcept {
			return start_or_resume_object().find_field_unordered(key);
		}

		inline Object operator[](std::string_view key) & noexcept {
			return start_or_resume_object()[key];
		}

		inline Object operator[](const char* key) & noexcept {
			return start_or_resume_object()[key];
		}

		inline void rewind() noexcept {
			iter.rewind();
		}

		inline std::string to_debug_string() noexcept {
			return iter.to_string();
		}

		inline const char* current_location() noexcept {
			return iter.current_location();
		}

		inline int32_t current_depth() const noexcept {
			return iter.depth();
		}

		inline bool is_alive() noexcept {
			return iter.is_alive();
		}

		inline Object get_value() noexcept {
			iter.assert_at_document_depth();
			switch (*iter.peek()) {
				case '[':
				case '{':
					return Object(get_root_value_iterator());
				default:
					return Object{};
			}
		}

		inline Array get_array() & noexcept {
			auto value = get_root_value_iterator();
			return Array::start_root(value);
		}

		inline uint64_t get_uint64() noexcept {
			return get_root_value_iterator().get_root_uint64();
		}

		inline uint64_t get_uint64_in_string() noexcept {
			return get_root_value_iterator().get_root_uint64_in_string();
		}

		inline int64_t get_int64() noexcept {
			return get_root_value_iterator().get_root_int64();
		}

		inline int64_t get_int64_in_string() noexcept {
			return get_root_value_iterator().get_root_int64_in_string();
		}

		inline double get_double() noexcept {
			return get_root_value_iterator().get_root_double();
		}

		inline double get_double_in_string() noexcept {
			return get_root_value_iterator().get_root_double_in_string();
		}

		inline std::string_view get_string() noexcept {
			return get_root_value_iterator().get_root_string();
		}

		inline RawJsonString get_raw_json_string() noexcept {
			return get_root_value_iterator().get_root_raw_json_string();
		}

		inline bool get_bool() noexcept {
			return get_root_value_iterator().get_root_bool();
		}

		inline bool is_null() noexcept {
			return get_root_value_iterator().is_root_null();
		}

		template<typename OTy> inline OTy get() & noexcept;

		template<typename OTy> inline OTy get() && noexcept;

		template<> inline Array get() & noexcept {
			return get_array();
		}

		template<> inline Object get() & noexcept {
			return get_object();
		}

		template<> inline RawJsonString get() & noexcept {
			return get_raw_json_string();
		}

		template<> inline std::string_view get() & noexcept {
			return get_string();
		}

		template<> inline double get() & noexcept {
			return get_double();
		}

		template<> inline uint64_t get() & noexcept {
			return get_uint64();
		}

		template<> inline int64_t get() & noexcept {
			return get_int64();
		}

		template<> inline bool get() & noexcept {
			return get_bool();
		}

		template<> inline RawJsonString get() && noexcept {
			return get_raw_json_string();
		}

		template<> inline std::string_view get() && noexcept {
			return get_string();
		}

		template<> inline double get() && noexcept {
			return std::forward<Document>(*this).get_double();
		}

		template<> inline uint64_t get() && noexcept {
			return std::forward<Document>(*this).get_uint64();
		}

		template<> inline int64_t get() && noexcept {
			return std::forward<Document>(*this).get_int64();
		}

		template<> inline bool get() && noexcept {
			return std::forward<Document>(*this).get_bool();
		}

		template<> inline Object get() && noexcept {
			return get_value();
		}

		template<typename T> inline ErrorCode get(T& out) & noexcept {
			return get<T>().get(out);
		}

		template<typename T> inline ErrorCode get(T& out) && noexcept {
			return std::forward<Document>(*this).get<T>().get(out);
		}

		inline size_t count_elements() & noexcept {
			auto a = get_array();
			size_t answer = a.count_elements();
			if (answer != 0) {
				rewind();
			}
			return answer;
		}

		inline size_t count_fields() & noexcept {
			auto a = get_object();
			size_t answer = a.count_fields();
			if (answer != 0) {
				rewind();
			}
			return answer;
		}

		inline Object at(size_t index) & noexcept {
			auto a = get_array();
			return a.at(index);
		}

		inline ArrayIterator begin() & noexcept {
			return get_array().begin();
		}

		inline ArrayIterator end() & noexcept {
			return {};
		}

	  protected:
		JsonIterator iter;
	};

	inline int64_t totalTimePassed{};
	inline int64_t totalTimePassed02{};
	inline int64_t iterationCount{};

	class JsonifierCore {
	  public:
		inline Document getDocument() {
			Document returnValue{ JsonIterator{ this->getStringBuffer(), this } };
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
		auto Object = this->advance();
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
		auto Object = this->advance();
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
		this->getTapeLength() = (this->getTape()[0] & JSON_VALUE_MASK);
		dumpRawTape(this->getTape(), this->getStringBuffer());
		return this->getDocument();
	}

	inline Field ValueIterator::parseJsonField(const char* fieldKey) noexcept {
		return this->findField(fieldKey);
	}

	inline Object ValueIterator::parseJsonObject() noexcept {
		this->assertAtObjectStart();
		if (*this->jsonIterator->peek() == '{') {
			//std::cout << "PEEKING THE JSONOBJECT:!" << std::endl;
			this->error = ErrorCode::Success;
			return Object{ this->getCurrentIterator() };
		} else {
			return Object{ this->getCurrentIterator() };
		}
	}

	inline Array ValueIterator::parseJsonArray() noexcept {
		//std::cout << "PEEKING THE JSONARRAY!" << this->peek() << std::endl;
		this->assertAtArrayStart();
		if (*this->jsonIterator->peek() == '[') {
			//			std::cout << "PEEKING THE JSONARRAY!" << std::endl;
			this->error = ErrorCode::Success;
			return Array{ this->getCurrentIterator() };

		} else {
			return Array{};
		}
	}

	template<> inline JsonifierResult<Array> ValueIterator::get<Array>() noexcept {
		return { this->parseJsonArray(), std::move(this->error) };
	}

	template<> inline JsonifierResult<Field> ValueIterator::get<Field>(const char*keyNew) noexcept {
		return { this->parseJsonField(keyNew), std::move(this->error) };
	}

	template<> inline JsonifierResult<Object> ValueIterator::get<Object>() noexcept {
		return { this->get_object(), std::move(this->error) };
	}

	template<> inline JsonifierResult<const char*> ValueIterator::get<const char*>() noexcept {
		return { this->get_string().data(), std::move(this->error) };
	}

	template<> inline JsonifierResult<std::string_view> ValueIterator::get<std::string_view>() noexcept {
		return { this->get_string(), std::move(this->error) };
	}

	template<> inline JsonifierResult<int64_t> ValueIterator::get<int64_t>() noexcept {
		return { this->get_int64(), std::move(this->error) };
	}

	template<> inline JsonifierResult<uint64_t> ValueIterator::get<uint64_t>() noexcept {
		return { this->get_uint64(), std::move(this->error) };
	}

	template<> inline JsonifierResult<double> ValueIterator::get<double>() noexcept {
		return { this->get_double(), std::move(this->error) };
	}

	template<> inline JsonifierResult<bool> ValueIterator::get<bool>() noexcept {
		return { this->get_bool(), std::move(this->error) };
	}

	template<> inline ErrorCode ValueIterator::get<Object>(Object& value) noexcept {
		value = this->get_object();
		return std::move(this->error);
	}

	template<> inline ErrorCode ValueIterator::get<Array>(Array& value) noexcept {
		value = this->parseJsonArray();
		return std::move(this->error);
	}

	template<> inline ErrorCode ValueIterator::get<double>(double& value) noexcept {
		value = this->get_double();
		return std::move(this->error);
	}

	template<> inline ErrorCode ValueIterator::get<bool>(bool& value) noexcept {
		value = this->get_bool();
		return std::move(this->error);
	}

	template<> inline ErrorCode ValueIterator::get<int64_t>(int64_t& value) noexcept {
		value = this->get_int64();
		return std::move(this->error);
	}

	template<> inline ErrorCode ValueIterator::get<uint64_t>(uint64_t& value) noexcept {
		value = this->get_uint64();
		return std::move(this->error);
	}

	template<> inline ErrorCode ValueIterator::get<std::string>(std::string& value) noexcept {
		value = this->get_string();
		return std::move(this->error);
	}

	template<> inline ErrorCode ValueIterator::get<std::string_view>(std::string_view& value) noexcept {
		value = this->get_string();
		return std::move(this->error);
	}

	inline ValueIterator::ValueIterator(JsonifierCore* other) noexcept
		: root(other->getStructuralIndexes()), stringView(other->getStringView()),
		  stringBufferLocation(other->getStringBuffer()), jsonIterator{ std::make_unique<JsonIterator>(other->getStringView(), other) } {
		this->parser = other;
	}

	inline JsonIterator::JsonIterator(uint8_t* buffer, JsonifierCore* jsonifieriCore) noexcept
		: token{ jsonifieriCore->getStringView(), jsonifieriCore->getStructuralIndexes() } {
		this->rootPosition = jsonifieriCore->getStructuralIndexes();
		this->stringBuffer = buffer;
		this->parser = jsonifieriCore;
	}

	inline ValueIterator& ValueIterator::operator=(const ValueIterator& other) noexcept {
		this->stringBufferLocation = other.stringBufferLocation;
		*this->jsonIterator = *other.jsonIterator;
		this->currentDepth = other.currentDepth;
		this->stringView = other.stringView;
		this->parser = other.parser;
		this->error = other.error;
		this->root = other.root;
		return *this;
	}

	inline ValueIterator::ValueIterator(const ValueIterator& other) noexcept {
		*this = other;
	}

	inline ValueIterator& ValueIterator::operator=(ValueIterator && other) noexcept {
		this->stringBufferLocation = other.stringBufferLocation;
		*this->jsonIterator = *other.jsonIterator;
		this->currentDepth = other.currentDepth;
		this->stringView = other.stringView;
		this->parser = other.parser;
		this->error = other.error;
		this->root = other.root;
		return *this;
	}

	inline ValueIterator::ValueIterator(ValueIterator&& other) noexcept {
		*this = std::move(other);
	}

	inline ValueIterator& ValueIterator::getCurrentIterator() noexcept {
		return *this;
	}

	inline void ValueIterator::asserAtFieldStart(size_t amountToOffset) noexcept {
		assert(*this->jsonIterator->peek() == '"');
	}

	inline void ValueIterator::assertAtObjectStart(size_t amountToOffset) noexcept {
		assert(*this->jsonIterator->peek() == '{');
	}

	inline void ValueIterator::assertAtArrayStart(size_t amountToOffset) noexcept {
		assert(*this->jsonIterator->peek() == '[');
	}

	inline void ValueIterator::assertAtStringStart(size_t amountToOffset) noexcept {
		assert(*this->jsonIterator->peek() == '"');
	}

	inline size_t ValueIterator::getCurrentCount() noexcept {
		return uint32_t((*this->parser->getTape() & JSON_VALUE_MASK)) - (this->parser->getTape() - this->parser->getTape()) - 2;
	}

	inline uint8_t ValueIterator::getRootKey() noexcept {
		return (*this->parser->getTape() >> 56);
	}

	inline size_t ValueIterator::size() noexcept {
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

	inline JsonType ValueIterator::type() noexcept {
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

	JsonifierCore* ValueIterator::getCore() noexcept {
		return this->parser;
	}

	inline uint64_t ValueIterator::get_uint64() noexcept {
		auto result = NumberParser::parseUnsigned(peek_non_root_scalar());
		advance_non_root_scalar();
		return result;
	}

	inline const uint8_t* ValueIterator::peek_non_root_scalar() noexcept {
		if (!is_at_start()) {
			return peek_start();
		}

		assert_at_non_root_start();
		return this->jsonIterator->peek();
	}

	inline int64_t ValueIterator::get_int64() noexcept {
		auto result = NumberParser::parseInteger(peek_non_root_scalar());
		advance_non_root_scalar();
		return result;
	}

	inline bool ValueIterator::get_bool() noexcept {
		auto result = parse_bool(peek_non_root_scalar());
		advance_non_root_scalar();
		return result;
	}

	inline bool ValueIterator::parse_bool(const uint8_t* json) const noexcept {
		auto not_true = StringParser::str4ncmp(json, "true");
		auto not_false = StringParser::str4ncmp(json, "fals") | (json[4] ^ 'e');
		bool error = (not_true && not_false) || NumberParser::isNotStructuralOrWhitespace(json[not_true ? 5 : 4]);
		if (error) {
			return false;
		}
		return bool{ !not_true };
	}

	inline void ValueIterator::advance_non_root_scalar() noexcept {
		if (!is_at_start()) {
			return;
		}

		assert_at_non_root_start();
		this->jsonIterator->return_current_and_advance();
		this->jsonIterator->ascend_to(depth() - 1);
	}

	inline void ValueIterator::assert_at_non_root_start() const noexcept {
		assert_at_start();
		assert(currentDepth > 1);
	}
		
	inline void ValueIterator::assert_at_next() const noexcept {
		//assert(jsonIterator->token.position() > rootPosition);
		//		assert(jsonIterator->currentDepth == currentDepth);
		//assert(currentDepth > 0);
	}

	inline void ValueIterator::assert_at_start() const noexcept {
		assert(position() == root);
	}

	inline bool ValueIterator::is_at_start() const noexcept {
		return position() == start_position();
	}

	inline uint32_t* ValueIterator::start_position() const noexcept {
		return root;
	}

	inline const uint8_t* ValueIterator::peek_start() const noexcept {
		return &this->stringView[*start_position()];
	}

	inline int32_t ValueIterator::depth() const noexcept {
		return currentDepth;
	}

	inline double ValueIterator::get_double() noexcept {
		auto result = NumberParser::parseDouble(peek_non_root_scalar());
		advance_non_root_scalar();
		return result;
	}

	inline Array::Array(const ValueIterator& iterNew) noexcept {
		this->iter = iterNew;
	}

	inline std::string_view ValueIterator::get_string() noexcept {
		return get_raw_json_string().unescape(json_iter());
	}

	inline RawJsonString ValueIterator::get_raw_json_string() noexcept {
		auto json = peek_scalar();
		if (*json != '"') {
			return RawJsonString{};
		}
		advance_scalar();
		return RawJsonString(json + 1);
	}

	inline std::string_view RawJsonString::unescape(JsonIterator& iter) const noexcept {
		auto newValue = ( uint8_t* )(iter.stringBuffer);
		return iter.unescape(*this);
	}

	inline std::string_view ValueIterator::unescape(RawJsonString in, uint8_t*& dst) const noexcept {
		uint8_t* end = StringParser::parseString(in.buf, dst);
		if (!end) {
			return "";
		}
		std::string_view result(reinterpret_cast<const char*>(dst), end - dst);
		dst = end;
		return result;
	}

	inline const uint8_t* ValueIterator::peek_scalar() noexcept {
		if (!is_at_start()) {
			return peek_start();
		}

		assert_at_start();
		return this->jsonIterator->peek();
	}

	inline void ValueIterator::advance_scalar() noexcept {
		if (!is_at_start()) {
			return;
		}

		assert_at_start();
		this->jsonIterator->return_current_and_advance();
		this->jsonIterator->ascend_to(depth() - 1);
	}

	inline RawJsonString::RawJsonString(const uint8_t* bufNew) noexcept {
		this->buf = bufNew;
	}
	
	inline bool ValueIterator::start_object() noexcept {
		start_container('{', "Not an object", "object");
		return started_object();
	}

	inline bool ValueIterator::start_root_object() noexcept {
		start_container('{', "Not an object", "object");
		return started_root_object();
	}

	inline Object ValueIterator::get_object() &  noexcept {
		return Object::start(*this);
	}

	inline ValueIterator ValueIterator::resume_value_iterator() noexcept {
		this->currentDepth++;
		return ValueIterator(*this);
	}

	inline ValueIterator ValueIterator::get_root_value_iterator() noexcept {
		return resume_value_iterator();
	}

	inline bool ValueIterator::started_root_object() noexcept {
		return started_object();
	}

	inline bool ValueIterator::started_object() noexcept {
		assert_at_container_start();
		if (*this->jsonIterator->peek() == '}') {
			this->jsonIterator->return_current_and_advance();
			end_container();
			return false;
		}
		return true;
	}
	
	inline ErrorCode ValueIterator::start_container(uint8_t start_char, const char* incorrect_type_message, const char* type) noexcept {
		const uint8_t* json;
		if (!is_at_start()) {
			json = peek_start();
			if (*json != start_char) {
				return ErrorCode::Incorrect_Type;
			}
		} else {
			assert_at_start();
			json = this->jsonIterator->peek();
			if (*json != start_char) {
				return ErrorCode::Incorrect_Type;
			}
			this->jsonIterator->return_current_and_advance();
		}


		return ErrorCode::Success;
	}

	inline void ValueIterator::assert_at_container_start() const noexcept {
		//assert(jsonIterator->token.position() == rootPosition + 1);
		//assert(jsonIterator->currentDepth == currentDepth);
		//assert(currentDepth > 0);
	}

	inline ErrorCode ValueIterator::end_container() noexcept {
		this->jsonIterator->ascend_to(depth() - 1);
		return ErrorCode::Success;
	}

	inline bool ValueIterator::reset_object() noexcept {
		move_at_container_start();
		return started_object();
	}

	inline ErrorCode ValueIterator::getError() noexcept {
		return this->error;
	}

	inline size_t Object::count_fields()  noexcept {
		size_t count{ 0 };
		for ( auto v: *this) {
			count++;
		}
		if (getError()!=ErrorCode::Success) {
			return static_cast<size_t>(getError());
		}
		this->reset_object();
		return count;
	}

	inline void ValueIterator::abandon() noexcept {
		parser = nullptr;
		currentDepth = 0;
	}

	inline uint32_t* ValueIterator::end_position() const noexcept {
		size_t structuralIndexCount{ parser->getTapeLength() };
		return &parser->getStructuralIndexes()[structuralIndexCount];
	}

	inline ErrorCode ValueIterator::field_value() noexcept {
		assert_at_next();

		if (*this->jsonIterator->return_current_and_advance() != ':') {
			return ErrorCode::TapeError;
		}
		this->jsonIterator->descend_to(depth() + 1);
		return ErrorCode::Success;
	}

	inline std::string_view ValueIterator::field_key() noexcept {
		assert_at_next();

		const uint8_t* key = this->jsonIterator->return_current_and_advance();
		if (*(key++) != '"') {
			return "";
		}
		return RawJsonString{ key }.unescape(*this->jsonIterator);
	}

	inline const char* RawJsonString::raw() const noexcept {
		return reinterpret_cast<const char*>(buf);
	}

	inline bool ValueIterator::is_open() const noexcept {
		return depth() >= depth();
	}

	inline bool ValueIterator::has_next_field() noexcept {
		assert_at_next();

		switch (*this->jsonIterator->return_current_and_advance()) {
			case '}':
				end_container();
				return false;
			case ',':
				return true;
			default:
				return false;
		}
	}

	inline void ValueIterator::assert_at_child() const noexcept {
		assert(position() > start_position());
	}

	inline void JsonIterator::descend_to(size_t child_depth) noexcept {
		assert(child_depth >= 1 && child_depth < std::numeric_limits<int32_t>::max());
		assert(currentDepth == child_depth - 1);
		currentDepth = child_depth;
	}

	inline void JsonIterator::ascend_to(size_t parent_depth) noexcept {
		assert(parent_depth >= 0 && parent_depth < std::numeric_limits<int32_t>::max() - 1);
		assert(currentDepth == parent_depth + 1);
		currentDepth = parent_depth;
	}

	inline const uint8_t* JsonIterator::return_current_and_advance() noexcept {
 		std::cout << "CURRENT KEY: " <<token.buf[*token.currentPosition] << std::endl;
		return token.return_current_and_advance();
	}

	inline const uint8_t* TokenIterator::return_current_and_advance() noexcept {
		return &buf[*(currentPosition++)];
	}

	inline const uint8_t* JsonIterator::peek(int32_t delta) const noexcept {
		return token.peek(delta);
	}

	inline const uint8_t* TokenIterator::peek(uint32_t* position) const noexcept {
		return &buf[*position];
	}

	inline const uint8_t* TokenIterator::peek(int32_t position) const noexcept {
		return &buf[position];
	}

	inline ErrorCode ValueIterator::skip_child() noexcept {
		//assert(jsonIterator->position() > rootPosition);
		//assert(jsonIterator->depth() >= currentDepth);

		return jsonIterator->skip_child(depth());
	}

	inline size_t JsonIterator::depth() const noexcept {
		return this->currentDepth;
	}

	inline uint32_t* JsonIterator::position() const noexcept {
		return this->token.currentPosition;
	}

	inline ErrorCode JsonIterator::skip_child(size_t parent_depth) noexcept {
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
				currentDepth--;
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
				currentDepth--;
				if (depth() <= parent_depth) {
					return ErrorCode::Success;
				}
				break;
		}

		while (position() < end_position()) {
			switch (*return_current_and_advance()) {
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

	inline std::string_view JsonIterator::unescape(RawJsonString in) noexcept {
		auto newValue = StringParser::parseString(reinterpret_cast<const uint8_t*>(in.raw()), stringBuffer);
		size_t index{};
		while (1) {
			index++;
			if (newValue[index] == '\0') {
				break;
			}
		}
		return std::string_view{ reinterpret_cast<char*>(newValue), index };
	}

	inline JsonIterator& ValueIterator::json_iter() noexcept {
		return *jsonIterator;
	}

	inline ValueIterator ValueIterator::child()  noexcept {
		assert_at_child();
		return ValueIterator{ jsonIterator.get(), static_cast<size_t>(depth() + 1), jsonIterator->token.position() };
	}

	inline uint32_t* JsonIterator::end_position() const noexcept {
		size_t structuralIndexCount{ parser->getTapeLength() };
		return &parser->getStructuralIndexes()[structuralIndexCount];
	}

	inline void ValueIterator::move_at_container_start() noexcept {
		jsonIterator->currentDepth = currentDepth;
		jsonIterator->token.set_position(root + 1);
	}

	inline void TokenIterator::set_position(uint32_t* target_position) noexcept {
		currentPosition = target_position;
	}

	inline uint32_t* TokenIterator::position() const noexcept {
		return currentPosition;
	}

	inline ValueIterator::ValueIterator(JsonIterator* jsonIterator, uint64_t depth, uint32_t* rootPosition)noexcept {
		this->stringBufferLocation = jsonIterator->parser->getStringBuffer();
		this->stringView = jsonIterator->parser->getStringView();
		*this->jsonIterator = *jsonIterator;
		this->parser = jsonIterator->parser;
		this->currentDepth = depth;
		this->root = rootPosition;
	}

	inline uint32_t* ValueIterator::position() const noexcept {
		return jsonIterator->position();
	}

	inline TokenIterator::TokenIterator(const uint8_t* buf, uint32_t* position) noexcept {
		this->currentPosition = position;
		this->buf = buf;
	}

	inline bool ValueIterator::operator!=(const ValueIterator&other) {
		return (this->stringBufferLocation != other.stringBufferLocation && this->stringView != other.stringView);
	}

	inline bool ValueIterator::find_field_unordered_raw(const std::string_view key) noexcept {
		ErrorCode error{};
		bool has_value{};
		uint32_t* search_start = jsonIterator->position();
		bool at_first = at_first_field();
		if (at_first) {
			has_value = true;
		} else if (!is_open()) {
			reset_object();
			at_first = true;
		} else {
			if (error = skip_child(); error != ErrorCode::Success) {
				abandon();
				return false;
			}
			search_start = jsonIterator->position();
			if (!has_next_field()) {
				abandon();
				return false;
			}
		}

		while (has_value) {
			RawJsonString actual_key{};
			if (field_key() == "") {
				abandon();
				return false;
			};
			if (field_value() != ErrorCode::Success) {
				abandon();
				return false;
			}
			if (actual_key.unsafe_is_equal(key)) {
				return true;
			}
			skip_child();
			if (!has_next_field()) {
				abandon();
				return false;
			}
		}
		if (at_first) {
			return false;
		}
		reset_object();
		while (true) {
			RawJsonString actual_key{};
			assert(field_key() != "");
			error = field_value();
			assert(error == ErrorCode::Success);
			if (actual_key.unsafe_is_equal(key)) {
				return true;
			}

			assert(skip_child() == ErrorCode::Success);
			if (jsonIterator->position() == search_start) {
				return false;
			}
			assert(has_next_field());
		}
		return false;
	}

	inline Object ValueIterator::find_field_unordered(const std::string_view key) noexcept {
		bool has_value{};
		this->find_field_unordered_raw(key);
		if (!has_value) {
			return Object{};
		}
		return Object(this->child());
	}

	inline Object ValueIterator::operator[](const std::string_view key) & noexcept {
		return find_field_unordered(key);
	}

	inline Object ValueIterator::operator[](const std::string_view key) && noexcept {
		return std::forward<Object>(*this).find_field_unordered(key);
	}

	inline bool RawJsonString::unsafe_is_equal(size_t length, std::string_view target) const noexcept {
		return (length >= target.size()) && (raw()[target.size()] == '"') && !memcmp(raw(), target.data(), target.size());
	}

	inline bool RawJsonString::unsafe_is_equal(std::string_view target) const noexcept {
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

	inline bool ValueIterator::at_first_field() const noexcept {
		return jsonIterator->token.position() == start_position() + 1;
	}

	inline bool JsonIterator::at_root() const noexcept {
		return position() == root_position();
	}

	inline uint32_t*JsonIterator::root_position() const noexcept {
		return rootPosition;
	}

	inline JsonIterator& JsonIterator::operator=(JsonIterator&& other) noexcept {
		this->stringBuffer = other.stringBuffer;
		this->currentDepth = other.currentDepth;
		this->rootPosition = other.rootPosition;
		this->parser = other.parser;
		this->error = other.error;
		this->token = other.token;
		return *this;
	}

	inline JsonIterator::JsonIterator(JsonIterator&& other) noexcept : token{other.token} {
		*this = std::move(other);
	}

	inline Field ObjectIterator::operator*() noexcept {
		ErrorCode error = iter.getError();
		if (error != ErrorCode::Success) {
			iter.abandon();
			return Field{};
		}
		auto result = Field::start(iter);
		if (result.getError() != ErrorCode::Success) {
			iter.abandon();
		}
		return result;
	}

	inline bool ObjectIterator::operator==(ObjectIterator& other) noexcept {
		return !(this->iter != other.iter);
	}

	inline ObjectIterator& ObjectIterator::operator++() noexcept {
		if (!iter.is_open()) {
			return *this;
		}

		ErrorCode error{};
		if (error = iter.skip_child(); error != ErrorCode::Success) {
			return *this;
		}

		bool has_value{};
		if (!iter.has_next_field()) {
			return *this;
		};
		return *this;
	}

	inline Array Array::start_root(ValueIterator& iter) noexcept {
		bool has_value{};
		iter.start_root_array();
		return Array(iter);
	}

};
