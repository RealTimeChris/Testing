#pragma once

#include "Jsonifier.hpp"

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
		inline std::string_view unescape(JsonIterator& iter) const noexcept;

		const uint8_t* buf{};
		friend class field;
		friend class parser;
		friend class ValueIterator;
	};

	class TokenIterator {
	  public:
		inline TokenIterator() noexcept = default;
		inline TokenIterator(TokenIterator&& other) noexcept = default;
		inline TokenIterator& operator=(TokenIterator&& other) noexcept = default;
		inline TokenIterator(const TokenIterator& other) noexcept = default;
		inline TokenIterator& operator=(const TokenIterator& other) noexcept = default;
		inline const uint8_t* return_current_and_advance() noexcept;
		inline uint32_t current_offset() const noexcept;
		inline const uint8_t* peek(int32_t delta = 0) const noexcept;
		inline uint32_t peek_length(int32_t delta = 0) const noexcept;
		inline const uint8_t* peek(uint32_t* position) const noexcept;
		inline uint32_t peek_length(uint32_t* position) const noexcept;
		inline uint32_t* position() const noexcept;
		inline void set_position(uint32_t* target_position) noexcept;
		inline bool operator==(const TokenIterator& other) const noexcept;
		inline bool operator!=(const TokenIterator& other) const noexcept;
		inline bool operator>(const TokenIterator& other) const noexcept;
		inline bool operator>=(const TokenIterator& other) const noexcept;
		inline bool operator<(const TokenIterator& other) const noexcept;
		inline bool operator<=(const TokenIterator& other) const noexcept;

	  protected:
		inline TokenIterator(const uint8_t* buf, uint32_t* position) noexcept;
		inline uint32_t peek_index(int32_t delta = 0) const noexcept;
		inline uint32_t peek_index(uint32_t* position) const noexcept;

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
		uint32_t* rootPosition{};
		JsonifierCore* parser{};
		uint8_t* stringBuffer{};
		size_t currentDepth{};
		TokenIterator token;

	  public:
		inline JsonIterator() noexcept {};
		inline JsonIterator(JsonIterator&& other) noexcept;
		inline JsonIterator& operator=(JsonIterator&& other) noexcept;
		inline explicit JsonIterator(const JsonIterator& other) noexcept = default;
		inline JsonIterator& operator=(const JsonIterator& other) noexcept = default;
		inline ErrorCode skip_child(size_t parent_depth) noexcept;
		inline bool at_root() const noexcept;
		inline bool streaming() const noexcept;
		inline uint32_t* root_position() const noexcept;
		inline void assert_at_document_depth() const noexcept;
		inline void assert_at_root() const noexcept;
		inline bool at_end() const noexcept;
		inline bool is_alive() const noexcept;
		inline void abandon() noexcept;
		inline const uint8_t* return_current_and_advance() noexcept;
		inline bool is_single_token() const noexcept;
		inline void assert_more_tokens(uint32_t required_tokens = 1) const noexcept;
		inline void assert_valid_position(uint32_t* position) const noexcept;
		inline const uint8_t* peek(int32_t delta = 0) const noexcept;
		inline uint32_t peek_length(int32_t delta = 0) const noexcept;
		inline const uint8_t* unsafe_pointer() const noexcept;
		inline const uint8_t* peek(uint32_t* position) const noexcept;
		inline uint32_t peek_length(uint32_t* position) const noexcept;
		inline const uint8_t* peek_last() const noexcept;
		inline void ascend_to(size_t parent_depth) noexcept;
		inline void descend_to(size_t child_depth) noexcept;
		inline void descend_to(size_t child_depth, int32_t delta) noexcept;
		inline size_t depth() const noexcept;
		inline uint8_t*& string_buf_loc() noexcept;
		inline ErrorCode report_error(ErrorCode error, const char* message) noexcept;
		inline ErrorCode optional_error(ErrorCode error, const char* message) noexcept;

		template<int N> inline bool copy_to_buffer(const uint8_t* json, uint32_t max_len, uint8_t (&tmpbuf)[N]) noexcept;

		inline uint32_t* position() const noexcept;
		inline std::string_view unescape(RawJsonString in) noexcept;
		inline void reenter_child(uint32_t* position, size_t child_depth) noexcept;
		inline std::string to_string() const noexcept;
		inline const char* current_location() noexcept;
		inline void rewind() noexcept;
		inline bool balanced() const noexcept;

		inline JsonIterator(uint8_t* buf, JsonifierCore* parser) noexcept;

	  protected:
		inline uint32_t* last_position() const noexcept;
		inline uint32_t* end_position() const noexcept;
		inline uint32_t* end() const noexcept;

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

		inline bool find_field_unordered_raw(const std::string_view key) noexcept;

		inline Object find_field_unordered(const std::string_view key) noexcept;

		inline Object operator[](const std::string_view key) & noexcept;

		inline Object operator[](const std::string_view key) && noexcept;

		inline ValueIterator resume_value_iterator() noexcept;

		inline ValueIterator get_root_value_iterator() noexcept;

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
		inline ErrorCode skip_child() noexcept;
		inline bool at_end() const noexcept;
		inline bool at_start() const noexcept;
		inline bool is_open() const noexcept;
		inline bool at_first_field() const noexcept;
		inline void abandon() noexcept;
		inline ValueIterator child_value() const noexcept;
		inline int32_t depth() const noexcept;
		inline JsonType type() const noexcept;
		inline bool start_object() noexcept;
		inline bool start_root_object() noexcept;
		inline bool started_object() noexcept;
		inline bool started_root_object() noexcept;
		inline bool has_next_field() noexcept;
		inline std::string_view field_key() noexcept;
		inline ErrorCode field_value() noexcept;
		inline ErrorCode find_field(const std::string_view key) noexcept;
		inline bool find_field_raw(const std::string_view key) noexcept;
		inline bool start_array() noexcept;
		inline bool start_root_array() noexcept;
		inline bool started_array() noexcept;
		inline bool started_root_array() noexcept;
		inline bool has_next_element() noexcept;
		inline ValueIterator child() noexcept;

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
		inline ErrorCode getError() noexcept;
		inline uint8_t*& string_buf_loc() noexcept;
		inline JsonIterator& json_iter() noexcept;

		inline void assert_is_valid() const noexcept;
		inline bool is_valid() const noexcept;

	  protected:
		inline bool reset_array() noexcept;
		inline bool reset_object() noexcept;
		inline void move_at_start() noexcept;
		inline void move_at_container_start() noexcept;
		inline std::string to_string() const noexcept;
		inline ValueIterator(JsonIterator* json_iter, size_t depth, uint32_t* start_index) noexcept;

		inline bool parse_null(const uint8_t* json) const noexcept;
		inline bool parse_bool(const uint8_t* json) const noexcept;
		inline const uint8_t* peek_start() const noexcept;
		inline uint32_t peek_start_length() const noexcept;

		inline void advance_scalar() noexcept;
		inline void advance_root_scalar() noexcept;
		inline void advance_non_root_scalar() noexcept;

		inline const uint8_t* peek_scalar() noexcept;
		inline const uint8_t* peek_root_scalar() noexcept;
		inline const uint8_t* peek_non_root_scalar() noexcept;


		inline ErrorCode start_container(uint8_t start_char, const char* incorrect_type_message, const char* type) noexcept;
		inline ErrorCode end_container() noexcept;
		inline const uint8_t* advance_to_value() noexcept;

		inline ErrorCode error_unless_more_tokens(uint32_t tokens = 1) const noexcept;

		inline bool is_at_start() const noexcept;
		inline bool is_at_iterator_start() const noexcept;
		inline bool is_at_key() const noexcept;

		inline void assert_at_start() const noexcept;
		inline void assert_at_container_start() const noexcept;
		inline void assert_at_root() const noexcept;
		inline void assert_at_child() const noexcept;
		inline void assert_at_next() const noexcept;
		inline void assert_at_non_root_start() const noexcept;
		inline uint32_t* start_position() const noexcept;

		inline uint32_t* position() const noexcept;
		inline uint32_t* last_position() const noexcept;
		inline uint32_t* end_position() const noexcept;
		inline ErrorCode report_error(ErrorCode error, const char* message) noexcept;

		friend class Document;
		friend class RawJsonString;
		friend class Array;

	  protected:
		ErrorCode error{ ErrorCode::Success };
		uint8_t* stringBufferLocation{};
		JsonifierCore* parser{};
		uint8_t* stringView{};
		uint32_t* root{};
	};
}
