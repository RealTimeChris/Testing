#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class JsonValueBase;

	class Field;

	class ObjectIterator {
	  public:
		inline ObjectIterator(const JsonValueBase& _iter) noexcept : iter{ _iter } {
		}

		inline Field operator*() noexcept;

		inline bool operator==(ObjectIterator& other) noexcept;

		inline ObjectIterator& operator++() noexcept;
		

	  private:
		JsonValueBase iter{};
		friend struct JsonifierResult<ObjectIterator>;
	};

	class Object : public JsonValueBase {
	  public:
		inline auto begin() noexcept {
			return ObjectIterator{ *this };
		}

		inline auto end() noexcept {
			return ObjectIterator{ *this };
		}
		inline size_t count_fields() noexcept;
		inline Object() noexcept = default;

		static inline Object start(JsonValueBase& iter) noexcept {
			iter.start_object();
			return Object(iter);
		}

		static inline Object resume(JsonValueBase&& iter) noexcept {
			return iter;
		}

		static inline Object start_root(JsonValueBase& iter) noexcept {
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

		inline Object(JsonValueBase&& other) : JsonValueBase{ std::move(other) } {};

		inline Object(JsonValueBase& other) : JsonValueBase{ std::move(other) } {};
	};
}
