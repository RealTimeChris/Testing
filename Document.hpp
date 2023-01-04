#pragma once

#include "JsonValueBase.hpp"
#include "JsonifierResult.hpp"

namespace Jsonifier {

	class Document {
	  public:
		inline Document() noexcept = default;
		inline Document(const Document& other) noexcept = delete;
		inline Document(Document&& other) noexcept = default;
		inline Document& operator=(const Document& other) noexcept = delete;
		inline Document& operator=(Document&& other) noexcept = default;

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
		inline JsonifierResult<Value> get_value() noexcept;
		inline JsonifierResult<bool> is_null() noexcept;
		template<typename T> inline JsonifierResult<T> get() & noexcept {
			static_assert(!sizeof(T), "The get method with given type is not implemented by the Jsonifier library.");
		}
		template<typename T> inline JsonifierResult<T> get() && noexcept {
			static_assert(!sizeof(T), "The get method with given type is not implemented by the Jsonifier library.");
		}
		template<typename T> inline ErrorCode get(T& out) & noexcept;
		template<typename T> inline ErrorCode get(T& out) && noexcept;
		inline JsonifierResult<size_t> countElements() noexcept;
		inline JsonifierResult<size_t> countFields() noexcept;
		inline JsonifierResult<Value> at(size_t index) noexcept;
		inline JsonifierResult<ArrayIterator> begin() noexcept;
		inline JsonifierResult<ArrayIterator> end() noexcept;
		inline JsonifierResult<Value> findField(std::string_view key) noexcept;
		inline JsonifierResult<Value> findField(const char* key) noexcept;
		inline JsonifierResult<Value> findFieldUnordered(std::string_view key) noexcept;
		inline JsonifierResult<Value> findFieldUnordered(const char* key) noexcept;
		inline JsonifierResult<Value> operator[](std::string_view key) noexcept;
		inline JsonifierResult<Value> operator[](const char* key) noexcept;
		inline JsonifierResult<JsonType> type() noexcept;
		inline JsonifierResult<bool> is_scalar() noexcept;
		inline bool is_negative() noexcept;
		inline JsonifierResult<bool> is_integer() noexcept;
		inline JsonifierResult<std::string_view> raw_json_token() noexcept;
		inline void rewind() noexcept;
		inline std::string to_debug_string() noexcept;
		inline bool is_alive() noexcept;
		inline JsonifierResult<const char*> current_location() noexcept;
		inline int32_t current_depth() const noexcept;
		inline JsonifierResult<Value> atPointer(std::string_view jsonPointer) noexcept;
		inline JsonifierResult<std::string_view> rawJson() noexcept;

	  protected:
		inline ErrorCode consume() noexcept;

		inline Document(JsonIterator&& iterator) noexcept;
		inline const uint8_t* text(uint32_t idx) const noexcept;

		inline ValueIterator resume_value_iterator() noexcept;
		inline ValueIterator get_root_value_iterator() noexcept;
		inline JsonifierResult<Object> start_or_resume_object() noexcept;
		static inline Document start(JsonIterator&& iterator) noexcept;

		JsonIterator iterator;
		static constexpr size_t DOCUMENT_DEPTH = 0;

		friend class ArrayIterator;
		friend class JsonifierCore;
		friend class Object;
		friend class Value;
		friend class Array;
		friend class Field;
		friend class Token;
	};

	template<>
	struct JsonifierResult<Document>
		: public JsonifierResultBase<Document> {
	  public:
		inline JsonifierResult(Document&& value) noexcept;///< @private
		inline JsonifierResult(ErrorCode error) noexcept;///< @private
		inline JsonifierResult() noexcept = default;
		inline ErrorCode rewind() noexcept;

		inline JsonifierResult<Array> get_array() & noexcept;
		inline JsonifierResult<Object> get_object() & noexcept;
		inline JsonifierResult<uint64_t> get_uint64() noexcept;
		inline JsonifierResult<int64_t> get_int64() noexcept;
		inline JsonifierResult<double> get_double() noexcept;
		inline JsonifierResult<double> get_double_from_string() noexcept;
		inline JsonifierResult<std::string_view> get_string() noexcept;
		inline JsonifierResult<RawJsonString> get_raw_json_string() noexcept;
		inline JsonifierResult<bool> get_bool() noexcept;
		inline JsonifierResult<Value> get_value() noexcept;
		inline JsonifierResult<bool> is_null() noexcept;

		template<typename T> inline JsonifierResult<T> get() & noexcept;
		template<typename T> inline JsonifierResult<T> get() && noexcept;

		template<typename T> inline ErrorCode get(T& out) & noexcept;
		template<typename T> inline ErrorCode get(T& out) && noexcept;

		inline JsonifierResult<size_t> count_elements() & noexcept;
		inline JsonifierResult<size_t> count_fields() & noexcept;
		inline JsonifierResult<Value> at(size_t index) & noexcept;
		inline JsonifierResult<ArrayIterator> begin() & noexcept;
		inline JsonifierResult<ArrayIterator> end() & noexcept;
		inline JsonifierResult<Value> findField(std::string_view key) & noexcept;
		inline JsonifierResult<Value> findField(const char* key) & noexcept;
		inline JsonifierResult<Value> operator[](std::string_view key) & noexcept;
		inline JsonifierResult<Value> operator[](const char* key) & noexcept;
		inline JsonifierResult<Value> findFieldUnordered(std::string_view key) & noexcept;
		inline JsonifierResult<Value> findFieldUnordered(const char* key) & noexcept;
		inline JsonifierResult<JsonType> type() noexcept;
		inline JsonifierResult<bool> is_scalar() noexcept;
		inline JsonifierResult<const char*> current_location() noexcept;
		inline int32_t current_depth() const noexcept;
		inline bool is_negative() noexcept;
		inline JsonifierResult<bool> is_integer() noexcept;
		inline JsonifierResult<std::string_view> raw_json_token() noexcept;

		inline JsonifierResult<Value> at_pointer(std::string_view json_pointer) noexcept;
	};
}