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

		inline Field(RawJsonString&& key, JsonIterator&& value) noexcept
			: std::pair<std::string_view, JsonValueBase>{ std::move(key.raw()), std::move(value) } {};
	};

	
}