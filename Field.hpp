#pragma once

#include "JsonValueBase.hpp"
#include "Value.hpp"

namespace Jsonifier {

	class Field : public std::pair<RawJsonString, Value> {
	  public:
		inline Field() noexcept;
		inline JsonifierResult<std::string_view> unescaped_key() noexcept;
		inline RawJsonString key() const noexcept;
		inline Value& value() & noexcept;
		inline Value value() && noexcept;

	  protected:
		inline Field(RawJsonString key, Value&& Value) noexcept;
		static inline JsonifierResult<Field> start(ValueIterator& parent_iter) noexcept;
		static inline JsonifierResult<Field> start(const ValueIterator& parent_iter, RawJsonString key) noexcept;
		friend struct JsonifierResult<Field>;
		friend class ObjectIterator;
	};
}