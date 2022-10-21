#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4251)

#include <unordered_map>
#include <string_view>
#include <concepts>
#include <iostream>
#include <vector>
#include <atomic>
#include <memory>
#include <thread>
#include <chrono>
#include <deque>
#include <map>

#ifdef _WIN32
	#pragma comment(lib, "Ws2_32.lib")
	#include <WinSock2.h>
	#include <process.h>
	#ifdef max
		#undef max
	#endif
	#ifdef min
		#undef min
	#endif
#else
	#define Jsonifier_Dll
inline uint64_t ntohll(uint64_t x) {
	uint8_t theData[8]{};
	std::copy(&(x), &(x) + sizeof(x), theData);
	uint64_t theValue{};
	for (uint32_t y = 0; y < sizeof(uint64_t); ++y) {
		theValue |= static_cast<uint64_t>(theData[y]) << 8 * (sizeof(uint64_t) - y - 1);
	}
	return theValue;
}
	#include <arpa/inet.h>
#endif

template<typename ObjectType, typename DeleterType = void> using UniquePtrD = std::unique_ptr<ObjectType, DeleterType>;
template<typename KeyType, typename ObjectType> using UMap = std::unordered_map<KeyType, ObjectType>;
template<typename KeyType, typename ObjectType> using Map = std::map<KeyType, ObjectType>;
template<typename ObjectType> using UniquePtr = std::unique_ptr<ObjectType>;
template<typename ObjectType> using Atomic = std::atomic<ObjectType>;
template<typename ObjectType> using Vector = std::vector<ObjectType>;
template<typename ObjectType> using Deque = std::deque<ObjectType>;
using AtomicUint64 = std::atomic_uint64_t;
using AtomicUint32 = std::atomic_uint32_t;
using AtomicInt64 = std::atomic_int64_t;
using AtomicInt32 = std::atomic_int32_t;
using AtomicBool = std::atomic_bool;
using StringStream = std::stringstream;
using StringView = std::string_view;
using String = std::string;
using Uint64 = uint64_t;
using Uint32 = uint32_t;
using Uint16 = uint16_t;
using Uint8 = uint8_t;
using Int64 = int64_t;
using Int32 = int32_t;
using Int16 = int16_t;
using Int8 = int8_t;
using Float = float;
using Double = double;
using Snowflake = Uint64;
using Bool = bool;
using Void = void;

template<typename TimeType> class StopWatch {
  public:
	StopWatch() = delete;

	StopWatch<TimeType>& operator=(const StopWatch<TimeType>& other) {
		this->maxNumberOfMs.store(other.maxNumberOfMs.load());
		this->startTime.store(other.startTime.load());
		return *this;
	}

	StopWatch(const StopWatch<TimeType>& other) {
		*this = other;
	}

	StopWatch(TimeType maxNumberOfMsNew) {
		this->maxNumberOfMs.store(maxNumberOfMsNew.count());
		this->startTime.store(static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count()));
	}

	Uint64 totalTimePassed() {
		Uint64 currentTime = static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
		Uint64 elapsedTime = currentTime - this->startTime.load();
		return elapsedTime;
	}

	bool hasTimePassed() {
		Uint64 currentTime = static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count());
		Uint64 elapsedTime = currentTime - this->startTime.load();
		if (elapsedTime >= this->maxNumberOfMs.load()) {
			return true;
		} else {
			return false;
		}
	}

	Void resetTimer() {
		this->startTime.store(static_cast<Uint64>(std::chrono::duration_cast<TimeType>(std::chrono::system_clock::now().time_since_epoch()).count()));
	}

  protected:
	AtomicUint64 maxNumberOfMs{ 0 };
	AtomicUint64 startTime{ 0 };
};

template<typename ReturnType> Void storeBits(String& to, ReturnType num) {
	const Uint8 byteSize{ 8 };
	ReturnType newValue = reverseByteOrder<ReturnType>(num);
	for (Uint32 x = 0; x < sizeof(ReturnType); ++x) {
		to.push_back(static_cast<Uint8>(newValue >> (byteSize * x)));
	}
}

constexpr Uint8 formatVersion{ 131 };

enum class EtfType : Uint8 {
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

template<typename ReturnType> ReturnType reverseByteOrder(const ReturnType net) {
	switch (sizeof(ReturnType)) {
		case 1: {
			return net;
		}
		case 2: {
			return ntohs(static_cast<short int>(net));
		}
		case 4: {
			return ntohl(static_cast<long int>(net));
		}
		case 8: {
			return ntohll(static_cast<unsigned long long>(net));
		}
	}
	return ReturnType{};
}

template<typename TheType>
concept IsEnum = std::is_enum<TheType>::value;

template<typename TheType>
concept IsString = std::same_as<TheType, String>;

struct EnumConverter {
	template<IsEnum EnumType> EnumConverter& operator=(EnumType other) {
		this->theUint = static_cast<Uint64>(other);
		return *this;
	};

	template<IsEnum EnumType> EnumConverter(EnumType other) {
		*this = other;
	};

	EnumConverter& operator=(EnumConverter&&) noexcept;
	EnumConverter(EnumConverter&&) noexcept;

	EnumConverter& operator=(const EnumConverter&) noexcept = delete;
	EnumConverter(const EnumConverter&) noexcept = delete;

	template<IsEnum EnumType> EnumConverter& operator=(Vector<EnumType> other) {
		this->theVector = std::move(other);
		return *this;
	};

	template<IsEnum EnumType> EnumConverter(Vector<EnumType> other) {
		*this = other;
	};

	operator Vector<Uint64>() const noexcept;

	explicit operator Uint64() const noexcept;

	bool isItAVector() const noexcept;

  protected:
	Vector<Uint64> theVector{};
	Bool vectorType{ false };
	Uint64 theUint{};
};

enum class JsonType : Int8 { Unset = 0, Object = 1, Array = 2, String = 3, Float = 4, Uint64 = 5, Int64 = 6, Bool = 7, Null = 8 };

enum class JsonifierSerializeType { Etf = 0, Json = 1 };

class Jsonifier {
  public:
	using MapAllocatorType = std::allocator<std::pair<const String, Jsonifier>>;
	template<typename ObjectType> using AllocatorType = std::allocator<ObjectType>;
	template<typename ObjectType> using AllocatorTraits = std::allocator_traits<AllocatorType<ObjectType>>;
	using ObjectType = std::map<String, Jsonifier, std::less<>, MapAllocatorType>;
	using ArrayType = std::vector<Jsonifier, AllocatorType<Jsonifier>>;
	using StringType = String;
	using FloatType = Double;
	using UintType = Uint64;
	using IntType = Int64;
	using BoolType = Bool;

	Jsonifier() noexcept = default;

	Jsonifier& operator=(const Jsonifier&) noexcept;
	Jsonifier(const Jsonifier&) noexcept;

	Jsonifier& operator=(Jsonifier&&) noexcept;
	Jsonifier(Jsonifier&&) noexcept;

	operator String&&() noexcept;

	operator String() noexcept;

	JsonType type() noexcept;

	Void refreshString(JsonifierSerializeType theOpCode);

	template<typename ReturnObjectType> ReturnObjectType get();

	union JsonValue {
		ObjectType* object;
		StringType* string;
		ArrayType* array;
		FloatType numberDouble;
		UintType numberUint;
		IntType numberInt;
		BoolType boolean;
	};

	template<typename ObjectType> Jsonifier& operator=(Vector<ObjectType> theData) noexcept {
		this->setValue(JsonType::Array);
		for (auto& value: theData) {
			this->theValue.array->push_back(Jsonifier{ value });
		}
		return *this;
	}

	template<typename ObjectType> Jsonifier(Vector<ObjectType> theData) noexcept {
		*this = theData;
	}

	template<IsString KeyType, IsString ObjectType> Jsonifier& operator=(UMap<KeyType, ObjectType> theData) noexcept {
		this->setValue(JsonType::Object);
		for (auto& [key, value]: theData) {
			(*this->theValue.object)[key] = value;
		}
		return *this;
	}

	template<IsString KeyType, IsString ObjectType> Jsonifier(UMap<KeyType, ObjectType> theData) noexcept {
		*this = theData;
	}

	template<IsString KeyType, IsEnum ObjectType> Jsonifier& operator=(UMap<KeyType, ObjectType> theData) noexcept {
		this->setValue(JsonType::Object);
		for (auto& [key, value]: theData) {
			(*this->theValue.object)[key] = value;
		}
		return *this;
	}

	template<IsString KeyType, IsEnum ObjectType> Jsonifier(UMap<KeyType, ObjectType> theData) noexcept {
		*this = theData;
	};

	template<IsString KeyType, IsEnum ObjectType> Jsonifier& operator=(Map<KeyType, ObjectType> theData) noexcept {
		this->setValue(JsonType::Object);
		for (auto& [key, value]: theData) {
			(*this->theValue.object)[key] = value;
		}
		return *this;
	}

	template<IsString KeyType, IsEnum ObjectType> Jsonifier(Map<KeyType, ObjectType> theData) noexcept {
		*this = theData;
	};

	Jsonifier& operator=(EnumConverter&& theData) noexcept;
	Jsonifier(EnumConverter&&) noexcept;

	Jsonifier& operator=(const EnumConverter& theData) noexcept;
	Jsonifier(const EnumConverter&) noexcept;

	Jsonifier& operator=(StringType&& theData) noexcept;
	Jsonifier(StringType&&) noexcept;

	Jsonifier& operator=(const StringType& theData) noexcept;
	Jsonifier(const StringType&) noexcept;

	Jsonifier& operator=(const char* theData) noexcept;
	Jsonifier(const char* theData) noexcept;

	Jsonifier& operator=(Double theData) noexcept;
	Jsonifier(Double) noexcept;

	Jsonifier& operator=(Float theData) noexcept;
	Jsonifier(Float) noexcept;

	Jsonifier& operator=(Uint64 theData) noexcept;
	Jsonifier(Uint64) noexcept;

	Jsonifier& operator=(Uint32 theData) noexcept;
	Jsonifier(Uint32) noexcept;

	Jsonifier& operator=(Uint16 theData) noexcept;
	Jsonifier(Uint16) noexcept;

	Jsonifier& operator=(Uint8 theData) noexcept;
	Jsonifier(Uint8) noexcept;

	Jsonifier& operator=(Int64 theData) noexcept;
	Jsonifier(Int64) noexcept;

	Jsonifier& operator=(Int32 theData) noexcept;
	Jsonifier(Int32) noexcept;

	Jsonifier& operator=(Int16 theData) noexcept;
	Jsonifier(Int16) noexcept;

	Jsonifier& operator=(Int8 theData) noexcept;
	Jsonifier(Int8) noexcept;

	Jsonifier& operator=(Bool theData) noexcept;
	Jsonifier(Bool) noexcept;

	Jsonifier& operator=(JsonType) noexcept;
	Jsonifier(JsonType) noexcept;

	Jsonifier& operator=(std::nullptr_t) noexcept;
	Jsonifier(std::nullptr_t) noexcept;

	Jsonifier& operator[](typename ObjectType::key_type key);

	Jsonifier& operator[](Uint64 index);

	Void emplaceBack(Jsonifier&& other) noexcept;
	Void emplaceBack(Jsonifier& other) noexcept;

	friend bool operator==(const Jsonifier&, const Jsonifier&);

	~Jsonifier() noexcept;

  private:
	JsonType theType{ JsonType::Null };
	JsonValue theValue{};
	String theString{};

	Void serializeJsonToEtfString(const Jsonifier* dataToParse);

	Void serializeJsonToJsonString(const Jsonifier* dataToParse);

	Void writeJsonObject(const ObjectType& theObjectNew);

	Void writeJsonArray(const ArrayType& theArray);

	Void writeJsonString(const StringType& string);

	template<typename NumberType,
		std::enable_if_t<std::is_integral<NumberType>::value || std::is_same<NumberType, Uint64>::value || std::is_same<NumberType, Int64>::value, int> = 0>
	Void writeJsonInt(NumberType theInt) {
		auto theIntNew = std::to_string(theInt);
		this->writeString(theIntNew.data(), theIntNew.size());
	}

	Void writeJsonFloat(const FloatType theFloat);

	Void writeJsonBool(const BoolType jsonData);

	Void writeJsonNullObject();

	Void writeJsonNullArray();

	Void writeJsonNull();

	Void writeEtfObject(const ObjectType& jsonData);

	Void writeEtfArray(const ArrayType& jsonData);

	Void writeEtfString(const StringType& jsonData);

	Void writeEtfUint(const UintType jsonData);

	Void writeEtfInt(const IntType jsonData);

	Void writeEtfFloat(const FloatType jsonData);

	Void writeEtfBool(const BoolType jsonData);

	Void writeEtfNull();

	Void writeString(const char* theData, std::size_t length);

	Void writeCharacter(const char theChar);

	Void appendVersion();

	Void setValue(JsonType theTypeNew);

	Void destroy() noexcept;
};
