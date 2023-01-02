#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class Object : public JsonValueBase {
	  public:
		inline auto begin() noexcept {
			return IteratorBaseBase{ *this };
		}

		inline auto end() noexcept {
			return IteratorBaseBase{ *this };
		}
		inline size_t countFields() noexcept;
		inline Object() noexcept = default;

		static inline Object start(IteratorBaseBase& iter) noexcept {
			iter.startObject();
			return Object(iter);
		}

		static inline Object resume(IteratorBaseBase&& iter) noexcept {
			return iter;
		}

		static inline Object startRoot(IteratorBaseBase&& iter) noexcept;

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

		inline Object(const IteratorBaseBase&) noexcept;

		inline Object(IteratorBaseBase&&) noexcept;

		inline Object(JsonValueBase&& other) : JsonValueBase{ std::move(other) } {};

		inline Object(JsonValueBase& other) : JsonValueBase{ std::move(other) } {};
	};
}
