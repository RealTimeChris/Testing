#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class Array : public JsonValueBase {
	  public:
		inline Array(JsonIterator&) noexcept;
		static inline Array start(JsonIterator& iter) noexcept;
		inline size_t countElements() & noexcept;
		inline Object at(size_t index) noexcept;
	};

}