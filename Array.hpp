#pragma once

#include "JsonValueBase.hpp"
#include "Value.hpp"

namespace Jsonifier {

	class ArrayIterator {
	  public:
		inline ArrayIterator() noexcept = default;
		inline JsonifierResult<Value> operator*() noexcept;
		inline bool operator==(const ArrayIterator&) const noexcept;
		inline bool operator!=(const ArrayIterator&) const noexcept;
		inline ArrayIterator& operator++() noexcept;

	  private:
		ValueIterator iterator{};

		inline ArrayIterator(const ValueIterator& iterator) noexcept;

		friend class Array;
		friend class Value;
		friend struct JsonifierResult<ArrayIterator>;
	};

	class Array {
	  public:
		inline Array() noexcept = default;
		inline JsonifierResult<ArrayIterator> begin() noexcept;
		inline JsonifierResult<ArrayIterator> end() noexcept;
		inline JsonifierResult<size_t> count_elements() & noexcept;
		inline JsonifierResult<bool> is_empty() & noexcept;
		inline JsonifierResult<bool> reset() & noexcept;
		inline JsonifierResult<Value> at_pointer(std::string_view json_pointer) noexcept;
		inline JsonifierResult<std::string_view> raw_json() noexcept;
		inline JsonifierResult<Value> at(size_t index) noexcept;

	  protected:
		inline ErrorCode consume() noexcept;
		static inline JsonifierResult<Array> start(ValueIterator& iterator) noexcept;
		static inline JsonifierResult<Array> start_root(ValueIterator& iterator) noexcept;
		static inline JsonifierResult<Array> started(ValueIterator& iterator) noexcept;
		inline Array(const ValueIterator& iterator) noexcept;

		ValueIterator iterator{};

		friend class Value;
		friend class Document;
		friend struct JsonifierResult<Value>;
		friend struct JsonifierResult<Array>;
		friend class ArrayIterator;
	};

	
	template<>
	struct JsonifierResult<ArrayIterator>
		: public JsonifierResultBase<ArrayIterator> {
	  public:
		inline JsonifierResult(ArrayIterator&& value) noexcept;
		inline JsonifierResult(ErrorCode) noexcept;
		inline JsonifierResult() noexcept = default;

		inline JsonifierResult<Value>
		operator*() noexcept;
		inline bool operator==(const JsonifierResult<ArrayIterator>&) const noexcept;
		inline bool operator!=(const JsonifierResult<ArrayIterator>&) const noexcept;
		inline JsonifierResult<ArrayIterator>& operator++() noexcept;
	};
}