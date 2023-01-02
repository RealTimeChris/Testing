#pragma once

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

	enum class JsonType : uint8_t { Document = 0, Object = 1, Array = 2, String = 3, Float = 4, Uint64 = 5, Int64 = 6, Bool = 7, Null = 8 };

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

	class JsonIterator;
	class ValueIterator;

	class RawJsonString {
	  public:
		inline RawJsonString() noexcept = default;
		inline RawJsonString(uint8_t* _buf) noexcept;
		inline char* raw() const noexcept;
		inline bool unsafeIsEqual(size_t length, std::string_view target) const noexcept;
		inline bool unsafeIsEqual(std::string_view target) const noexcept;
		inline bool unsafeIsEqual(const char* target) const noexcept;
		inline bool is_equal(std::string_view target) const noexcept;
		inline bool is_equal(const char* target) const noexcept;
		static inline bool is_free_from_unescaped_quote(std::string_view target) noexcept;
		static inline bool is_free_from_unescaped_quote(const char* target) noexcept;

	  private:
		inline void consume() noexcept {
			stringView = nullptr;
		}
		inline bool alive() const noexcept {
			return stringView != nullptr;
		}
		inline std::string_view unescape(JsonIterator& iter) noexcept;

		uint8_t* stringView{};
		friend class JsonIterator;
		friend class ValueIterator;
	};

	class TokenIterator {
	  public:
		inline TokenIterator() noexcept = default;
		inline TokenIterator(TokenIterator&& other) noexcept = default;
		inline TokenIterator& operator=(TokenIterator&& other) noexcept = default;
		inline TokenIterator(const TokenIterator& other) noexcept = default;
		inline TokenIterator& operator=(const TokenIterator& other) noexcept = default;
		inline const uint8_t* returnCurrentAndAdvance() noexcept;
		inline const uint8_t* peek(int32_t delta = 0) const noexcept;
		inline const uint8_t* peek(uint32_t* position) const noexcept;
		inline uint32_t* position() const noexcept;
		inline void setPosition(uint32_t* target_position) noexcept;
		inline bool operator==(const TokenIterator& other) noexcept;
		inline bool operator!=(const TokenIterator& other) noexcept;
		inline bool operator>(const TokenIterator& other) noexcept;
		inline bool operator>=(const TokenIterator& other) noexcept;
		inline bool operator<(const TokenIterator& other) noexcept;
		inline bool operator<=(const TokenIterator& other) noexcept;

	  protected:
		inline TokenIterator(const uint8_t* buf, uint32_t* position) noexcept;

		uint32_t* currentPosition{};
		const uint8_t* buf{};

		friend class JsonIterator;
	};

	class JsonifierCore;
	class Field;
	class Object;
	class Array;

	class JsonIterator {
	  protected:
		ErrorCode error{ ErrorCode::Success };
		JsonifierCore* parser{};
		uint8_t* stringBuffer{};
		size_t currentDepth{};
		TokenIterator token;
		uint32_t* root{};

	  public:
		inline JsonIterator() noexcept {};
		inline JsonIterator(JsonIterator&& other) noexcept;
		inline JsonIterator& operator=(JsonIterator&& other) noexcept;
		inline JsonIterator(const JsonIterator& other) noexcept = default;
		inline JsonIterator& operator=(const JsonIterator& other) noexcept = default;
		inline ErrorCode skipChild(size_t parent_depth) noexcept;
		inline bool atRoot() const noexcept;
		inline uint32_t* rootPosition() const noexcept;
		inline void assertAtDocumentDepth() const noexcept;
		inline bool isAlive() const noexcept;
		inline void abandon() noexcept;
		inline const uint8_t* returnCurrentAndAdvance() noexcept;
		inline const uint8_t* peek(int32_t delta = 0) const noexcept;
		inline const uint8_t* peek(uint32_t* position) const noexcept;
		inline const uint8_t* peekLast() const noexcept;
		inline void ascendTo(size_t parent_depth) noexcept;
		inline void descendTo(size_t child_depth) noexcept;
		inline void descendTo(size_t child_depth, int32_t delta) noexcept;
		inline size_t depth() const noexcept;

		inline uint32_t* position() const noexcept;
		inline std::string_view unescape(RawJsonString&) noexcept;
		inline std::string toString() const noexcept;
		inline const char* currentLocation() noexcept;
		inline void rewind() noexcept;
		inline bool balanced() const noexcept;

		inline JsonIterator(uint8_t* buf, JsonifierCore* parser) noexcept;

	  protected:
		inline uint32_t* lastPosition() const noexcept;
		inline uint32_t* endPosition() const noexcept;

		friend class RawJsonString;
		friend class ValueIterator;
		friend class Array;
	};

	class ValueIterator {
	  public:
		inline bool operator!=(const ValueIterator&);
		inline ValueIterator() noexcept = default;
		inline ValueIterator& operator=(const ValueIterator& other) noexcept;
		inline ValueIterator(const ValueIterator& other) noexcept;
		inline ValueIterator& operator=(ValueIterator&& other) noexcept;
		inline ValueIterator(ValueIterator&& other) noexcept;

		inline ValueIterator(JsonifierCore* other) noexcept;

		template<typename OTy> inline ErrorCode get(OTy&) noexcept;

		template<typename OTy> inline JsonifierResult<OTy> get() noexcept;

		template<typename OTy> inline JsonifierResult<OTy> get(const char*) noexcept;

		inline bool findFieldUnorderedRaw(const std::string_view key) noexcept;

		inline Object findFieldUnordered(const std::string_view key) noexcept;

		inline Object operator[](const std::string_view key) & noexcept;

		inline Object operator[](const std::string_view key) && noexcept;

		inline ValueIterator resumeValueIterator() noexcept;

		inline ValueIterator getRootValueIterator() noexcept;

		inline Field findField(const char* keyNew) noexcept;

		inline JsonifierCore* getCore() noexcept;

		inline Object getObject() & noexcept;

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

		inline ValueIterator& getCurrentIterator() noexcept;

		inline void asserAtFieldStart(size_t amountToOffset) noexcept;

		inline void assertAtObjectStart(size_t amountToOffset = 0) noexcept;

		inline void assertAtArrayStart(size_t amountToOffset = 0) noexcept;

		inline void assertAtStringStart(size_t amountToOffset = 0) noexcept;

		inline size_t getCurrentCount() noexcept;

		inline uint8_t getRootKey() noexcept;

		inline size_t size() noexcept;
		inline std::string_view unescape(RawJsonString in, uint8_t*& dst) const noexcept;

		inline JsonType type() noexcept;
		std::unique_ptr<JsonIterator> jsonIterator{ std::make_unique<JsonIterator>() };
		size_t currentDepth{};

		inline void start_document() noexcept;
		inline ErrorCode skipChild() noexcept;
		inline bool isOpen() const noexcept;
		inline bool atFirstField() const noexcept;
		inline void abandon() noexcept;
		inline size_t depth() const noexcept;
		inline JsonType type() const noexcept;
		inline bool startObject() noexcept;
		inline bool startRootObject() noexcept;
		inline bool startedObject() noexcept;
		inline bool startedRootObject() noexcept;
		inline bool hasNextField() noexcept;
		inline std::string_view fieldKey() noexcept;
		inline ErrorCode fieldValue() noexcept;
		inline ErrorCode findField(const std::string_view key) noexcept;
		inline bool findFieldRaw(const std::string_view key) noexcept;
		inline void assertAtContainerStart() const noexcept;
		inline bool startRootArray() noexcept;
		inline bool startedArray() noexcept;
		inline bool startedRootArray() noexcept;
		inline bool hasNextElement() noexcept;
		inline ValueIterator child() noexcept;

		inline std::string_view getString() noexcept;
		inline RawJsonString getRawJsonString() noexcept;
		inline uint64_t getUint64() noexcept;
		inline uint64_t get_uint64_in_string() noexcept;
		inline int64_t getInt64() noexcept;
		inline int64_t getInt64InString() noexcept;
		inline double getDouble() noexcept;
		inline double getDoubleInString() noexcept;
		inline bool getBool() noexcept;

		inline std::string_view getRootString() noexcept;
		inline RawJsonString getRootRawJsonString() noexcept;
		inline uint64_t getRootUint64() noexcept;
		inline uint64_t getRootUint64InString() noexcept;
		inline int64_t getRootInt64() noexcept;
		inline int64_t getRootInt64InString() noexcept;
		inline double getRootDouble() noexcept;
		inline double getRootDoubleInString() noexcept;
		inline bool getRootBool() noexcept;
		inline bool isRootNull() noexcept;
		inline ErrorCode getError() noexcept;
		inline JsonIterator& jsonIter() noexcept;

		inline void assert_is_valid() const noexcept;
		inline bool is_valid() const noexcept;

	  protected:
		inline bool resetArray() noexcept;
		inline bool resetObject() noexcept;
		inline void moveAtContainerStart() noexcept;
		inline std::string toString() const noexcept;
		inline ValueIterator(JsonIterator* jsonIter, size_t depth, uint32_t* start_index) noexcept;

		inline const uint8_t* peekStart() const noexcept;

		inline void advanceScalar() noexcept;
		inline void advanceNonRootScalar() noexcept;

		inline const uint8_t* peekScalar() noexcept;
		inline const uint8_t* peekNonRootScalar() noexcept;


		inline ErrorCode startContainer(uint8_t start_char) noexcept;
		inline ErrorCode endContainer() noexcept;
		inline const uint8_t* advance_to_value() noexcept;

		inline ErrorCode error_unless_more_tokens(uint32_t tokens = 1) const noexcept;

		inline bool isAtStart() const noexcept;

		inline void assertAtStart() const noexcept;
		inline void assertAtChild() const noexcept;
		inline void assertAtNext() const noexcept;
		inline void assertAtNonRootStart() const noexcept;
		inline uint32_t* startPosition() const noexcept;

		inline uint32_t* position() const noexcept;
		inline uint32_t* lastPosition() const noexcept;
		inline uint32_t* endPosition() const noexcept;
		inline ErrorCode report_error(ErrorCode error, const char* message) noexcept;

		friend class Document;
		friend class RawJsonString;
		friend class Array;

	  protected:
		ErrorCode error{ ErrorCode::Success };
		uint32_t* rootPosition{};
		JsonifierCore* parser{};
	};
}
