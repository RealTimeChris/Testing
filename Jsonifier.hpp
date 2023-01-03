#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)

#include "NumberParsingUtils.hpp"
#include "StringParsingUtils.hpp"
#include "Simd.hpp"
#include "Object.hpp"
#include "Array.hpp"
#include "Document.hpp"
#include "Field.hpp"

namespace Jsonifier {

	template<typename OTy> class ObjectBuffer {
	  public:
		using AllocatorTraits = std::allocator_traits<std::allocator<OTy>>;

		inline ObjectBuffer& operator=(ObjectBuffer&&) = delete;
		inline ObjectBuffer(ObjectBuffer&&) = delete;

		inline ObjectBuffer& operator=(const ObjectBuffer&) = delete;
		inline ObjectBuffer(const ObjectBuffer&) = delete;

		inline ObjectBuffer() noexcept = default;

		inline OTy& operator[](size_t index) noexcept {
			return this->objects[index];
		}

		inline operator OTy*() noexcept {
			return this->objects;
		}

		inline void reset(size_t newSize) noexcept {
			this->deallocate();
			if (newSize != 0) {
				std::allocator<OTy> allocator{};
				this->objects = AllocatorTraits::allocate(allocator, newSize);
				this->currentSize = newSize;
			}
		}

		inline ~ObjectBuffer() noexcept {
			this->deallocate();
		}

	  protected:
		size_t currentSize{};
		OTy* objects{};
		
		inline void deallocate() {
			if (this->currentSize > 0 && this->objects) {
				std::allocator<OTy> allocator{};
				AllocatorTraits::deallocate(allocator, this->objects, this->currentSize);
				this->objects = nullptr;
				this->currentSize = 0;
			}
		}
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

		operator std::vector<uint64_t>() noexcept;

		operator uint64_t() noexcept;

		bool isItAVector() noexcept;

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

		Jsonifier& operator=(EnumConverter& data) noexcept;
		Jsonifier(EnumConverter& data) noexcept;

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

	template<typename OTy> inline void allocateNew(size_t newSize, OTy*&objects) noexcept {
		if (newSize != 0) {
			std::allocator<OTy> allocator{};
			using AllocatorTraits = std::allocator_traits<std::allocator<OTy>>;
			objects = AllocatorTraits::allocate(allocator, newSize);
		}
	}
	template<typename OTy> inline void deallocate(size_t currentSize, OTy* objects) {
		if (objects) {
			std::allocator<OTy> allocator{};
			using AllocatorTraits = std::allocator_traits<std::allocator<OTy>>;
			AllocatorTraits::deallocate(allocator, objects, currentSize);
			objects = nullptr;
		}
	}

	class JsonifierCore {
	  public:
		inline Document getDocument() {
			return std::forward<Document>(Document{ this });
		}

		inline JsonifierCore& operator=(JsonifierCore&&) = default;
		inline JsonifierCore(JsonifierCore&&) = default;
		inline JsonifierCore(){};

		inline uint64_t round(int64_t a, int64_t n) {
			return (((a) + (( n )-1)) & ~(( n )-1));
		}

		inline ErrorCode allocate(uint8_t* stringViewNew) noexcept {
			if (this->stringLengthRaw == 0) {
				return ErrorCode::Success;
			}
			this->stringBuffer.reset(round(5 * this->stringLengthRaw / 3 + 256, 256));
			this->structuralIndexes.reset(round(this->stringLengthRaw + 3, 256));
			this->stringView = stringViewNew;
			if (!(this->structuralIndexes && this->stringBuffer)) {
				this->stringBuffer.reset(0);
				this->structuralIndexes.reset(0);
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

				//iterationCount++;
				StringBlockReader<256> stringReader{ this->stringView, this->stringLengthRaw };
				//StopWatch stopWatch{ std::chrono::nanoseconds{ 1 } };
				size_t tapeCurrentIndex{ 0 };
				while (stringReader.hasFullBlock()) {
					this->section.submitDataForProcessing(stringReader.fullBlock());
					section.getStructuralIndices(this->structuralIndexes, tapeCurrentIndex, this->stringLengthRaw);
					stringReader.advance();
				}
				char block[256];
				stringReader.getRemainder(block);
				this->section.submitDataForProcessing(block);
				section.getStructuralIndices(this->structuralIndexes, tapeCurrentIndex, this->stringLengthRaw);
				//				totalTimePassed += stopWatch.totalTimePassed().count();
				this->getTapeLength() = tapeCurrentIndex;
				//std::cout << "TIME FOR STAGE1: " << totalTimePassed / iterationCount << std::endl;
				//for (size_t x = 0; x < this->tapeLength; ++x) {
				//std::cout << "CURRENT INDEX: " << this->structuralIndexes[x]
				//<< ", THE INDEX'S VALUE: " << this->stringView[this->structuralIndexes[x]] << std::endl;
				//}
			}
		}

		inline uint8_t* getStringView() {
			return this->stringView;
		}

		inline uint8_t* getStringBuffer() {
			return this->stringBuffer;
		}

		inline uint32_t* getStructuralIndices() {
			return this->structuralIndexes;
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
		: nextStructural(masterParserNew->getStructuralIndices()), masterParser{ masterParserNew }, tape{ masterParserNew->getTape() },
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
		return this->nextStructural == &this->masterParser->getStructuralIndices()[this->masterParser->getTapeLength() - 1];
	}

	inline bool TapeBuilder::atBeginning() noexcept {
		return this->nextStructural == this->masterParser->getStructuralIndices();
	}

	inline uint8_t TapeBuilder::lastStructural() noexcept {
		return this->masterParser->getStringView()[this->masterParser->getStructuralIndices()[this->masterParser->getTapeLength() - 1]];
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

		auto nextStructuralIndex = uint32_t(this->nextStructural - &this->masterParser->getStructuralIndices()[0]);

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
		//std::cout << "CURRENT TAPE: ";
		for (size_t x = 0; x < this->getTapeLength(); ++x) {
			std::cout << "THE INDEX: " << this->structuralIndexes[x] << " THE INDEX'S VALUE: " << this->stringView[this->structuralIndexes[x]]
			<< std::endl;
		}
		return std::forward<Document>(this->getDocument());
	}

	inline Document::Document(JsonIterator&& _iter) noexcept : iterator{ std::forward<JsonIterator>(_iter) } {
	}

	inline Document Document::start(JsonIterator&& iterator) noexcept {
		return Document(std::forward<JsonIterator>(iterator));
	}

	inline void Document::rewind() noexcept {
		iterator.rewind();
	}

	inline std::string Document::to_debug_string() noexcept {
		return iterator.to_string();
	}

	inline JsonifierResult<const char*> Document::current_location() noexcept {
		return iterator.current_location();
	}

	inline int32_t Document::current_depth() const noexcept {
		return iterator.depth();
	}

	inline bool Document::is_alive() noexcept {
		return iterator.is_alive();
	}
	inline ValueIterator Document::resume_value_iterator() noexcept {
		return ValueIterator(&iterator, 1, iterator.root_position());
	}
	inline ValueIterator Document::get_root_value_iterator() noexcept {
		return resume_value_iterator();
	}
	inline JsonifierResult<Object> Document::start_or_resume_object() noexcept {
		if (iterator.at_root()) {
			return get_object();
		} else {
			return Object::resume(resume_value_iterator());
		}
	}
	inline JsonifierResult<Value> Document::get_value() noexcept {
		iterator.assert_at_document_depth();
		switch (*iterator.peek()) {
			case '[':
			case '{':
				return Value(get_root_value_iterator());
			default:
				return ErrorCode::Empty;
		}
	}
	inline JsonifierResult<Array> Document::get_array() & noexcept {
		auto Value = get_root_value_iterator();
		return Array::start_root(Value);
	}
	inline JsonifierResult<Object> Document::get_object() & noexcept {
		auto Value = get_root_value_iterator();
		return Object::start_root(Value);
	}
	inline JsonifierResult<uint64_t> Document::get_uint64() noexcept {
		return get_root_value_iterator().get_root_uint64();
	}
	inline JsonifierResult<uint64_t> Document::get_uint64_in_string() noexcept {
		return get_root_value_iterator().get_root_uint64_in_string();
	}
	inline JsonifierResult<int64_t> Document::get_int64() noexcept {
		return get_root_value_iterator().get_root_int64();
	}
	inline JsonifierResult<int64_t> Document::get_int64_in_string() noexcept {
		return get_root_value_iterator().get_root_int64_in_string();
	}
	inline JsonifierResult<double> Document::get_double() noexcept {
		return get_root_value_iterator().get_root_double();
	}
	inline JsonifierResult<double> Document::get_double_in_string() noexcept {
		return get_root_value_iterator().get_root_double_in_string();
	}
	inline JsonifierResult<std::string_view> Document::get_string() noexcept {
		return get_root_value_iterator().get_root_string();
	}
	inline JsonifierResult<RawJsonString> Document::get_raw_json_string() noexcept {
		return get_root_value_iterator().get_root_raw_json_string();
	}
	inline JsonifierResult<bool> Document::get_bool() noexcept {
		return get_root_value_iterator().get_root_bool();
	}
	inline JsonifierResult<bool> Document::is_null() noexcept {
		return get_root_value_iterator().is_root_null();
	}

	template<> inline JsonifierResult<Array> Document::get() & noexcept {
		return get_array();
	}
	template<> inline JsonifierResult<Object> Document::get() & noexcept {
		return get_object();
	}
	template<> inline JsonifierResult<RawJsonString> Document::get() & noexcept {
		return get_raw_json_string();
	}
	template<> inline JsonifierResult<std::string_view> Document::get() & noexcept {
		return get_string();
	}
	template<> inline JsonifierResult<double> Document::get() & noexcept {
		return get_double();
	}
	template<> inline JsonifierResult<uint64_t> Document::get() & noexcept {
		return get_uint64();
	}
	template<> inline JsonifierResult<int64_t> Document::get() & noexcept {
		return get_int64();
	}
	template<> inline JsonifierResult<bool> Document::get() & noexcept {
		return get_bool();
	}
	template<> inline JsonifierResult<Value> Document::get() & noexcept {
		return get_value();
	}

	template<> inline JsonifierResult<RawJsonString> Document::get() && noexcept {
		return get_raw_json_string();
	}
	template<> inline JsonifierResult<std::string_view> Document::get() && noexcept {
		return get_string();
	}
	template<> inline JsonifierResult<double> Document::get() && noexcept {
		return std::forward<Document>(*this).get_double();
	}
	template<> inline JsonifierResult<uint64_t> Document::get() && noexcept {
		return std::forward<Document>(*this).get_uint64();
	}
	template<> inline JsonifierResult<int64_t> Document::get() && noexcept {
		return std::forward<Document>(*this).get_int64();
	}
	template<> inline JsonifierResult<bool> Document::get() && noexcept {
		return std::forward<Document>(*this).get_bool();
	}
	template<> inline JsonifierResult<Value> Document::get() && noexcept {
		return get_value();
	}

	template<typename T> inline ErrorCode Document::get(T& out) & noexcept {
		return get<T>().get(out);
	}
	template<typename T> inline ErrorCode Document::get(T& out) && noexcept {
		return std::forward<Document>(*this).get<T>().get(out);
	}

	inline JsonIterator::JsonIterator(JsonIterator&& other) noexcept
		: token(std::forward<TokenIterator>(other.token)), parser{ other.parser }, stringBuffer{ other.stringBuffer }, error{ other.error },
		  currentDepth{ other.currentDepth }, rootPosition{ other.rootPosition } {
		other.parser = nullptr;
	}

	inline JsonIterator& JsonIterator::operator=(JsonIterator&& other) noexcept {
		token = other.token;
		parser = other.parser;
		stringBuffer = other.stringBuffer;
		error = other.error;
		currentDepth = other.currentDepth;
		rootPosition = other.rootPosition;
		other.parser = nullptr;
		return *this;
	}

	inline JsonIterator::JsonIterator(JsonifierCore* _parser) noexcept
		: token(_parser->getStringView(), _parser->getStructuralIndices()), parser{ _parser }, stringBuffer{ _parser->getStringBuffer() },
		  currentDepth{ 1 }, rootPosition{ _parser->getStructuralIndices() }

		  {
		std::cout << "THE INDICES: ";
		for (size_t x = 0; x < parser->getTapeLength(); ++x) {
			std::cout << "THE INDEX: " << parser->getStringView()[this->rootPosition[x]] << std::endl;
		}
	};

	inline TokenIterator::TokenIterator(const uint8_t* _bufNew, uint32_t* positionNew) noexcept : buf{ _bufNew }, _position{ positionNew } {
		std::cout << "THE INDICES REAL: ";
		for (size_t x = 0; x < 344; ++x) {
			std::cout << "THE VALUE: " << buf[_position[x]] << std::endl;
		}
	}

	inline void JsonIterator::rewind() noexcept {
		token.set_position(root_position());
		stringBuffer = parser->getStringBuffer();
		currentDepth = 1;
	}

	inline bool JsonIterator::balanced() const noexcept {
		TokenIterator ti(token);
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

	inline ErrorCode JsonIterator::skip_child(size_t parent_depth) noexcept {
		if (depth() <= parent_depth) {
			return ErrorCode::Success;
		}
		switch (*return_current_and_advance()) {\
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

		return report_error(ErrorCode::Tape_Error, "not enough close braces");
	}

	inline bool JsonIterator::at_root() const noexcept {
		return position() == root_position();
	}

	inline bool JsonIterator::is_single_token() const noexcept {
		return parser->getTapeLength() == 1;
	}

	inline uint32_t* JsonIterator::root_position() const noexcept {
		return rootPosition;
	}

	inline void JsonIterator::assert_at_document_depth() const noexcept {
		assert(currentDepth == 1);
	}

	inline void JsonIterator::assert_at_root() const noexcept {
		assert(currentDepth == 1);
	}

	inline void JsonIterator::assert_more_tokens(uint32_t required_tokens) const noexcept {
		assert_valid_position(token._position + required_tokens - 1);
	}

	inline void JsonIterator::assert_valid_position(uint32_t* position) const noexcept {
#ifndef SIMDJSON_CLANG_VISUAL_STUDIO
		assert(position >= &parser->getStructuralIndices()[0]);
		assert(position < &parser->getStructuralIndices()[parser->getTapeLength()]);
#endif
	}

	inline bool JsonIterator::at_end() const noexcept {
		return position() == end_position();
	}
	inline uint32_t* JsonIterator::end_position() const noexcept {
		size_t n_structural_indexes{ parser->getTapeLength() };
		return &parser->getStructuralIndices()[n_structural_indexes];
	}

	inline std::string JsonIterator::to_string() const noexcept {
		if (!is_alive()) {
			return "dead JsonIterator instance";
		}
		const char* current_structural = reinterpret_cast<const char*>(token.peek());
		return std::string("JsonIterator [ depth : ") + std::to_string(currentDepth) + std::string(", structural : '") +
			std::string(current_structural, 1) + std::string("', offset : ") + std::to_string(token.current_offset()) + std::string("', error : ") +
			std::to_string(( int32_t )error) + std::string(" ]");
	}

	inline JsonifierResult<const char*> JsonIterator::current_location() noexcept {
		if (!is_alive()) {
			if (!at_root()) {
				return reinterpret_cast<const char*>(token.peek(-1));
			} else {
				return reinterpret_cast<const char*>(token.peek());
			}
		}
		if (at_end()) {
			return ErrorCode::Out_Of_Bounds;
		}
		return reinterpret_cast<const char*>(token.peek());
	}

	inline bool JsonIterator::is_alive() const noexcept {
		return parser;
	}

	inline void JsonIterator::abandon() noexcept {
		parser = nullptr;
		currentDepth = 0;
	}

	inline const uint8_t* JsonIterator::return_current_and_advance() noexcept {
		return token.return_current_and_advance();
	}

	inline const uint8_t* JsonIterator::unsafe_pointer() const noexcept {
		return token.peek(0);
	}

	inline const uint8_t* JsonIterator::peek(int32_t delta) const noexcept {
		std::cout << "CURRENT PEEK VALUE: " << *token.peek(delta) << std::endl;
		return token.peek(delta);
	}

	inline uint32_t JsonIterator::peek_length(int32_t delta) const noexcept {
		return token.peek_length(delta);
	}

	inline const uint8_t* JsonIterator::peek(uint32_t* position) const noexcept {
		return token.peek(position);
	}

	inline uint32_t JsonIterator::peek_length(uint32_t* position) const noexcept {
		return token.peek_length(position);
	}

	inline uint32_t* JsonIterator::last_position() const noexcept {
		size_t n_structural_indexes{ parser->getTapeLength() };
		assert(n_structural_indexes > 0);
		return &parser->getStructuralIndices()[n_structural_indexes - 1];
	}
	inline const uint8_t* JsonIterator::peek_last() const noexcept {
		return token.peek(last_position());
	}

	inline void JsonIterator::ascend_to(size_t parent_depth) noexcept {
		assert(parent_depth >= 0 && parent_depth < INT32_MAX - 1);
		assert(currentDepth == parent_depth + 1);
		currentDepth = parent_depth;
	}

	inline void JsonIterator::descend_to(size_t child_depth) noexcept {
		assert(child_depth >= 1 && child_depth < INT32_MAX);
		assert(currentDepth == child_depth - 1);
		currentDepth = child_depth;
	}

	inline size_t JsonIterator::depth() const noexcept {
		return currentDepth;
	}

	inline uint8_t*& JsonIterator::string_buf_loc() noexcept {
		return stringBuffer;
	}

	inline ErrorCode JsonIterator::report_error(ErrorCode _error, const char* message) noexcept {
		assert(_error != ErrorCode::Success && _error != Uninitialized&& _error != Incorrect_Type&& _error != No_Such_Field);
		error = _error;
		return error;
	}

	inline uint32_t* JsonIterator::position() const noexcept {
		return token.position();
	}

	inline JsonifierResult<std::string_view> JsonIterator::unescape(RawJsonString in) noexcept {
		return JsonifierResult<std::string_view>{ reinterpret_cast<const char*>(StringParser::parseString(( uint8_t* )(in.raw()), stringBuffer)) };
	}

	inline void JsonIterator::reenter_child(uint32_t* position, size_t child_depth) noexcept {
		assert(child_depth >= 1 && child_depth < INT32_MAX);
		assert(currentDepth == child_depth - 1);
	}

	inline ErrorCode JsonIterator::optional_error(ErrorCode _error, const char* message) noexcept {
		assert(_error == Incorrect_Type || _error == No_Such_Field);
		return _error;
	}

	template<int N>
	inline bool JsonIterator::copy_to_buffer(const uint8_t* json, uint32_t max_len, uint8_t (&tmpbuf)[N]) noexcept {
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

	inline Array::Array(const ValueIterator& _iter) noexcept : iterator{ _iter } {
	}

	inline JsonifierResult<Array> Array::start(ValueIterator& iterator) noexcept {
		bool has_value;
		iterator.start_array().get(has_value);
		return Array(iterator);
	}
	inline JsonifierResult<Array> Array::start_root(ValueIterator& iterator) noexcept {
		bool has_value;
		iterator.start_root_array().get(has_value);
		return Array(iterator);
	}
	inline JsonifierResult<Array> Array::started(ValueIterator& iterator) noexcept {
		bool has_value;
		iterator.started_array().get(has_value);
		return Array(iterator);
	}

	inline JsonifierResult<ArrayIterator> Array::begin() noexcept {
		return ArrayIterator(iterator);
	}
	inline JsonifierResult<ArrayIterator> Array::end() noexcept {
		return ArrayIterator(iterator);
	}
	inline ErrorCode Array::consume() noexcept {
		auto error = iterator.json_iter().skip_child(iterator.depth() - 1);
		if (error) {
			iterator.abandon();
		}
		return error;
	}

	inline JsonifierResult<std::string_view> Array::raw_json() noexcept {
		const uint8_t* starting_point{ iterator.peek_start() };
		auto error = consume();
		if (error) {
			return error;
		}
		const uint8_t* final_point{ iterator.jsonIterator->unsafe_pointer() };
		return std::string_view(reinterpret_cast<const char*>(starting_point), size_t(final_point - starting_point));
	}

	inline JsonifierResult<size_t> Array::count_elements() & noexcept {
		size_t count{ 0 };
		for (auto iterator = this->begin(); iterator != this->end(); ++iterator) {
			count++;
		}
		if (iterator.error()) {
			return iterator.error();
		}
		iterator.reset_array();
		return count;
	}

	inline JsonifierResult<bool> Array::is_empty() & noexcept {
		bool is_not_empty;
		auto error = iterator.reset_array().get(is_not_empty);
		if (error) {
			return error;
		}
		return !is_not_empty;
	}

	inline JsonifierResult<bool> Array::reset() & noexcept {
		return iterator.reset_array();
	}

	template<typename T> inline void JsonifierResultBase<T>::tie(T& value, ErrorCode& error) && noexcept {
		error = this->second;
		if (!error) {
			value = std::forward<JsonifierResultBase<T>>(*this).first;
		}
	}

	template<typename T> inline ErrorCode JsonifierResultBase<T>::get(T& value) && noexcept {
		ErrorCode error;
		std::forward<JsonifierResultBase<T>>(*this).tie(value, error);
		return error;
	}

	template<typename T> inline ErrorCode JsonifierResultBase<T>::error() const noexcept {
		return this->second;
	}

	template<typename T> inline const T& JsonifierResultBase<T>::value_unsafe() const& noexcept {
		return this->first;
	}

	template<typename T> inline T&& JsonifierResultBase<T>::value_unsafe() && noexcept {
		return std::forward<T>(this->first);
	}

	template<typename T>
	inline JsonifierResultBase<T>::JsonifierResultBase(T&& value, ErrorCode error) noexcept
		: std::pair<T, ErrorCode>(std::forward<T>(value), error) {
	}
	template<typename T> inline JsonifierResultBase<T>::JsonifierResultBase(ErrorCode error) noexcept : JsonifierResultBase(T{}, error) {
	}
	template<typename T>
	inline JsonifierResultBase<T>::JsonifierResultBase(T&& value) noexcept : JsonifierResultBase(std::forward<T>(value), Success) {
	}
	template<typename T> inline JsonifierResultBase<T>::JsonifierResultBase() noexcept : JsonifierResultBase(T{}, Uninitialized) {
	}

	template<typename T> inline void JsonifierResult<T>::tie(T& value, ErrorCode& error) && noexcept {
		std::forward<JsonifierResultBase<T>>(*this).tie(value, error);
	}

	template<typename T> inline ErrorCode JsonifierResult<T>::get(T& value) && noexcept {
		return std::forward<JsonifierResultBase<T>>(*this).get(value);
	}

	template<typename T> inline ErrorCode JsonifierResult<T>::error() const noexcept {
		return JsonifierResultBase<T>::error();
	}

	template<typename T> inline const T& JsonifierResult<T>::value_unsafe() const& noexcept {
		return JsonifierResultBase<T>::value_unsafe();
	}

	template<typename T> inline T&& JsonifierResult<T>::value_unsafe() && noexcept {
		return std::forward<JsonifierResultBase<T>>(*this).value_unsafe();
	}

	template<typename T>
	inline JsonifierResult<T>::JsonifierResult(T&& value, ErrorCode error) noexcept
		: JsonifierResultBase<T>(std::forward<T>(value), error) {
	}

	template<typename T> inline JsonifierResult<T>::JsonifierResult(ErrorCode error) noexcept : JsonifierResultBase<T>(error) {
	}

	template<typename T>
	inline JsonifierResult<T>::JsonifierResult(T&& value) noexcept : JsonifierResultBase<T>(std::forward<T>(value)) {
	}

	template<typename T> inline JsonifierResult<T>::JsonifierResult() noexcept : JsonifierResultBase<T>() {
	}

	inline Field::Field() noexcept : std::pair<RawJsonString, Value>() {
	}

	inline Field::Field(RawJsonString key, Value&& value) noexcept
		: std::pair<RawJsonString, Value>(key, std::forward<Value>(value)) {
	}

	inline JsonifierResult<Field> Field::start(ValueIterator& parent_iter) noexcept {
		RawJsonString key;
		parent_iter.field_key().get(key);
		parent_iter.field_value();
		return Field::start(parent_iter, key);
	}

	inline JsonifierResult<Field> Field::start(const ValueIterator& parent_iter, RawJsonString key) noexcept {
		return Field(key, parent_iter.child());
	}

	inline JsonifierResult<std::string_view> Field::unescaped_key() noexcept {
		assert(first.buf != nullptr);
		JsonifierResult<std::string_view> answer = first.unescape(second.iterator.json_iter());
		first.consume();
		return answer;
	}

	inline RawJsonString Field::key() const noexcept {
		assert(first.buf != nullptr);
		return first;
	}

	inline Value& Field::value() & noexcept {
		return second;
	}

	inline Value Field::value() && noexcept {
		return std::forward<Field>(*this).second;
	}

	inline JsonifierResult<Value> Object::find_field_unordered(const std::string_view key) & noexcept {
		bool has_value;
		iterator.find_field_unordered_raw(key).get(has_value);
		if (!has_value) {
			return No_Such_Field;
		}
		return Value(iterator.child());
	}
	inline JsonifierResult<Value> Object::find_field_unordered(const std::string_view key) && noexcept {
		bool has_value;
		iterator.find_field_unordered_raw(key).get(has_value);
		if (!has_value) {
			return No_Such_Field;
		}
		return Value(iterator.child());
	}
	inline JsonifierResult<Value> Object::operator[](const std::string_view key) & noexcept {
		return find_field_unordered(key);
	}
	inline JsonifierResult<Value> Object::operator[](const std::string_view key) && noexcept {
		return std::forward<Object>(*this).find_field_unordered(key);
	}
	inline JsonifierResult<Value> Object::find_field(const std::string_view key) & noexcept {
		bool has_value;
		iterator.find_field_raw(key).get(has_value);
		if (!has_value) {
			return No_Such_Field;
		}
		return Value(iterator.child());
	}
	inline JsonifierResult<Value> Object::find_field(const std::string_view key) && noexcept {
		bool has_value;
		iterator.find_field_raw(key).get(has_value);
		if (!has_value) {
			return No_Such_Field;
		}
		return Value(iterator.child());
	}

	inline JsonifierResult<Object> Object::start(ValueIterator& iterator) noexcept {
		iterator.start_object().error();
		return Object(iterator);
	}
	inline JsonifierResult<Object> Object::start_root(ValueIterator& iterator) noexcept {
		iterator.start_root_object().error();
		return Object(iterator);
	}
	inline ErrorCode Object::consume() noexcept {
		if (iterator.is_at_key()) {
			RawJsonString actual_key;
			auto error = iterator.field_key().get(actual_key);
			if (error) {
				iterator.abandon();
				return error;
			};
			if ((error = iterator.field_value())) {
				iterator.abandon();
				return error;
			}
		}
		auto error_skip = iterator.json_iter().skip_child(iterator.depth() - 1);
		if (error_skip) {
			iterator.abandon();
		}
		return error_skip;
	}

	inline JsonifierResult<std::string_view> Object::raw_json() noexcept {
		const uint8_t* starting_point{ iterator.peek_start() };
		auto error = consume();
		if (error) {
			return error;
		}
		const uint8_t* final_point{ iterator.jsonIterator->peek(0) };
		return std::string_view(reinterpret_cast<const char*>(starting_point), size_t(final_point - starting_point));
	}

	inline JsonifierResult<Object> Object::started(ValueIterator& iterator) noexcept {
		iterator.started_object().error();
		return Object(iterator);
	}

	inline Object Object::resume(const ValueIterator& iterator) noexcept {
		return iterator;
	}

	inline Object::Object(const ValueIterator& _iter) noexcept : iterator{ _iter } {
	}

	inline JsonifierResult<ObjectIterator> Object::begin() noexcept {
		return ObjectIterator(iterator);
	}
	inline JsonifierResult<ObjectIterator> Object::end() noexcept {
		return ObjectIterator(iterator);
	}

	inline JsonifierResult<size_t> Object::count_fields() & noexcept {
		size_t count{ 0 };
		for (auto v: *this) {
			count++;
		}
		if (iterator.error()) {
			return iterator.error();
		}
		iterator.reset_object();
		return count;
	}

	inline JsonifierResult<bool> Object::is_empty() & noexcept {
		bool is_not_empty;
		auto error = iterator.reset_object().get(is_not_empty);
		if (error) {
			return error;
		}
		return !is_not_empty;
	}

	inline JsonifierResult<bool> Object::reset() & noexcept {
		return iterator.reset_object();
	}
	inline JsonifierResult<ObjectIterator>::JsonifierResult(
		ObjectIterator&& value) noexcept
		:JsonifierResultBase<ObjectIterator>(
			  std::forward<ObjectIterator>(value)) {
		first.iterator.assert_is_valid();
	}
	inline JsonifierResult<ObjectIterator>::JsonifierResult(ErrorCode error) noexcept
		:JsonifierResultBase<ObjectIterator>({}, error) {
	}

	inline JsonifierResult<Field>
	JsonifierResult<ObjectIterator>::operator*() noexcept {
		if (error()) {
			return error();
		}
		return *first;
	}
	inline bool JsonifierResult<ObjectIterator>::operator==(
		const JsonifierResult<ObjectIterator>& other) const noexcept {
		if (!first.iterator.is_valid()) {
			return !error();
		}
		return first == other.first;
	}
	inline bool JsonifierResult<ObjectIterator>::operator!=(
		const JsonifierResult<ObjectIterator>& other) const noexcept {
		if (!first.iterator.is_valid()) {
			return error();
		}
		return first != other.first;
	}
	inline JsonifierResult<ObjectIterator>&
	JsonifierResult<ObjectIterator>::operator++() noexcept {
		if (error()) {
			second = Success;
			return *this;
		}
		++first;
		return *this;
	}

	inline ValueIterator::ValueIterator(JsonIterator* json_iter, size_t depth, uint32_t* start_position) noexcept
		: jsonIterator{ json_iter }, currentDepth{ depth }, rootPosition{ start_position } {
	}

	inline JsonifierResult<bool> ValueIterator::start_object() noexcept {
		start_container('{', "Not an object", "object");
		return started_object();
	}

	inline JsonifierResult<bool> ValueIterator::start_root_object() noexcept {
		start_container('{', "Not an object", "object");
		return started_root_object();
	}

	inline JsonifierResult<bool> ValueIterator::started_object() noexcept {
		assert_at_container_start();
		if (*jsonIterator->peek() == '}') {
			jsonIterator->return_current_and_advance();
			end_container();
			return false;
		}
		return true;
	}

	inline JsonifierResult<bool> ValueIterator::started_root_object() noexcept {
		return started_object();
	}

	inline ErrorCode ValueIterator::end_container() noexcept {
		jsonIterator->ascend_to(depth() - 1);
		return Success;
	}

	inline JsonifierResult<bool> ValueIterator::has_next_field() noexcept {
		assert_at_next();

		switch (*jsonIterator->return_current_and_advance()) {
			case '}':
				end_container();
				return false;
			case ',':
				return true;
			default:
				return report_error(Tape_Error, "Missing comma between object fields");
		}
	}

	inline JsonifierResult<bool> ValueIterator::find_field_raw(const std::string_view key) noexcept {
		ErrorCode error;
		bool has_value;
		if (at_first_field()) {
			has_value = true;
		} else if (!is_open()) {
			return false;
		} else {
			if ((error = skip_child())) {
				abandon();
				return error;
			}
			if ((error = has_next_field().get(has_value))) {
				abandon();
				return error;
			}
		}
		while (has_value) {
			RawJsonString actual_key;
			if ((error = field_key().get(actual_key))) {
				abandon();
				return error;
			};
			if ((error = field_value())) {
				abandon();
				return error;
			}
			if (actual_key.unsafe_is_equal(key)) {
				return true;
			}

			skip_child();
			if ((error = has_next_field().get(has_value))) {
				abandon();
				return error;
			}
		}
		return false;
	}

	inline JsonifierResult<bool> ValueIterator::find_field_unordered_raw(const std::string_view key) noexcept {
		ErrorCode error;
		bool has_value;
		uint32_t* search_start = jsonIterator->position();
		bool at_first = at_first_field();
		if (at_first) {
			has_value = true;
		} else if (!is_open()) {
			reset_object().get(has_value);
			at_first = true;
		} else {
			if ((error = skip_child())) {
				abandon();
				return error;
			}
			search_start = jsonIterator->position();
			if ((error = has_next_field().get(has_value))) {
				abandon();
				return error;
			}
		}
		while (has_value) {
			assert(jsonIterator->currentDepth == currentDepth);
			RawJsonString actual_key;
			if ((error = field_key().get(actual_key))) {
				abandon();
				return error;
			};
			if ((error = field_value())) {
				abandon();
				return error;
			}
			if (actual_key.unsafe_is_equal(key)) {
				return true;
			}

			skip_child();
			if ((error = has_next_field().get(has_value))) {
				abandon();
				return error;
			}
		}
		if (at_first) {
			return false;
		}

		reset_object().get(has_value);
		while (true) {
			assert(has_value);
			assert(jsonIterator->currentDepth == currentDepth);
			RawJsonString actual_key;
			error = field_key().get(actual_key);
			assert(!error);
			error = field_value();
			assert(!error);
			if (actual_key.unsafe_is_equal(key)) {
				return true;
			}

			skip_child();
			if (jsonIterator->position() == search_start) {
				return false;
			}
			error = has_next_field().get(has_value);
			assert(!error);
		}
		return false;
	}

	inline JsonifierResult<RawJsonString> ValueIterator::field_key() noexcept {
		assert_at_next();

		const uint8_t* key = jsonIterator->return_current_and_advance();
		if (*(key++) != '"') {
			return report_error(Tape_Error, "Object key is not a string");
		}
		return RawJsonString(key);
	}

	inline ErrorCode ValueIterator::field_value() noexcept {
		assert_at_next();

		if (*jsonIterator->return_current_and_advance() != ':') {
			return report_error(Tape_Error, "Missing colon in object Field");
		}
		jsonIterator->descend_to(depth() + 1);
		return Success;
	}

	inline JsonifierResult<bool> ValueIterator::start_array() noexcept {
		start_container('[', "Not an array", "array");
		return started_array();
	}

	inline JsonifierResult<bool> ValueIterator::start_root_array() noexcept {
		start_container('[', "Not an array", "array");
		return started_root_array();
	}

	inline std::string ValueIterator::to_string() const noexcept {
		auto answer = std::string("ValueIterator [ depth : ") + std::to_string(currentDepth) + std::string(", ");
		if (jsonIterator != nullptr) {
			answer += jsonIterator->to_string();
		}
		answer += std::string(" ]");
		return answer;
	}

	inline JsonifierResult<bool> ValueIterator::started_array() noexcept {
		assert_at_container_start();
		if (*jsonIterator->peek() == ']') {
			jsonIterator->return_current_and_advance();
			end_container();
			return false;
		}
		jsonIterator->descend_to(depth() + 1);
		return true;
	}

	inline JsonifierResult<bool> ValueIterator::started_root_array() noexcept {
		return started_array();
	}

	inline JsonifierResult<bool> ValueIterator::has_next_element() noexcept {
		assert_at_next();

		switch (*jsonIterator->return_current_and_advance()) {
			case ']':
				end_container();
				return false;
			case ',':
				jsonIterator->descend_to(depth() + 1);
				return true;
			default:
				return report_error(Tape_Error, "Missing comma between array elements");
		}
	}

	inline JsonifierResult<bool> ValueIterator::parse_bool(const uint8_t* json) const noexcept {
		auto not_true = StringParser::str4ncmp(json, "true");
		auto not_false = StringParser::str4ncmp(json, "fals") | (json[4] ^ 'e');
		bool error = (not_true && not_false) || NumberParser::isNotStructuralOrWhitespace(json[not_true ? 5 : 4]);
		if (error) {
			return incorrect_type_error("Not a boolean");
		}
		return JsonifierResult<bool>(!not_true);
	}
	inline JsonifierResult<bool> ValueIterator::parse_null(const uint8_t* json) const noexcept {
		bool is_null_string = !StringParser::str4ncmp(json, "null") && NumberParser::isNotStructuralOrWhitespace(json[4]);
		if (!is_null_string && json[0] == 'n') {
			return incorrect_type_error("Not a null but starts with n");
		}
		return is_null_string;
	}

	inline JsonifierResult<std::string_view> ValueIterator::get_string() noexcept {
		return get_raw_json_string().unescape(json_iter());
	}
	inline JsonifierResult<RawJsonString> ValueIterator::get_raw_json_string() noexcept {
		auto json = peek_scalar("string");
		if (*json != '"') {
			return incorrect_type_error("Not a string");
		}
		advance_scalar("string");
		return RawJsonString(json + 1);
	}
	inline JsonifierResult<uint64_t> ValueIterator::get_uint64() noexcept {
		auto result = NumberParser::parseUnsigned(peek_non_root_scalar("uint64"));
		advance_non_root_scalar("uint64");
		return result;
	}
	inline JsonifierResult<int64_t> ValueIterator::get_int64() noexcept {
		auto result = NumberParser::parseInteger(peek_non_root_scalar("int64"));
		advance_non_root_scalar("int64");
		return result;
	}
	inline JsonifierResult<double> ValueIterator::get_double() noexcept {
		auto result = NumberParser::parseDouble(peek_non_root_scalar("double"));
		advance_non_root_scalar("double");
		return result;
	}
	inline JsonifierResult<bool> ValueIterator::get_bool() noexcept {
		auto result = parse_bool(peek_non_root_scalar("bool"));
		if (result.error() == Success) {
			advance_non_root_scalar("bool");
		}
		return result;
	}
	inline JsonifierResult<bool> ValueIterator::is_null() noexcept {
		bool is_null_value;
		parse_null(peek_non_root_scalar("null")).get(is_null_value);
		if (is_null_value) {
			advance_non_root_scalar("null");
		}
		return is_null_value;
	}

	inline JsonifierResult<std::string_view> ValueIterator::get_root_string() noexcept {
		return get_string();
	}
	inline JsonifierResult<RawJsonString> ValueIterator::get_root_raw_json_string() noexcept {
		return get_raw_json_string();
	}
	inline JsonifierResult<uint64_t> ValueIterator::get_root_uint64() noexcept {
		auto max_len = peek_start_length();
		auto json = peek_root_scalar("uint64");
		uint8_t tmpbuf[20 + 1];
		if (!jsonIterator->copy_to_buffer(json, max_len, tmpbuf)) {
			return Number_Error;
		}
		auto result = NumberParser::parseUnsigned(tmpbuf);
		if (!jsonIterator->is_single_token()) {
			return Trailing_Content;
		}
		advance_root_scalar("uint64");
		return result;
	}

	inline JsonifierResult<int64_t> ValueIterator::get_root_int64() noexcept {
		auto max_len = peek_start_length();
		auto json = peek_root_scalar("int64");
		uint8_t tmpbuf[20 + 1];
		if (!jsonIterator->copy_to_buffer(json, max_len, tmpbuf)) {
			return Number_Error;
		}

		auto result = NumberParser::parseInteger(tmpbuf);
		if (!jsonIterator->is_single_token()) {
			return Trailing_Content;
		}
		advance_root_scalar("int64");
		return result;
	}

	inline JsonifierResult<double> ValueIterator::get_root_double() noexcept {
		auto max_len = peek_start_length();
		auto json = peek_root_scalar("double");
		uint8_t tmpbuf[1074 + 8 + 1];
		if (!jsonIterator->copy_to_buffer(json, max_len, tmpbuf)) {
			return Number_Error;
		}
		auto result = NumberParser::parseDouble(tmpbuf);
		if (!jsonIterator->is_single_token()) {
			return Trailing_Content;
		}
		advance_root_scalar("double");
		return result;
	}

	inline JsonifierResult<bool> ValueIterator::get_root_bool() noexcept {
		auto max_len = peek_start_length();
		auto json = peek_root_scalar("bool");
		uint8_t tmpbuf[5 + 1];
		if (!jsonIterator->copy_to_buffer(json, max_len, tmpbuf)) {
			return incorrect_type_error("Not a boolean");
		}
		auto result = parse_bool(tmpbuf);
		if (result.error() == Success) {
			if (!jsonIterator->is_single_token()) {
				return Trailing_Content;
			}
			advance_root_scalar("bool");
		}
		return result;
	}

	inline bool ValueIterator::is_root_null() noexcept {
		if (!jsonIterator->is_single_token()) {
			return false;
		}
		auto max_len = peek_start_length();
		auto json = peek_root_scalar("null");
		bool result =
			(max_len >= 4 && !StringParser::str4ncmp(json, "null") && (max_len == 4 || !NumberParser::isNotStructuralOrWhitespace(json[5])));
		if (result) {
			advance_root_scalar("null");
		}
		return result;
	}

	inline ErrorCode ValueIterator::skip_child() noexcept {
		assert(jsonIterator->token._position > rootPosition);
		assert(jsonIterator->currentDepth >= currentDepth);

		return jsonIterator->skip_child(depth());
	}

	inline ValueIterator ValueIterator::child() const noexcept {
		assert_at_child();
		return { jsonIterator, static_cast<size_t>(depth() + 1), jsonIterator->token.position() };
	}

	
	inline bool ValueIterator::is_open() const noexcept {
		return jsonIterator->depth() >= depth();
	}

	inline bool ValueIterator::at_end() const noexcept {
		return jsonIterator->at_end();
	}

	inline bool ValueIterator::at_start() const noexcept {
		return jsonIterator->token.position() == start_position();
	}

	inline bool ValueIterator::at_first_field() const noexcept {
		assert(jsonIterator->token._position > rootPosition);
		return jsonIterator->token.position() == start_position() + 1;
	}

	inline void ValueIterator::abandon() noexcept {
		jsonIterator->abandon();
	}

	inline int32_t ValueIterator::depth() const noexcept {
		return currentDepth;
	}

	inline ErrorCode ValueIterator::error() const noexcept {
		return jsonIterator->error;
	}
	inline uint8_t*& ValueIterator::string_buf_loc() noexcept {
		return jsonIterator->string_buf_loc();
	}
	inline const JsonIterator& ValueIterator::json_iter() const noexcept {
		return *jsonIterator;
	}
	inline JsonIterator& ValueIterator::json_iter() noexcept {
		return *jsonIterator;
	}

	inline const uint8_t* ValueIterator::peek_start() const noexcept {
		return jsonIterator->peek(start_position());
	}
	inline uint32_t ValueIterator::peek_start_length() const noexcept {
		return jsonIterator->peek_length(start_position());
	}

	inline const uint8_t* ValueIterator::peek_scalar(const char* type) noexcept {
		if (!is_at_start()) {
			return peek_start();
		}
		assert_at_start();
		return jsonIterator->peek();
	}

	inline void ValueIterator::advance_scalar(const char* type) noexcept {
		if (!is_at_start()) {
			return;
		}
		assert_at_start();
		jsonIterator->return_current_and_advance();
		jsonIterator->ascend_to(depth() - 1);
	}

	inline ErrorCode ValueIterator::start_container(uint8_t start_char, const char* incorrect_type_message, const char* type) noexcept {
		const uint8_t* json;
		if (!is_at_start()) {
			json = peek_start();
			if (*json != start_char) {
				return incorrect_type_error(incorrect_type_message);
			}
		} else {
			assert_at_start();
			json = jsonIterator->peek();
			if (*json != start_char) {
				return incorrect_type_error(incorrect_type_message);
			}
			jsonIterator->return_current_and_advance();
		}


		return Success;
	}


	inline const uint8_t* ValueIterator::peek_root_scalar(const char* type) noexcept {
		if (!is_at_start()) {
			return peek_start();
		}

		assert_at_root();
		return jsonIterator->peek();
	}
	inline const uint8_t* ValueIterator::peek_non_root_scalar(const char* type) noexcept {
		if (!is_at_start()) {
			return peek_start();
		}

		assert_at_non_root_start();
		return jsonIterator->peek();
	}

	inline void ValueIterator::advance_root_scalar(const char* type) noexcept {
		if (!is_at_start()) {
			return;
		}

		assert_at_root();
		jsonIterator->return_current_and_advance();
		jsonIterator->ascend_to(depth() - 1);
	}
	inline void ValueIterator::advance_non_root_scalar(const char* type) noexcept {
		if (!is_at_start()) {
			return;
		}

		assert_at_non_root_start();
		jsonIterator->return_current_and_advance();
		jsonIterator->ascend_to(depth() - 1);
	}

	inline ErrorCode ValueIterator::incorrect_type_error(const char* message) const noexcept {
		return Incorrect_Type;
	}

	inline bool ValueIterator::is_at_start() const noexcept {
		return position() == start_position();
	}

	inline bool ValueIterator::is_at_key() const noexcept {
		return currentDepth == jsonIterator->currentDepth && *jsonIterator->peek() == '"';
	}

	inline bool ValueIterator::is_at_iterator_start() const noexcept {
		auto delta = position() - start_position();
		return delta == 1 || delta == 2;
	}

	inline void ValueIterator::assert_at_start() const noexcept {
		assert(jsonIterator->token._position == rootPosition);
		assert(jsonIterator->currentDepth == currentDepth);
		assert(currentDepth > 0);
	}

	inline void ValueIterator::assert_at_container_start() const noexcept {
		assert(jsonIterator->token._position == rootPosition + 1);
		assert(jsonIterator->currentDepth == currentDepth);
		assert(currentDepth > 0);
	}

	inline void ValueIterator::assert_at_next() const noexcept {
		assert(jsonIterator->token._position > rootPosition);
		assert(jsonIterator->currentDepth == currentDepth);
		assert(currentDepth > 0);
	}

	inline void ValueIterator::move_at_start() noexcept {
		jsonIterator->currentDepth = currentDepth;
		jsonIterator->token.set_position(rootPosition);
	}

	inline void ValueIterator::move_at_container_start() noexcept {
		jsonIterator->currentDepth = currentDepth;
		jsonIterator->token.set_position(rootPosition + 1);
	}

	inline JsonifierResult<bool> ValueIterator::reset_array() noexcept {
		move_at_container_start();
		return started_array();
	}

	inline JsonifierResult<bool> ValueIterator::reset_object() noexcept {
		move_at_container_start();
		return started_object();
	}

	inline void ValueIterator::assert_at_child() const noexcept {
		assert(jsonIterator->token._position > rootPosition);
		assert(jsonIterator->currentDepth == currentDepth + 1);
		assert(currentDepth > 0);
	}

	inline void ValueIterator::assert_at_root() const noexcept {
		assert_at_start();
		assert(currentDepth == 1);
	}

	inline void ValueIterator::assert_at_non_root_start() const noexcept {
		assert_at_start();
		assert(currentDepth > 1);
	}

	inline void ValueIterator::assert_is_valid() const noexcept {
		assert(jsonIterator != nullptr);
	}

	inline bool ValueIterator::is_valid() const noexcept {
		return jsonIterator != nullptr;
	}

	inline JsonifierResult<JsonType> ValueIterator::type() const noexcept {
		switch (*peek_start()) {
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
				return JsonType::Number;
			default:
				return Tape_Error;
		}
	}

	inline uint32_t* ValueIterator::start_position() const noexcept {
		return rootPosition;
	}

	inline uint32_t* ValueIterator::position() const noexcept {
		return jsonIterator->position();
	}

	inline uint32_t* ValueIterator::end_position() const noexcept {
		return jsonIterator->end_position();
	}

	inline uint32_t* ValueIterator::last_position() const noexcept {
		return jsonIterator->last_position();
	}

	inline ErrorCode ValueIterator::report_error(ErrorCode error, const char* message) noexcept {
		return jsonIterator->report_error(error, message);
	}

	inline ObjectIterator::ObjectIterator(const ValueIterator& _iter) noexcept : iterator{ _iter } {
	}

	inline JsonifierResult<Field> ObjectIterator::operator*() noexcept {
		ErrorCode error = iterator.error();
		if (error) {
			iterator.abandon();
			return error;
		}
		auto result = Field::start(iterator);
		if (result.error()) {
			iterator.abandon();
		}
		return result;
	}
	inline bool ObjectIterator::operator==(const ObjectIterator& other) const noexcept {
		return !(*this != other);
	}
	inline bool ObjectIterator::operator!=(const ObjectIterator&) const noexcept {
		return iterator.is_open();
	}

	inline ObjectIterator& ObjectIterator::operator++() noexcept {
		if (!iterator.is_open()) {
			return *this;
		}

		ErrorCode error;
		if ((error = iterator.skip_child())) {
			return *this;
		}

		bool has_value;
		if ((error = iterator.has_next_field().get(has_value))) {
			return *this;
		};
		return *this;
	}

	inline uint32_t TokenIterator::current_offset() const noexcept {
		return *(_position);
	}


	inline const uint8_t* TokenIterator::return_current_and_advance() noexcept {
		return &buf[*(_position++)];
	}

	inline const uint8_t* TokenIterator::peek(uint32_t* position) const noexcept {
		return &buf[*position];
	}
	inline uint32_t TokenIterator::peek_index(uint32_t* position) const noexcept {
		return *position;
	}
	inline uint32_t TokenIterator::peek_length(uint32_t* position) const noexcept {
		return *(position + 1) - *position;
	}

	inline const uint8_t* TokenIterator::peek(int32_t delta) const noexcept {
		return &buf[*(_position + delta)];
	}
	inline uint32_t TokenIterator::peek_index(int32_t delta) const noexcept {
		return *(_position + delta);
	}
	inline uint32_t TokenIterator::peek_length(int32_t delta) const noexcept {
		return *(_position + delta + 1) - *(_position + delta);
	}

	inline uint32_t* TokenIterator::position() const noexcept {
		return _position;
	}
	inline void TokenIterator::set_position(uint32_t* target_position) noexcept {
		_position = target_position;
	}

	inline bool TokenIterator::operator==(const TokenIterator& other) const noexcept {
		return _position == other._position;
	}
	inline bool TokenIterator::operator!=(const TokenIterator& other) const noexcept {
		return _position != other._position;
	}
	inline bool TokenIterator::operator>(const TokenIterator& other) const noexcept {
		return _position > other._position;
	}
	inline bool TokenIterator::operator>=(const TokenIterator& other) const noexcept {
		return _position >= other._position;
	}
	inline bool TokenIterator::operator<(const TokenIterator& other) const noexcept {
		return _position < other._position;
	}
	inline bool TokenIterator::operator<=(const TokenIterator& other) const noexcept {
		return _position <= other._position;
	}

	
	inline JsonifierResult<RawJsonString>::JsonifierResult(
		RawJsonString&& value) noexcept
		: JsonifierResultBase<RawJsonString>(
			  std::forward<RawJsonString>(value)) {
	}
	inline JsonifierResult<RawJsonString>::JsonifierResult(ErrorCode error) noexcept : JsonifierResultBase<RawJsonString>(error) {
	}

	inline JsonifierResult<const char*> JsonifierResult<RawJsonString>::raw() const noexcept {
		if (error()) {
			return error();
		}
		return first.raw();
	}
	inline JsonifierResult<std::string_view>
	JsonifierResult<RawJsonString>::unescape(
		JsonIterator& iterator) const noexcept {
		if (error()) {
			return error();
		}
		return first.unescape(iterator);
	}

	inline RawJsonString::RawJsonString(const uint8_t* _buf) noexcept : buf{ _buf } {
	}

	inline const char* RawJsonString::raw() const noexcept {
		return reinterpret_cast<const char*>(buf);
	}


	inline bool RawJsonString::is_free_from_unescaped_quote(std::string_view target) noexcept {
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

	inline bool RawJsonString::is_free_from_unescaped_quote(const char* target) noexcept {
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

	inline bool RawJsonString::is_equal(std::string_view target) const noexcept {
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


	inline bool RawJsonString::unsafe_is_equal(const char* target) const noexcept {
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

	inline bool RawJsonString::is_equal(const char* target) const noexcept {
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

	inline Value::Value(const ValueIterator& _iter) noexcept : iterator{ _iter } {
	}
	inline Value Value::start(const ValueIterator& iterator) noexcept {
		return iterator;
	}
	inline Value Value::resume(const ValueIterator& iterator) noexcept {
		return iterator;
	}

	inline JsonifierResult<Array> Value::get_array() noexcept {
		return Array::start(iterator);
	}
	inline JsonifierResult<Object> Value::get_object() noexcept {
		return Object::start(iterator);
	}
	inline JsonifierResult<Object> Value::start_or_resume_object() noexcept {
		if (iterator.at_start()) {
			return get_object();
		} else {
			return Object::resume(iterator);
		}
	}

	inline JsonifierResult<RawJsonString> Value::get_raw_json_string() noexcept {
		return iterator.get_raw_json_string();
	}
	inline JsonifierResult<std::string_view> Value::get_string() noexcept {
		return iterator.get_string();
	}
	inline JsonifierResult<double> Value::get_double() noexcept {
		return iterator.get_double();
	}
	inline JsonifierResult<double> Value::get_double_in_string() noexcept {
		return iterator.get_double_in_string();
	}
	inline JsonifierResult<uint64_t> Value::get_uint64() noexcept {
		return iterator.get_uint64();
	}
	inline JsonifierResult<uint64_t> Value::get_uint64_in_string() noexcept {
		return iterator.get_uint64_in_string();
	}
	inline JsonifierResult<int64_t> Value::get_int64() noexcept {
		return iterator.get_int64();
	}
	inline JsonifierResult<int64_t> Value::get_int64_in_string() noexcept {
		return iterator.get_int64_in_string();
	}
	inline JsonifierResult<bool> Value::get_bool() noexcept {
		return iterator.get_bool();
	}
	inline JsonifierResult<bool> Value::is_null() noexcept {
		return iterator.is_null();
	}
	template<> inline JsonifierResult<Array> Value::get() noexcept {
		return get_array();
	}
	template<> inline JsonifierResult<Object> Value::get() noexcept {
		return get_object();
	}
	template<> inline JsonifierResult<RawJsonString> Value::get() noexcept {
		return get_raw_json_string();
	}
	template<> inline JsonifierResult<std::string_view> Value::get() noexcept {
		return get_string();
	}
	template<> inline JsonifierResult<double> Value::get() noexcept {
		return get_double();
	}
	template<> inline JsonifierResult<uint64_t> Value::get() noexcept {
		return get_uint64();
	}
	template<> inline JsonifierResult<int64_t> Value::get() noexcept {
		return get_int64();
	}
	template<> inline JsonifierResult<bool> Value::get() noexcept {
		return get_bool();
	}

	template<typename T> inline ErrorCode Value::get(T& out) noexcept {
		return get<T>().get(out);
	}
		inline JsonifierResult<JsonType> Value::type() noexcept {
		return iterator.type();
	}

	inline JsonifierResult<bool> Value::is_scalar() noexcept {
		JsonType this_type;
		auto error = type().get(this_type);
		if (error) {
			return error;
		}
		return !((this_type == JsonType::Array) || (this_type == JsonType::Object));
	}

	inline bool Value::is_negative() noexcept {
		return iterator.is_negative();
	}

	inline JsonifierResult<bool> Value::is_integer() noexcept {
		return iterator.is_integer();
	}

	inline std::string_view Value::raw_json_token() noexcept {
		return std::string_view(reinterpret_cast<const char*>(iterator.peek_start()), iterator.peek_start_length());
	}

	inline JsonifierResult<const char*> Value::current_location() noexcept {
		return iterator.json_iter().current_location();
	}

	inline int32_t Value::current_depth() const noexcept {
		return iterator.json_iter().depth();
	}

	inline JsonifierResult<Value> Document::operator[](std::string_view key) & noexcept {
		return start_or_resume_object()[key];
	}
	inline JsonifierResult<Value> Document::operator[](const char* key) & noexcept {
		return start_or_resume_object()[key];
	}

	inline JsonifierResult<Object>::JsonifierResult(
		Object&& value) noexcept
		: JsonifierResultBase<Object>(
			  std::forward<Object>(value)) {
	}
	inline JsonifierResult<Object>::JsonifierResult(ErrorCode error) noexcept
		: JsonifierResultBase<Object>(error) {
	}

	inline JsonifierResult<ObjectIterator>
	JsonifierResult<Object>::begin() noexcept {
		if (error()) {
			return error();
		}
		return first.begin();
	}
	inline JsonifierResult<ObjectIterator>
	JsonifierResult<Object>::end() noexcept {
		if (error()) {
			return error();
		}
		return first.end();
	}
	inline JsonifierResult<Value>
	JsonifierResult<Object>::find_field_unordered(std::string_view key) & noexcept {
		if (error()) {
			return error();
		}
		return first.find_field_unordered(key);
	}
	inline JsonifierResult<Value>
	JsonifierResult<Object>::find_field_unordered(std::string_view key) && noexcept {
		if (error()) {
			return error();
		}
		return std::forward<Object>(first).find_field_unordered(key);
	}
	inline JsonifierResult<Value>
	JsonifierResult<Object>::operator[](std::string_view key) & noexcept {
		if (error()) {
			return error();
		}
		return first[key];
	}
	inline JsonifierResult<Value>
	JsonifierResult<Object>::operator[](std::string_view key) && noexcept {
		if (error()) {
			return error();
		}
		return std::forward<Object>(first)[key];
	}
	inline JsonifierResult<Value>
	JsonifierResult<Object>::find_field(std::string_view key) & noexcept {
		if (error()) {
			return error();
		}
		return first.find_field(key);
	}
	inline JsonifierResult<Value>
	JsonifierResult<Object>::find_field(std::string_view key) && noexcept {
		if (error()) {
			return error();
		}
		return std::forward<Object>(first).find_field(key);
	}

	inline JsonifierResult<Value>
	JsonifierResult<Object>::at_pointer(std::string_view json_pointer) noexcept {
		if (error()) {
			return error();
		}
		return first.at_pointer(json_pointer);
	}

	inline JsonifierResult<bool> JsonifierResult<Object>::reset() noexcept {
		if (error()) {
			return error();
		}
		return first.reset();
	}

	inline JsonifierResult<bool> JsonifierResult<Object>::is_empty() noexcept {
		if (error()) {
			return error();
		}
		return first.is_empty();
	}

	inline JsonifierResult<size_t> JsonifierResult<Object>::count_fields() & noexcept {
		if (error()) {
			return error();
		}
		return first.count_fields();
	}

	inline JsonifierResult<size_t> Value::count_fields() & noexcept {
		JsonifierResult<size_t> answer;
		auto a = get_object();
		answer = a.count_fields();
		iterator.move_at_start();
		return answer;
	}


	inline JsonifierResult<size_t> Document::count_fields() & noexcept {
		auto a = get_object();
		JsonifierResult<size_t> answer = a.count_fields();
		/* If there was an object, we are now left pointing at its first element. */
		if (answer.error() == Success) {
			rewind();
		}
		return answer;
	}

	inline ErrorCode Document::consume() noexcept {
		auto error = iterator.skip_child(0);
		if (error) {
			iterator.abandon();
		}
		return error;
	}

	inline JsonifierResult<std::string_view> Document::raw_json() noexcept {
		auto _iter = get_root_value_iterator();
		const uint8_t* starting_point{ _iter.peek_start() };
		auto error = consume();
		if (error) {
			return error;
		}
		const uint8_t* final_point{ iterator.unsafe_pointer() };
		return std::string_view(reinterpret_cast<const char*>(starting_point), size_t(final_point - starting_point));
	}

	inline JsonifierResult<Value> Value::operator[](std::string_view key) noexcept {
		return start_or_resume_object()[key];
	}
	inline JsonifierResult<Value> Value::operator[](const char* key) noexcept {
		return start_or_resume_object()[key];
	}
};
