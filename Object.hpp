#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class ObjectIterator : public JsonIterator {
	  public:
		inline ObjectIterator() noexcept = default;
		inline Field operator*() noexcept;
		inline bool operator==(const ObjectIterator&) const noexcept;
		inline bool operator!=(const ObjectIterator&) const noexcept;
		inline ObjectIterator& operator++() noexcept;

		inline ObjectIterator(Object* iter) noexcept;
	};

	class Object : public JsonValueBase {
	  public:
		friend class JsonIterator;

		inline size_t countFields() noexcept;
		inline Object() noexcept = default;

		auto end() noexcept {
			return ObjectIterator{ this };
		}

		auto begin() noexcept {
			return ObjectIterator{ this };
		}

		static inline Object start(JsonIterator& iterator) noexcept {
			iterator.startObject();
			return Object(iterator);
		}

		static inline Object resume(JsonIterator&& iterator) noexcept {
			return iterator;
		}

		static inline Object startRoot(JsonIterator&& iterator) noexcept;

		inline Object findFieldUnordered(const std::string_view key) & noexcept {
			bool hasValue{ this->iterator.findFieldUnorderedRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(iterator.child());
		}

		inline Object findFieldUnordered(const std::string_view key) && noexcept {
			bool hasValue{ this->iterator.findFieldUnorderedRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(iterator.child());
		}

		inline Object operator[](const std::string_view key) & noexcept {
			return findFieldUnordered(key);
		}

		inline Object operator[](const std::string_view key) && noexcept {
			return std::forward<Object>(*this).findFieldUnordered(key);
		}

		inline Object findField(const std::string_view key) & noexcept {
			bool hasValue{ this->iterator.findFieldRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(iterator.child());
		}

		inline Object findField(const std::string_view key) && noexcept {
			bool hasValue{ this->iterator.findFieldRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(iterator.child());
		}

		inline Object(JsonValueBase&& other) noexcept;

		inline Object(JsonValueBase& other) noexcept;

		inline Object(JsonIterator&& other) noexcept;

		inline Object(JsonIterator& other) noexcept;
	};
}
