#pragma once

#include "JsonValueBase.hpp"
#include "Field.hpp"

namespace Jsonifier {

	class ObjectIterator {
	  public:
		inline ObjectIterator() noexcept = default;
		inline JsonifierResult<Field> operator*() noexcept;
		inline bool operator==(const ObjectIterator&) const noexcept;
		inline bool operator!=(const ObjectIterator&) const noexcept;
		inline ObjectIterator& operator++() noexcept;

	  protected:
		ValueIterator iterator{};

		inline ObjectIterator(const ValueIterator& iterator) noexcept;
		friend struct JsonifierResult<ObjectIterator>;
		friend class Object;
	};

	class Object {
	  public:
		inline Object() noexcept = default;

		inline JsonifierResult<ObjectIterator> begin() noexcept;
		inline JsonifierResult<ObjectIterator> end() noexcept;
		inline JsonifierResult<Value> find_field(std::string_view key) noexcept;
		inline JsonifierResult<Value> find_field_unordered(std::string_view key) noexcept;
		inline JsonifierResult<Value> operator[](std::string_view key) noexcept;
		inline JsonifierResult<Value> at_pointer(std::string_view json_pointer) noexcept;
		inline JsonifierResult<bool> reset() noexcept;
		inline JsonifierResult<bool> is_empty() noexcept;
		inline JsonifierResult<size_t> count_fields() noexcept;
		inline JsonifierResult<std::string_view> raw_json() noexcept;

	  protected:
		inline ErrorCode consume() noexcept;
		static inline JsonifierResult<Object> start(ValueIterator& iterator) noexcept;
		static inline JsonifierResult<Object> start_root(ValueIterator& iterator) noexcept;
		static inline JsonifierResult<Object> started(ValueIterator& iterator) noexcept;
		static inline Object resume(const ValueIterator& iterator) noexcept;
		inline Object(const ValueIterator& iterator) noexcept;

		inline ErrorCode find_field_raw(const std::string_view key) noexcept;

		ValueIterator iterator{};

		friend class Value;
		friend class Document;
		friend struct JsonifierResult<Object>;
	};

	template<> struct JsonifierResult<ObjectIterator> : public JsonifierResultBase<ObjectIterator> {
	  public:
		inline JsonifierResult(ObjectIterator&& value) noexcept;
		inline JsonifierResult(ErrorCode error) noexcept;
		inline JsonifierResult() noexcept = default;
		inline JsonifierResult<Field> operator*() noexcept;
		inline bool operator==(const JsonifierResult<ObjectIterator>&) const noexcept;
		inline bool operator!=(const JsonifierResult<ObjectIterator>&) const noexcept;
		inline JsonifierResult<ObjectIterator>& operator++() noexcept;
	};

	template<> struct JsonifierResult<Object> : public JsonifierResultBase<Object> {
	  public:
		JsonifierResult() noexcept = default;
		JsonifierResult(Object&& value) noexcept;
		JsonifierResult(ErrorCode error) noexcept;

		JsonifierResult<ObjectIterator> begin() noexcept;
		JsonifierResult<ObjectIterator> end() noexcept;
		JsonifierResult<Value> find_field(std::string_view key) noexcept;
		JsonifierResult<Value> find_field_unordered(std::string_view key) noexcept;
		JsonifierResult<Value> operator[](std::string_view key) noexcept;
		JsonifierResult<Value> at_pointer(std::string_view json_pointer) noexcept;
		inline JsonifierResult<bool> reset() noexcept;
		inline JsonifierResult<bool> is_empty() noexcept;
		inline JsonifierResult<size_t> count_fields() noexcept;
	};
}