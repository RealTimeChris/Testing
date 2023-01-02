#pragma once

#include "JsonValueBase.hpp"

namespace Jsonifier {

	class Document {
	  public:
		inline Document(JsonIterator&& iteratorNew) noexcept;

		inline Document start(JsonIterator&& iterator) noexcept {
			return Document(std::forward<JsonIterator>(iterator));
		}

		template<typename OTy> inline JsonIterator getRootValueIterator() noexcept {
			return resumeValueIterator<OTy>();
		}

		inline Object getObject() & noexcept {
			auto value = getRootValueIterator<Object>();
			return Object::startRoot(std::move(value));
		}

		inline Object startOrResumeObject() noexcept {
			if (this->iterator.atRoot()) {
				return getObject();
			} else {
				return Object::resume(std::move(resumeValueIterator<Object>()));
			}
		}

		template<typename OTy> inline JsonIterator resumeValueIterator() noexcept {
			return JsonIterator{ *iterator };
		}

		inline Object findField(std::string_view key) & noexcept {
			return startOrResumeObject().findField(key);
		}

		inline Object findField(const char* key) & noexcept {
			return startOrResumeObject().findField(key);
		}

		inline Object findFieldUnordered(std::string_view key) & noexcept {
			return startOrResumeObject().findFieldUnordered(key);
		}

		inline Object findFieldUnordered(const char* key) & noexcept {
			return startOrResumeObject().findFieldUnordered(key);
		}

		inline Object operator[](std::string_view key) & noexcept {
			return startOrResumeObject()[key];
		}

		inline Object operator[](const char* key) & noexcept {
			return startOrResumeObject()[key];
		}

		inline void rewind() noexcept {
			this->iterator.rewind();
		}

		inline std::string toDebugString() noexcept {
			return this->iterator.toString();
		}

		inline int32_t currentDepth() const noexcept {
			return this->iterator.depth();
		}

		inline bool isAlive() noexcept {
			return this->iterator.isAlive();
		}

		inline Object getValue() noexcept {
			this->iterator.assertAtDocumentDepth();
			switch (*this->iterator.peek()) {
				case '[':
				case '{':
					return Object(getRootValueIterator<Object>().operator*());
				default:
					return Object{};
			}
		}

		inline Array getArray() & noexcept {
			auto value = getRootValueIterator<Array>();
			return iterator.startRoot(value);
		}

		inline uint64_t getUint64() noexcept {
			return getRootValueIterator<uint64_t>().getRootUint64();
		}

		inline uint64_t get_uint64_in_string() noexcept {
			return getRootValueIterator<uint64_t>().getRootUint64InString();
		}

		inline int64_t getInt64() noexcept {
			return getRootValueIterator<int64_t>().getRootInt64();
		}

		inline int64_t getInt64InString() noexcept {
			return getRootValueIterator<int64_t>().getRootInt64InString();
		}

		inline double getDouble() noexcept {
			return getRootValueIterator<double>().getRootDouble();
		}

		inline double getDoubleInString() noexcept {
			return getRootValueIterator<double>().getRootDoubleInString();
		}

		inline std::string_view getString() noexcept {
			return getRootValueIterator<std::string_view>().getRootString();
		}

		inline RawJsonString getRawJsonString() noexcept {
			return getRootValueIterator<RawJsonString>().getRootRawJsonString();
		}

		inline bool getBool() noexcept {
			return getRootValueIterator<bool>().getRootBool();
		}

		inline bool is_null() noexcept {
			return getRootValueIterator<bool>().isRootNull();
		}

		template<typename OTy> inline OTy get() & noexcept;

		template<typename OTy> inline OTy get() && noexcept;

		template<> inline Array get() & noexcept {
			return getArray();
		}

		template<> inline Object get() & noexcept {
			return getObject();
		}

		template<> inline RawJsonString get() & noexcept {
			return getRawJsonString();
		}

		template<> inline std::string_view get() & noexcept {
			return getString();
		}

		template<> inline double get() & noexcept {
			return getDouble();
		}

		template<> inline uint64_t get() & noexcept {
			return getUint64();
		}

		template<> inline int64_t get() & noexcept {
			return getInt64();
		}

		template<> inline bool get() & noexcept {
			return getBool();
		}

		template<> inline RawJsonString get() && noexcept {
			return getRawJsonString();
		}

		template<> inline std::string_view get() && noexcept {
			return getString();
		}

		template<> inline double get() && noexcept {
			return std::forward<Document>(*this).getDouble();
		}

		template<> inline uint64_t get() && noexcept {
			return std::forward<Document>(*this).getUint64();
		}

		template<> inline int64_t get() && noexcept {
			return std::forward<Document>(*this).getInt64();
		}

		template<> inline bool get() && noexcept {
			return std::forward<Document>(*this).getBool();
		}

		template<> inline Object get() && noexcept {
			return getValue();
		}

		template<typename T> inline ErrorCode get(T& out) & noexcept {
			return get<T>().get(out);
		}

		template<typename T> inline ErrorCode get(T& out) && noexcept {
			return std::forward<Document>(*this).get<T>().get(out);
		}

		inline size_t countElements() & noexcept {
			auto a = getArray();
			size_t answer = a.countElements();
			if (answer != 0) {
				rewind();
			}
			return answer;
		}

		inline size_t countFields() & noexcept {
			auto a = getObject();
			size_t answer = a.countFields();
			if (answer != 0) {
				rewind();
			}
			return answer;
		}

		inline Object at(size_t index) & noexcept {
			auto a = getArray();
			return a.at(index);
		}

		inline JsonIterator begin() & noexcept {
			return getArray().begin();
		}

		inline JsonIterator end() & noexcept {
			return {};
		}

	  protected:
		JsonIterator iterator{};
		JsonifierCore* core{};
	};
}