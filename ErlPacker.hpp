/*
	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2021, 2022 Chris M. (RealTimeChris)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
	USA
*/
/// ErlPacker.hpp - Header for the erlpacking class.
/// Nov 8, 2021
/// https://discordcoreapi.com
/// \file ErlPacker.hpp

#ifndef ERL_PACKER_02
#define ERL_PACKER_02

#include <discordcoreapi/FoundationEntities.hpp>
#include <nlohmann/json.hpp>
#include <simdjson.h>
#include <coroutine>
#include <stdint.h>
#include <set>

template<typename T>
class generator;

namespace detail
{
	template<typename T>
	class generator_promise
	{
	public:

		using value_type = std::remove_reference_t<T>;
		using reference_type = std::conditional_t<std::is_reference_v<T>, T, T&>;
		using pointer_type = value_type*;

		generator_promise() = default;

		generator<T> get_return_object() noexcept;

		constexpr std::suspend_always initial_suspend() const noexcept { return {}; }
		constexpr std::suspend_always final_suspend() const noexcept { return {}; }

		template<
			typename U = T,
			std::enable_if_t<!std::is_rvalue_reference<U>::value, int> = 0>
		std::suspend_always yield_value(std::remove_reference_t<T>& value) noexcept
		{
			m_value = std::addressof(value);
			return {};
		}

		std::suspend_always yield_value(std::remove_reference_t<T>&& value) noexcept
		{
			m_value = std::addressof(value);
			return {};
		}

		void unhandled_exception()
		{
			m_exception = std::current_exception();
		}

		void return_void()
		{
		}

		reference_type value() const noexcept
		{
			return static_cast<reference_type>(*m_value);
		}

		// Don't allow any use of 'co_await' inside the generator coroutine.
		template<typename U>
		std::suspend_never await_transform(U&& value) = delete;

		void rethrow_if_exception()
		{
			if (m_exception)
			{
				std::rethrow_exception(m_exception);
			}
		}

	private:

		pointer_type m_value;
		std::exception_ptr m_exception;

	};

	struct generator_sentinel {};

	template<typename T>
	class generator_iterator
	{
		using coroutine_handle = std::coroutine_handle<generator_promise<T>>;

	public:

		using iterator_category = std::input_iterator_tag;
		// What type should we use for counting elements of a potentially infinite sequence?
		using difference_type = std::ptrdiff_t;
		using value_type = typename generator_promise<T>::value_type;
		using reference = typename generator_promise<T>::reference_type;
		using pointer = typename generator_promise<T>::pointer_type;

		// Iterator needs to be default-constructible to satisfy the Range concept.
		generator_iterator() noexcept
			: m_coroutine(nullptr)
		{}

		explicit generator_iterator(coroutine_handle coroutine) noexcept
			: m_coroutine(coroutine)
		{}

		friend bool operator==(const generator_iterator& it, generator_sentinel) noexcept
		{
			return !it.m_coroutine || it.m_coroutine.done();
		}

		friend bool operator!=(const generator_iterator& it, generator_sentinel s) noexcept
		{
			return !(it == s);
		}

		friend bool operator==(generator_sentinel s, const generator_iterator& it) noexcept
		{
			return (it == s);
		}

		friend bool operator!=(generator_sentinel s, const generator_iterator& it) noexcept
		{
			return it != s;
		}

		generator_iterator& operator++()
		{
			m_coroutine.resume();
			if (m_coroutine.done())
			{
				m_coroutine.promise().rethrow_if_exception();
			}

			return *this;
		}

		// Need to provide post-increment operator to implement the 'Range' concept.
		void operator++(int)
		{
			(void)operator++();
		}

		reference operator*() const noexcept
		{
			return m_coroutine.promise().value();
		}

		pointer operator->() const noexcept
		{
			return std::addressof(operator*());
		}

	private:

		coroutine_handle m_coroutine;
	};
}

template<typename T>
class [[nodiscard]] generator
{
public:

	using promise_type = detail::generator_promise<T>;
	using iterator = detail::generator_iterator<T>;

	generator() noexcept
		: m_coroutine(nullptr)
	{}

	generator(generator&& other) noexcept
		: m_coroutine(other.m_coroutine)
	{
		other.m_coroutine = nullptr;
	}

	generator(const generator& other) = delete;

	~generator()
	{
		if (m_coroutine)
		{
			m_coroutine.destroy();
		}
	}

	generator& operator=(generator other) noexcept
	{
		swap(other);
		return *this;
	}

	iterator begin()
	{
		if (m_coroutine)
		{
			m_coroutine.resume();
			if (m_coroutine.done())
			{
				m_coroutine.promise().rethrow_if_exception();
			}
		}

		return iterator{ m_coroutine };
	}

	detail::generator_sentinel end() noexcept
	{
		return detail::generator_sentinel{};
	}

	void swap(generator& other) noexcept
	{
		std::swap(m_coroutine, other.m_coroutine);
	}

private:

	friend class detail::generator_promise<T>;

	explicit generator(std::coroutine_handle<promise_type> coroutine) noexcept
		: m_coroutine(coroutine)
	{}

	std::coroutine_handle<promise_type> m_coroutine;

};

template<typename T>
void swap(generator<T>& a, generator<T>& b)
{
	a.swap(b);
}

namespace detail
{
	template<typename T>
	generator<T> generator_promise<T>::get_return_object() noexcept
	{
		using coroutine_handle = std::coroutine_handle<generator_promise<T>>;
		return generator<T>{ coroutine_handle::from_promise(*this) };
	}
}

template<typename FUNC, typename T>
generator<std::invoke_result_t<FUNC&, typename generator<T>::iterator::reference>> fmap(FUNC func, generator<T> source)
{
	for (auto&& value : source)
	{
		co_yield std::invoke(func, static_cast<decltype(value)>(value));
	}
}

template<typename T>
class [[nodiscard]] recursive_generator
{
public:

	class promise_type final
	{
	public:

		promise_type() noexcept
			: m_value(nullptr)
			, m_exception(nullptr)
			, m_root(this)
			, m_parentOrLeaf(this)
		{}

		promise_type(const promise_type&) = delete;
		promise_type(promise_type&&) = delete;

		auto get_return_object() noexcept
		{
			return recursive_generator<T>{ *this };
		}

		std::suspend_always initial_suspend() noexcept
		{
			return {};
		}

		std::suspend_always final_suspend() noexcept
		{
			return {};
		}

		void unhandled_exception() noexcept
		{
			m_exception = std::current_exception();
		}

		void return_void() noexcept {}

		std::suspend_always yield_value(T& value) noexcept
		{
			m_value = std::addressof(value);
			return {};
		}

		std::suspend_always yield_value(T&& value) noexcept
		{
			m_value = std::addressof(value);
			return {};
		}

		auto yield_value(recursive_generator&& generator) noexcept
		{
			return yield_value(generator);
		}

		auto yield_value(recursive_generator& generator) noexcept
		{
			struct awaitable
			{

				awaitable(promise_type* childPromise)
					: m_childPromise(childPromise)
				{}

				bool await_ready() noexcept
				{
					return this->m_childPromise == nullptr;
				}

				void await_suspend(std::coroutine_handle<promise_type>) noexcept
				{}

				void await_resume()
				{
					if (this->m_childPromise != nullptr)
					{
						this->m_childPromise->throw_if_exception();
					}
				}

			private:
				promise_type* m_childPromise;
			};

			if (generator.m_promise != nullptr)
			{
				m_root->m_parentOrLeaf = generator.m_promise;
				generator.m_promise->m_root = m_root;
				generator.m_promise->m_parentOrLeaf = this;
				generator.m_promise->resume();

				if (!generator.m_promise->is_complete())
				{
					return awaitable{ generator.m_promise };
				}

				m_root->m_parentOrLeaf = this;
			}

			return awaitable{ nullptr };
		}

		// Don't allow any use of 'co_await' inside the recursive_generator coroutine.
		template<typename U>
		std::suspend_never await_transform(U&& value) = delete;

		void destroy() noexcept
		{
			std::coroutine_handle<promise_type>::from_promise(*this).destroy();
		}

		void throw_if_exception()
		{
			if (m_exception != nullptr)
			{
				std::rethrow_exception(std::move(m_exception));
			}
		}

		bool is_complete() noexcept
		{
			return std::coroutine_handle<promise_type>::from_promise(*this).done();
		}

		T& value() noexcept
		{
			assert(this == m_root);
			assert(!is_complete());
			return *(m_parentOrLeaf->m_value);
		}

		void pull() noexcept
		{
			assert(this == m_root);
			assert(!m_parentOrLeaf->is_complete());

			m_parentOrLeaf->resume();

			while (m_parentOrLeaf != this && m_parentOrLeaf->is_complete())
			{
				m_parentOrLeaf = m_parentOrLeaf->m_parentOrLeaf;
				m_parentOrLeaf->resume();
			}
		}

	private:

		void resume() noexcept
		{
			std::coroutine_handle<promise_type>::from_promise(*this).resume();
		}

		std::add_pointer_t<T> m_value;
		std::exception_ptr m_exception;

		promise_type* m_root;

		// If this is the promise of the root generator then this field
		// is a pointer to the leaf promise.
		// For non-root generators this is a pointer to the parent promise.
		promise_type* m_parentOrLeaf;

	};

	recursive_generator() noexcept
		: m_promise(nullptr)
	{}

	recursive_generator(promise_type& promise) noexcept
		: m_promise(&promise)
	{}

	recursive_generator(recursive_generator&& other) noexcept
		: m_promise(other.m_promise)
	{
		other.m_promise = nullptr;
	}

	recursive_generator(const recursive_generator& other) = delete;
	recursive_generator& operator=(const recursive_generator& other) = delete;

	~recursive_generator()
	{
		if (m_promise != nullptr)
		{
			m_promise->destroy();
		}
	}

	recursive_generator& operator=(recursive_generator&& other) noexcept
	{
		if (this != &other)
		{
			if (m_promise != nullptr)
			{
				m_promise->destroy();
			}

			m_promise = other.m_promise;
			other.m_promise = nullptr;
		}

		return *this;
	}

	class iterator
	{
	public:

		using iterator_category = std::input_iterator_tag;
		// What type should we use for counting elements of a potentially infinite sequence?
		using difference_type = std::ptrdiff_t;
		using value_type = std::remove_reference_t<T>;
		using reference = std::conditional_t<std::is_reference_v<T>, T, T&>;
		using pointer = std::add_pointer_t<T>;

		iterator() noexcept
			: m_promise(nullptr)
		{}

		explicit iterator(promise_type* promise) noexcept
			: m_promise(promise)
		{}

		bool operator==(const iterator& other) const noexcept
		{
			return m_promise == other.m_promise;
		}

		bool operator!=(const iterator& other) const noexcept
		{
			return m_promise != other.m_promise;
		}

		iterator& operator++()
		{
			assert(m_promise != nullptr);
			assert(!m_promise->is_complete());

			m_promise->pull();
			if (m_promise->is_complete())
			{
				auto* temp = m_promise;
				m_promise = nullptr;
				temp->throw_if_exception();
			}

			return *this;
		}

		void operator++(int)
		{
			(void)operator++();
		}

		reference operator*() const noexcept
		{
			assert(m_promise != nullptr);
			return static_cast<reference>(m_promise->value());
		}

		pointer operator->() const noexcept
		{
			return std::addressof(operator*());
		}

	private:

		promise_type* m_promise;

	};

	iterator begin()
	{
		if (m_promise != nullptr)
		{
			m_promise->pull();
			if (!m_promise->is_complete())
			{
				return iterator(m_promise);
			}

			m_promise->throw_if_exception();
		}

		return iterator(nullptr);
	}

	iterator end() noexcept
	{
		return iterator(nullptr);
	}

	void swap(recursive_generator& other) noexcept
	{
		std::swap(m_promise, other.m_promise);
	}

private:

	friend class promise_type;

	promise_type* m_promise;

};

template<typename T>
void swap(recursive_generator<T>& a, recursive_generator<T>& b) noexcept
{
	a.swap(b);
}

// Note: When applying fmap operator to a recursive_generator we just yield a non-recursive
// generator since we generally won't be using the result in a recursive context.
template<typename FUNC, typename T>
recursive_generator<std::invoke_result_t<FUNC&, typename recursive_generator<T>::iterator::reference>> fmap(FUNC func, recursive_generator<T> source)
{
	for (auto&& value : source)
	{
		co_yield std::invoke(func, static_cast<decltype(value)>(value));
	}
}

enum class JsonParserState { Adding_Object_Elements = 0, Adding_Array_Elements = 1 };

enum class JsonParseEvent : uint16_t {
	Unset = 0 << 0,
	Null_Value = 1 << 1,
	Object_Start = 1 << 2,
	Object_End = 1 << 3,
	Array_Start = 1 << 4,
	Array_End = 1 << 5,
	String = 1 << 6,
	Boolean = 1 << 7,
	Number_Integer = 1 << 8,
	Number_Integer_Small = 1 << 9,
	Number_Integer_Large = 1 << 10,
	Number_Float = 1 << 11,
	Number_Double = 1 << 12
};

class JsonSerializer;

class JsonObject;

struct JsonScalarValue {
	
	friend class JsonSerializer;

	friend class JsonObject;

	JsonScalarValue() noexcept = default;
	JsonScalarValue& operator=(const JsonScalarValue&) noexcept;
	JsonScalarValue(const JsonScalarValue&) noexcept;
	JsonScalarValue& operator=(int8_t) noexcept;
	JsonScalarValue& operator=(int16_t) noexcept;
	JsonScalarValue& operator=(int32_t) noexcept;
	JsonScalarValue& operator=(int64_t) noexcept;
	JsonScalarValue& operator=(uint8_t) noexcept;
	JsonScalarValue& operator=(uint16_t) noexcept;
	JsonScalarValue& operator=(uint32_t) noexcept;
	JsonScalarValue& operator=(uint64_t) noexcept;
	JsonScalarValue& operator=(bool) noexcept;
	JsonScalarValue& operator=(double) noexcept;
	JsonScalarValue& operator=(float) noexcept;
	JsonScalarValue& operator=(std::string) noexcept;
	JsonScalarValue& operator=(const char*) noexcept;
	JsonScalarValue(int8_t) noexcept;
	JsonScalarValue(int16_t) noexcept;
	JsonScalarValue(int32_t) noexcept;
	JsonScalarValue(int64_t) noexcept;
	JsonScalarValue(uint8_t) noexcept;
	JsonScalarValue(uint16_t) noexcept;
	JsonScalarValue(uint32_t) noexcept;
	JsonScalarValue(uint64_t) noexcept;
	JsonScalarValue(bool) noexcept;
	JsonScalarValue(double) noexcept;
	JsonScalarValue(float) noexcept;
	JsonScalarValue(std::string) noexcept;
	JsonScalarValue(const char*) noexcept;

	operator std::string() noexcept;
protected:
	JsonParseEvent theEvent{};
	std::string theValue{};
	
};

class JsonObject {
public:
	friend class JsonSerializer;

	JsonObject() noexcept = default;
	JsonObject& operator=(const JsonObject&) noexcept = default;
	JsonObject(const JsonObject&) noexcept = default;
	JsonObject& operator=(JsonObject&&) noexcept = default;
	JsonObject(JsonObject&&) noexcept = default;

	
	JsonObject& operator=(const JsonScalarValue& theData) noexcept;
	JsonObject(const JsonScalarValue& theData) noexcept;
	JsonObject& operator[](const char*) noexcept;
protected:
	std::map<std::string, JsonObject> theValues{};
	JsonScalarValue theScalarValue{};
	size_t theIndentationLevel{ 0 };
	std::string theMostRecentKey{};
	JsonParseEvent theEvent{};
	std::string theKey{};
};

class JsonSerializer {
public:
	JsonSerializer(const char*) noexcept;
	JsonSerializer(const JsonSerializer& other) noexcept = default;
	JsonSerializer& operator=(const JsonSerializer& other) noexcept = default;
	std::string getString(JsonObject& theValueNew) noexcept;
	std::string getScalarObject(JsonObject& theObject) noexcept;
	bool doesItExist(const char* keyName, JsonObject& theRecords) noexcept;

	JsonObject& operator[](const char* keyName) noexcept;

	operator std::string() noexcept;

protected:
	JsonParserState theCurrentState{};
	size_t theIndentationLevel{ 0 };
	std::string theMostRecentKey{};
	JsonObject theJsonData{};
};

	struct ErlPackError : public std::runtime_error {
	public:
		explicit ErlPackError(const std::string& message);
	};

	constexpr uint8_t formatVersion{ 131 };

	enum class ETFTokenType : uint8_t {
		New_Float_Ext = 70,
		Small_Integer_Ext = 97,
		Integer_Ext = 98,
		Float_Ext = 99,
		Atom_Ext = 100,
		Small_Tuple_Ext = 104,
		Large_Tuple_Ext = 105,
		Nil_Ext = 106,
		String_Ext = 107,
		List_Ext = 108,
		Binary_Ext = 109,
		Small_Big_Ext = 110,
		Large_Big_Ext = 111,
		Small_Atom_Ext = 115,
		Map_Ext = 116,
		Atom_Utf8_Ext = 118
	};

	class ErlPacker {
	public:
		ErlPacker() noexcept {};

		std::string parseJsonToEtf(std::string&);

		std::string& parseEtfToJson(std::string_view dataToParse);
		~ErlPacker() {};
	protected:
		std::string comparisongStringFalse{ "false" };
		std::string comparisongStringNil{ "nil" };
		std::string falseString{ "false" };
		std::string nilString{ "nil" };
		std::string bufferString{};
		std::string_view buffer{};
		uint64_t offSet{};
		uint64_t size{};

		void singleValueJsonToETF(simdjson::ondemand::value jsonData);

		void writeString(simdjson::ondemand::value jsonData);

		void writeNumber(simdjson::ondemand::value jsonData);

		void writeBool(simdjson::ondemand::value jsonData);

		void writeToBuffer(const std::string&);

		void appendVersion();

		void appendSmallIntegerExt(uint8_t);

		void appendIntegerExt(uint32_t);

		void appendNewFloatExt(double);

		void appendNilExt();

		void appendNil();

		void appendFalse();

		void appendTrue();

		void appendUnsignedLongLong(uint64_t);

		void appendBinaryExt(const std::string&, uint32_t);

		void appendListHeader(uint32_t);

		void appendMapHeader(uint32_t);

		template<typename ReturnType> ReturnType readBits() {
			if (this->offSet + sizeof(ReturnType) > this->size) {
				throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of the buffer.\n\n" };
			}
			const ReturnType newValue = *reinterpret_cast<const ReturnType*>(this->buffer.data() + this->offSet);
			this->offSet += sizeof(ReturnType);
			return DiscordCoreAPI::reverseByteOrder<const ReturnType>(newValue);
		}

		const char* readString(uint32_t length);

		std::string singleValueETFToJson();

		std::string parseSmallIntegerExt();

		std::string parseBigint(uint32_t);

		std::string parseIntegerExt();

		std::string parseNewFloatExt();

		std::string parseFloatExt();

		std::string processAtom(const char* atom, uint32_t length);

		std::string parseTuple(uint32_t);

		std::string parseSmallTupleExt();

		std::string parseLargeTupleExt();

		std::string parseNilExt();

		std::string parseStringAsList();

		std::string parseListExt();

		std::string parseBinaryExt();

		std::string parseSmallBigExt();

		std::string parseLargeBigExt();

		std::string parseArray(uint32_t);

		std::string parseSmallAtomExt();

		std::string parseMapExt();

		std::string parseAtomUtf8Ext();
	};

#endif // !ERL_PACKER