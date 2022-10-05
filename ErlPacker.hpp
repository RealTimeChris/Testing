/*
	, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

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

#include "JsonObject.hpp"
/*
#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include <coroutine>
#include <stdint.h>
#include <set>




class ErlPacker {
  public:
	ErlPacker() noexcept {};

	String& parseJsonToEtf(JsonObject&& dataToParse);

	String& parseEtfToJson(StringView dataToParse);

	~ErlPacker(){};

  protected:
	String comparisongStringFalse{ "false" };
	String comparisongStringNil{ "nil" };
	String falseString{ "false" };
	String nilString{ "nil" };
	String bufferString{};
	StringView buffer{};
	Uint64 offSet{};
	Uint64 size{};

	void singleValueJsonToETF(JsonObject&& dataToParse);

	void writeObject(JsonObject::ObjectType&& jsonData);

	void writeString(JsonObject::StringType&& jsonData);

	void writeInt(JsonObject::IntType jsonData);

	void writeUint(JsonObject::UintType jsonData);

	void writeFloat(JsonObject::FloatType jsonData);

	void writeArray(JsonObject::ArrayType&& jsonData);

	void writeBool(JsonObject::BoolType jsonData);

	void writeNullExt();

	void writeNull();

	void writeToBuffer(String&&);

	void appendBinaryExt(String&&, Uint32);

	void appendUnsignedLongLong(Uint64);

	void appendSmallIntegerExt(Uint8);

	void appendIntegerExt(Uint32);

	void appendListHeader(Uint32);

	void appendMapHeader(Uint32);

	void appendNewFloatExt(Double);

	void appendVersion();

	void appendNilExt();

	void appendFalse();

	void appendTrue();

	void appendNil();

	template<typename ReturnType> ReturnType readBits() {
		if (this->offSet + sizeof(ReturnType) > this->size) {
			throw ErlPackError{ "ErlPacker::readBits() Error: readBits() past end of the buffer.\n\n" };
		}
		const ReturnType newValue = *reinterpret_cast<const ReturnType*>(this->buffer.data() + this->offSet);
		this->offSet += sizeof(ReturnType);
		return DiscordCoreAPI::reverseByteOrder<const ReturnType>(newValue);
	}

	Uint64 readString(Uint32 length);

	String processAtom(const char* atom, Uint32 length);

	String singleValueETFToJson();

	String parseSmallIntegerExt();

	String parseBigint(Uint32);

	String parseArray(Uint32);

	String parseTuple(Uint32);

	String parseSmallTupleExt();

	String parseLargeTupleExt();

	String parseSmallAtomExt();

	String parseStringAsList();

	String parseNewFloatExt();

	String parseSmallBigExt();

	String parseLargeBigExt();

	String parseAtomUtf8Ext();

	String parseIntegerExt();

	String parseBinaryExt();

	String parseFloatExt();

	String parseListExt();

	String parseNilExt();

	String parseMapExt();
};
*/
#endif // !ERL_PACKER