#pragma once

#include "JsonValueBase.hpp"
#include "Field.hpp"

namespace Jsonifier {

	class ObjectIterator : public JsonIterator {
	  public:
		inline ObjectIterator() noexcept = default;

		inline ObjectIterator(JsonIterator& iter) noexcept;

		inline ObjectIterator(const ValueIterator& _iter) noexcept : iterator{ _iter } {};

		inline Field operator*() noexcept {
			ErrorCode error = iterator.getError();
			if (error != ErrorCode::Success) {
				iterator.abandon();
				return Field{};
			}
			auto result = Field::start(iterator);
			if (result.key().raw() == "") {
				iterator.abandon();
			}
			return result;
		}

		inline bool operator==(const ObjectIterator& other) noexcept {
			return !(*this != other);
		}

		inline bool operator!=(const ObjectIterator&) noexcept {
			return iterator.isOpen();
		}

		inline ObjectIterator& operator++() noexcept {
			if (!iterator.isOpen()) {
				return *this;
			}

			ErrorCode error{};
			if (error = iterator.skipChild(); error != ErrorCode::Success) {
				return *this;
			}

			bool has_value{};
			if (!iterator.hasNextField()) {
				return *this;
			};
			return *this;
		}

	  protected:
		ValueIterator iterator{};

	};

	class Object {
	  public:
		friend class JsonIterator;
		inline Object() noexcept = default;

		ObjectIterator end() noexcept {
			return ObjectIterator{ this->iterator };
		}

		ObjectIterator begin() noexcept {
			return ObjectIterator{ this->iterator };
		}

		static inline Object resume(JsonIterator&& iterator) noexcept {
			return iterator;
		}

		inline Object(const ValueIterator& _iter) noexcept : iterator{ _iter } {
		}

		static inline Object startRoot(ValueIterator& iter) noexcept {
			iter.startRootObject();
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

		
		inline size_t countFields() & noexcept {
			size_t count{ 0 };
			for (auto v: *this) {
				count++;
			}
			if (iterator.getError()!=ErrorCode::Success) {
				return -1;
			}
			iterator.resetObject();
			return count;
		}

		inline Object(JsonValueBase&& other) noexcept;

		inline Object(JsonValueBase& other) noexcept;

		inline Object(JsonIterator&& other) noexcept;

		inline Object(JsonIterator& other) noexcept;

	  protected:
		ValueIterator iterator{};
	};
}
