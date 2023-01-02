#pragma once

#include <source_location>
#include <unordered_map>
#include <immintrin.h>
#include <string_view>
#include <functional>
#include <concepts>
#include <iostream>
#include <vector>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <bitset>
#include <array>
#include <deque>
#include <coroutine>
#include <map>

#include "NumberParsingUtils.hpp"

namespace Jsonifier {

	enum class JsonType : uint8_t {
		Document = 0,
		Object = 1,
		Array = 2,
		String = 3,
		Float = 4,
		Number = 5,
		Uint64 = 6,
		Int64 = 7,
		Bool = 8,
		Null = 9
	};

	class JsonValueBase;
	class RawJsonString;
	class JsonifierCore;
	class Object;
	class Field;
	class Array;

	class JsonIterator {
	  protected:
		uint32_t* currentPosition{};
		uint8_t* stringView{};
		uint8_t* stringBuffer{};
		ErrorCode error{ ErrorCode::Success };
		size_t currentDepth{};
		uint32_t* rootStructural{};
		size_t structuralCount{};

	  public:
		inline JsonIterator() noexcept = default;
		inline JsonIterator(JsonIterator&& other) noexcept = default;
		inline JsonIterator& operator=(JsonIterator&& other) noexcept = default;
		inline explicit JsonIterator(const JsonIterator& other) noexcept = default;
		inline JsonIterator& operator=(const JsonIterator& other) noexcept = default;
		inline ErrorCode skipChild(size_t parent_depth) noexcept;
		inline bool atRoot() noexcept;
		inline uint32_t* rootPosition() noexcept;
		inline void assertAtDocumentDepth() noexcept;
		inline void assertAtRoot() noexcept;
		inline bool atEnd() noexcept;
		inline bool isAlive() noexcept;
		inline void abandon() noexcept;
		inline const uint8_t* returnCurrentAndAdvance() noexcept;
		inline bool is_single_token() noexcept;
		inline void assertMoreTokens(uint32_t required_tokens = 1) noexcept;
		inline void assertValidPosition(uint32_t* position) noexcept;
		inline const uint8_t* peek(int32_t delta = 0) noexcept;
		inline uint32_t peekLength(int32_t delta = 0) noexcept;
		inline const uint8_t* unsafePointer() noexcept;
		inline const uint8_t* peek(uint32_t* position) noexcept;
		inline uint32_t peekLength(uint32_t* position) noexcept;
		inline const uint8_t* peekLast() noexcept;
		inline void ascendTo(size_t parent_depth) noexcept;
		inline void descendTo(size_t child_depth) noexcept;
		inline void descendTo(size_t child_depth, int32_t delta) noexcept;
		inline size_t depth() noexcept;
		inline uint8_t*& stringBufLoc() noexcept;
		inline ErrorCode reportError(ErrorCode error, const char* message) noexcept;
		inline ErrorCode optionalError(ErrorCode error, const char* message) noexcept;
		inline uint32_t currentOffset() const noexcept;
		inline const uint8_t* peek(int32_t delta = 0) const noexcept;
		inline uint32_t peekLength(int32_t delta = 0) const noexcept;
		inline const uint8_t* peek(uint32_t* position) const noexcept;
		inline uint32_t peekLength(uint32_t* position) const noexcept;
		inline uint32_t* position() const noexcept;
		inline void setPosition(uint32_t* target_position) noexcept;

	  protected:
		inline uint32_t peekIndex(int32_t delta = 0) const noexcept;
		inline uint32_t peekIndex(uint32_t* position) const noexcept;

		template<int N>
		inline bool copyToBuffer(const uint8_t* json, uint32_t max_len, uint8_t (&tmpbuf)[N]) noexcept;

		inline uint32_t* position() noexcept;
		inline std::string_view unescape(RawJsonString in) noexcept;
		inline void reenterChild(uint32_t* position, size_t child_depth) noexcept;
		inline std::string toString() noexcept;
		inline const char* currentLocation() noexcept;
		inline void rewind() noexcept;
		inline bool balanced() noexcept;

	  protected:
		inline JsonIterator(JsonifierCore* buf) noexcept;
		inline uint32_t* lastPosition() noexcept;
		inline uint32_t* endPosition() noexcept;
		inline uint32_t* end() noexcept;

		friend class Document;
		friend class Object;
		friend class Array;
		friend class Value;
		friend class RawJsonString;
		friend class JsonValueBase;
		friend class ValueIterator;
	};
}