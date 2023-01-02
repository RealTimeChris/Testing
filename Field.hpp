#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class Field : protected std::pair<std::string_view, JsonValueBase> {
	  public:
		static inline Field start(IteratorBaseBase& parent_iter) noexcept {
			std::string_view key{};
			key = parent_iter.fieldKey();
			parent_iter.fieldValue();
			return Field::start(parent_iter, RawJsonString{ ( uint8_t* )(key.data()) });
		}

		static inline Field start(IteratorBaseBase& parent_iter, RawJsonString key) noexcept {
			return Field(std::move(key), parent_iter.child());
		}

		inline ErrorCode getError() {
			return this->second.getError();
		}

		inline IteratorBaseBase end() noexcept {
			return IteratorBaseBase{ this->second };
		}

		inline IteratorBaseBase begin() noexcept {
			return IteratorBaseBase{ this->second };
		}

		inline std::string_view getKey() {
			return this->first;
		}

		inline Field() noexcept = default;

		inline Field(RawJsonString&& key, IteratorBaseBase&& value) noexcept
			: std::pair<std::string_view, JsonValueBase>{ std::move(key.raw()), std::move(value) } {};

		inline Field(RawJsonString&& key, JsonValueBase&& value) : std::pair<std::string_view, JsonValueBase>{ std::move(key.raw()), Object{} } {};

		inline Field(std::string_view&& key, JsonValueBase&& value) : std::pair<std::string_view, JsonValueBase>{ std::move(key), Object{} } {};

		inline Field(std::string_view& key, JsonValueBase&& value) : std::pair<std::string_view, JsonValueBase>{ std::move(key), Object{} } {};

	  protected:
		IteratorBaseBase* iterator{};
	};

	
}