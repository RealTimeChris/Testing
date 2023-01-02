#pragma once

#include "JsonValueBase.hpp" 

namespace Jsonifier {

	class Field : protected std::pair<std::string_view, JsonValueBase> {
	  public:

		static inline Field start(JsonIterator& iteratorNew, RawJsonString key) noexcept {
			return Field(std::move(key), iteratorNew.child());
		}

		inline std::string_view getKey() {
			return this->first;
		}

		inline Field() noexcept = default;

		static inline Field start(JsonIterator& parent_iter) noexcept {
			RawJsonString key{ ( uint8_t* )(parent_iter.fieldKey().data()) };
			parent_iter.fieldValue();
			return Field::start(parent_iter, key);
		}

		inline Field(RawJsonString key, JsonIterator&& value) noexcept
			: std::pair<std::string_view, JsonValueBase>{ std::move(key.raw()), std::move(value) } {};
	};

	
}