#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class ArrayIterator : public JsonIterator {
	  public:
		inline ArrayIterator() noexcept = default;
		inline Object operator*() noexcept;
		inline bool operator==(const ArrayIterator&) noexcept;
		inline bool operator!=(const ArrayIterator&) noexcept;
		inline ArrayIterator& operator++() noexcept;

		inline ArrayIterator(JsonIterator& iter) noexcept;
	};

	class Array : public JsonValueBase {
	  public:
		inline Array(JsonIterator&) noexcept;
		static inline Array start(JsonIterator& iter) noexcept;
		inline size_t countElements() & noexcept;
		inline Object at(size_t index) noexcept;
		static Array startRoot(JsonIterator& iter) noexcept;

	};

}