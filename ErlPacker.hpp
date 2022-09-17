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


#include <discordcoreapi/FoundationEntities.hpp>

enum class JsonParserState { Starting_Object = 0, Adding_Object_Elements = 1, Starting_Array = 2, Adding_Array_Elements = 3 };

enum class JsonParseEvent : uint16_t {
	Null_Value = 1 << 0,
	Object_Start = 1 << 1,
	Object_End = 1 << 2,
	Array_Start = 1 << 3,
	Array_End = 1 << 4,
	String = 1 << 5,
	Boolean = 1 << 6,
	Number_Integer = 1 << 7,
	Number_Integer_Small = 1 << 8,
	Number_Integer_Large = 1 << 9,
	Number_Float = 1 << 11,
	Number_Double = 1 << 12,
	Key = 1 << 13
};

struct JsonValue {
	template<typename JsonObjectType>
	JsonValue& operator=(std::vector<JsonObjectType>);
	JsonValue& operator=(uint32_t);
	JsonValue()noexcept = default;
	JsonParseEvent theEvent{};
	std::string theValue{};
	std::vector<JsonValue> theValues{};
	operator std::string();
};

class JsonSerializer {
public:
	JsonSerializer() noexcept = default;

	JsonSerializer& operator=(bool);
	template<typename JsonObjectType>
	JsonSerializer& operator=(std::vector<JsonObjectType>theData);
	JsonSerializer& operator=(uint32_t);
	JsonSerializer& operator=(JsonParseEvent);

	std::string getString();

	std::vector<JsonValue>& operator[](std::string keyName);

	JsonSerializer& operator[](const char* keyName);

	template<std::same_as<uint64_t> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		std::string theString = std::to_string(theData);
		JsonValue theValue{};
		theValue.theEvent = JsonParseEvent::Number_Integer_Large;
		theValue.theValue = theString;
		this->theValues.push_back(theValue);
	}

	template<std::same_as<int64_t> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		std::string theString = std::to_string(theData);
		JsonValue theValue{};
		theValue.theEvent = JsonParseEvent::Number_Integer_Large;
		theValue.theValue = theString;
		this->theValues.push_back(theValue);
	}

	template<std::same_as<int32_t> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		std::string theString = std::to_string(theData);
		JsonValue theValue{};
		theValue.theEvent = JsonParseEvent::Number_Integer;
		theValue.theValue = theString;
		this->theValues.push_back(theValue);
	}

	template<std::same_as<uint32_t> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		std::string theString = std::to_string(theData);
		JsonValue theValue{};
		theValue.theEvent = JsonParseEvent::Number_Integer;
		theValue.theValue = theString;
		this->theValues.push_back(theValue);
	}

	template<std::same_as<int16_t> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		std::string theString = std::to_string(theData);
		this->theValues.push_back({ .theEvent = JsonParseEvent::Number_Integer, .theValue = theString });
	}

	template<std::same_as<uint16_t> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		std::string theString = std::to_string(theData);
		this->theValues.push_back({ .theEvent = JsonParseEvent::Number_Integer, .theValue = theString });
	}

	template<std::same_as<int8_t> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		std::string theString = std::to_string(theData);
		JsonValue theValue{};
		theValue.theEvent = JsonParseEvent::Number_Integer_Small;
		theValue.theValue = theString;
		this->theValues.push_back(theValue);
	}

	template<std::same_as<uint8_t> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		std::string theString = std::to_string(theData);
		JsonValue theValue{};
		theValue.theEvent = JsonParseEvent::Number_Integer_Small;
		theValue.theValue = theString;
		this->theValues.push_back(theValue);
	}

	template<std::same_as<std::string> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		JsonValue theValue{};
		theValue.theEvent = JsonParseEvent::String;
		theValue.theValue = theData;
		this->theValues.push_back(theValue);
	}

	template<std::same_as<bool> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		std::string theString{};
		if (theData) {
			theString = "true";
		}
		else {
			theString = "false";
		}
		JsonValue theValue{};
		theValue.theEvent = JsonParseEvent::Boolean;
		theValue.theValue = theString;
		this->theValues.push_back(theValue);
	}

	template<std::same_as<float> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		std::string theString = std::to_string(theData);
		this->theValues.push_back({ .theEvent = JsonParseEvent::Number_Integer, .theValue = theString });
	}

	template<std::same_as<double> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		std::string theString = std::to_string(theData);
		this->theValues.push_back({ .theEvent = JsonParseEvent::Number_Integer, .theValue = theString });
	}

	template<std::same_as<const char*> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
		}
		JsonValue theValue{};
		theValue.theEvent = JsonParseEvent::String;
		theValue.theValue = theData;
		this->theValues.push_back(theValue);
	}

	template<std::same_as<JsonParseEvent> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
			theValue.theEvent = theData;
			theValue.theValue = "null";
			this->theValues.push_back(theValue);
		}
		else {
			JsonValue theValue{};
			theValue.theEvent = theData;
			theValue.theValue = "";
			this->theValues.push_back(theValue);
		}
	}

	template<std::same_as<JsonSerializer> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
			for (auto& value : theData.theValues) {
				this->theValues.push_back(value);
			}
		}
		else {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = "";
			this->theValues.push_back(theValue);
			for (auto& value : theData.theValues) {
				this->theValues.push_back(value);
			}
		}
	}

	template<std::same_as<std::nullptr_t> JsonObjectType> void addEvent(JsonObjectType theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			JsonValue theValue{};
			theValue.theEvent = JsonParseEvent::Key;
			theValue.theValue = keyName;
			this->theValues.push_back(theValue);
			this->theValues.push_back({ .theEvent = theData, .theValue = "null" });
		}
		else {
			this->theValues.push_back({ .theEvent = theData, .theValue = "" });
		}
	}

	template<typename KeyType, typename ObjectType> void addEvent(std::unordered_map<KeyType, ObjectType> theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			this->theValues.push_back({ .theEvent = JsonParseEvent::Object_Start, .theValue = keyName });
			for (auto& [key, value] : theData) {
				this->addEvent(value, key);
			}
			this->theValues.push_back({ .theEvent = JsonParseEvent::Object_End, .theValue = keyName });
		}
		else {
			this->theValues.push_back({ .theEvent = JsonParseEvent::Object_Start, .theValue = "" });
			for (auto& [key, value] : theData) {
				this->addEvent(value, key);
			}
			this->theValues.push_back({ .theEvent = JsonParseEvent::Object_End, .theValue = "" });
		}
	}

	template<typename JsonObjectType> void addEvent(std::vector<JsonObjectType> theData, const char* keyName = nullptr) {
		if (keyName != nullptr) {
			this->theValues.push_back({ .theEvent = JsonParseEvent::Array_Start, .theValue = keyName });
			for (auto& [key, value] : theData) {
				this->addEvent(value, key);
			}
			this->theValues.push_back({ .theEvent = JsonParseEvent::Array_End, .theValue = keyName });
		}
		else {
			this->theValues.push_back({ .theEvent = JsonParseEvent::Array_Start, .theValue = "" });
			for (auto& [key, value] : theData) {
				this->addEvent(value, key);
			}
			this->theValues.push_back({ .theEvent = JsonParseEvent::Array_End, .theValue = "" });
		}
	}

	operator std::string();

protected:
	std::unordered_map<std::string, int32_t> theDepths{};
	std::vector<JsonValue> theValues{};
	JsonParserState theState{};
	size_t currentPosition{};
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

