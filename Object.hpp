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
		inline size_t count_fields() noexcept;
		inline Object() noexcept = default;

		static inline Object start(ValueIterator& iter) noexcept {
			iter.start_object();
			return Object(iter);
		}

		static inline Object resume(ValueIterator&& iter) noexcept {
			return iter;
		}

		static inline Object start_root(ValueIterator& iter) noexcept {
			iter.start_root_object();
			return Object(iter);
		}

		inline Object find_field_unordered(const std::string_view key) & noexcept {
			bool has_value{ this->find_field_unordered_raw(key) };
			if (!has_value) {
				return Object{};
			}
			return Object(child());
		}

		inline Object find_field_unordered(const std::string_view key) && noexcept {
			bool has_value{ this->find_field_unordered_raw(key) };
			std::cout << "DO WE HAVE VALUE?: " << std::boolalpha << has_value << std::endl;
			if (!has_value) {
				return Object{};
			}
			return Object(child());
		}

		inline Object operator[](const std::string_view key) & noexcept {
			return find_field_unordered(key);
		}

		inline Object operator[](const std::string_view key) && noexcept {
			return std::forward<Object>(*this).find_field_unordered(key);
		}

		inline Object find_field(const std::string_view key) & noexcept {
			bool has_value{ this->find_field_raw(key) };
			if (!has_value) {
				return Object{};
			}
			return Object(child());
		}

		inline Object find_field(const std::string_view key) && noexcept {
			bool has_value{ this->find_field_raw(key) };
			if (!has_value) {
				return Object{};
			}
			return Object(child());
		}

		inline Object(ValueIterator&& other) : ValueIterator{ std::move(other) } {};

		inline Object(ValueIterator& other) : ValueIterator{ std::move(other) } {};
	};
}
