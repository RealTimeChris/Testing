#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class Array : public JsonValueBase {
	  public:
		inline Array() noexcept = default;
		inline JsonIterator begin() noexcept;
		inline JsonIterator end() noexcept;
		inline size_t countElements() & noexcept;
		inline bool isEmpty() & noexcept;
		inline bool reset() & noexcept;
		inline Object atPointer(std::string_view json_pointer) noexcept;
		inline std::string_view rawJson() noexcept;
		inline Object at(size_t index) noexcept;
		inline Array(JsonIterator&) noexcept;
	  protected:
		inline ErrorCode consume() noexcept;
	};

}