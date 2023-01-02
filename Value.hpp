#pragma once

#include "JsonValueBase.hpp"
#include "Array.hpp"

namespace Jsonifier {

	class JsonIterator;

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
				case ErrorCode::TAtom_Error: {
					return "TAtom Error";
				}
				case ErrorCode::FAtom_Error: {
					return "FAtom Error";
				}
				case ErrorCode::NAtom_Error: {
					return "NAtom Error";
				}
				case ErrorCode::Mem_Alloc_Error: {
					return "Mem Alloc";
				}
				case ErrorCode::Invalid_Number: {
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
		inline RawJsonString(uint8_t* _buf) noexcept;
		inline char* raw() const noexcept;
		inline bool unsafeIsEqual(size_t length, std::string_view target) const noexcept;
		inline bool unsafeIsEqual(std::string_view target) const noexcept;
		inline bool unsafeIsEqual(const char* target) const noexcept;
		inline std::string_view unescape(JsonIterator& iterator) noexcept;

	  protected:
		uint8_t* stringView{};
		friend class JsonIterator;
		friend class JsonValueBase;
		friend class Field;
		friend class JsonIterator;
	};

	class ValueIterator {
	  protected:
		JsonIterator* jsonIterator{};
		uint32_t* rootPosition{};
		size_t currentDepth{};
		

	  public:
		inline ValueIterator() noexcept = default;
		inline void startDocument() noexcept;
		inline ErrorCode skipChild() noexcept;
		inline bool atEnd() const noexcept;
		inline bool atStart() const noexcept;
		inline bool isOpen() const noexcept;
		inline bool atFirstField() const noexcept;
		inline void abandon() noexcept;
		inline ValueIterator childValue() const noexcept;
		inline int32_t depth() const noexcept;
		inline JsonType  type() const noexcept;
		inline bool startObject() noexcept;
		inline bool startRootObject() noexcept;
		inline bool startedObject() noexcept;
		inline bool startedRootObject() noexcept;
		inline bool hasNextField() noexcept;
		inline RawJsonString fieldKey() noexcept;
		inline ErrorCode fieldValue() noexcept;
		inline ErrorCode findField(const std::string_view key) noexcept;
		inline bool findFieldRaw(const std::string_view key) noexcept;
		inline bool findFieldUnorderedRaw(const std::string_view key) noexcept;
		inline bool startArray() noexcept;
		inline bool startRootArray() noexcept;
		inline bool startedArray() noexcept;
		inline bool startedRootArray() noexcept;
		inline bool hasNextElement() noexcept;
		inline ValueIterator child() const noexcept;

		inline std::string_view getString() noexcept;
		inline RawJsonString getRawJsonString() noexcept;
		inline uint64_t getUint64() noexcept;
		inline uint64_t getUint64InString() noexcept;
		inline int64_t getInt64() noexcept;
		inline int64_t getInt64InString() noexcept;
		inline double getDouble() noexcept;
		inline double getDoubleInString() noexcept;
		inline bool getBool() noexcept;
		inline bool isNull() noexcept;
		inline bool isNegative() noexcept;
		inline bool isInteger() noexcept;

		inline std::string_view getRootString() noexcept;
		inline RawJsonString getRootRawJsonString() noexcept;
		inline uint64_t getRootUint64() noexcept;
		inline uint64_t getRootUint64InString() noexcept;
		inline int64_t getRootInt64() noexcept;
		inline int64_t getRootInt64InString() noexcept;
		inline double getRootDouble() noexcept;
		inline double getRootDoubleInString() noexcept;
		inline bool getRootBool() noexcept;
		inline bool isRootNegative() noexcept;
		inline bool isRootInteger() noexcept;
		inline bool isRootNull() noexcept;

		inline ErrorCode getError() const noexcept;
		inline uint8_t*& stringBufLoc() noexcept;
		inline const JsonIterator& jsonIter() const noexcept;
		inline JsonIterator& jsonIter() noexcept;

		inline void assertIsValid() const noexcept;
		inline bool isValid() const noexcept;
	  protected:
		inline bool resetArray() noexcept;
		inline bool resetObject() noexcept;
		inline void moveAtStart() noexcept;
		inline void moveAtContainerStart() noexcept;
		inline std::string toString() const noexcept;
		inline ValueIterator(JsonIterator* json_iter, size_t depth, uint32_t* start_index) noexcept;

		inline bool parseNull(const uint8_t* json) const noexcept;
		inline bool parseBool(const uint8_t* json) const noexcept;
		inline const uint8_t* peekStart() const noexcept;
		inline uint32_t peekStartLength() const noexcept;

		inline void advanceScalar(const char* type) noexcept;
		inline void advanceRootScalar(const char* type) noexcept;
		inline void advanceNonRootScalar(const char* type) noexcept;

		inline const uint8_t* peekScalar(const char* type) noexcept;
		inline const uint8_t* peekRootScalar(const char* type) noexcept;
		inline const uint8_t* peekNonRootScalar(const char* type) noexcept;


		inline ErrorCode startContainer(uint8_t start_char, const char* incorrect_type_message, const char* type) noexcept;
		inline ErrorCode endContainer() noexcept;
		inline const uint8_t* advanceToValue() noexcept;

		inline bool isAtStart() const noexcept;
		inline bool isAtIteratorStart() const noexcept;
		inline bool isAtKey() const noexcept;

		inline void assertAtStart() const noexcept;
		inline void assertAtContainerStart() const noexcept;
		inline void assertAtRoot() const noexcept;
		inline void assertAtChild() const noexcept;
		inline void assertAtNext() const noexcept;
		inline void assertAtNonRootStart() const noexcept;
		inline uint32_t* startPosition() const noexcept;
		inline uint32_t* position() const noexcept;
		inline uint32_t* lastPosition() const noexcept;
		inline uint32_t* endPosition() const noexcept;
		inline ErrorCode reportError(ErrorCode error, const char* message) noexcept;

		friend class Document;
		friend class Object;
		friend class Array;
		friend class JsonIterator;
	};

	class Value {
	  public:
		inline Value() noexcept = default;
		template<typename T> inline T get() noexcept {
			static_assert(!sizeof(T), "The get method with given type is not implemented by the simdjson library.");
		}

		template<typename T> inline ErrorCode get(T& out) noexcept;
		inline Array getArray() noexcept;
		inline Object getObject() noexcept;
		inline uint64_t getUint64() noexcept;
		inline uint64_t getUint64InString() noexcept;
		inline int64_t getInt64() noexcept;
		inline int64_t getInt64InString() noexcept;
		inline double getDouble() noexcept;
		inline double getDoubleInString() noexcept;
		inline std::string_view getString() noexcept;
		inline RawJsonString getRawJsonString() noexcept;
		inline bool getBool() noexcept;
		inline bool isNull() noexcept;
		inline ArrayIterator begin() & noexcept;
		inline ArrayIterator end() & noexcept;
		inline size_t  countElements() & noexcept;
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
		inline std::string_view rawJsonToken() noexcept;
		inline const char* currentLocation() noexcept;
		inline int32_t currentDepth() const noexcept;

		inline Value atPointer(std::string_view json_pointer) noexcept;

	  protected:
		inline Value(const ValueIterator& iter) noexcept;
		inline void skip() noexcept;
		static inline Value start(const ValueIterator& iter) noexcept;
		static inline Value resume(const ValueIterator& iter) noexcept;
		inline Object  startOrResumeObject() noexcept;

		ValueIterator iterator{};

		friend class Document;
		friend class ArrayIterator;
		friend class Field;
		friend class Object;
		friend struct Value;
		friend struct field;
	};
	
}
