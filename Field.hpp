#pragma once

#include "JsonValueBase.hpp" 

namespace Jsonifier {

	class Field : protected std::pair<RawJsonString, ValueIterator> {
	  public:

		inline RawJsonString key() noexcept {
			assert(first.stringView != nullptr);
			return first;
		}
		inline Field() noexcept = default;

		static inline Field start(ValueIterator& parent_iter) noexcept {
			RawJsonString key{};
			key = parent_iter.fieldKey();
			parent_iter.fieldValue();
			return Field::start(parent_iter, key);
		}

		static inline Field start(ValueIterator& parent_iter, RawJsonString key) noexcept {
			return Field(key, parent_iter.child());
		}

		inline Field(RawJsonString key, ValueIterator&& value) noexcept : std::pair<RawJsonString, ValueIterator>{ key, std::move(value) } {};
	};

	
}