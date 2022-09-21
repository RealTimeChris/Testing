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

enum class BaseEnum :uint8_t;
enum class DerivedEnum :uint8_t {
	test_value = 1
};

enum class JsonParserState { Starting_Object = 0, Adding_Object_Elements = 1, Starting_Array = 2, Adding_Array_Elements = 3 };

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

template<typename TheType>
concept IsEnum = std::is_enum<TheType>::value;

struct EnumConverter {
	template<IsEnum EnumType> EnumConverter(EnumType other) {
		this->thePtr = new uint64_t{};
		*static_cast<uint64_t*>(this->thePtr) = static_cast<uint64_t>(other);
	};

	template<IsEnum EnumType> EnumConverter(std::vector<EnumType> other) {
		this->thePtr = new std::vector<uint64_t>{};
		for (auto& value : other) {
			static_cast<std::vector<uint64_t>*>(this->thePtr)->emplace_back(static_cast<uint64_t>(value));
		}
		this->vectorType = true;
	};

	operator std::vector<uint64_t>() {
		std::vector<uint64_t> theObject{};
		for (auto& value : *static_cast<std::vector<uint64_t>*>(this->thePtr)) {
			theObject.emplace_back(value);
		}

		return theObject;
	}

	explicit operator uint64_t() {
		uint64_t theObject{};
		theObject = *static_cast<uint64_t*>(this->thePtr);
		return theObject;
	}

	~EnumConverter() {
		delete this->thePtr;
	}

	void* thePtr{};
	bool vectorType{ false };
};

class JsonRecord {
public:
	JsonRecord() noexcept = default;
	JsonRecord& operator=(EnumConverter other);
	JsonRecord& operator=(int8_t) noexcept;
	JsonRecord& operator=(int16_t) noexcept;
	JsonRecord& operator=(int32_t) noexcept;
	JsonRecord& operator=(int64_t) noexcept;
	JsonRecord& operator=(uint8_t) noexcept;
	JsonRecord& operator=(uint16_t) noexcept;
	JsonRecord& operator=(uint32_t) noexcept;
	JsonRecord& operator=(uint64_t) noexcept;
	JsonRecord& operator=(bool) noexcept;
	JsonRecord& operator=(double) noexcept;
	JsonRecord& operator=(float) noexcept;
	JsonRecord& operator=(std::string&) noexcept;
	JsonRecord& operator=(const char*) noexcept;
	JsonRecord& operator[](const char*) noexcept;
	JsonRecord(int8_t) noexcept;
	JsonRecord(int16_t) noexcept;
	JsonRecord(int32_t) noexcept;
	JsonRecord(int64_t) noexcept;
	JsonRecord(uint8_t) noexcept;
	JsonRecord(uint16_t) noexcept;
	JsonRecord(uint32_t) noexcept;
	JsonRecord(uint64_t) noexcept;
	JsonRecord(bool) noexcept;
	JsonRecord(double) noexcept;
	JsonRecord(float) noexcept;
	JsonRecord(std::string&) noexcept;
	JsonRecord(const char*) noexcept;

	void pushBack(JsonRecord& other) noexcept;
	void pushBack(JsonRecord&& other) noexcept;
	operator std::string() noexcept;
	std::unordered_map<std::string, JsonRecord> theJsonData{};
	std::vector<JsonRecord> theArrayData{};
	JsonParseEvent theEvent{ JsonParseEvent::Unset };
	size_t currentObjectOrArrayStartIndex{ 0 };
	JsonParserState theState{};
	std::string theValue{};
	std::string theKey{};
};

class JsonSerializer {
public:
	friend class JsonRecord;
	JsonSerializer() noexcept = default;

	template<typename KeyType, typename ObjectType> JsonSerializer& operator=(std::unordered_map<KeyType, ObjectType> other) {
		for (auto& [key, value] : other) {
			JsonRecord theRecord{};
			theRecord = value;
			theRecord.theKey = key;
			this->theJsonData[key] = theRecord;
		}
		if (other.size() == 0) {
			JsonRecord theRecord{};
			theRecord.theValue = "";
			theRecord.theEvent = JsonParseEvent::Null_Value;
			this->theJsonData[""] = theRecord;
		}
		return *this;
	}

	template<typename ObjectType, std::enable_if<std::is_enum<EnumConverter>::value, ObjectType>> JsonSerializer& operator=(std::vector<ObjectType> other) {
		for (auto& [key, value] : other) {
			JsonRecord theRecord{};
			theRecord = value;
			theRecord.theKey = key;
			this->theJsonData[key] = theRecord;
		}
		if (other.size() == 0) {
			JsonRecord theRecord{};
			theRecord.theValue = "";
			theRecord.theEvent = JsonParseEvent::Null_Value;
			this->theJsonData[""] = theRecord;
		}
		return *this;
	}

	template<std::same_as<size_t> ObjectType> JsonSerializer& operator=(std::vector<ObjectType> other) {
		for (auto& [key, value] : other) {
			JsonRecord theRecord{};
			theRecord = value;
			theRecord.theKey = key;
			this->theJsonData[key] = theRecord;
		}
		if (other.size() == 0) {
			JsonRecord theRecord{};
			theRecord.theValue = "";
			theRecord.theEvent = JsonParseEvent::Null_Value;
			this->theJsonData[""] = theRecord;
		}
		return *this;
	}

	template<std::same_as<std::string> ObjectType> JsonSerializer& operator=(std::vector<ObjectType> other) {
		for (auto& [key, value] : other) {
			JsonRecord theRecord{};
			theRecord = value;
			theRecord.theKey = key;
			this->theJsonData[key] = theRecord;
		}
		if (other.size() == 0) {
			JsonRecord theRecord{};
			theRecord.theValue = "";
			theRecord.theEvent = JsonParseEvent::Null_Value;
			this->theJsonData[""] = theRecord;
		}
		return *this;
	}

	std::string getString();
	JsonRecord& operator[](const char* keyName) noexcept;
	operator std::string() noexcept;

protected:
	std::unordered_map<std::string, JsonRecord> theJsonData{};
	size_t currentObjectOrArrayStartIndex{ 0 };
	JsonParserState theState{};
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