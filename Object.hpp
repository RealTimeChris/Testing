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

		inline ObjectIterator(JsonIterator& iter) noexcept;
	};

	class Object {
	  public:
		friend class JsonIterator;

		inline size_t countFields() noexcept;
		inline Object() noexcept = default;

		ObjectIterator end() noexcept {
			return ObjectIterator{ this->iterator };
		}

		ObjectIterator begin() noexcept {
			return ObjectIterator{ this->iterator };
		}

		static inline Object start(JsonIterator& iterator) noexcept {
			iterator.startObject();
			return Object(iterator);
		}

		static inline Object resume(JsonIterator&& iterator) noexcept {
			return iterator;
		}

		inline Object(const ValueIterator& _iter) noexcept : iterator{ _iter } {
		}

		inline Object startRoot(ValueIterator& iter) noexcept {
			iter.start_root_object();
			return Object(iter);
		}

		inline Object findFieldUnordered(const std::string_view key) & noexcept {
			bool hasValue{ this->iterator.findFieldUnorderedRaw(key) };
			if (!hasValue) {
				return Object(iterator.child());
			}
			return Object(iterator.child());
		}

		inline Object findFieldUnordered(const std::string_view key) && noexcept {
			bool hasValue{ this->iterator.findFieldUnorderedRaw(key) };
			if (!hasValue) {
				return Object(iterator.child());
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
				return Object(iterator.child());
			}
			return Object(iterator.child());
		}

		inline Object findField(const std::string_view key) && noexcept {
			bool hasValue{ this->iterator.findFieldRaw(key) };
			if (!hasValue) {
				return Object(iterator.child());
			}
			return Object(iterator.child());
		}

		inline Object(JsonValueBase&& other) noexcept;

		inline Object(JsonValueBase& other) noexcept;

		inline Object(JsonIterator&& other) noexcept;

		inline Object(JsonIterator& other) noexcept;

	  protected:
		ValueIterator iterator{};
	};
}
