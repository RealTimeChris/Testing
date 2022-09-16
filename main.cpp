#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include "ErlPacker.hpp"
#include <stdint.h>
#include <set>

enum class ObjectType : int8_t { Null = 0, Object = 1, Array = 2, String = 3, Boolean = 4, Number_Integer = 5, Number_Unsigned = 6, Number_Float = 7, Number_Double = 8 };
enum class JsonParseEvent { Null_Value = 0, Object_Start = 1, Object_End = 2, Array_Start = 3, Array_End = 4, String = 5, Boolean = 6, Number_Integer = 7, Number_Unsigned = 8, Number_Float = 9, Number_Double = 10 };

struct JsonValue {
	JsonParseEvent theEvent{};
	std::string theValue{};
	std::string theKey{};
};
class JsonSerializer {
public:
	JsonSerializer()noexcept = default;
	std::string getString();

	template<std::same_as<uint64_t> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
		std::string theString = std::to_string(theData);
		this->theValues.push_back({ .theEvent = JsonParseEvent::Number_Unsigned,.theValue = theString,.theKey = keyName });
	}

	template<std::same_as<int64_t> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
	}

	template<std::same_as<std::string> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
	}

	template<std::same_as<bool> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
	}

	template<std::same_as<float> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
	}

	template<std::same_as<double> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
	}

	template<std::same_as<JsonObjectBase> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
	}

	template<std::same_as<const char*> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
	}

	template<std::same_as<Object> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
	}

	template<std::same_as<Array> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
	}

	template<std::same_as<ObjectType> JsonObjectType>
	void addEvent(const char* keyName, JsonObjectType theData) {
	}

protected:

	std::vector<JsonValue> theValues{};
};
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

	JsonObjectBase(const char* keyName, Object& value) noexcept;

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
	JsonObject()noexcept = default;

	JsonObjectBase theObject{};
	

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
	Object(const Object& theData) noexcept;
	Object(const char* keyName, JsonObject) noexcept;
	Object(const char* keyName) noexcept;
	Object()noexcept = default;
	void append(JsonObject& theObject);
	void append(JsonObject&& theObject);
	void append(Array& theObject);
	void append(Array&& theObject);
	std::string toString(bool doWeAddComma = false, bool doWeAddCurlyBrackets = true);
	std::unordered_map<std::string, Object>& getMap();
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


Object::Object(const Object& theData) noexcept {
	this->theObject.thePtr = new std::unordered_map<std::string, Object>{};
	std::cout << "WERE CONSTRUCTING!" << std::endl;
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

Object::Object(const char* keyName, JsonObject theData) noexcept:JsonObject(theData) {
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

std::string Object::toString(bool doWeAddComma, bool doWeAddCurlyBrackets) {
	std::string theString{}; 
	if (doWeAddCurlyBrackets) {
		theString += "\"" + this->theObject.theKey + "\":";
		theString += "{";
	}
	int32_t currentIndex{};
	for (auto& [key, value] : this->getMap()) {
		if (value.theObject.theType == ObjectType::Object) {
			doWeAddCurlyBrackets = true;
			theString += "{";
		}
		else if (value.theObject.theType == ObjectType::Boolean || value.theObject.theType == ObjectType::Number_Double || value.theObject.theType == ObjectType::Number_Float
			|| value.theObject.theType == ObjectType::Number_Integer || value.theObject.theType == ObjectType::Number_Unsigned || value.theObject.theType == ObjectType::String
			|| value.theObject.theType == ObjectType::Null) {
			doWeAddCurlyBrackets = false;
		}
		currentIndex++;
		theString += "\"" + key + "\":";
		if (currentIndex > 0 && currentIndex < this->getMap().size()) {
			theString += ",";
		}
		theString += value.toString(doWeAddComma, doWeAddCurlyBrackets);
		
		std::cout << "THE STRING: 0303" << std::string{ value.toString() } << std::endl;
		if (value.theObject.theType == ObjectType::Object) {
			theString += "}";
		}
		
	}
	if (doWeAddCurlyBrackets) {
		theString += "}";
	}
	
	
	std::cout << "THE STRING: 0202" << theString << std::endl;
	return theString;
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

JsonObjectBase::JsonObjectBase(const char* keyName, Object& value) noexcept {
	this->thePtr = std::make_unique<Object>().release();
	this->theType = ObjectType::Object;
	*(Object*)this->thePtr = value;
	this->theKey = keyName;
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


JsonObjectBase::~JsonObjectBase() {
	if (this->thePtr) {
		delete this->thePtr;
		this->thePtr = nullptr;
	}
};


    struct WebSocketIdentifyData {
        DiscordCoreInternal::UpdatePresenceData presence{};
        std::int32_t largeThreshold{};
        std::int32_t numberOfShards{};
        std::int32_t currentShard{};
        std::string botToken{};
        int64_t intents{};

        operator JsonSerializer();
    };

    WebSocketIdentifyData::operator JsonSerializer() {
		JsonSerializer theSerializer{};
		theSerializer.addEvent("token", this->botToken);
		/*		Object data{ "d" };
		Object theProperties{ "properties" };
		theProperties["browser"] = "DiscordCoreAPI";
		theProperties["device"] = "DiscordCoreAPI";
		std::cout << "THE OBJET RELADER: " << std::string{ theProperties.toString(false, true) } << std::endl;
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
		data.append({ "properties", theProperties });*/
		return theSerializer;
    }

    int32_t main() {
        try {
			WebSocketIdentifyData theData{};
			theData.botToken = "TESTING VALUE 23123123123";
			std::cout << "THE STRING FINAL: " << theData.operator JsonSerializer().getString() << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        }
        catch (...) { DiscordCoreAPI::reportException("main()"); };

        return 0;
    }
