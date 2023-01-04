#pragma once

#include "NumberParsingUtils.hpp"
#include "JsonifierResult.hpp"

namespace Jsonifier {

	class JsonifierCore;
	class JsonIterator;
	class Object;
	class Array;
	class Field;

	enum class JsonType : uint8_t {
		Document = 0,
		Object = 1,
		Array = 2,
		String = 3,
		Float = 4,
		Number = 5,
		Uint64 = 6,
		Int64 = 7,
		Bool = 8,
		Null = 9
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

	  protected:
		inline void consume() noexcept {
			stringView = nullptr;
		}
		inline bool alive() const noexcept {
			return stringView != nullptr;
		}
		inline JsonifierResult<std::string_view> unescape(JsonIterator& iterator) const noexcept;

		const uint8_t* stringView{};
		friend class object;
		friend class Field;
		friend class parser;
		friend struct JsonifierResult<RawJsonString>;
	};

	class TokenIterator {
	  public:
		inline TokenIterator(TokenIterator&& other) noexcept = default;
		inline TokenIterator& operator=(TokenIterator&& other) noexcept = default;
		inline TokenIterator(const TokenIterator& other) noexcept = default;
		inline TokenIterator& operator=(const TokenIterator& other) noexcept = default;

		inline const uint8_t* return_current_and_advance() noexcept;
		inline uint32_t current_offset() const noexcept;
		inline const uint8_t* peek(int32_t delta = 0, std::source_location = std::source_location::current()) const noexcept;
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
		inline TokenIterator(const uint8_t* stringView, uint32_t* position) noexcept;
		inline uint32_t peek_index(int32_t delta = 0) const noexcept;
		inline uint32_t peek_index(uint32_t* position) const noexcept;

		const uint8_t* stringView{};
		uint32_t* currentPosition{};

		friend class JsonIterator;
		friend class ValueIterator;
		friend class Object;
	};


	class JsonIterator {
	  protected:
		TokenIterator token;
		JsonifierCore* parser{};
		uint8_t* stringBuffer{};
		ErrorCode error{ ErrorCode::Success };
		size_t currentDepth{ 1 };
		uint32_t* rootPosition{};

	  public:
		inline JsonIterator(JsonIterator&& other) noexcept;
		inline JsonIterator& operator=(JsonIterator&& other) noexcept;
		inline ErrorCode skip_child(size_t parent_depth) noexcept;
		inline bool at_root() const noexcept;
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
		inline const uint8_t* peek(int32_t delta = 0, std::source_location = std::source_location::current()) const noexcept;
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
		inline JsonifierResult<std::string_view> unescape(RawJsonString in) noexcept;
		inline void reenter_child(uint32_t* position, size_t child_depth) noexcept;
		inline std::string to_string() const noexcept;
		inline JsonifierResult<const char*> current_location() noexcept;
		inline void rewind() noexcept;
		inline bool balanced() const noexcept;

	  protected:
		inline JsonIterator(JsonifierCore* parser) noexcept;
		inline uint32_t* last_position() const noexcept;
		inline uint32_t* end_position() const noexcept;
		inline uint32_t* end() const noexcept;

		friend class Document;
		friend class document_stream;
		friend class Object;
		friend class Orray;
		friend class Value;
		friend class RawJsonString;
		friend class JsonifierCore;
		friend class ValueIterator;
	};

	template<> struct JsonifierResult<RawJsonString> : public JsonifierResultBase<RawJsonString> {
	  public:
		JsonifierResult(RawJsonString&& value) noexcept;
		JsonifierResult(ErrorCode error) noexcept;
		JsonifierResult() noexcept = default;
		~JsonifierResult() noexcept = default;

		JsonifierResult<const char*> raw() const noexcept;
		JsonifierResult<std::string_view> unescape(JsonIterator& iter) const noexcept;
	};

}