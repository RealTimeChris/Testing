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
		ErrorCode error{ ErrorCode::Success };
		JsonifierCore* parser{};
		uint8_t* stringBuffer{};
		size_t currentDepth{};
		uint32_t* root{};

	  public:
		inline ErrorCode getError();
		inline Object getObject() & noexcept;
		template<typename OTy> inline ErrorCode get(OTy&) noexcept;
		inline JsonValueBase() noexcept {};
		inline JsonValueBase(JsonifierCore* other) noexcept;
		inline JsonValueBase(JsonValueBase&& other) noexcept;
		inline JsonValueBase& operator=(JsonValueBase&& other) noexcept;
		inline JsonValueBase(const JsonValueBase& other) noexcept = default;
		inline JsonValueBase& operator=(const JsonValueBase& other) noexcept = default;
		inline ErrorCode skipChild(size_t parent_depth) noexcept;
		inline bool atRoot() const noexcept;
		inline uint32_t* rootPosition() const noexcept;
		inline void assertAtDocumentDepth() const noexcept;
		inline bool isAlive() const noexcept;
		inline void abandon() noexcept;
		inline const uint8_t* returnCurrentAndAdvance() noexcept;
		inline const uint8_t* peek(int32_t delta = 0) const noexcept;
		inline const uint8_t* peek(uint32_t* position) const noexcept;
		inline const uint8_t* peekLast() const noexcept;
		inline size_t depth() const noexcept;

		inline uint32_t* position() const noexcept;
		inline std::string_view unescape(RawJsonString&) noexcept;
		inline std::string toString() const noexcept;
		inline const char* currentLocation() noexcept;
		inline void rewind() noexcept;
		inline bool balanced() const noexcept;

		inline JsonValueBase(uint8_t* buf, JsonifierCore* parser) noexcept;

		inline void start_document() noexcept;
		inline ErrorCode skipChild() noexcept;
		inline bool isOpen() const noexcept;
		inline bool atFirstField() const noexcept;
		inline JsonType type() const noexcept;
		inline bool startObject() noexcept;
		inline bool startRootObject() noexcept;
		inline bool startedObject() noexcept;
		inline bool startedRootObject() noexcept;
		inline bool hasNextField() noexcept;
		inline std::string_view fieldKey() noexcept;
		inline ErrorCode fieldValue() noexcept;
		inline ErrorCode findField(const std::string_view key) noexcept;
		inline bool findFieldRaw(const std::string_view key) noexcept;
		inline void assertAtContainerStart() const noexcept;
		inline bool startRootArray() noexcept;
		inline bool startedArray() noexcept;
		inline bool startedRootArray() noexcept;
		inline bool hasNextElement() noexcept;
		template<typename OTy> inline OTy child() noexcept;

		inline bool findFieldUnorderedRaw(const std::string_view key) noexcept;

		inline Object findFieldUnordered(const std::string_view key) noexcept;

	  protected:
		JsonIteratorBase<> iterator{};
		inline uint32_t* lastPosition() const noexcept;
		inline uint32_t* endPosition() const noexcept;

	};

}