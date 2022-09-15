#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include "ErlPacker.hpp"
#include <stdint.h>
#include <set>

enum class ObjectType : int8_t { JsonObjectBase = 0, Array = 1, String = 2, Boolean = 3, Number_Integer = 4, Number_Unsigned = 5, Number_Float = 6, Number_Double = 7, Null = 8 };

struct Array;
struct Object;

struct JsonObjectBase {
	void* thePtr{};

	JsonObjectBase(const char* keyName, bool value) noexcept;

	JsonObjectBase(const char* keyName, int64_t value) noexcept;

	JsonObjectBase(const char* keyName, uint64_t value) noexcept;

	JsonObjectBase(const char* keyName, double value) noexcept;

	JsonObjectBase(const char* keyName, float value) noexcept;

	JsonObjectBase(const char* keyName, const char* value) noexcept;

	JsonObjectBase(const char* keyName, std::string& value) noexcept;

	JsonObjectBase(const char* keyName, std::string&& value) noexcept;

	JsonObjectBase& operator=(const JsonObjectBase& other) noexcept;

	JsonObjectBase(const JsonObjectBase& other) noexcept;

	JsonObjectBase() noexcept;

	ObjectType type();

	std::string getString();

	uint64_t getUint64();

	int64_t getInt64();

	bool getBool();

	float getFloat();

	double getDouble();

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

	ObjectType theType{};

	std::string theKey{};

};

class JsonObject {
public:

	JsonObject() noexcept = default;

	JsonObjectBase theObject;
	template<std::same_as<uint64_t> JsonObjectType>
	JsonObject(const char*keyName,JsonObjectType theData) {
		this->theObject = JsonObjectBase{ keyName, theData };
	}

	template<std::same_as<int64_t> JsonObjectType>
	JsonObject(const char* keyName, JsonObjectType theData) {
		this->theObject = JsonObjectBase{ keyName, theData };
	}

	template<std::same_as<std::string> JsonObjectType>
	JsonObject(const char* keyName, JsonObjectType theData) {
		this->theObject = JsonObjectBase{ keyName, theData };
	}

	template<std::same_as<bool> JsonObjectType>
	JsonObject(const char* keyName, JsonObjectType theData) {
		this->theObject = JsonObjectBase{ keyName, theData };
	}

	template<std::same_as<float> JsonObjectType>
	JsonObject(const char* keyName, JsonObjectType theData) {
		this->theObject = JsonObjectBase{ keyName, theData };
	}

	template<std::same_as<double> JsonObjectType>
	JsonObject(const char* keyName, JsonObjectType theData) {
		this->theObject = JsonObjectBase{ keyName, theData };
	}

	template<std::same_as<JsonObjectBase> JsonObjectType>
	JsonObject(const char* keyName, JsonObjectType theData) {
		this->theObject = JsonObjectBase{ keyName, theData };
	}

	template<std::same_as<const char*> JsonObjectType>
	JsonObject(const char* keyName, JsonObjectType theData) {
		this->theObject = JsonObjectBase{ keyName, theData };
	}

	template<std::same_as<Object> JsonObjectType>
	JsonObject(const char* keyName, JsonObjectType theData) {
		this->theObject = JsonObjectBase{ keyName, theData };
	}

	template<std::same_as<Array> JsonObjectType>
	JsonObject(const char* keyName, JsonObjectType theData) {
		this->theObject = JsonObjectBase{ keyName, theData };
	}
	
	template<std::same_as<ObjectType> JsonObjectType>
	JsonObject(const char* keyName, JsonObjectType theData) {
		this->theObject = JsonObjectBase{ keyName, theData };
	}
	

};
JsonObjectBase::JsonObjectBase() noexcept {
	this->theType = ObjectType::Null;
	this->thePtr = new uint64_t{};
}

std::string JsonObjectBase::JsonObjectBase::getString(){
	return *static_cast<std::string*>(this->thePtr);
}

uint64_t JsonObjectBase::JsonObjectBase::getUint64(){
	return *static_cast<uint64_t*>(this->thePtr);
}

int64_t JsonObjectBase::JsonObjectBase::getInt64(){
	return *static_cast<int64_t*>(this->thePtr);
}

ObjectType JsonObjectBase::JsonObjectBase::type() {
	return this->theType;
}

bool JsonObjectBase::JsonObjectBase::getBool(){
	return *static_cast<bool*>(this->thePtr);
}

float JsonObjectBase::JsonObjectBase::getFloat(){
	return *static_cast<float*>(this->thePtr);
}

double JsonObjectBase::JsonObjectBase::getDouble(){
	return *static_cast<double*>(this->thePtr);
}


struct Array;

struct Object : public JsonObject {
	Object& operator=(JsonObject& theData) noexcept;
	Object& operator=(JsonObject&& theData) noexcept;
	Object& operator=(const Object& theData) noexcept;
	Object& operator=(Object&& theData) noexcept;
	Object(const Object& theData) noexcept;
	Object(const char* keyName, JsonObject) noexcept;
	Object(const char* keyName) noexcept;
	Object()noexcept;
	void append(JsonObject& theObject);
	void append(JsonObject&& theObject);
	void append(Array& theObject);
	void append(Array&& theObject);
	std::unordered_map<std::string, Object>& getMap();
	operator std::string();
	Object& operator[](const char* theKey);
	~Object() noexcept;
};

struct Array : public JsonObject {
	Array()noexcept = default;
	Array(const char* keyName) noexcept;
	void append(Object& theObject);
	void append(Object&& theObject);
	std::vector<Object>& getVector();
	operator std::string();
	~Array() noexcept;
};



Object::Object() noexcept {
	this->theObject.thePtr = new std::unordered_map<std::string, Object>{};
}

Object& Object::operator[](const char* keyName) {
	if (!this->getMap().contains(keyName)) {
		this->getMap()[keyName] = Object{ keyName };
	}
	std::cout << "THE OBJECT: 01 " << std::string{ this->getMap()[keyName] } << std::endl;
	return this->getMap()[keyName];
}

Object::Object(const Object& theData) noexcept {
	this->theObject.thePtr= new std::unordered_map<std::string, Object>{};
	*this = theData;
}

Object& Object::operator=(JsonObject& theData) noexcept {
	this->theObject = theData.theObject;
	return *this;
}
Object& Object::operator=(JsonObject&& theData) noexcept{
	this->theObject = theData.theObject;
	return *this;
}

Object& Object::operator=(const Object& theData) noexcept {
	this->theObject = theData.theObject;
	return *this;

}

Object& Object::operator=(Object&& theData) noexcept {
	this->theObject = std::move(theData.theObject);
	return *this;

}

Object::Object(const char* keyName, JsonObject theData) noexcept {
	this->theObject.thePtr = new std::unordered_map<std::string, Object>{};
	*this = theData;
}

Object::Object(const char* keyName) noexcept {
	this->theObject.thePtr= new std::unordered_map<std::string, Object>{};
	this->theObject.theKey = keyName;
};

void Object::append(JsonObject& theObject) {
	this->getMap()[theObject.theObject.theKey] = theObject;
}

void Object::append(JsonObject&& theObject) {
	this->getMap()[theObject.theObject.theKey] = std::move(theObject);
};

void Object::append(Array& theObject) {
	this->getMap()[theObject.theObject.theKey] = theObject;
}

void Object::append(Array&& theObject) {
	this->getMap()[theObject.theObject.theKey] = std::move(theObject);
};

std::unordered_map<std::string, Object>& Object::getMap(){
	return *static_cast<std::unordered_map<std::string, Object>*>(this->theObject.thePtr);
}

Object::operator std::string(){
	std::string theString{};
	theString += "\"" + this->theObject.theKey + "\":{";
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
	if (this->theObject.thePtr) {
		delete this->theObject.thePtr;
		this->theObject.thePtr= nullptr;
	}
}

Array::Array(const char* keyName) noexcept {
	this->theObject.thePtr= new std::vector<JsonObject>{};
	this->theObject.theKey = keyName;
}

void Array::append(Object& theObject){
	this->getVector().push_back(theObject);
}

void Array::append(Object&& theObject){
	this->getVector().push_back(std::move(theObject));
}

std::vector<Object>& Array::getVector() {
	return *static_cast<std::vector<Object>*>(this->theObject.thePtr);
}

Array::operator std::string() {
	return std::string{};
}

Array::~Array() noexcept {
	if (this->theObject.thePtr) {
		delete this->theObject.thePtr;
		this->theObject.thePtr= nullptr;
	}
}

JsonObjectBase& JsonObjectBase::operator=(const JsonObjectBase& other) noexcept {
	if (!other.thePtr) {
		const_cast<JsonObjectBase*>(&other)->thePtr = std::make_unique<JsonObject>().release();
	}
	if (other.theType == ObjectType::Array) {
		this->thePtr= std::make_unique<Array>().release();
		*(Array*)this->thePtr= *(Array*)other.thePtr;
	}
	else if (other.theType == ObjectType::Boolean) {
		this->thePtr= std::make_unique<bool>().release();
		*(bool*)this->thePtr= *(bool*)other.thePtr;
	}
	else if (other.theType == ObjectType::Number_Double) {
		this->thePtr= std::make_unique<double>().release();
		*(double*)this->thePtr= *(double*)other.thePtr;
	}
	else if (other.theType == ObjectType::Number_Float) {
		this->thePtr= std::make_unique<float>().release();
		*(float*)this->thePtr= *(float*)other.thePtr;
	}
	else if (other.theType == ObjectType::Number_Integer) {
		this->thePtr= std::make_unique<int64_t>().release();
		*(int64_t*)this->thePtr= *(int64_t*)other.thePtr;
	}
	else if (other.theType == ObjectType::Number_Unsigned) {
		this->thePtr= std::make_unique<uint64_t>().release();
		*(uint64_t*)this->thePtr= *(uint64_t*)other.thePtr;
	}
	else if (other.theType == ObjectType::String) {
		this->thePtr= std::make_unique<std::string>().release();
		*(std::string*)this->thePtr= *(std::string*)other.thePtr;
	}
	this->theType = other.theType;
	this->theKey = other.theKey;
	return *this;
}

JsonObjectBase::JsonObjectBase(const JsonObjectBase& other) noexcept {
	*this = other;
}


JsonObjectBase::JsonObjectBase(const char* keyName, uint64_t other) noexcept {
	this->thePtr= std::make_unique<uint64_t>().release();
	this->theType = ObjectType::Number_Unsigned;
	*(uint64_t*)this->thePtr= other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, int64_t other) noexcept {
	this->thePtr= std::make_unique<int64_t>().release();
	this->theType = ObjectType::Number_Integer;
	*(int64_t*)this->thePtr= other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, const char* value) noexcept {
	this->thePtr= std::make_unique<std::string>().release();
	this->theType = ObjectType::String;
	*(std::string*)this->thePtr= value;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, bool other) noexcept {
	this->thePtr= std::make_unique<bool>().release();
	this->theType = ObjectType::Boolean;
	*(bool*)this->thePtr= other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, float other) noexcept {
	this->thePtr= std::make_unique<float>().release();
	this->theType = ObjectType::Number_Float;
	*(float*)this->thePtr= other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, double other) noexcept {
	this->thePtr= std::make_unique<double>().release();
	this->theType = ObjectType::Number_Double;
	*(double*)this->thePtr= other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, std::string&& other) noexcept {
	this->thePtr= std::make_unique<std::string>().release();
	this->theType = ObjectType::String;
	*(std::string*)this->thePtr= other;
	this->theKey = keyName;
}

JsonObjectBase::JsonObjectBase(const char* keyName, std::string& other) noexcept {
	this->thePtr= std::make_unique<std::string>().release();
	this->theType = ObjectType::String;
	*(std::string*)this->thePtr= other;
	this->theKey = keyName;
}


JsonObjectBase::~JsonObjectBase() {};


    struct WebSocketIdentifyData {
        DiscordCoreInternal::UpdatePresenceData presence{};
        std::int32_t largeThreshold{};
        std::int32_t numberOfShards{};
        std::int32_t currentShard{};
        std::string botToken{};
        int64_t intents{};

        operator JsonObject();
    };

    WebSocketIdentifyData::operator JsonObject() {
		Object data{ "d" };
		Object theProperties{ "properties" };
		theProperties["browser"] = "DiscordCoreAPI";
		theProperties["device"] = "DiscordCoreAPI";
		std::cout << "THE OBJET RELADER: " << std::string{ theProperties} << std::endl;
		Array theShard{ "shard" };
		theShard.append(Object{ "" });
		theShard.append(Object{ "",JsonObject{"",static_cast<uint64_t>(this->numberOfShards)} });
		data.append(theShard);
		data.append(Object{ "large_threshold",JsonObject{ "",static_cast<uint64_t>(250)}});
		data.append(Object{ "intents", JsonObject{ "",static_cast<uint64_t>(this->intents) } });
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
			std::string theString = theData.operator JsonObject().theObject.operator std::string();
			std::cout << "THE STRING FINAL: " << theString << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        }
        catch (...) { DiscordCoreAPI::reportException("main()"); };

        return 0;
    }
