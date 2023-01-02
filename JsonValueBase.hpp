#pragma once

#include "JsonIterator.hpp"

namespace Jsonifier {

	class JsonifierCore;
	class JsonPrimitive;
	class Object;
	class Array;
	class Field;

	class JsonValueBase {
	  public:
		friend class JsonIterator;

		inline ErrorCode getError() noexcept;
		inline Object getObject() noexcept;
		inline Array getArray() noexcept;
		template<typename OTy> inline ErrorCode get(OTy&) noexcept;
		inline JsonValueBase(JsonIterator&& other) noexcept;
		inline JsonValueBase(JsonIterator& other) noexcept;
		inline JsonValueBase(JsonValueBase&& other) noexcept = default;
		inline JsonValueBase& operator=(JsonValueBase&& other) noexcept = default;
		inline JsonValueBase(const JsonValueBase& other) noexcept = default;
		inline JsonValueBase& operator=(const JsonValueBase& other) noexcept;
		inline std::string_view unescape(RawJsonString&) noexcept;
		inline std::string toString() noexcept;

		JsonIterator begin() noexcept;
		JsonIterator end() noexcept;
		inline JsonType type() const noexcept;

	  protected:
		JsonIterator iterator{};
		ErrorCode error{ ErrorCode::Success };

		inline uint8_t*& getStringBuffer() noexcept;
		
		inline uint32_t* getStructuralIndices() noexcept;
	};

}