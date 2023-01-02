#pragma once

#include "Value.hpp"

namespace Jsonifier {

	class ValueIterator;
	class ArrayIterator;

	class Value {
	  public:
		inline Value() noexcept = default;
		template<typename T> inline T  get() noexcept {
			static_assert(!sizeof(T), "The get method with given type is not implemented by the simdjson library.");
		}
		template<typename T> inline ErrorCode get(T& out) noexcept;
		inline Array getArray() noexcept;
		inline Object getObject() noexcept;
		inline uint64_t getUint64() noexcept;
		inline uint64_t getUint64InString() noexcept;
		inline int64_t getInt64() noexcept;
		inline int64_t getInt64InString() noexcept;
		inline double getDouble() noexcept;
		inline double getDoubleInString() noexcept;
		inline std::string_view getString() noexcept;
		inline RawJsonString getRawJsonString() noexcept;
		inline bool getBool() noexcept;
		inline bool isNull() noexcept;
		inline ArrayIterator begin() & noexcept;
		inline ArrayIterator end() & noexcept;
		inline size_t countElements() & noexcept;
		inline size_t countFields() & noexcept;
		inline Value at(size_t index) noexcept;
		inline Value findField(std::string_view key) noexcept;
		inline Value findField(const char* key) noexcept;
		inline Value findFieldUnordered(std::string_view key) noexcept;
		inline Value findFieldUnordered(const char* key) noexcept;
		inline Value operator[](std::string_view key) noexcept;
		inline Value operator[](const char* key) noexcept;
		inline JsonType type() noexcept;
		inline bool isScalar() noexcept;
		inline std::string_view rawJsonToken() noexcept;
		inline const char* currentLocation() noexcept;
		inline int32_t currentDepth() const noexcept;
		inline Value atPointer(std::string_view json_pointer) noexcept;

	  protected:
		inline Value(const ValueIterator& iter) noexcept;
		inline void skip() noexcept;
		static inline Value start(const ValueIterator& iter) noexcept;
		static inline Value resume(const ValueIterator& iter) noexcept;
		inline Object startOrResumeObject() noexcept;
		ValueIterator iter{};

		friend class ArrayIterator;
		friend class Object;
		friend struct Value;
	};

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
			iter.startObject();
			return Object(iter);
		}

		static inline Object resume(ValueIterator&& iter) noexcept {
			return iter;
		}

		static inline Object startRoot(ValueIterator& iter) noexcept {
			iter.startRootObject();
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

		inline Object findField(const std::string_view key) & noexcept {
			bool hasValue{ this->findFieldRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(child());
		}

		inline Object findField(const std::string_view key) && noexcept {
			bool hasValue{ this->findFieldRaw(key) };
			if (!hasValue) {
				return Object{};
			}
			return Object(child());
		}

		inline Object(ValueIterator&& other) : ValueIterator{ std::move(other) } {};

		inline Object(ValueIterator& other) : ValueIterator{ std::move(other) } {};
	};
}
