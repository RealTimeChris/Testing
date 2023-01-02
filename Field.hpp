#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class Field : protected std::pair<std::string_view, JsonValueBase> {
	  public:
		static inline Field start(JsonValueBase& parent_iter) noexcept {
			std::string_view key{};
			key = parent_iter.fieldKey();
			parent_iter.fieldValue();
			return Field::start(parent_iter, RawJsonString{ ( uint8_t* )(key.data()) });
		}

		static inline Field start(JsonValueBase& parent_iter, RawJsonString key) noexcept {
			return Field(key, parent_iter.child<Field>().second);
		}

		inline ErrorCode getError() {
			return this->second.getError();
		}

		inline JsonIteratorBase<Object> end() noexcept {
			return JsonIteratorBase<Object>{ this->second };
		}

		inline JsonIteratorBase<Object> begin() noexcept {
			return JsonIteratorBase<Object>{ this->second };
		}

		inline std::string_view getKey() {
			return this->first;
		}

		inline Field() noexcept = default;

		inline Field(RawJsonString& key, JsonValueBase&& value) : std::pair<std::string_view, JsonValueBase>{ std::move(key.raw()), Object{} } {};

		inline Field(std::string_view&& key, JsonValueBase&& value) : std::pair<std::string_view, JsonValueBase>{ std::move(key), Object{} } {};
	};

	
}