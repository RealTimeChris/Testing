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
	  public:
		friend class Object;
		inline JsonIterator(JsonifierCore* other) noexcept;
		inline JsonIterator() noexcept = default;
		friend class JsonValueBase;
		inline JsonIterator(JsonIterator&& other) noexcept = default;
		inline JsonIterator& operator=(JsonIterator&& other) noexcept = default;
		inline JsonIterator(const JsonIterator& other) noexcept = default;
		inline JsonIterator& operator=(const JsonIterator& other) noexcept = default;
		inline const uint8_t* returnCurrentAndAdvance() noexcept;
		inline const uint8_t* peek(int32_t delta = 0) noexcept;
		inline const uint8_t* peek(uint32_t* position) noexcept;
		inline uint32_t* position() noexcept;
		inline void setPosition(uint32_t* target_position) noexcept;

		inline void assertAtContainerStart() noexcept;

		template<typename OTy> inline ErrorCode get(OTy&) noexcept;

		inline Array getArray() noexcept;
		inline Object operator[](const std::string_view key) & noexcept;

		inline Object operator[](const std::string_view key) && noexcept;

		inline JsonIterator resumeValueIterator() noexcept;

		inline JsonIterator getRootValueIterator() noexcept;

		inline Field findField(const char* keyNew) noexcept;

		inline uint32_t currentOffset() noexcept;

		inline Object getObject() & noexcept;

		inline uint64_t* advance() noexcept;

		inline void setPosition(int32_t delta = 0) noexcept;

		inline size_t getOffset() noexcept;

		inline JsonIterator& getCurrentIterator() noexcept;

		inline void asserAtFieldStart(size_t amountToOffset) noexcept;

		inline void assertAtObjectStart(size_t amountToOffset = 0) noexcept;

		inline void assertAtArrayStart(size_t amountToOffset = 0) noexcept;

		inline void assertAtStringStart(size_t amountToOffset = 0) noexcept;

		inline size_t getCurrentCount() noexcept;

		inline size_t size() noexcept;
		inline std::string_view unescape(RawJsonString in, uint8_t*& dst) noexcept;

		inline JsonType type() noexcept;

		inline size_t& depth() noexcept;
		inline void rewind() noexcept;

		inline void ascendTo(size_t parentDepth) noexcept;
		inline void descendTo(size_t child_depth) noexcept;
		inline std::string_view getString() noexcept;
		inline RawJsonString getRawJsonString() noexcept;
		inline uint64_t getUint64() noexcept;
		inline int64_t getInt64() noexcept;
		inline double getDouble() noexcept;
		inline bool getBool() noexcept;

		inline bool atRoot() noexcept;
		inline const uint8_t* peekLast() noexcept;
		inline JsonIterator child() noexcept;
		inline uint32_t* rootPosition() noexcept;
		inline void assertAtDocumentDepth() noexcept;
		inline bool isAlive() noexcept;
		inline void abandon() noexcept;

		inline bool findFieldUnorderedRaw(const std::string_view key) noexcept;

		inline Object findFieldUnordered(const std::string_view key) noexcept;
		inline ErrorCode skipChild(size_t parentDepth) noexcept;
		inline bool isOpen() noexcept;
		inline bool atFirstField() noexcept;

		inline bool hasNextField() noexcept;
		inline std::string_view fieldKey() noexcept;
		inline ErrorCode fieldValue() noexcept;
		inline ErrorCode findField(const std::string_view key) noexcept;
		inline bool findFieldRaw(const std::string_view key) noexcept;
		inline bool startRootArray() noexcept;
		inline bool startedArray() noexcept;
		inline bool startedRootArray() noexcept;

		inline std::string_view getRootString() noexcept;
		inline RawJsonString getRootRawJsonString() noexcept;
		inline uint64_t getRootUint64() noexcept;
		inline uint64_t getRootUint64InString() noexcept;
		inline int64_t getRootInt64() noexcept;
		inline int64_t getRootInt64InString() noexcept;
		inline double getRootDouble() noexcept;
		inline double getRootDoubleInString() noexcept;
		inline bool getRootBool() noexcept;
		inline bool isRootNull() noexcept;
		inline ErrorCode getError() noexcept;

		inline bool resetArray() noexcept;
		inline bool resetObject() noexcept;
		inline void moveAtContainerStart() noexcept;
		inline std::string toString() noexcept;

		inline const uint8_t* peekStart() noexcept;

		inline void advanceScalar() noexcept;
		inline void advanceNonRootScalar() noexcept;

		inline const uint8_t* peekScalar() noexcept;
		inline const uint8_t* peekNonRootScalar() noexcept;

		inline bool hasNextElement() noexcept;

		inline bool balanced() noexcept;
		inline ErrorCode startContainer(uint8_t start_char) noexcept;
		inline ErrorCode endContainer() noexcept;

		static inline Array startRoot(JsonIterator& iterator) noexcept;
		static inline Array started(JsonIterator& iterator) noexcept;
		inline bool startArray() noexcept;
		inline bool startObject() noexcept;
		inline bool startRootObject() noexcept;
		inline bool startedObject() noexcept;
		inline bool startedRootObject() noexcept;
		inline bool isAtStart() noexcept;

		inline void assertAtStart() noexcept;
		inline void assertAtChild() noexcept;
		inline void assertAtNext() noexcept;
		inline void assertAtNonRootStart() noexcept;
		inline uint32_t* startPosition() noexcept;
		inline uint32_t* lastPosition() noexcept;
		inline uint32_t* endPosition() noexcept;
		friend class RawJsonString;
		inline Object operator*() noexcept;
		inline bool operator!=(const JsonIterator&) noexcept;
		inline JsonIterator& operator++() noexcept;
		inline JsonIterator(const Object& iterator) noexcept;

	  protected:
		ErrorCode error{ ErrorCode::Success };
		uint8_t* currentStringBuffer{};
		uint32_t* currentStructural{};
		uint8_t* rootStringBuffer{};
		uint32_t* rootStructural{};
		uint8_t* rootStringView{};
		size_t structuralCount{};
		size_t currentDepth{ 0 };
	};
}