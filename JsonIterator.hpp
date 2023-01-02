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
		inline JsonIterator() noexcept = default;
		inline JsonIterator(JsonIterator&& other) noexcept = default;
		inline JsonIterator& operator=(JsonIterator&& other) noexcept = default;
		inline JsonIterator(const JsonIterator& other) noexcept = default;
		inline JsonIterator& operator=(const JsonIterator& other) noexcept = default;
		inline const uint8_t* returnCurrentAndAdvance() noexcept;
		inline const uint8_t* peek(int32_t delta = 0) const noexcept;
		inline const uint8_t* peek(uint32_t* position) const noexcept;
		inline uint32_t* position() const noexcept;
		inline void setPosition(uint32_t* target_position) noexcept;
		inline JsonIterator(const uint8_t* stringView, uint32_t* position) noexcept;

		inline void assertAtContainerStart() const noexcept;
		inline JsonIterator(JsonifierCore* other) noexcept;

		template<typename OTy> inline ErrorCode get(OTy&) noexcept;

		inline Array getArray() noexcept;
		inline Object operator[](const std::string_view key) & noexcept;

		inline Object operator[](const std::string_view key) && noexcept;

		inline JsonIterator resumeValueIterator() noexcept;

		inline JsonIterator getRootValueIterator() noexcept;

		inline Field findField(const char* keyNew) noexcept;

		inline uint32_t currentOffset() noexcept;

		inline JsonifierCore* getCore() noexcept;

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
		inline std::string_view unescape(RawJsonString in, uint8_t*& dst) const noexcept;

		inline JsonType type() noexcept;

		inline size_t depth() const noexcept;
		inline void rewind() noexcept;
		
		inline void ascendTo(size_t parentDepth) noexcept;
		inline void descendTo(size_t child_depth) noexcept;
		inline void descendTo(size_t child_depth, int32_t delta) noexcept;
		inline std::string_view getString() noexcept;
		inline RawJsonString getRawJsonString() noexcept;
		inline uint64_t getUint64() noexcept;
		inline uint64_t get_uint64_in_string() noexcept;
		inline int64_t getInt64() noexcept;
		inline int64_t getInt64InString() noexcept;
		inline double getDouble() noexcept;
		inline double getDoubleInString() noexcept;
		inline bool getBool() noexcept;

		inline bool atRoot() const noexcept;
		inline const uint8_t* peekLast() const noexcept;
		inline JsonIterator child() noexcept;
		inline uint32_t* rootPosition() const noexcept;
		inline void assertAtDocumentDepth() const noexcept;
		inline bool isAlive() const noexcept;
		inline void abandon() noexcept;

		inline bool findFieldUnorderedRaw(const std::string_view key) noexcept;

		inline Object findFieldUnordered(const std::string_view key) noexcept;
		inline ErrorCode skipChild(size_t parentDepth) noexcept;
		inline bool isOpen() const noexcept;
		inline bool atFirstField() const noexcept;
		
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
		template<typename OTy> inline JsonIterator(JsonIterator* jsonIter, size_t depth, uint32_t* start_index) noexcept;

		inline const uint8_t* peekStart() const noexcept;

		inline void advanceScalar() noexcept;
		inline void advanceNonRootScalar() noexcept;

		inline const uint8_t* peekScalar() noexcept;
		inline const uint8_t* peekNonRootScalar() noexcept;

		inline bool hasNextElement() noexcept;

		inline bool balanced() noexcept;
		inline ErrorCode startContainer(uint8_t start_char) noexcept;
		inline ErrorCode endContainer() noexcept;
		inline const uint8_t* advance_to_value() noexcept;

		inline ErrorCode error_unless_more_tokens(uint32_t tokens = 1) const noexcept;

		static inline Array startRoot(JsonIterator& iterator) noexcept;
		static inline Array started(JsonIterator& iterator) noexcept;
		inline bool startObject() noexcept;
		inline bool startRootObject() noexcept;
		inline bool startedObject() noexcept;
		inline bool startedRootObject() noexcept;
		inline bool isAtStart() const noexcept;

		inline void assertAtStart() const noexcept;
		inline void assertAtChild() const noexcept;
		inline void assertAtNext() const noexcept;
		inline void assertAtNonRootStart() const noexcept;
		inline uint32_t* startPosition() const noexcept;
		inline uint32_t* lastPosition() const noexcept;
		inline uint32_t* endPosition() const noexcept;
		inline ErrorCode report_error(ErrorCode error, const char* message) noexcept;
		friend class RawJsonString;
		inline Object operator*() noexcept;
		inline bool operator==(const JsonIterator&) const noexcept;
		inline JsonIterator& operator++() noexcept;
		inline JsonIterator(const Object& iterator) noexcept;
		JsonifierCore* parser{};
	  protected:
		ErrorCode error{ ErrorCode::Success };
		uint32_t* currentPosition{};
		uint32_t* rootPositionVal{};
		const uint8_t* stringView{};
		size_t currentDepth{1};
	};
}