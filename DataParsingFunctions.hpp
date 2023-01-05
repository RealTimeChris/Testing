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
/// DataParsingFunctions.hpp - Header file for the data-parsing functions.
/// Oct 1, 2021
/// https://discordcoreapi.com
/// \file DataParsingFunctions.hpp

#pragma once

#ifndef DATA_PARSING_FUNCTIONS
	#define DATA_PARSING_FUNCTIONS

	#include <simdjson.h>
	#include <map>
	#include "Jsonifier.hpp"

namespace Jsonifier {

	struct ObjectReturnDataJson {
		ObjectReturnDataJson(){};
		Object object;
		bool didItSucceed{ false };
	};

	struct ArrayReturnDataJson {
		ArrayReturnDataJson(){};
		Array arrayValue;
		bool didItSucceed{ false };
	};

	int64_t getInt64(Value& jsonData, const char* key);

	int32_t getInt32(Value& jsonData, const char* key);

	int16_t getInt16(Value& jsonData, const char* key);

	int8_t getInt8(Value& jsonData, const char* key);

	uint64_t getUint64(Value& jsonData, const char* key);

	uint32_t getUint32(Value& jsonData, const char* key);

	uint16_t getUint16(Value& jsonData, const char* key);

	uint8_t getUint8(Value& jsonData, const char* key);

	float getFloat(Value& jsonData, const char* key);

	bool getBool(Value& jsonData, const char* key);

	std::string getString(Value& jsonData, const char* key);

	std::string getString(ObjectReturnDataJson& jsonData, const char* key);

	bool getObject(Value& object, const char* key, Value jsonObjectData);

	bool getArray(Array& array, const char* key, Value jsonObjectData);

	bool getArray(Array& array, Value jsonObjectData);

	std::string getString(Value& jsonData);

	ObjectReturnDataJson getObject(Value& jsonObjectData, const char* objectName);

	ObjectReturnDataJson getObject(ObjectReturnDataJson& jsonObjectData, const char* objectName);

	ObjectReturnDataJson getObject(ArrayReturnDataJson& jsonObjectData, uint64_t objectIndex);

	ArrayReturnDataJson getArray(Value& jsonObjectData, const char* arrayName);

	ArrayReturnDataJson getArray(ObjectReturnDataJson& jsonObjectData, const char* arrayName);

	struct ObjectReturnData {
		simdjson::ondemand::value object{};
		bool didItSucceed{ false };
	};

	struct ArrayReturnData {
		simdjson::ondemand::array arrayValue{};
		bool didItSucceed{ false };
	};

	int64_t getInt64(simdjson::ondemand::value jsonData, const char* key);

	int32_t getInt32(simdjson::ondemand::value jsonData, const char* key);

	int16_t getInt16(simdjson::ondemand::value jsonData, const char* key);

	int8_t getInt8(simdjson::ondemand::value jsonData, const char* key);

	uint64_t getUint64(simdjson::ondemand::value jsonData, const char* key);

	uint32_t getUint32(simdjson::ondemand::value jsonData, const char* key);

	uint16_t getUint16(simdjson::ondemand::value jsonData, const char* key);

	uint8_t getUint8(simdjson::ondemand::value jsonData, const char* key);

	float getFloat(simdjson::ondemand::value jsonData, const char* key);

	bool getBool(simdjson::ondemand::value jsonData, const char* key);

	std::string getString(simdjson::ondemand::value jsonData, const char* key);

	std::string getString(ObjectReturnData jsonData, const char* key);

	bool getObject(simdjson::ondemand::value& object, const char* key, simdjson::ondemand::value jsonObjectData);

	bool getArray(simdjson::ondemand::array& array, const char* key, simdjson::ondemand::value jsonObjectData);

	bool getArray(simdjson::ondemand::array& array, simdjson::ondemand::value jsonObjectData);

	std::string getString(simdjson::ondemand::value jsonData);

	ObjectReturnData getObject(simdjson::ondemand::value jsonObjectData, const char* objectName);

	ObjectReturnData getObject(ObjectReturnData jsonObjectData, const char* objectName);

	ObjectReturnData getObject(ArrayReturnData jsonObjectData, uint64_t objectIndex);

	ArrayReturnData getArray(simdjson::ondemand::value jsonObjectData, const char* arrayName);

	ArrayReturnData getArray(ObjectReturnData jsonObjectData, const char* arrayName);
};
#endif