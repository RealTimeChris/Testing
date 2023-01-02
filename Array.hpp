#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class Array : public JsonValueBase {
	  public:
		inline Array() noexcept = default;
		inline JsonIteratorBase<Array> begin() noexcept;
		inline JsonIteratorBase<Array> end() noexcept;
		inline size_t countElements() & noexcept;
		inline bool isEmpty() & noexcept;
		inline bool reset() & noexcept;
		inline Object atPointer(std::string_view json_pointer) noexcept;
		inline std::string_view rawJson() noexcept;
		inline Object at(size_t index) noexcept;
		inline Array(const JsonIteratorBase<Array>& iter) noexcept;
		static inline Array startRoot(JsonIteratorBase<Array>& iter) noexcept;
		static inline Array started(JsonIteratorBase<Array>& iter) noexcept;

	  protected:
		inline ErrorCode consume() noexcept;
		static inline Array start(JsonIteratorBase<Array>& iter) noexcept;

		JsonIteratorBase<Array> iter{};
	};

}