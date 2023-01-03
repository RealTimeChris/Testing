#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class JsonIterator;
	class ArrayIterator;

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

	class ValueIterator {
	  protected:
		JsonIterator* _json_iter{};
		size_t currentDepth{};
		uint32_t* _start_position{};

	  public:
		inline ValueIterator() noexcept = default;
		inline void start_document() noexcept;
		inline ErrorCode skip_child() noexcept;
		inline bool at_end() const noexcept;
		inline bool at_start() const noexcept;
		inline bool is_open() const noexcept;
		inline bool at_first_field() const noexcept;
		inline void abandon() noexcept;
		inline ValueIterator child_value() const noexcept;
		inline int32_t depth() const noexcept;
		inline JsonifierResult<JsonType> type() const noexcept;
		inline JsonifierResult<bool> start_object() noexcept;
		inline JsonifierResult<bool> start_root_object() noexcept;
		inline JsonifierResult<bool> started_object() noexcept;
		inline JsonifierResult<bool> started_root_object() noexcept;
		inline JsonifierResult<bool> has_next_field() noexcept;
		inline JsonifierResult<RawJsonString> field_key() noexcept;
		inline ErrorCode field_value() noexcept;
		inline ErrorCode find_field(const std::string_view key) noexcept;
		inline JsonifierResult<bool> find_field_raw(const std::string_view key) noexcept;
		inline JsonifierResult<bool> find_field_unordered_raw(const std::string_view key) noexcept;
		inline JsonifierResult<bool> start_array() noexcept;
		inline JsonifierResult<bool> start_root_array() noexcept;
		inline JsonifierResult<bool> started_array() noexcept;
		inline JsonifierResult<bool> started_root_array() noexcept;
		inline JsonifierResult<bool> has_next_element() noexcept;
		inline ValueIterator child() const noexcept;

		inline JsonifierResult<std::string_view> get_string() noexcept;
		inline JsonifierResult<RawJsonString> get_raw_json_string() noexcept;
		inline JsonifierResult<uint64_t> get_uint64() noexcept;
		inline JsonifierResult<uint64_t> get_uint64_in_string() noexcept;
		inline JsonifierResult<int64_t> get_int64() noexcept;
		inline JsonifierResult<int64_t> get_int64_in_string() noexcept;
		inline JsonifierResult<double> get_double() noexcept;
		inline JsonifierResult<double> get_double_in_string() noexcept;
		inline JsonifierResult<bool> get_bool() noexcept;
		inline JsonifierResult<bool> is_null() noexcept;
		inline bool is_negative() noexcept;
		inline JsonifierResult<bool> is_integer() noexcept;
		inline JsonifierResult<NumberType> get_number_type() noexcept;
		inline JsonifierResult<Number> get_number() noexcept;

		inline JsonifierResult<std::string_view> get_root_string() noexcept;
		inline JsonifierResult<RawJsonString> get_root_raw_json_string() noexcept;
		inline JsonifierResult<uint64_t> get_root_uint64() noexcept;
		inline JsonifierResult<uint64_t> get_root_uint64_in_string() noexcept;
		inline JsonifierResult<int64_t> get_root_int64() noexcept;
		inline JsonifierResult<int64_t> get_root_int64_in_string() noexcept;
		inline JsonifierResult<double> get_root_double() noexcept;
		inline JsonifierResult<double> get_root_double_in_string() noexcept;
		inline JsonifierResult<bool> get_root_bool() noexcept;
		inline bool is_root_negative() noexcept;
		inline JsonifierResult<bool> is_root_integer() noexcept;
		inline JsonifierResult<NumberType> get_root_number_type() noexcept;
		inline JsonifierResult<Number> get_root_number() noexcept;
		inline bool is_root_null() noexcept;

		inline ErrorCode error() const noexcept;
		inline uint8_t*& string_buf_loc() noexcept;
		inline const JsonIterator& json_iter() const noexcept;
		inline JsonIterator& json_iter() noexcept;

		inline void assert_is_valid() const noexcept;
		inline bool is_valid() const noexcept;
	  protected:
		inline JsonifierResult<bool> reset_array() noexcept;
		inline JsonifierResult<bool> reset_object() noexcept;
		inline void move_at_start() noexcept;
		inline void move_at_container_start() noexcept;
		inline std::string to_string() const noexcept;
		inline ValueIterator(JsonIterator* json_iter, size_t depth, uint32_t* start_index) noexcept;

		inline JsonifierResult<bool> parse_null(const uint8_t* json) const noexcept;
		inline JsonifierResult<bool> parse_bool(const uint8_t* json) const noexcept;
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
		inline JsonifierResult<const uint8_t*> advance_to_value() noexcept;

		inline ErrorCode incorrect_type_error(const char* message) const noexcept;
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
		friend class Object;
		friend class Array;
		friend class Value;
	};

	class Value {
	  public:
		inline Value() noexcept = default;
		template<typename T> inline JsonifierResult<T> get() noexcept {
			static_assert(!sizeof(T), "The get method with given type is not implemented by the simdjson library.");
		}
		template<typename T> inline ErrorCode get(T& out) noexcept;
		inline JsonifierResult<Array> get_array() noexcept;
		inline JsonifierResult<Object> get_object() noexcept;
		inline JsonifierResult<uint64_t> get_uint64() noexcept;
		inline JsonifierResult<uint64_t> get_uint64_in_string() noexcept;
		inline JsonifierResult<int64_t> get_int64() noexcept;
		inline JsonifierResult<int64_t> get_int64_in_string() noexcept;
		inline JsonifierResult<double> get_double() noexcept;
		inline JsonifierResult<double> get_double_in_string() noexcept;
		inline JsonifierResult<std::string_view> get_string() noexcept;
		inline JsonifierResult<RawJsonString> get_raw_json_string() noexcept;
		inline JsonifierResult<bool> get_bool() noexcept;
		inline JsonifierResult<bool> is_null() noexcept;
		inline JsonifierResult<ArrayIterator> begin() & noexcept;
		inline JsonifierResult<ArrayIterator> end() & noexcept;
		inline JsonifierResult<size_t> count_elements() & noexcept;
		inline JsonifierResult<size_t> count_fields() & noexcept;
		inline JsonifierResult<Value> at(size_t index) noexcept;
		inline JsonifierResult<Value> find_field(std::string_view key) noexcept;
		inline JsonifierResult<Value> find_field(const char* key) noexcept;
		inline JsonifierResult<Value> find_field_unordered(std::string_view key) noexcept;
		inline JsonifierResult<Value> find_field_unordered(const char* key) noexcept;
		inline JsonifierResult<Value> operator[](std::string_view key) noexcept;
		inline JsonifierResult<Value> operator[](const char* key) noexcept;
		inline JsonifierResult<JsonType> type() noexcept;
		inline JsonifierResult<bool> is_scalar() noexcept;
		inline bool is_negative() noexcept;
		inline JsonifierResult<bool> is_integer() noexcept;
		inline JsonifierResult<NumberType> get_number_type() noexcept;
		inline JsonifierResult<Number> get_number() noexcept;
		inline std::string_view raw_json_token() noexcept;
		inline JsonifierResult<const char*> current_location() noexcept;
		inline int32_t current_depth() const noexcept;
		inline JsonifierResult<Value> at_pointer(std::string_view json_pointer) noexcept;

	  protected:
		inline Value(const ValueIterator& iterator) noexcept;
		inline void skip() noexcept;
		static inline Value start(const ValueIterator& iterator) noexcept;
		static inline Value resume(const ValueIterator& iterator) noexcept;
		inline JsonifierResult<Object> start_or_resume_object() noexcept;

		ValueIterator iterator{};

		friend class Document;
		friend class ArrayIterator;
		friend class Field;
		friend class Object;
		friend struct JsonifierResult<Value>;
		friend struct JsonifierResult<Field>;
	};
	
}
