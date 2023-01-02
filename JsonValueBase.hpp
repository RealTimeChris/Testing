#pragma once

#include "JsonIteratorBase.hpp"

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
		inline Object getObject() & noexcept;
		template<typename OTy> inline ErrorCode get(OTy&) noexcept;
		inline JsonValueBase() noexcept {};
		inline JsonValueBase(JsonIterator&& other) noexcept;
		inline JsonValueBase(JsonifierCore* other) noexcept;
		inline JsonValueBase(JsonValueBase&& other) noexcept;
		inline JsonValueBase& operator=(JsonValueBase&& other) noexcept;
		inline JsonValueBase(const JsonValueBase& other) noexcept;
		inline JsonValueBase& operator=(const JsonValueBase& other) noexcept;

		inline uint32_t* position() const noexcept;
		inline std::string_view unescape(RawJsonString&) noexcept;
		inline std::string toString() noexcept;
		inline const char* currentLocation() noexcept;

		inline JsonValueBase(uint8_t* stringView, JsonifierCore* parser) noexcept;

		inline void start_document() noexcept;
		inline JsonType type() const noexcept;

	  protected:
		std::unique_ptr<JsonIterator> iterator{ std::make_unique<JsonIterator>() };
		ErrorCode error{ ErrorCode::Success };
		JsonifierCore* parser{};
		uint8_t* stringBuffer{};
		size_t currentDepth{};
		uint32_t* root{};

	};

}