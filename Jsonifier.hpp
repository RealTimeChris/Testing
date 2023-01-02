#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)

#include "NumberParsingUtils.hpp"
#include "StringParsingUtils.hpp"
#include "Simd.hpp"
#include "Value.hpp"
#include "Object.hpp"
#include "Primitive.hpp"
#include "Array.hpp"
#include "Document.hpp"
#include "Field.hpp"

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
	concept IsEnum = std::is_enum<Ty>::value;

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
		cout << tape_idx << " : " << type << "\t// pointing to " << (tape_val & JSON_VALUE_MASK) << " (start rootPosition)\n";
		return true;
	}

	class JsonifierCore;
	class Object;

	inline int64_t totalTimePassed{};
	inline int64_t totalTimePassed02{};
	inline int64_t iterationCount{};

	class JsonifierCore {
	  public:
		inline Document getDocument() {
			return std::forward<Document>(Document{ std::forward<IteratorBaseBase>(IteratorBaseBase{ this }) });
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
			this->stringView = stringViewNew;
			if (!(this->structuralIndexes.get() && this->stringBuffer.get())) {
				this->structuralIndexes.deallocate();
				this->stringBuffer.deallocate();
				return ErrorCode::Mem_Alloc_Error;
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
				char block[256];
				stringReader.getRemainder(block);
				this->section.submitDataForProcessing(block);
				auto indexCount = section.getStructuralIndices(this->structuralIndexes.get(), tapeCurrentIndex, this->stringLengthRaw);
				totalTimePassed += stopWatch.totalTimePassed().count();
				this->getTapeLength() = tapeCurrentIndex;
				std::cout << "TIME FOR STAGE1: " << totalTimePassed / iterationCount << std::endl;
			}
		}

		inline uint8_t* getStringView() {
			return this->stringView;
		}

		inline uint8_t* getStringBuffer() {
			return this->stringBuffer.get();
		}

		inline uint32_t* getStructuralIndexes() {
			return this->structuralIndexes.get();
		}

		inline Document parseJson(std::string& string);

		inline size_t& getTapeLength() {
			return this->tapeLength;
		}

	  protected:
		ObjectBuffer<uint32_t> structuralIndexes{};
		ObjectBuffer<uint8_t> stringBuffer{};
		SimdStringSection section{};
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
	/*
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
			return ErrorCode::String_Error;
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
			return ErrorCode::Mem_Alloc_Error;
		}
		std::memcpy(copy.get(), value, this->remainingLen());
		std::memset(copy.get() + this->remainingLen(), ' ', 256);
		return visitNumber(copy.get());
	}

	inline ErrorCode TapeBuilder::visitTrueAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidTrueAtom(value)) {
			return ErrorCode::TAtom_Error;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootTrueAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidTrueAtom(value)) {
			return ErrorCode::NAtom_Error;
		}
		this->tape.append(0, TapeType::True_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitFalseAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidFalseAtom(value)) {
			return ErrorCode::FAtom_Error;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootFalseAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidFalseAtom(value)) {
			return ErrorCode::FAtom_Error;
		}
		this->tape.append(0, TapeType::False_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitNullAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidNullAtom(value)) {
			return ErrorCode::NAtom_Error;
		}
		this->tape.append(0, TapeType::Null_Value);
		return ErrorCode::Success;
	}

	inline ErrorCode TapeBuilder::visitRootNullAtom(const uint8_t* value) noexcept {
		if (!StringParser::isValidNullAtom(value)) {
			return ErrorCode::NAtom_Error;
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
			return ErrorCode::Depth_Error;
		}
		this->masterParser->getIsArray()[this->depth] = false;
		this->visitObjectStart();
		auto key = this->advance();
		if (*key != '"') {
			return ErrorCode::Tape_Error;
		}
		this->visitKey(key);
		this->incrementCount();
	}

	Object_Field : {
		auto newValue = *this->advance();
		if (newValue != ':') {
			return ErrorCode::Tape_Error;
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
						return ErrorCode::Tape_Error;
					}
					this->visitKey(key);
				}
				goto Object_Field;
			case '}':
				this->visitObjectEnd();
				goto Scope_End;
			default:
				return ErrorCode::Tape_Error;
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
			return ErrorCode::Depth_Error;
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
				return ErrorCode::Tape_Error;
		}
	}

	Document_End : {
		this->visitDocumentEnd();

		auto nextStructuralIndex = uint32_t(this->nextStructural - &this->masterParser->getStructuralIndexes()[0]);

		if (nextStructuralIndex != this->masterParser->getTapeLength()) {
			return ErrorCode::Tape_Error;
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
					std::string{ static_cast<EnumStringConverter>(ErrorCode::Tape_Error) } +
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
					std::string{ static_cast<EnumStringConverter>(ErrorCode::Tape_Error) } +
					", at the following index into the string: " + std::to_string(*this->nextStructural) };
		}
	}
	*/
	Document JsonifierCore::parseJson(std::string& string) {
		this->generateJsonEvents(reinterpret_cast<uint8_t*>(string.data()), string.size());
		return this->getDocument();
	}
	template<> inline ErrorCode JsonValueBase::get<Object>(Object& value) noexcept {
		value = this->getObject();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<double>(double& value) noexcept {
		value = this->iterator.getDouble();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<bool>(bool& value) noexcept {
		value = this->iterator.getBool();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<int64_t>(int64_t& value) noexcept {
		value = this->iterator.getInt64();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<uint64_t>(uint64_t& value) noexcept {
		value = this->iterator.getUint64();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<std::string>(std::string& value) noexcept {
		value = this->iterator.getString();
		return std::move(this->error);
	}

	template<> inline ErrorCode JsonValueBase::get<std::string_view>(std::string_view& value) noexcept {
		value = this->iterator.getString();
		return std::move(this->error);
	}

	inline JsonValueBase::JsonValueBase(JsonifierCore* other) noexcept
		: iterator{} {
		this->parser = other;
		this->stringBuffer = other->getStringBuffer();
		this->root = other->getStructuralIndexes();
	}

	inline JsonValueBase::JsonValueBase(uint8_t* buf, JsonifierCore* _parser) noexcept
		: parser{ _parser }, stringBuffer{ parser->getStringBuffer() }, currentDepth{ 1 }, root{ parser->getStructuralIndexes() } {};

	inline JsonValueBase& JsonValueBase::operator=(const JsonValueBase& other) noexcept {
		this->stringBuffer = other.stringBuffer;
		this->currentDepth = other.currentDepth;
		this->iterator = other.iterator;
		this->parser = other.parser;
		this->error = other.error;
		this->root = other.root;
		return *this;
	}

	inline JsonValueBase::JsonValueBase(const JsonValueBase& other) noexcept {
		*this = other;
	}

	inline JsonValueBase& JsonValueBase::operator=(JsonValueBase && other) noexcept {
		this->stringBuffer = other.stringBuffer;
		this->currentDepth = other.currentDepth;
		this->iterator = other.iterator;
		this->parser = other.parser;
		this->error = other.error;
		this->root = other.root;
		return *this;
	}

	inline JsonValueBase::JsonValueBase(JsonValueBase&& other) noexcept {
		*this = std::move(other);
	}

	inline IteratorBaseBase& IteratorBaseBase::getCurrentIterator() noexcept {
		return *this;
	}

	inline void IteratorBaseBase::asserAtFieldStart(size_t amountToOffset) noexcept {
		assert(*this->peek() == '"');
	}

	inline void IteratorBaseBase::assertAtObjectStart(size_t amountToOffset) noexcept {
		assert(*this->peek() == '{');
	}

	inline void IteratorBaseBase::assertAtArrayStart(size_t amountToOffset) noexcept {
		assert(*this->peek() == '[');
	}

	inline void IteratorBaseBase::assertAtStringStart(size_t amountToOffset) noexcept {
		assert(*this->peek() == '"');
	}

	inline JsonType IteratorBaseBase::type() noexcept {
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

	JsonifierCore* IteratorBaseBase::getCore() noexcept {
		return this->parser;
	}

	inline const uint8_t* IteratorBaseBase::peekNonRootScalar() noexcept {
		if (!isAtStart()) {
			return peekStart();
		}

		assertAtNonRootStart();
		return this->peek();
	}

	inline void IteratorBaseBase::advanceNonRootScalar() noexcept {
		if (!isAtStart()) {
			return;
		}

		assertAtNonRootStart();
		this->returnCurrentAndAdvance();
		this->ascendTo(depth() - 1);
	}

	inline void IteratorBaseBase::assertAtNonRootStart() const noexcept {
		assertAtStart();
		assert(currentDepth > 1);
	}
		
	inline void IteratorBaseBase::assertAtNext() const noexcept {
		//assert(jsonIterator->token.position() > rootPosition);
		std::cout << "JSON ITERATOR DEPTH: " << this->currentDepth << ", CURRENT DEPTH: " << currentDepth << std::endl;
		assert(this->currentDepth == currentDepth);
		assert(currentDepth > 0);
	}

	inline void IteratorBaseBase::assertAtStart() const noexcept {
		assert(position() == rootPositionVal);
	}

	inline bool IteratorBaseBase::isAtStart() const noexcept {
		return position() == startPosition();
	}

	inline uint32_t* IteratorBaseBase::startPosition() const noexcept {
		return rootPositionVal;
	}

	inline const uint8_t* IteratorBaseBase::peekStart() const noexcept {
		return &this->parser->getStringView()[*startPosition()];
	}

	inline size_t IteratorBaseBase::depth() const noexcept {
		return currentDepth;
	}

	inline double IteratorBaseBase::getDouble() noexcept {
		auto result = NumberParser::parseDouble(peekNonRootScalar());
		advanceNonRootScalar();
		return result;
	}

	inline std::string_view IteratorBaseBase::getString() noexcept {
		return getRawJsonString().unescape(*this);
	}

	inline RawJsonString IteratorBaseBase::getRawJsonString() noexcept {
		auto json = peekScalar();
		if (*json != '"') {
			return RawJsonString{};
		}
		advanceScalar();
		return RawJsonString(( uint8_t* )json + 1);
	}

	inline std::string_view RawJsonString::unescape(IteratorBaseBase& iter) noexcept {
		auto newValue = ( uint8_t* )(iter.parser->getStringBuffer());
		return iter.unescape(*this, newValue);
	}

	inline const uint8_t* IteratorBaseBase::peekScalar() noexcept {
		if (!isAtStart()) {
			return peekStart();
		}

		assertAtStart();
		return this->peek();
	}

	inline void IteratorBaseBase::advanceScalar() noexcept {
		if (!isAtStart()) {
			return;
		}

		assertAtStart();
		this->returnCurrentAndAdvance();
		this->ascendTo(depth() - 1);
	}

	inline RawJsonString::RawJsonString(uint8_t* bufNew) noexcept {
		this->stringView = bufNew;
	}
	
	inline bool IteratorBaseBase::startObject() noexcept {
		startContainer('{');
		return startedObject();
	}

	inline bool IteratorBaseBase::startRootObject() noexcept {
		startContainer('{');
		return startedRootObject();
	}

	inline Object JsonValueBase::getObject() &  noexcept {
		return Object::start(this->iterator);
	}

	inline IteratorBaseBase IteratorBaseBase::resumeValueIterator() noexcept {
		this->currentDepth++;
		return IteratorBaseBase(*this);
	}

	inline IteratorBaseBase IteratorBaseBase::getRootValueIterator() noexcept {
		return resumeValueIterator();
	}

	inline bool IteratorBaseBase::startedRootObject() noexcept {
		return startedObject();
	}

	inline bool IteratorBaseBase::startedObject() noexcept {
		assertAtContainerStart();
		if (*this->peek() == '}') {
			this->returnCurrentAndAdvance();
			endContainer();
			return false;
		}
		return true;
	}
	
	inline ErrorCode IteratorBaseBase::startContainer(uint8_t start_char) noexcept {
		const uint8_t* json;
		if (!isAtStart()) {
			json = peekStart();
			if (*json != start_char) {
				return ErrorCode::Incorrect_Type;
			}
		} else {
			assertAtStart();
			json = this->peek();
			if (*json != start_char) {
				return ErrorCode::Incorrect_Type;
			}
			this->returnCurrentAndAdvance();
		}


		return ErrorCode::Success;
	}

	inline void IteratorBaseBase::assertAtContainerStart() const noexcept {
		//assert(jsonIterator->token.position() == rootPosition + 1);
		//assert(jsonIterator->currentDepth == currentDepth);
		//assert(currentDepth > 0);
	}

	inline ErrorCode IteratorBaseBase::endContainer() noexcept {
		this->ascendTo(depth() - 1);
		return ErrorCode::Success;
	}

	inline bool IteratorBaseBase::resetObject() noexcept {
		moveAtContainerStart();
		return startedObject();
	}

	inline ErrorCode JsonValueBase::getError() noexcept {
		return this->error;
	}

	inline size_t Object::countFields()  noexcept {
		size_t count{ 0 };
		for ( auto v: *this) {
			count++;
		}
		if (getError()!=ErrorCode::Success) {
			return static_cast<size_t>(getError());
		}
		this->iterator.resetObject();
		return count;
	}

	inline void IteratorBaseBase::abandon() noexcept {
		parser = nullptr;
		currentDepth = 0;
	}

	inline uint32_t* JsonValueBase::endPosition() const noexcept {
		size_t structuralIndexCount{ parser->getTapeLength() };
		return &parser->getStructuralIndexes()[structuralIndexCount];
	}

	inline ErrorCode IteratorBaseBase::fieldValue() noexcept {
		assertAtNext();

		if (*this->returnCurrentAndAdvance() != ':') {
			return ErrorCode::Tape_Error;
		}
		this->descendTo(depth() + 1);
		return ErrorCode::Success;
	}

	inline std::string_view IteratorBaseBase::fieldKey() noexcept {
		assertAtNext();

		const uint8_t* key = this->returnCurrentAndAdvance();
		if (*(key++) != '"') {
			return "";
		}
		return RawJsonString{ ( uint8_t* )key }.raw();
	}

	inline char* RawJsonString::raw() const noexcept {
		return reinterpret_cast<char*>(stringView);
	}

	inline bool IteratorBaseBase::isOpen() const noexcept {
		return depth() >= depth();
	}

	inline bool IteratorBaseBase::hasNextField() noexcept {
		assertAtNext();

		switch (*this->returnCurrentAndAdvance()) {
			case '}':
				endContainer();
				return false;
			case ',':
				return true;
			default:
				return true;
		}
	}

	inline void IteratorBaseBase::assertAtChild() const noexcept {
		assert(position() > startPosition());
	}

	inline void IteratorBaseBase::descendTo(size_t child_depth) noexcept {
		assert(child_depth >= 1 && child_depth < std::numeric_limits<int32_t>::max());
		//assert(currentDepth == child_depth - 1);
		currentDepth = child_depth;
	}

	inline void IteratorBaseBase::ascendTo(size_t parentDepth) noexcept {
		assert(parentDepth >= 0 && parentDepth < std::numeric_limits<int32_t>::max() - 1ull);
		assert(currentDepth == parentDepth + 1);
		currentDepth = parentDepth;
	}

	inline const uint8_t* IteratorBaseBase::returnCurrentAndAdvance() noexcept {
		return &buf[*(currentPosition++)];
	}

	inline const uint8_t* IteratorBaseBase::peek(int32_t delta) const noexcept {
		return &buf[*currentPosition];
	}

	inline ErrorCode IteratorBaseBase::skipChild(size_t parentDepth) noexcept {
		assert(this->depth() >= currentDepth);
		if (depth() <= parentDepth) {
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
				if (depth() <= parentDepth) {
					return ErrorCode::Success;
				}
				break;
			case '"':
				if (*peek() == ':') {
					returnCurrentAndAdvance();
					break;
				}
				[[fallthrough]];
			default:
				currentDepth--;
				if (depth() <= parentDepth) {
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
					if (depth() <= parentDepth) {
						return ErrorCode::Success;
					}
					break;
				default:
					break;
			}
		}

		return ErrorCode::Tape_Error;
	}

	inline uint32_t* IteratorBaseBase::position() const noexcept {
		return this->currentPosition;
	}

	inline std::string_view JsonValueBase::unescape(RawJsonString& in) noexcept {
		uint8_t* end = StringParser::parseString(in.stringView, this->stringBuffer);
		if (!end) {
			return "";
		}
		std::string_view result(reinterpret_cast<const char*>(this->stringBuffer), end - this->stringBuffer);
		this->stringBuffer = end;
		return result;
	}

	inline uint32_t* IteratorBaseBase::endPosition() const noexcept {
		size_t structuralIndexCount{ parser->getTapeLength() };
		return &parser->getStructuralIndexes()[structuralIndexCount];
	}

	inline void IteratorBaseBase::moveAtContainerStart() noexcept {
		this->currentDepth = currentDepth;
		this->setPosition(rootPositionVal + 1);
	}

	inline void IteratorBaseBase::setPosition(uint32_t* target_position) noexcept {
		currentPosition = target_position;
	}

	inline bool IteratorBaseBase::findFieldUnorderedRaw(const std::string_view key) noexcept {
		ErrorCode error;
		bool hasValue{};
		uint32_t* searchStart = this->rootPositionVal;
		bool atFirst = atFirstField();
		if (atFirst) {
			hasValue = true;
		} else if (!isOpen()) {
			hasValue = resetObject();
			atFirst = true;
		} else {
			if (error = skipChild(this->currentDepth); error != ErrorCode::Success) {
				abandon();
				return false;
			}
			searchStart = this->rootPositionVal;
			if (!hasNextField()) {
				abandon();
				return false;
			}
		}
		while (hasValue) {
			assert(this->depth() == currentDepth);
			RawJsonString actualKey{};
			if (fieldKey() == "") {
				abandon();
				return false;
			};
			if (error = fieldValue(); error != ErrorCode::Success) {
				abandon();
				return false;
			}
			if (actualKey.unsafeIsEqual(key)) {
				return true;
			}
			skipChild(this->currentDepth);
			if (hasValue = hasNextField();!hasValue) {
				abandon();
				return false;
			}
		}
		if (atFirst) {
			return false;
		}
		hasValue = resetObject();
		while (true) {
			hasValue;
			assert(this->depth() == currentDepth);

			RawJsonString actualKey{ ( uint8_t* )(fieldKey().data()) };
			error = fieldValue();
			//assert(error == ErrorCode::Success);
			if (actualKey.unsafeIsEqual(key)) {
				return true;
			}
			assert(skipChild(this->currentDepth) == ErrorCode::Success);
			if (this->position() == searchStart) {
				return false;
			}
			hasValue = hasNextField();
		}
		return false;
	}

	inline Object IteratorBaseBase::findFieldUnordered(const std::string_view key) noexcept {
		bool hasValue{};
		this->findFieldUnorderedRaw(key);
		if (!hasValue) {
			return Object{};
		}
		return Object(this->child());
	}

	inline Object IteratorBaseBase::operator[](const std::string_view key) & noexcept {
		return findFieldUnordered(key);
	}

	inline Object IteratorBaseBase::operator[](const std::string_view key) && noexcept {
		return std::forward<Object>(*this).findFieldUnordered(key);
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

	inline bool IteratorBaseBase::atFirstField() const noexcept {
		return this->position() == startPosition() + 1;
	}

	inline bool IteratorBaseBase::atRoot() const noexcept {
		return position() == rootPositionVal;
	}

	inline uint32_t* IteratorBaseBase::rootPosition() const noexcept {
		return rootPositionVal;
	}

	inline Object JsonIteratorBase<Object>::operator*() noexcept {
		ErrorCode error = this->getError();
		if (error != ErrorCode::Success) {
			this->abandon();
			return Object{};
		}
		auto result = Object::start(*this);
		if (result.getError() != ErrorCode::Success) {
			this->abandon();
		}
		return result;
	}

	inline bool JsonIteratorBase<Object>::operator==(const JsonIteratorBase<Object>& other) const noexcept {
		return !(this->iter != other.iter);
	}

	inline JsonIteratorBase<Object>& JsonIteratorBase<Object>::operator++() noexcept {
		if (!this->isOpen()) {
			return *this;
		}

		ErrorCode error{};
		if (error = this->skipChild(this->currentDepth); error != ErrorCode::Success) {
			return *this;
		}

		bool hasValue{};
		if (!this->hasNextField()) {
			return *this;
		};
		return *this;
	}

	inline Array IteratorBaseBase::startRoot(IteratorBaseBase& iter) noexcept {
		bool hasValue{};
		iter.startRootArray();
		return Array(iter);
	}

	inline void IteratorBaseBase::rewind() noexcept {
		this->setPosition(rootPosition());
		this->currentDepth = 1;
	}

	inline size_t Array::countElements() & noexcept {
		size_t count{ 0 };
		for (auto v: *this) {
			count++;
		}
		if (iter.getError() != ErrorCode::Success) {
			return static_cast<size_t>(iter.getError());
		}
		iter.resetArray();
		return count;
	}

	inline bool IteratorBaseBase::startRootArray() noexcept {
		startContainer('[');
		return startedRootArray();
	}

	inline bool IteratorBaseBase::startedRootArray() noexcept {
		if (*this->peekLast() != ']') {
			this->abandon();
			return false;
		}
		if ((*this->peek(this->endPosition()) == ']') && (!this->balanced())) {
			this->abandon();
			return false;
		}
		return startedArray();
	}

	inline const uint8_t* IteratorBaseBase::peekLast() const noexcept {
		return this->peek(lastPosition());
	}

	inline const uint8_t* IteratorBaseBase::peek(uint32_t* position) const noexcept {
		return this->peek(position);
	}

	inline bool IteratorBaseBase::startedArray() noexcept {
		assertAtContainerStart();
		if (*this->peek() == ']') {
			this->returnCurrentAndAdvance();
			endContainer();
			return false;
		}
		this->descendTo(depth() + 1);
		return true;
	}

	inline bool IteratorBaseBase::hasNextElement() noexcept {
		assertAtNext();

		switch (*this->returnCurrentAndAdvance()) {
			case ']':
				endContainer();
				return false;
			case ',':
				this->descendTo(depth() + 1);
				return true;
			default:
				return false;
		}
	}

	inline bool IteratorBaseBase::balanced() noexcept {
		int32_t count{ 0 };
		this->setPosition(rootPosition());
		while (this->peek() <= peekLast()) {
			switch (*this->returnCurrentAndAdvance()) {
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

	inline uint32_t* JsonValueBase::lastPosition() const noexcept {
		size_t n_structural_indexes{ parser->getTapeLength() };
		assert(n_structural_indexes > 0);
		return &parser->getStructuralIndexes()[n_structural_indexes - 1];
	}

	inline bool IteratorBaseBase::resetArray() noexcept {
		moveAtContainerStart();
		return startedArray();
	}

};
