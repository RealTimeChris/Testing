#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class ValueIterator;

	class Field;

	class ObjectIterator {
	  public:
		inline ObjectIterator(const ValueIterator& _iter) noexcept : iter{ _iter } {
		}

		inline Field operator*() noexcept;

		inline bool operator==(ObjectIterator& other) noexcept;

		inline ObjectIterator& operator++() noexcept;
		

	  private:
		ValueIterator iter{};
		friend struct JsonifierResult<ObjectIterator>;
	};

	class Object : public ValueIterator {
	  public:
		inline auto begin() noexcept {
			return ObjectIterator{ *this };
		}

		inline auto end() noexcept {
			return ObjectIterator{ *this };
		}
		inline size_t countFields() noexcept;
		inline Object() noexcept = default;

		static inline Object start(ValueIterator& iter) noexcept {
			iter.start_object();
			return Object(iter);
		}

		static inline Object resume(ValueIterator&& iter) noexcept {
			return iter;
		}

		static inline Object startRoot(ValueIterator& iter) noexcept {
			iter.start_root_object();
			return Object(iter);
		}

		inline Object findFieldUnordered(const std::string_view key) & noexcept {
			bool hasValue{ this->findFieldUnorderedRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(child());
		}

		inline Object findFieldUnordered(const std::string_view key) && noexcept {
			bool hasValue{ this->findFieldUnorderedRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(child());
		}

		inline Object operator[](const std::string_view key) & noexcept {
			return findFieldUnordered(key);
		}

		inline Object operator[](const std::string_view key) && noexcept {
			return std::forward<Object>(*this).findFieldUnordered(key);
		}

		inline Object find_field(const std::string_view key) & noexcept {
			bool hasValue{ this->find_field_raw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(child());
		}

		inline Object find_field(const std::string_view key) && noexcept {
			bool hasValue{ this->find_field_raw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(child());
		}

		inline Object(ValueIterator&& other) : ValueIterator{ std::move(other) } {};

		inline Object(ValueIterator& other) : ValueIterator{ std::move(other) } {};
	};
}
