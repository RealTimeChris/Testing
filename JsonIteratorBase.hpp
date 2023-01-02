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

	enum class JsonType : uint8_t { Document = 0, Object = 1, Array = 2, String = 3, Float = 4, Uint64 = 5, Int64 = 6, Bool = 7, Null = 8 };

	class JsonValueBase;
	class RawJsonString;
	class JsonifierCore;
	class Object;
	class Field;
	class Array;

	class IteratorBaseBase {
	  public:
		inline IteratorBaseBase() noexcept = default;
		inline IteratorBaseBase(IteratorBaseBase&& other) noexcept = default;
		inline IteratorBaseBase& operator=(IteratorBaseBase&& other) noexcept = default;
		inline IteratorBaseBase(const IteratorBaseBase& other) noexcept = default;
		inline IteratorBaseBase& operator=(const IteratorBaseBase& other) noexcept = default;
		inline const uint8_t* returnCurrentAndAdvance() noexcept;
		inline const uint8_t* peek(int32_t delta = 0) const noexcept;
		inline const uint8_t* peek(uint32_t* position) const noexcept;
		inline uint32_t* position() const noexcept;
		inline void setPosition(uint32_t* target_position) noexcept;
		inline bool operator==(const IteratorBaseBase& other) noexcept;
		inline bool operator!=(const IteratorBaseBase& other) noexcept;
		inline bool operator>(const IteratorBaseBase& other) noexcept;
		inline bool operator>=(const IteratorBaseBase& other) noexcept;
		inline bool operator<(const IteratorBaseBase& other) noexcept;
		inline bool operator<=(const IteratorBaseBase& other) noexcept;
		inline IteratorBaseBase(const uint8_t* buf, uint32_t* position) noexcept;

		inline IteratorBaseBase(JsonifierCore* other) noexcept;

		template<typename OTy> inline ErrorCode get(OTy&) noexcept;

		inline Object operator[](const std::string_view key) & noexcept;

		inline Object operator[](const std::string_view key) && noexcept;

		inline IteratorBaseBase resumeValueIterator() noexcept;

		inline IteratorBaseBase getRootValueIterator() noexcept;

		inline Field findField(const char* keyNew) noexcept;

		inline JsonifierCore* getCore() noexcept;

		inline Object getObject() & noexcept;

		inline uint64_t* advance() noexcept;

		inline void setPosition(int32_t delta = 0) noexcept;

		inline size_t getOffset() noexcept;

		inline IteratorBaseBase& getCurrentIterator() noexcept;

		inline void asserAtFieldStart(size_t amountToOffset) noexcept;

		inline void assertAtObjectStart(size_t amountToOffset = 0) noexcept;

		inline void assertAtArrayStart(size_t amountToOffset = 0) noexcept;

		inline void assertAtStringStart(size_t amountToOffset = 0) noexcept;

		inline size_t getCurrentCount() noexcept;

		inline uint8_t getRootKey() noexcept;

		inline size_t size() noexcept;
		inline std::string_view unescape(RawJsonString in, uint8_t*& dst) const noexcept;

		inline JsonType type() noexcept;

		
		inline void ascendTo(size_t parent_depth) noexcept;
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

		inline void assert_is_valid() const noexcept;
		inline bool is_valid() const noexcept;

	  protected:
		ErrorCode error{ ErrorCode::Success };
		uint32_t* currentPosition{};
		uint32_t* rootPosition{};
		JsonifierCore* parser{};
		size_t currentDepth{};
		const uint8_t* buf{};

		inline bool resetArray() noexcept;
		inline bool resetObject() noexcept;
		inline void moveAtContainerStart() noexcept;
		inline std::string toString() const noexcept;
		template<typename OTy> inline IteratorBaseBase(IteratorBaseBase* jsonIter, size_t depth, uint32_t* start_index) noexcept;

		inline const uint8_t* peekStart() const noexcept;

		inline void advanceScalar() noexcept;
		inline void advanceNonRootScalar() noexcept;

		inline const uint8_t* peekScalar() noexcept;
		inline const uint8_t* peekNonRootScalar() noexcept;


		inline ErrorCode startContainer(uint8_t start_char) noexcept;
		inline ErrorCode endContainer() noexcept;
		inline const uint8_t* advance_to_value() noexcept;

		inline ErrorCode error_unless_more_tokens(uint32_t tokens = 1) const noexcept;

		inline bool isAtStart() const noexcept;

		inline void assertAtStart() const noexcept;
		inline void assertAtChild() const noexcept;
		inline void assertAtNext() const noexcept;
		inline void assertAtNonRootStart() const noexcept;
		inline uint32_t* startPosition() const noexcept;
		inline uint32_t* lastPosition() const noexcept;
		inline uint32_t* endPosition() const noexcept;
		inline ErrorCode report_error(ErrorCode error, const char* message) noexcept;
	};

	
	template<class = void> class JsonIteratorBase;
	template<> class JsonIteratorBase<void> : public IteratorBaseBase {};

	template<> class JsonIteratorBase<uint64_t> : public IteratorBaseBase {};
	template<> class JsonIteratorBase<int64_t> : public IteratorBaseBase {};
	template<> class JsonIteratorBase<double> : public IteratorBaseBase {};
	template<> class JsonIteratorBase<std::string_view> : public IteratorBaseBase {};
	template<> class JsonIteratorBase<std::string> : public IteratorBaseBase {};
	template<> class JsonIteratorBase<bool> : public IteratorBaseBase {};
	template<> class JsonIteratorBase<RawJsonString> : public IteratorBaseBase {};
	template<> class JsonIteratorBase<Object> : public IteratorBaseBase {
	  public:
		inline JsonIteratorBase<Object>() noexcept = default;
		inline Object operator*() noexcept;
		inline bool operator==(const JsonIteratorBase<Object>&) const noexcept;
		inline bool operator!=(const JsonIteratorBase<Object>&) const noexcept;
		inline JsonIteratorBase<Object>& operator++() noexcept;
		inline JsonIteratorBase<Object>(const Object& iter) noexcept;

	  protected:
		JsonValueBase* iter{};
	};
	template<> class JsonIteratorBase<Array> : public IteratorBaseBase {
	  public:
		inline JsonIteratorBase<Array>() noexcept = default;
		inline Array operator*() noexcept;
		inline bool operator==(const JsonIteratorBase<Array>&) const noexcept;
		inline bool operator!=(const JsonIteratorBase<Array>&) const noexcept;
		inline JsonIteratorBase<Array>& operator++() noexcept;
		inline JsonIteratorBase<Array>(const Array& iter) noexcept;

	  protected:
		JsonValueBase* iter{};
	};
	template<> class JsonIteratorBase<Field> : public IteratorBaseBase {
	  public:
		inline JsonIteratorBase<Field>() noexcept = default;
		inline Field operator*() noexcept;
		inline bool operator==(const JsonIteratorBase<Field>&) const noexcept;
		inline bool operator!=(const JsonIteratorBase<Field>&) const noexcept;
		inline JsonIteratorBase<Field>& operator++() noexcept;
		inline JsonIteratorBase<Field>(const Field& iter) noexcept;

	  protected:
		JsonValueBase* iter{};
	};
}