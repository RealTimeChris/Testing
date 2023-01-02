#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class Array : public JsonValueBase {
	  public:
		inline Array() noexcept = default;
		inline size_t countElements() & noexcept;
		inline Object at(size_t index) noexcept;
		inline Array(JsonIterator&) noexcept;
	};

}