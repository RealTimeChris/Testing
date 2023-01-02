#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class Field : protected std::pair<std::string_view, JsonValueBase> {
	  public:
		static inline Field start(JsonIterator& iteratorNew) noexcept {
			std::string_view key{ iteratorNew.fieldKey() };
			iteratorNew.fieldValue();
			return Field::start(iteratorNew, RawJsonString{ ( uint8_t* )(key.data()) });
		}

		static inline Field start(JsonIterator& iteratorNew, RawJsonString key) noexcept {
			return Field(std::move(key), iteratorNew.child());
		}

		inline ErrorCode getError() {
			return this->second.getError();
		}

		inline JsonIterator end() noexcept {
			return JsonIterator{ this->second };
		}

		inline JsonIterator begin() noexcept {
			return JsonIterator{ this->second };
		}

		inline std::string_view getKey() {
			return this->first;
		}

		inline Field() noexcept = default;

		inline Field(RawJsonString&& key, JsonIterator&& value) noexcept
			: std::pair<std::string_view, JsonValueBase>{ std::move(key.raw()), std::move(value) } {};

		inline Field(RawJsonString&& key, JsonValueBase&& value) : std::pair<std::string_view, JsonValueBase>{ std::move(key.raw()), value } {};

		inline Field(std::string_view&& key, JsonValueBase&& value) : std::pair<std::string_view, JsonValueBase>{ std::move(key), value } {};

		inline Field(std::string_view& key, JsonValueBase&& value) : std::pair<std::string_view, JsonValueBase>{ std::move(key), value } {};
	};

	
}