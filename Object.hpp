#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class Object : public JsonValueBase {
	  public:
		inline size_t countFields() noexcept;
		inline Object() noexcept = default;

		static inline Object start(JsonIterator& iterator) noexcept {
			iterator.startObject();
			return Object(iterator);
		}

		static inline Object resume(JsonIterator&& iterator) noexcept {
			return iterator;
		}

		static inline Object startRoot(JsonIterator&& iterator) noexcept;

		inline Object findFieldUnordered(const std::string_view key) & noexcept {
			bool hasValue{ this->iterator->findFieldUnorderedRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(iterator->child());
		}

		inline Object findFieldUnordered(const std::string_view key) && noexcept {
			bool hasValue{ this->iterator->findFieldUnorderedRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(iterator->child());
		}

		inline Object operator[](const std::string_view key) & noexcept {
			return findFieldUnordered(key);
		}

		inline Object operator[](const std::string_view key) && noexcept {
			return std::forward<Object>(*this).findFieldUnordered(key);
		}

		inline Object findField(const std::string_view key) & noexcept {
			bool hasValue{ this->iterator->findFieldRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(iterator->child());
		}

		inline Object findField(const std::string_view key) && noexcept {
			bool hasValue{ this->iterator->findFieldRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(iterator->child());
		}

		inline Object(JsonValueBase&& other) noexcept;

		inline Object(JsonValueBase& other) noexcept;

		inline Object(JsonIterator&& other) noexcept;

		inline Object(JsonIterator& other) noexcept;
	};
}
