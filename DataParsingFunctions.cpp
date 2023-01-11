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
/// DataParsingFunctions.cpp - Source file for the data-parsing functions.
/// Oct 1, 2021
/// https://discordcoreapi.com
/// \file DataParsingFunctions.cpp

#include "DataParsingFunctions.hpp"
#include <jsonifier/Index.hpp>

namespace Jsonifier {

	int64_t getInt64(simdjson::ondemand::value jsonData, const char* key) {
		int64_t value{};
		if (jsonData[key].get(value) == ErrorCode::Success) {
			return int64_t{ value };
		} else {
			return 0;
		}
	}

	int32_t getInt32(simdjson::ondemand::value jsonData, const char* key) {
		int64_t value{};
		if (jsonData[key].get(value) == ErrorCode::Success) {
			return static_cast<int32_t>(value);
		} else {
			return 0;
		}
	}

	int16_t getInt16(simdjson::ondemand::value jsonData, const char* key) {
		int64_t value{};
		if (jsonData[key].get(value) == ErrorCode::Success) {
			return static_cast<int16_t>(value);
		} else {
			return 0;
		}
	}

	int8_t getInt8(simdjson::ondemand::value jsonData, const char* key) {
		int64_t value{};
		if (jsonData[key].get(value) == ErrorCode::Success) {
			return static_cast<int8_t>(value);
		} else {
			return 0;
		}
	}

	uint64_t getUint64(simdjson::ondemand::value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData[key].get(value) == ErrorCode::Success) {
			return uint64_t{ value };
		} else {
			return 0;
		}
	}

	uint32_t getUint32(simdjson::ondemand::value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData[key].get(value) == ErrorCode::Success) {
			return static_cast<uint32_t>(value);
		} else {
			return 0;
		}
	}

	uint16_t getUint16(simdjson::ondemand::value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData[key].get(value) == ErrorCode::Success) {
			return static_cast<uint16_t>(value);
		} else {
			return 0;
		}
	}

	uint8_t getUint8(simdjson::ondemand::value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData[key].get(value) == ErrorCode::Success) {
			return static_cast<uint8_t>(value);
		} else {
			return 0;
		}
	}

	float getFloat(simdjson::ondemand::value jsonData, const char* key) {
		double value{};
		if (jsonData[key].get(value) == ErrorCode::Success) {
			return double{ value };
		} else {
			return 0.0f;
		}
	}

	bool getBool(simdjson::ondemand::value jsonData, const char* key) {
		bool value{};
		if (jsonData[key].get(value) == ErrorCode::Success) {
			return bool{ value };
		} else {
			return false;
		}
	}

	std::string getString(simdjson::ondemand::value jsonData, const char* key) {
		std::string_view value{};
		if (jsonData[key].get(value) == ErrorCode::Success) {
			return std::string{ value.data(), value.size() };
		} else {
			return "";
		}
	}

	bool getObject(simdjson::ondemand::value& object, const char* key, simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData[key].get(object) == ErrorCode::Success) {
			return true;
		} else {
			return false;
		}
	}

	bool getArray(simdjson::ondemand::array& array, const char* key, simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData[key].get(array) == ErrorCode::Success) {
			return true;
		} else {
			return false;
		}
	}

	bool getArray(simdjson::ondemand::array& array, simdjson::ondemand::value jsonObjectData) {
		if (jsonObjectData.get(array) == ErrorCode::Success) {
			return true;
		} else {
			return false;
		}
	}

	std::string getString(ObjectReturnData jsonData, const char* key) {
		std::string_view value{};
		if (jsonData.didItSucceed && jsonData.object[key].get(value) == ErrorCode::Success) {
			return static_cast<std::string>(value);
		}
		return static_cast<std::string>(value);
	}
	std::string getString(simdjson::ondemand::value jsonData) {
		std::string_view value{};
		if (jsonData.get(value) == ErrorCode::Success) {
			return std::string{ value.data(), value.size() };
		} else {
			return "";
		}
	}

	ObjectReturnData getObject(simdjson::ondemand::value jsonData, const char* objectName) {
		ObjectReturnData value{};
		if (jsonData[objectName].get(value.object) == ErrorCode::Success) {
			value.didItSucceed = true;
		}
		return value;
	}
	ObjectReturnData getObject(ObjectReturnData jsonData, const char* objectName) {
		ObjectReturnData value{};
		if (jsonData.didItSucceed && jsonData.object[objectName].get(value.object) == ErrorCode::Success) {
			value.didItSucceed = true;
		}
		return value;
	}
	ObjectReturnData getObject(ArrayReturnData jsonData, uint64_t objectIndex) {
		ObjectReturnData value{};
		if (jsonData.didItSucceed && jsonData.arrayValue.at(objectIndex).get(value.object) == ErrorCode::Success) {
			value.didItSucceed = true;
		}
		return value;
	}
	ArrayReturnData getArray(simdjson::ondemand::value jsonData, const char* arrayName) {
		ArrayReturnData value{};
		if (jsonData[arrayName].get(value.arrayValue) == ErrorCode::Success) {
			value.didItSucceed = true;
		}
		return value;
	}
	ArrayReturnData getArray(ObjectReturnData jsonData, const char* arrayName) {
		ArrayReturnData value{};
		if (jsonData.didItSucceed && jsonData.object[arrayName].get(value.arrayValue) == ErrorCode::Success) {
			value.didItSucceed = true;
		}
		return value;
	}

	int64_t getInt64(Value jsonData, const char* key) {
		int64_t value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return value;
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	int32_t getInt32(Value jsonData, const char* key) {
		int64_t value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return static_cast<int32_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	int16_t getInt16(Value jsonData, const char* key) {
		int64_t value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return static_cast<int16_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	int8_t getInt8(Value jsonData, const char* key) {
		int64_t value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return static_cast<int8_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	uint64_t getUint64(Value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return uint64_t{ value };
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	uint32_t getUint32(Value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return static_cast<uint32_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	uint16_t getUint16(Value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return static_cast<uint16_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	uint8_t getUint8(Value jsonData, const char* key) {
		uint64_t value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return static_cast<uint8_t>(value);
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	float getFloat(Value jsonData, const char* key) {
		double value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return double{ value };
			} else {
				return 0;
			}
		} else {
			return 0.0f;
		}
	}

	double getFloat(Object jsonData, const char* key) {
		double value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return double{ value };
			} else {
				return 0;
			}
		} else {
			return 0.0f;
		}
	}

	bool getBool(Value jsonData, const char* key) {
		bool value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return bool{ value };
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	std::string getString(Value jsonData, const char* key) {
		std::string_view value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(value) == ErrorCode::Success) {
				return std::string{ value.data(), value.size() };
			} else {
				return "";
			}
		} else {
			return "";
		}
	}

	bool getObject(Object& object, const char* key, Value jsonData) {
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(object) == ErrorCode::Success) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	bool getObject(Value& object, const char* key, Value jsonData) {
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(object) == ErrorCode::Success) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	bool getArray(Array& array, const char* key, Value jsonData) {
		if (jsonData.type() != JsonType::Null) {
			if (jsonData[key].get(array) == ErrorCode::Success) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	bool getArray(Array& array, Value jsonData) {
		if (jsonData.type() != JsonType::Null) {
			if (jsonData.get(array) == ErrorCode::Success) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	std::string getString(Value jsonData) {
		std::string_view value{};
		if (jsonData.type() != JsonType::Null) {
			if (jsonData.get(value) == ErrorCode::Success) {
				return std::string{ value.data(), value.size() };
			} else {
				return "";
			}
		} else {
			return "";
		}
	}
};
