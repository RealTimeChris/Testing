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

#pragma once

#include <discordcoreapi/FoundationEntities.hpp>
#include <nlohmann/json.hpp>
#include <simdjson.h>

struct JsonString{

	JsonString() {
	}
	void appendKey(std::string theKey){
		if (haveWeStarted) {
			this->theObject.append(",");
		}
		else {
			haveWeStarted = true;
		}
		this->theObject.append("\"");
		this->theObject.append(theKey);
		this->theObject.append("\":");
	}
	void startNewObject() {
		this->theObject.append("{");
	}
	void endNewObject() {
		this->theObject.append("}");
	}
	void finishAppending() {
	}
	void appendUint8( uint8_t objectValue) {
		this->theObject.append(std::to_string(objectValue));
	}

	void appendUint16(uint16_t objectValue) {
		this->theObject.append(std::to_string(objectValue));
	}

	void appendUint32(uint32_t objectValue) {
		this->theObject.append(std::to_string(objectValue));
	}

	void appendUint64(uint64_t objectValue) {
		this->theObject.append(std::to_string(objectValue));
	}

	void appendBool(bool objectValue) {
		std::stringstream theStream{};
		theStream << std::boolalpha << objectValue;
		this->theObject.append(theStream.str());
	}

	void appendBinary(std::string objectValue) {
		this->theObject.append(objectValue);
	}

	void appendString(std::string objectValue) {
		this->theObject.append(R"(")");
		this->theObject.append(objectValue);
		this->theObject.append(R"(")");
	}

	template<typename ObjectType02>
	void appendArray(std::vector<ObjectType02>objectValue) {

	}

	operator std::string() {
		return this->theObject;
	}

protected:
	bool haveWeStarted{ false };
	std::string theObject{};
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
		std::string parseJsonToEtf(nlohmann::json&);

		std::string parseEtfToJson(std::string& dataToParse);

		ErlPacker() noexcept = default;

	protected:
		std::string comparisongStringFalse{ "false" };
		std::string comparisongStringNil{ "nil" };
		std::string falseString{ "false" };
		simdjson::dom::parser theParser{};
		std::string nilString{ "nil" };
		std::string bufferString{};
		char* buffer{ nullptr };
		uint64_t offSet{};
		uint64_t size{};

		void singleValueJsonToETF(nlohmann::json&);

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
			const ReturnType newValue = *reinterpret_cast<const ReturnType*>(this->buffer + this->offSet);
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