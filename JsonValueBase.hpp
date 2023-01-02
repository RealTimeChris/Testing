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
		inline JsonValueBase() noexcept {};
		inline JsonValueBase(JsonIterator&& other) noexcept;
		inline JsonValueBase(JsonifierCore* other) noexcept;
		inline JsonValueBase(JsonValueBase&& other) noexcept;
		inline JsonValueBase& operator=(JsonValueBase&& other) noexcept;
		inline JsonValueBase(const JsonValueBase& other) noexcept;
		inline JsonValueBase& operator=(const JsonValueBase& other) noexcept;
		inline std::string_view unescape(RawJsonString&) noexcept;
		inline std::string toString() noexcept;

		JsonIterator begin() noexcept;
		JsonIterator end() noexcept;

		inline JsonValueBase(uint8_t* stringView, JsonifierCore* parser) noexcept;
		inline JsonType type() const noexcept;

	  protected:
		std::unique_ptr<JsonIterator> iterator{ std::make_unique<JsonIterator>() };
		ErrorCode error{ ErrorCode::Success };
		uint8_t* stringBufferLocation{};
		JsonifierCore* parser{};

		inline uint8_t*& getStringBuffer() noexcept;
		
		inline uint32_t* getStructuralIndices() noexcept;
	};

}