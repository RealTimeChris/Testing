#include <discordcoreapi/Index.hpp>
#include "ErlPacker.hpp"
#include "JsonSerializer.hpp"

String& JsonSerializer::parseJsonToEtf(JsonSerializer&& dataToParse) {
	this->bufferString.clear();
	this->offSet = 0;
	this->size = 0;
	this->buffer = {};
	this->appendVersion();
	this->singleValueJsonToETF(std::move(dataToParse));
	return this->bufferString;
}

void JsonSerializer::singleValueJsonToETF(JsonSerializer&& jsonData) {
	switch (jsonData.theType) {
		case ValueType::Array: {
			this->writeArray(std::move(*jsonData.theValue.array));
			break;
		}
		case ValueType::Object: {
			this->writeObject(std::move(*jsonData.theValue.object));
			break;
		}
		case ValueType::Bool: {
			this->writeBool(jsonData.theValue.boolean);
			break;
		}
		case ValueType::Float: {
			this->writeFloat(jsonData.theValue.numberDouble);
			break;
		}
		case ValueType::Int64: {
			this->writeInt(jsonData.theValue.numberInt);
			break;
		}
		case ValueType::Uint64: {
			this->writeUint(jsonData.theValue.numberUint);
			break;
		}
		case ValueType::String: {
			this->writeString(std::move(*jsonData.theValue.string));
			break;
		}
		case ValueType::Null: {
			this->writeNull();
			break;
		}
		case ValueType::Null_Ext: {
			this->writeNullExt();
			break;
		}
	}
}

void JsonSerializer::writeNullExt() {
	this->appendNilExt();
}

void JsonSerializer::writeNull() {
	this->appendNil();
}

void JsonSerializer::writeObject(JsonSerializer::ObjectType&& jsonData) {
	Bool add_comma{ false };
	this->appendMapHeader(static_cast<Uint32>(jsonData.size()));
	for (auto field: jsonData) {
		if (add_comma) {
		}
		StringStream theStream{};
		theStream << field.first;
		String theKey = theStream.str();

		auto theSize = theKey.size();
		this->appendBinaryExt(std::move(theKey), static_cast<Uint32>(theSize));
		this->singleValueJsonToETF(std::move(field.second));
		add_comma = true;
	}
}

void JsonSerializer::writeString(JsonSerializer::StringType&& jsonData) {
	StringStream theStream{};
	theStream << jsonData;
	auto theSize = static_cast<Uint32>(theStream.str().size());
	this->appendBinaryExt(theStream.str(), theSize);
}

void JsonSerializer::writeFloat(JsonSerializer::FloatType jsonData) {
	auto theFloat = jsonData;
	this->appendNewFloatExt(theFloat);
}

void JsonSerializer::writeUint(JsonSerializer::UintType jsonData) {
	auto theInt = jsonData;
	if (theInt <= 255 && theInt >= 0) {
		this->appendSmallIntegerExt(static_cast<Uint8>(theInt));
	} else if (theInt <= std::numeric_limits<Uint32>::max() && theInt >= 0) {
		this->appendIntegerExt(static_cast<Uint32>(theInt));
	} else {
		this->appendUnsignedLongLong(theInt);
	}
}

void JsonSerializer::writeInt(JsonSerializer::IntType jsonData) {
	auto theInt = jsonData;
	if (theInt <= 127 && theInt >= -127) {
		this->appendSmallIntegerExt(static_cast<Uint8>(theInt));
	} else if (theInt <= std::numeric_limits<Int32>::max() && theInt >= std::numeric_limits<Int32>::min()) {
		this->appendIntegerExt(static_cast<Uint32>(theInt));
	} else {
		this->appendUnsignedLongLong(static_cast<Uint64>(theInt));
	}
}

void JsonSerializer::writeArray(JsonSerializer::ArrayType&& jsonData) {
	Bool add_comma{ false };
	this->appendListHeader(static_cast<Uint32>(jsonData.size()));
	for (auto element: jsonData) {
		this->singleValueJsonToETF(std::move(element));
		add_comma = true;
	}
	this->appendNilExt();
}

void JsonSerializer::writeBool(JsonSerializer::BoolType jsonData) {
	auto theBool = jsonData;
	if (theBool) {
		this->appendTrue();
	} else {
		this->appendFalse();
	}
}

void JsonSerializer::writeToBuffer(String&& bytes) {
	this->bufferString.insert(this->bufferString.end(), bytes.begin(), bytes.end());
	this->offSet += bytes.size();
}

void JsonSerializer::appendBinaryExt(String&& bytes, Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Binary_Ext) };
	DiscordCoreAPI::storeBits(bufferNew, sizeNew);
	this->writeToBuffer(std::move(bufferNew));
	this->writeToBuffer(std::move(bytes));
}

void JsonSerializer::appendUnsignedLongLong(Uint64 value) {
	String bufferNew{};
	bufferNew.resize(static_cast<Uint64>(1) + 2 + sizeof(Uint64));
	bufferNew[0] = static_cast<Uint8>(ETFTokenType::Small_Big_Ext);
	DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::milliseconds{ 1500 } };
	Uint8 bytesToEncode = 0;
	while (value > 0) {
		if (theStopWatch.hasTimePassed()) {
			break;
		}
		bufferNew[static_cast<Uint64>(3) + bytesToEncode] = value & 0xF;
		value >>= 8;
		bytesToEncode++;
	}
	bufferNew[1] = bytesToEncode;
	bufferNew[2] = 0;
	this->writeToBuffer(std::move(bufferNew));
}

void JsonSerializer::appendSmallIntegerExt(Uint8 value) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Integer_Ext), static_cast<char>(value) };
	this->writeToBuffer(std::move(bufferNew));
}

void JsonSerializer::appendIntegerExt(Uint32 value) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Integer_Ext) };
	DiscordCoreAPI::storeBits(bufferNew, value);
	this->writeToBuffer(std::move(bufferNew));
}

void JsonSerializer::appendListHeader(Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::List_Ext) };
	DiscordCoreAPI::storeBits(bufferNew, sizeNew);
	this->writeToBuffer(std::move(bufferNew));
}

void JsonSerializer::appendMapHeader(Uint32 sizeNew) {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Map_Ext) };
	DiscordCoreAPI::storeBits(bufferNew, sizeNew);
	this->writeToBuffer(std::move(bufferNew));
}

void JsonSerializer::appendNewFloatExt(Double FloatValue) {
	String bufferNew{};
	bufferNew.push_back(static_cast<unsigned char>(ETFTokenType::New_Float_Ext));

	void* punner{ &FloatValue };
	DiscordCoreAPI::storeBits(bufferNew, *static_cast<Uint64*>(punner));
	this->writeToBuffer(std::move(bufferNew));
}

void JsonSerializer::appendVersion() {
	String bufferNew{};
	bufferNew.push_back(static_cast<char>(formatVersion));
	this->writeToBuffer(std::move(bufferNew));
}

void JsonSerializer::appendNilExt() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Nil_Ext) };
	this->writeToBuffer(std::move(bufferNew));
}

void JsonSerializer::appendFalse() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 5, static_cast<Uint8>('f'), static_cast<Uint8>('a'), static_cast<Uint8>('l'), static_cast<Uint8>('s'),
		static_cast<Uint8>('e') };
	this->writeToBuffer(std::move(bufferNew));
}

void JsonSerializer::appendTrue() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 4, static_cast<Uint8>('t'), static_cast<Uint8>('r'), static_cast<Uint8>('u'), static_cast<Uint8>('e') };
	this->writeToBuffer(std::move(bufferNew));
}

void JsonSerializer::appendNil() {
	String bufferNew{ static_cast<Uint8>(ETFTokenType::Small_Atom_Ext), 3, static_cast<Uint8>('n'), static_cast<Uint8>('i'), static_cast<Uint8>('l') };
	this->writeToBuffer(std::move(bufferNew));
}

JsonSerializer::JsonValue::JsonValue() noexcept {};

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(const ArrayType& theData) noexcept {
	*this->array = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(ArrayType&& theData) noexcept {
	*this->array = std::move(theData);
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(const ObjectType& theData) noexcept {
	*this->object = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(ObjectType&& theData) noexcept {
	*this->object = std::move(theData);
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(const StringType& theData) noexcept {
	*this->string = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(StringType&& theData) noexcept {
	*this->string = std::move(theData);
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(const char* theData) noexcept {
	*this->string = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(Uint64 theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(Uint32 theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(Uint16 theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(Uint8 theData) noexcept {
	this->numberUint = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(Int64 theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(Int32 theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(Int16 theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(Int8 theData) noexcept {
	this->numberInt = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(Double theData) noexcept {
	this->numberDouble = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(Float theData) noexcept {
	this->numberDouble = theData;
	return *this;
}

JsonSerializer::JsonValue& JsonSerializer::JsonValue::operator=(Bool theData) noexcept {
	this->boolean = theData;
	return *this;
}

JsonSerializer::JsonValue::~JsonValue() noexcept {};

JsonSerializer& JsonSerializer::operator=(JsonSerializer&& theKey) noexcept {
	switch (theKey.theType) {
		case ValueType::Object: {
			this->set(std::make_unique<ObjectType>());
			this->theValue = std::move(*theKey.theValue.object);
			break;
		}
		case ValueType::Array: {
			this->set(std::make_unique<ArrayType>());
			this->theValue = std::move(*theKey.theValue.array);
			break;
		}
		case ValueType::String: {
			this->set(std::make_unique<StringType>());
			this->theValue = std::move(*theKey.theValue.string);
			break;
		}
		case ValueType::Bool: {
			this->theValue.boolean = theKey.theValue.boolean;
			break;
		}
		case ValueType::Int64: {
			this->theValue.numberInt = theKey.theValue.numberInt;
			break;
		}
		case ValueType::Uint64: {
			this->theValue.numberUint = theKey.theValue.numberUint;
			break;
		}
		case ValueType::Float: {
			this->theValue.numberDouble = theKey.theValue.numberDouble;
			break;
		}
		case ValueType::Null: {
			break;
		}
	}
	this->theType = theKey.theType;
	return *this;
}

JsonSerializer::JsonSerializer(JsonSerializer&& theKey) noexcept {
	*this = std::move(theKey);
}

JsonSerializer& JsonSerializer::operator=(const JsonSerializer& theKey) noexcept {
	switch (theKey.theType) {
		case ValueType::Object: {
			this->set(std::make_unique<ObjectType>());
			this->theValue = std::move(*theKey.theValue.object);
			this->theType = ValueType::Object;
			break;
		}
		case ValueType::Array: {
			this->set(std::make_unique<ArrayType>());
			this->theValue = std::move(*theKey.theValue.array);
			this->theType = ValueType::Array;
			break;
		}
		case ValueType::String: {
			this->set(std::make_unique<StringType>());
			this->theValue = std::move(*theKey.theValue.string);
			this->theType = ValueType::String;
			break;
		}
		case ValueType::Bool: {
			this->theValue.boolean = theKey.theValue.boolean;
			this->theType = ValueType::Bool;
			break;
		}
		case ValueType::Int64: {
			this->theValue.numberInt = theKey.theValue.numberInt;
			this->theType = ValueType::Int64;
			break;
		}
		case ValueType::Uint64: {
			this->theValue.numberUint = theKey.theValue.numberUint;
			this->theType = ValueType::Uint64;
			break;
		}
		case ValueType::Float: {
			this->theValue.numberDouble = theKey.theValue.numberDouble;
			this->theType = ValueType::Float;
			break;
		}
		case ValueType::Null: {
			this->theType = ValueType::Null;
			break;
		}
		default: {
			break;
		}
	}
	this->theType = theKey.theType;
	return *this;
}

JsonSerializer::JsonSerializer(const JsonSerializer& theKey) noexcept {
	*this = theKey;
}

JsonSerializer& JsonSerializer::operator=(String&& theData) noexcept {
	this->set(std::make_unique<StringType>());
	*this->theValue.string = std::move(theData);
	this->theType = ValueType::String;
	return *this;
}

JsonSerializer::JsonSerializer(String&& theData) noexcept {
	*this = std::move(theData);
}

JsonSerializer& JsonSerializer::operator=(const String& theData) noexcept {
	this->set(std::make_unique<StringType>());
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	return *this;
}

JsonSerializer::JsonSerializer(const String& theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(const char* theData) noexcept {
	this->set(std::make_unique<StringType>());
	*this->theValue.string = theData;
	this->theType = ValueType::String;
	return *this;
}

JsonSerializer::JsonSerializer(const char* theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Uint64 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonSerializer::JsonSerializer(Uint64 theData) noexcept {
	this->appendUnsignedLongLong(theData);
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Uint32 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonSerializer::JsonSerializer(Uint32 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Uint16 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonSerializer::JsonSerializer(Uint16 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Uint8 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Uint64;
	return *this;
}

JsonSerializer::JsonSerializer(Uint8 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Int64 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonSerializer::JsonSerializer(Int64 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Int32 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonSerializer::JsonSerializer(Int32 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Int16 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonSerializer::JsonSerializer(Int16 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Int8 theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Int64;
	return *this;
}

JsonSerializer::JsonSerializer(Int8 theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Double theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Float;
	return *this;
}

JsonSerializer::JsonSerializer(Double theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(Float theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Float;
	return *this;
}

JsonSerializer::JsonSerializer(Float theData) noexcept {
	this->theValue = theData;
}

JsonSerializer& JsonSerializer::operator=(Bool theData) noexcept {
	this->theValue = theData;
	this->theType = ValueType::Bool;
	return *this;
}

JsonSerializer::JsonSerializer(Bool theData) noexcept {
	*this = theData;
}

JsonSerializer& JsonSerializer::operator=(ValueType theType) noexcept {
	this->theType = theType;
	return *this;
}

JsonSerializer::JsonSerializer(ValueType theType) noexcept {
	*this = theType;
}

JsonSerializer& JsonSerializer::operator[](Uint64 index) const {
	return this->theValue.array->operator[](index);
}

JsonSerializer& JsonSerializer::operator[](Uint64 index) {
	if (this->theType == ValueType::Null) {
		this->set(std::make_unique<ArrayType>());
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		if (index >= this->theValue.array->size()) {
			this->theValue.array->resize(index + 1);
		}

		return this->theValue.array->operator[](index);
	}
	throw std::runtime_error{ "Sorry, but that index could not be produced/accessed." };
}

JsonSerializer& JsonSerializer::operator[](const typename ObjectType::key_type& key) const {
	if (this->theType == ValueType::Object) {
		auto result = this->theValue.object->emplace(key, nullptr);
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

JsonSerializer& JsonSerializer::operator[](typename ObjectType::key_type key) {
	if (this->theType == ValueType::Null) {
		this->set(std::make_unique<ObjectType>());
		this->theType = ValueType::Object;
	}

	if (this->theType == ValueType::Object) {
		auto result = this->theValue.object->emplace(std::move(key), JsonSerializer{});
		return result.first->second;
	}
	throw std::runtime_error{ "Sorry, but that item-key could not be produced/accessed." };
}

void JsonSerializer::pushBack(JsonSerializer&& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->set(std::make_unique<ArrayType>());
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(std::move(other));
	}
}

void JsonSerializer::pushBack(JsonSerializer& other) noexcept {
	if (this->theType == ValueType::Null) {
		this->set(std::make_unique<ArrayType>());
		this->theType = ValueType::Array;
	}

	if (this->theType == ValueType::Array) {
		this->theValue.array->emplace_back(std::move(other));
	}
}

void JsonSerializer::convertToString(String& theString) {
	switch (this->theType) {
		case ValueType::Object: {
			if (this->theValue.object->empty()) {
				theString += "{}";
			}

			theString += '{';

			Uint64 theIndex{};
			for (auto [key, value]: *this->theValue.object) {
				theString += '\"';
				theString += key;
				theString += "\":";
				value.theCurrentStringMemory = { theString.data() + theString.size() };
				value.convertToString(theString);
				if (theIndex < this->theValue.object->size() - 1) {
					theString += ',';
				}
				theIndex++;
			}
			theString += '}';
			break;
		}
		case ValueType::Array: {
			if (this->theValue.array->empty()) {
				theString += "[]";
				break;
			}

			theString += '[';

			Uint64 theIndex{};
			for (auto value: *this->theValue.array) {
				value.theCurrentStringMemory = { theString.data() + theString.size() };
				value.convertToString(theString);
				if (theIndex < this->theValue.object->size() - 1) {
					theString += ',';
				}
				theIndex++;
			}

			theString += ']';
			break;
		}

		case ValueType::String: {
			theString += '\"';
			theString += std::move(*this->theValue.string);
			theString += '\"';
			break;
		}
		case ValueType::Bool: {
			std::stringstream theStream{};
			theStream << std::boolalpha << this->theValue.boolean;
			theString += theStream.str();
			break;
		}
		case ValueType::Float: {
			theString += std::to_string(this->theValue.numberDouble);
			break;
		}
		case ValueType::Uint64: {
			theString += std::to_string(this->theValue.numberUint);
			break;
		}
		case ValueType::Int64: {
			theString += std::to_string(this->theValue.numberInt);
			break;
		}
		case ValueType::Null: {
			theString += "null";
			break;
		}
		case ValueType::Null_Ext: {
			theString += "[]";
			break;
		}
	}
	return;
}

JsonSerializer::operator String() noexcept {
	this->convertToString(this->theString);
	return this->theString;
}

void JsonSerializer::set(std::unique_ptr<String> p) {
	destroy();
	new (&this->theValue.string) std::unique_ptr<String>{ std::move(p) };
	this->theType = ValueType::String;
}

void JsonSerializer::set(std::unique_ptr<ArrayType> p) {
	destroy();
	new (&this->theValue.string) std::unique_ptr<ArrayType>{ std::move(p) };
	this->theType = ValueType::Array;
}

void JsonSerializer::set(std::unique_ptr<ObjectType> p) {
	destroy();
	new (&this->theValue.string) std::unique_ptr<ObjectType>{ std::move(p) };
	this->theType = ValueType::Object;
}

void JsonSerializer::destroy() noexcept {
	switch (this->theType) {
		case ValueType::Array: {
			this->theValue.array.reset(nullptr);
			break;
		}
		case ValueType::Object: {
			this->theValue.object.reset(nullptr);
			break;
		}
		case ValueType::String: {
			this->theValue.string.reset(nullptr);
			break;
		}
	}
}

JsonSerializer::~JsonSerializer() noexcept {
	this->destroy();
}

struct UpdatePresenceData {
	String status{};///< Current status.
	Int64 since{ 0 };///< When was the activity started?
	Bool afk{ false };///< Are we afk.

	operator JsonSerializer();
};

UpdatePresenceData ::operator JsonSerializer() {
	JsonSerializer theData{};
	theData["TEST"] = this->afk;
	theData["since"] = this->since;
	return theData;
}

struct WebSocketIdentifyData {
	UpdatePresenceData presence{};
	int32_t largeThreshold{ 250 };
	int32_t numberOfShards{};
	int32_t currentShard{};
	std::string botToken{};
	int64_t intents{};

	operator JsonSerializer();
};

WebSocketIdentifyData::operator JsonSerializer() {
	JsonSerializer theSerializer{};
	theSerializer["d"]["intents"] = static_cast<uint32_t>(this->intents);
	theSerializer["d"]["large_threshold"] = static_cast<uint32_t>(250);
	
	JsonSerializer theSerializer02{};
	theSerializer02 = this->presence;
	theSerializer["d"]["presence"]["activities"].pushBack(std::move(theSerializer02));
	
	theSerializer["d"]["afk"] = this->presence.afk;
	if (this->presence.since != 0) {
			theSerializer["since"] = this->presence.since;
	}
	
	theSerializer["d"]["status"] = this->presence.status;
	theSerializer["d"]["properties"]["browser"] = "DiscordCoreAPI";
	theSerializer["d"]["properties"]["device"] = "DiscordCoreAPI";
#ifdef _WIN32
	theSerializer["d"]["properties"]["os"] = "Windows";
#else
	theSerializer["d"]["properties"]["os"] = "Linux";
#endif
	theSerializer["d"]["shard"].pushBack(static_cast<uint32_t>(this->currentShard));
	theSerializer["d"]["shard"].pushBack(static_cast<uint32_t>(this->numberOfShards));
	theSerializer["d"]["token"] = this->botToken;
	
	theSerializer["op"] = static_cast<uint32_t>(2);
	std::cout << "ID 'd''s distance from the start of the string: " << theSerializer["d"]["intents"].theCurrentStringMemory.data() - theSerializer.theCurrentStringMemory.data()
			  << std::endl;
	
	return theSerializer;

}

int32_t main() noexcept {
	try {
		DiscordCoreAPI::StopWatch theStopWatch{ std::chrono::milliseconds{ 1 } };
		WebSocketIdentifyData theDataBewTwo{};
		DiscordCoreAPI::ActivityData theData{};
		std::vector<std::string> theResults01{};
		std::vector<DiscordCoreAPI::ChannelType> theVector{};
		
		theVector.push_back(DiscordCoreAPI::ChannelType::Dm);
		theVector.push_back(DiscordCoreAPI::ChannelType::Guild_Category);
		theData.name = "TESTING";
		theDataBewTwo.numberOfShards = 0;
		theDataBewTwo.currentShard = 23;
		std::vector<std::string> theResults02{};
		theStopWatch.resetTimer();
		for (int32_t x = 0; x < 128 * 128; ++x) {
			theResults01.push_back(theDataBewTwo.operator JsonSerializer());
		}

		WebSocketIdentifyData theDataBew{};
		theDataBew.numberOfShards = 0;
		theDataBew.currentShard = 23;
		theStopWatch.resetTimer();
		std::cout << static_cast<String>(theDataBew.operator JsonSerializer()) << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds{ 5000 });


	} catch (...) {
		DiscordCoreAPI::reportException("main()");
	};

	return 0;
}

