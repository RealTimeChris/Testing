#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include "ErlPacker.hpp"
#include <stdint.h>
#include <set>

enum class ObjectType : int8_t { JsonObjectBase = 0, Array = 1, String = 2, Boolean = 3, Number_Integer = 4, Number_Unsigned = 5, Number_Float = 6, Number_Double = 7, Null = 8 };

struct Array;

struct JsonObjectBase {
	void* thePtr{ nullptr };

	JsonObjectBase(const char* keyName, bool value) noexcept;

	JsonObjectBase(const char* keyName, int64_t value) noexcept;

	JsonObjectBase(const char* keyName, uint64_t value) noexcept;

	JsonObjectBase(const char* keyName, double value) noexcept;

	JsonObjectBase(const char* keyName, float value) noexcept;

	JsonObjectBase(const char* keyName, const char* value) noexcept;

	JsonObjectBase(const char* keyName, std::string& value) noexcept;

	JsonObjectBase(const char* keyName, std::string&& value) noexcept;

	JsonObjectBase& operator=(JsonObjectBase& other) noexcept;

	JsonObjectBase(JsonObjectBase& other) noexcept;

	JsonObjectBase& operator=(const JsonObjectBase& other) noexcept = default;
	
	JsonObjectBase(const JsonObjectBase& other) noexcept = default;

	ObjectType type();

	std::string getString();

	uint64_t getUint64();

	int64_t getInt64();

	bool getBool();

	float getFloat();

	double getDouble();

	JsonObjectBase()noexcept {};

	operator std::string() {
		std::string theString{};
		switch (this->theType) {
		case ObjectType::String: {
			theString += "\"";
			theString += this->getString();
			theString += "\"";
			break;
		}

		case ObjectType::Boolean: {
			std::stringstream theStream{};
			theStream << std::boolalpha << this->getBool();
			theString += theStream.str();
			break;
		}

		case ObjectType::Number_Integer: {
			theString += std::to_string(this->getInt64());
			break;
		}

		case ObjectType::Number_Unsigned: {
			theString += std::to_string(this->getUint64());
			break;
		}

		case ObjectType::Number_Float: {
			theString += std::to_string(this->getFloat());
			break;
		}

		case ObjectType::Number_Double: {
			theString += std::to_string(this->getDouble());
			break;
		}

		case ObjectType::Null: {
			theString += "null";
			break;
		}
		}
		return theString;
	}

	~JsonObjectBase();

	std::string theKey{};

	ObjectType theType{};

};

std::string JsonObjectBase::getString(){
	return *static_cast<std::string*>(this->thePtr);
}

uint64_t JsonObjectBase::getUint64(){
	return *static_cast<uint64_t*>(this->thePtr);
}

int64_t JsonObjectBase::getInt64(){
	return *static_cast<int64_t*>(this->thePtr);
}

ObjectType JsonObjectBase::type() {
	return this->theType;
}

bool JsonObjectBase::getBool(){
	return *static_cast<bool*>(this->thePtr);
}

float JsonObjectBase::getFloat(){
	return *static_cast<float*>(this->thePtr);
}

double JsonObjectBase::getDouble(){
	return *static_cast<double*>(this->thePtr);
}


struct Array;

struct Object : public JsonObjectBase {
	Object& operator=(JsonObjectBase) noexcept;
	Object(JsonObjectBase) noexcept;
	Object(const char* keyName) noexcept;
	void append(JsonObjectBase& theObject);
	void append(JsonObjectBase&& theObject);
	void append(Array& theObject);
	void append(Array&& theObject);
	std::unordered_map<std::string, JsonObjectBase>& getMap();
	operator std::string();
	Object& operator[](const char* theKey);
	~Object() noexcept;
};

struct Array : public JsonObjectBase {
	Array()noexcept = default;
	Array(const char* keyName) noexcept;
	void append(Object& theObject);
	void append(Object&& theObject);
	std::vector<Object>& getVector();
	operator std::string();
	~Array() noexcept;
};

Object& Object::operator=(JsonObjectBase theData) noexcept {
	this->theKey = theData.theKey;
	switch (theData.theType) {
	case ObjectType::String: {
		this->thePtr = std::make_unique<std::string>().release();
		break;
	}

	case ObjectType::Boolean: {
		this->thePtr = std::make_unique<bool>().release();
		break;
	}

	case ObjectType::Number_Integer: {
		this->thePtr = std::make_unique<int64_t>().release();
		break;
	}

	case ObjectType::Number_Unsigned: {
		this->thePtr = std::make_unique<uint64_t>().release();
		break;
	}

	case ObjectType::Number_Float: {
		this->thePtr = std::make_unique<float>().release();
		break;
	}

	case ObjectType::Number_Double: {
		this->thePtr = std::make_unique<double>().release();
		break;
	}

	case ObjectType::Null: {
		this->thePtr = std::make_unique<JsonObjectBase>().release();
		break;
	}
	}
	*(int*)this->thePtr = *(int*)theData.thePtr;
	this->theType = theData.theType;
	return *this;
}

Object& Object::operator[](const char* keyName) {
	this->getMap()[keyName] = Object{ keyName };
	return (Object&)this->getMap()[keyName];
}

Object::Object(JsonObjectBase theData) noexcept{
	*this = theData;
}

Object::Object(const char* keyName) noexcept {
	this->thePtr = new std::unordered_map<std::string, JsonObjectBase>{};
	this->theKey = keyName;
};

void Object::append(JsonObjectBase& theObject) {
	this->getMap()[theObject.theKey] = theObject;
}

void Object::append(JsonObjectBase&& theObject) {
	this->getMap()[theObject.theKey] = std::move(theObject);
};

void Object::append(Array& theObject) {
	this->getMap()[theObject.theKey] = theObject;
}

void Object::append(Array&& theObject) {
	this->getMap()[theObject.theKey] = std::move(theObject);
};

std::unordered_map<std::string, JsonObjectBase>& Object::getMap(){
	return *static_cast<std::unordered_map<std::string, JsonObjectBase>*>(this->thePtr);
}

Object::operator std::string(){
	std::string theString{};
	theString += "\"" + this->theKey + "\":{";
	int32_t currentIndex{};
	for (auto& [key, value] : this->getMap()) {
		currentIndex++;
		theString += "\"" + key + "\":";
		theString += value;
		if (currentIndex > 0 && currentIndex < this->getMap().size() ) {
			theString += ",";
		}
	}
	theString += "}";
	std::cout << "THE STRING: " << theString << std::endl;
	return std::string{};
}

Object::~Object() noexcept{
	if (this->thePtr) {
		delete this->thePtr;
		this->thePtr = nullptr;
	}
}

Array::Array(const char* keyName) noexcept {
	this->thePtr = new std::vector<JsonObjectBase>{};
	this->theKey = keyName;
}

void Array::append(Object& theObject){
	this->getVector().push_back(theObject);
}

void Array::append(Object&& theObject){
	this->getVector().push_back(std::move(theObject));
}

std::vector<Object>& Array::getVector() {
	return *static_cast<std::vector<Object>*>(this->thePtr);
}

Array::operator std::string() {
	return std::string{};
}

Array::~Array() noexcept {
	if (this->thePtr) {
		delete this->thePtr;
		this->thePtr = nullptr;
	}
}

JsonObjectBase& JsonObjectBase::operator=(JsonObjectBase& other) noexcept {
	if (!other.thePtr) {
		other.thePtr = std::make_unique<JsonObjectBase>().release();
	}
	if (other.theType==ObjectType::Array) {
		this->thePtr = std::make_unique<Array>().release();
		*(Array*)this->thePtr = *(Array*)other.thePtr;
	}
	else if (other.theType == ObjectType::Boolean) {
		this->thePtr = std::make_unique<bool>().release();
		*(bool*)this->thePtr = *(bool*)other.thePtr;
	}
	else if (other.theType == ObjectType::Number_Double) {
		this->thePtr = std::make_unique<double>().release();
		*(double*)this->thePtr = *(double*)other.thePtr;
	}
	else if (other.theType == ObjectType::Number_Float) {
		this->thePtr = std::make_unique<float>().release();
		*(float*)this->thePtr = *(float*)other.thePtr;
	}
	else if (other.theType == ObjectType::Number_Integer) {
		this->thePtr = std::make_unique<int64_t>().release();
		*(int64_t*)this->thePtr = *(int64_t*)other.thePtr;
	}
	else if (other.theType == ObjectType::Number_Unsigned) {
		this->thePtr = std::make_unique<uint64_t>().release();
		*(uint64_t*)this->thePtr = *(uint64_t*)other.thePtr;
	}
	else if (other.theType == ObjectType::String) {
		this->thePtr = std::make_unique<std::string>().release();
		*(std::string*)this->thePtr = *(std::string*)other.thePtr;
	}
	this->theType = other.theType;
	this->theKey = other.theKey;
	return *this;
}

JsonObjectBase::JsonObjectBase(JsonObjectBase& other) noexcept {
	*this = other;
}

JsonObjectBase::JsonObjectBase(const char* keyName, uint64_t other) noexcept {
	this->thePtr = std::make_unique<uint64_t>().release();
	this->theType = ObjectType::Number_Unsigned;
	*(uint64_t*)this->thePtr = other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, int64_t other) noexcept {
	this->thePtr = std::make_unique<int64_t>().release();
	this->theType = ObjectType::Number_Integer;
	*(int64_t*)this->thePtr = other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, const char* value) noexcept {
	this->thePtr = std::make_unique<std::string>().release();
	this->theType = ObjectType::String;
	*(std::string*)this->thePtr = value;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, bool other) noexcept {
	this->thePtr = std::make_unique<bool>().release();
	this->theType = ObjectType::Boolean;
	*(bool*)this->thePtr = other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, float other) noexcept {
	this->thePtr = std::make_unique<float>().release();
	this->theType = ObjectType::Number_Float;
	*(float*)this->thePtr = other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, double other) noexcept {
	this->thePtr = std::make_unique<double>().release();
	this->theType = ObjectType::Number_Double;
	*(double*)this->thePtr = other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, std::string&& other) noexcept {
	this->thePtr = std::make_unique<std::string>().release();
	this->theType = ObjectType::String;
	*(std::string*)this->thePtr = other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, std::string& other) noexcept {
	this->thePtr = std::make_unique<std::string>().release();
	this->theType = ObjectType::String;
	*(std::string*)this->thePtr = other;
	this->theKey = keyName;
}


JsonObjectBase::~JsonObjectBase() {}

    struct JsonObject {

        
    protected:
        int32_t currentDepth{ 0 };
    };


    struct WebSocketIdentifyData {
        DiscordCoreInternal::UpdatePresenceData presence{};
        std::int32_t largeThreshold{};
        std::int32_t numberOfShards{};
        std::int32_t currentShard{};
        std::string botToken{};
        int64_t intents{};

        operator JsonObjectBase();
    };

    WebSocketIdentifyData::operator JsonObjectBase() {
		Object data{ "d" };
		Object theProperties{ "properties" };
		theProperties.append({ "browser", "DiscordCoreAPI" });
		theProperties.append({ "device", "DiscordCoreAPI" });
		Array theShard{ "shard" };
		theShard.append(JsonObjectBase{ "",static_cast<uint64_t>(this->currentShard) });
		theShard.append(JsonObjectBase{ "",static_cast<uint64_t>(this->numberOfShards) });
		data.append(theShard);
		data.append({ "large_threshold", static_cast<uint64_t>(250) });
		data.append({ "intents", static_cast<uint64_t>(this->intents) });
		data.append({ "compress", false });
		data.append({ "token", this->botToken });
		Array theActivities{ "activities" };
        for (auto& value : this->presence.activities) {
			Object dataNew{ "" };
            if (static_cast<std::string>(value.url) != "") {
				dataNew.append({ "url", std::string{ value.url } });
            }
			dataNew.append({ "name", std::string{ value.name } });
			dataNew.append({ "type", static_cast<uint64_t>(value.type) });
            theActivities.append(dataNew);
        }
		Object presence{ "presence" };
		presence.append({ "status", this->presence.status });
		presence.append({ "since", static_cast<uint64_t>(0) });
		presence.append({ "afk", this->presence.afk });
		data.append(presence);
		data.append({ "op", static_cast<uint64_t>(2) });
#ifdef _WIN32
		theProperties.append({ "os", "Windows" });
#else
		theProperties.append({ "os", "Linux" });
#endif
		data.append({ "properties", theProperties });
        return data;
    }

    int32_t main() {
        try {
			WebSocketIdentifyData theData{};
			theData.botToken = "TESTING VALUE 23123123123";
			std::string theString = static_cast<std::string>(theData.operator JsonObjectBase().operator std::string());

            std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        }
        catch (...) { DiscordCoreAPI::reportException("main()"); };

        return 0;
    }
