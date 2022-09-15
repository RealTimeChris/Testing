#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include "ErlPacker.hpp"
#include <stdint.h>
#include <set>

enum class ObjectType : int8_t { JsonObjectBase = 0, Array = 1, String = 2, Boolean = 3, Number_Integer = 4, Number_Unsigned = 5, Number_Float = 6, Number_Double = 7, Null = 8 };

struct Array;

struct JsonObjectBase {
	void* thePtr{ nullptr };

	JsonObjectBase(ObjectType t);

	JsonObjectBase& operator=(bool value) noexcept;

	JsonObjectBase(bool value) noexcept;

	JsonObjectBase& operator=(std::int64_t value) noexcept;

	JsonObjectBase(int64_t value) noexcept;

	JsonObjectBase& operator=(std::uint64_t value) noexcept;

	JsonObjectBase(uint64_t value) noexcept;

	JsonObjectBase& operator=(double value) noexcept;

	JsonObjectBase(double value) noexcept;

	JsonObjectBase& operator=(float value) noexcept;

	JsonObjectBase(float value) noexcept;

	JsonObjectBase& operator=(std::string& value) noexcept;

	JsonObjectBase(std::string& value) noexcept;

	JsonObjectBase& operator=(std::string&& value) noexcept;

	JsonObjectBase(std::string&& value) noexcept;

	JsonObjectBase&operator=(JsonObjectBase&& value) noexcept;

	JsonObjectBase(JsonObjectBase&& value) noexcept;

	JsonObjectBase& operator=(Array& value) noexcept;

	JsonObjectBase(Array& value) noexcept;

	JsonObjectBase& operator=(JsonObjectBase& other) noexcept;

	JsonObjectBase(JsonObjectBase& other) noexcept;

	JsonObjectBase& operator=(const JsonObjectBase& other) noexcept = default;

	JsonObjectBase(const JsonObjectBase& other) noexcept = default;

	void append(const char* keyName, JsonObjectBase&& theData);

	void append(const char* keyName, JsonObjectBase& theData);

	std::unordered_map<std::string, JsonObjectBase>& getMap();

	std::unordered_map<std::string, JsonObjectBase> theMap{};

	ObjectType type();

	const char* toString(std::unordered_map<std::string, JsonObjectBase>& theMap);

	std::string toString(Array& theValue, bool doWeDrawAComma);

	std::string toString(JsonObjectBase& theValue, bool doWeDrawAComma);

	std::string toString(std::string& theValue, bool doWeDrawAComma);

	std::string toString(bool& theValue, bool doWeDrawAComma);

	std::string toString(float& theValue, bool doWeDrawAComma);

	std::string toString(double& theValue, bool doWeDrawAComma);

	std::string toString(uint64_t& theValue, bool doWeDrawAComma);

	std::string toString(int64_t& theValue, bool doWeDrawAComma);

	JsonObjectBase()noexcept {};

	operator std::string() {
		std::string theString{};
		theString.append("{");
		theString.append(this->toString(*this, false));
		theString.append("}");
		if (theString[theString.size() - 2] == ',') {
			theString.erase(theString.begin() + theString.size() - 2);
		}
		std::cout << "THE STRING: 0101: " << theString << std::endl;
		return theString;
	}

	JsonObjectBase& operator[](const char* key) {
		return this->theMap[key];
	}


	~JsonObjectBase();

	std::string theStringNew{};
	int32_t currentDepth{};
	std::string theKey{};
	ObjectType theType{};
};

struct Array : public JsonObjectBase {
	friend class JsonObjectBase;
	void append(JsonObjectBase& theObject);
	void append(JsonObjectBase&& theObject);
	std::vector<JsonObjectBase>& getVector();
	operator std::string();
protected:
	std::vector<JsonObjectBase> theVector{};
};


void JsonObjectBase::append(const char* keyName, JsonObjectBase& theData) {
	this->theMap.emplace(keyName, theData);
	this->theMap[keyName].theKey = keyName;
}

void JsonObjectBase::append(const char* keyName, JsonObjectBase&& theData) {
	this->theMap.emplace(keyName, std::move(theData));
	this->theMap[keyName].theKey = keyName;
}

std::unordered_map<std::string, JsonObjectBase>& JsonObjectBase::getMap() {
	return this->theMap;
}

void Array::append(JsonObjectBase& theObject) {
	this->theVector.push_back(theObject);
}

std::vector<JsonObjectBase>& Array::getVector() {
	return this->theVector;
}

void Array::append(JsonObjectBase&& theObject) {
	this->theVector.push_back(std::move(theObject));
}

bool operator==(const JsonObjectBase& lhs, const JsonObjectBase& rhs) {
	if (lhs.currentDepth == rhs.currentDepth) {
		return true;
	}
	else {
		return false;
	}
}

JsonObjectBase::JsonObjectBase(ObjectType theType) {
	switch (theType) {
	case ObjectType::JsonObjectBase: {
		thePtr = std::make_unique<JsonObjectBase>().release();
		break;
	}

	case ObjectType::Array: {
		thePtr = std::make_unique<Array>().release();
		break;
	}

	case ObjectType::String: {
		thePtr = std::make_unique<std::string>().release();
		break;
	}

	case ObjectType::Boolean: {
		thePtr = std::make_unique<bool>().release();
		break;
	}

	case ObjectType::Number_Integer: {
		thePtr = std::make_unique<int64_t>().release();
		break;
	}

	case ObjectType::Number_Unsigned: {
		thePtr = std::make_unique<uint64_t>().release();
		break;
	}

	case ObjectType::Number_Float: {
		thePtr = std::make_unique<float>().release();
		break;
	}

	case ObjectType::Number_Double: {
		thePtr = std::make_unique<double>().release();
		break;
	}

	case ObjectType::Null: {
		thePtr = std::make_unique<JsonObjectBase>().release();
		break;
	}
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
	this->currentDepth = other.currentDepth;
	this->theStringNew = other.theStringNew;
	this->theType = other.theType;
	this->theKey = other.theKey;
	this->theMap = other.theMap;
	this->thePtr = other.thePtr;
	return *this;
}

JsonObjectBase& JsonObjectBase::operator=(JsonObjectBase&& value) noexcept {
	this->currentDepth = value.currentDepth;
	
	this->theKey = std::move(value.theKey);
	this->theMap = std::move(value.theMap);
	this->theStringNew = std::move(value.theStringNew);
	this->theType = std::move(value.theType);
	this->thePtr = value.thePtr;
	return *this;
}

JsonObjectBase::JsonObjectBase(JsonObjectBase&& value) noexcept {
	*this = std::move(value);
}

JsonObjectBase::JsonObjectBase(JsonObjectBase& other) noexcept {
	*this = other;
}

ObjectType JsonObjectBase::type() {
	return this->theType;
}


JsonObjectBase& JsonObjectBase::operator=(Array& theArray) noexcept {
	this->thePtr = std::make_unique<Array>().release();
	this->theType = ObjectType::Array;
	*(Array*)this->thePtr = theArray;
	return *this;
}

JsonObjectBase::JsonObjectBase(Array& theArray) noexcept {
	*this = theArray;
};

JsonObjectBase& JsonObjectBase::operator=(uint64_t other) noexcept {
	this->thePtr = std::make_unique<uint64_t>().release();
	this->theType = ObjectType::Number_Unsigned;
	*(uint64_t*)this->thePtr = other;
	return *this;
}

JsonObjectBase::JsonObjectBase(uint64_t other) noexcept {
	*this = other;
}

JsonObjectBase& JsonObjectBase::operator=(int64_t other) noexcept {
	this->thePtr = std::make_unique<int64_t>().release();
	this->theType = ObjectType::Number_Integer;
	*(int64_t*)this->thePtr = other;
	return *this;
}

JsonObjectBase::JsonObjectBase(int64_t other) noexcept {
	*this = other;
}

JsonObjectBase& JsonObjectBase::operator=(bool other) noexcept {
	this->thePtr = std::make_unique<bool>().release();
	this->theType = ObjectType::Boolean;
	*(bool*)this->thePtr = other;
	return *this;
}

JsonObjectBase::JsonObjectBase(bool other) noexcept {
	*this = other;
}

JsonObjectBase& JsonObjectBase::operator=(float other) noexcept {
	this->thePtr = std::make_unique<float>().release();
	this->theType = ObjectType::Number_Float;
	*(float*)this->thePtr = other;
	return *this;
}

JsonObjectBase::JsonObjectBase(float other) noexcept {
	*this = other;
}

JsonObjectBase& JsonObjectBase::operator=(double other) noexcept {
	this->thePtr = std::make_unique<double>().release();
	this->theType = ObjectType::Number_Double;
	*(double*)this->thePtr = other;
	return *this;
}

JsonObjectBase::JsonObjectBase(double other) noexcept {
	*this = other;
}

JsonObjectBase& JsonObjectBase::operator=(std::string&& other) noexcept {
	this->thePtr = std::make_unique<std::string>().release();
	this->theType = ObjectType::String;
	*(std::string*)this->thePtr = other;
	return *this;
}

JsonObjectBase::JsonObjectBase(std::string&& other) noexcept {
	*this = other;
}

JsonObjectBase& JsonObjectBase::operator=(std::string& other) noexcept {
	this->thePtr = std::make_unique<std::string>().release();
	this->theType = ObjectType::String;
	*(std::string*)this->thePtr = other;
	return *this;
}

JsonObjectBase::JsonObjectBase(std::string& other) noexcept {
	*this = other;
}

std::string JsonObjectBase::toString(Array& theValue, bool doWeDrawAComma){
	if (!this->theKey.empty()) {
		this->theStringNew.push_back('\"');
		this->theStringNew += this->theKey;
		this->theStringNew.push_back('\"');
		this->theStringNew.push_back(':');
	}
	this->theStringNew += "["; 
	for (auto& value : theValue.theVector) {
		switch (value.theType) {
		case ObjectType::Number_Float: {
			this->theStringNew += this->toString(*(float*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Number_Double: {
			this->theStringNew += this->toString(*(double*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::String: {
			this->theStringNew += this->toString(*(std::string*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Boolean: {
			this->theStringNew += this->toString(*(bool*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Number_Integer: {
			this->theStringNew += this->toString(*(int64_t*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Array: {
			this->theStringNew += this->toString(*(Array*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::JsonObjectBase: {
			this->theStringNew += this->toString(*(JsonObjectBase*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Number_Unsigned: {
			this->theStringNew += this->toString(*(uint64_t*)value.thePtr, false);
			return this->theStringNew.data();
		}
		}
	}
	this->theStringNew += "]";
	if (this->theStringNew[this->theStringNew.size() - 2] == ',') {
		this->theStringNew.erase(this->theStringNew.begin() + this->theStringNew.size() - 2);
	}
	std::cout << "THE STRING: " << theStringNew << std::endl;
	this->theStringNew = this->theStringNew.substr(0, this->theStringNew.size() - 2);
	return this->theStringNew.data();
}

std::string JsonObjectBase::toString(JsonObjectBase& theValue, bool doWeDrawAComma){
	if (!this->theKey.empty()) {
		this->theStringNew.push_back('\"');
		this->theStringNew += this->theKey;
		this->theStringNew.push_back('\"');
		this->theStringNew.push_back(':');
	}
	this->theStringNew += "{";
	for (auto& [key,value] : theValue.theMap) {
		switch (value.theType) {
		case ObjectType::Number_Float: {
			this->theStringNew += this->toString(*(float*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Number_Double: {
			this->theStringNew += this->toString(*(double*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::String: {
			this->theStringNew += this->toString(*(std::string*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Boolean: {
			this->theStringNew += this->toString(*(bool*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Number_Integer: {
			this->theStringNew += this->toString(*(int64_t*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Array: {
			this->theStringNew += this->toString(*(Array*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::JsonObjectBase: {
			this->theStringNew += this->toString(*(JsonObjectBase*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Number_Unsigned: {
			this->theStringNew += this->toString(*(uint64_t*)value.thePtr, false);
			return this->theStringNew.data();
		}
		}
	}
	this->theStringNew += "}";
	if (this->theStringNew[this->theStringNew.size() - 2] == ',') {
		this->theStringNew.erase(this->theStringNew.begin() + this->theStringNew.size() - 2);
	}
	std::cout << "THE STRING: " << theStringNew << std::endl;
	this->theStringNew = this->theStringNew.substr(0, this->theStringNew.size() - 2);
	return this->theStringNew.data();
}

std::string JsonObjectBase::toString(std::string& theValue, bool doWeDrawAComma){
	if (!this->theKey.empty()) {
		this->theStringNew.push_back('\"');
		this->theStringNew += this->theKey;
		this->theStringNew.push_back('\"');
		this->theStringNew.push_back(':');
	}
	this->theStringNew += "\"";
	this->theStringNew += theValue;
	this->theStringNew += "\"";
	std::cout << "THE STRING: " << theStringNew << std::endl;
	this->theStringNew = this->theStringNew.substr(0, this->theStringNew.size() - 2);
	return this->theStringNew.data();
}

std::string JsonObjectBase::toString(float& theValue, bool doWeDrawAComma){
	if (!this->theKey.empty()) {
		this->theStringNew.push_back('\"');
		this->theStringNew += this->theKey;
		this->theStringNew.push_back('\"');
		this->theStringNew.push_back(':');
	}
	std::stringstream theStream{};
	theStream << std::setprecision(12) << theValue;
	this->theStringNew += theStream.str();
	std::cout << "THE STRING: " << theStringNew << std::endl;
	this->theStringNew = this->theStringNew.substr(0, this->theStringNew.size() - 2);
	return this->theStringNew.data();
}

std::string JsonObjectBase::toString(double& theValue, bool doWeDrawAComma){
	if (!this->theKey.empty()) {
		this->theStringNew.push_back('\"');
		this->theStringNew += this->theKey;
		this->theStringNew.push_back('\"');
		this->theStringNew.push_back(':');
	}
	std::stringstream theStream{};
	theStream << std::setprecision(12) << theValue;
	this->theStringNew += theStream.str();
	std::cout << "THE STRING: " << theStringNew << std::endl;
	this->theStringNew = this->theStringNew.substr(0, this->theStringNew.size() - 2);
	return this->theStringNew.data();
}

std::string JsonObjectBase::toString(uint64_t& theValue, bool doWeDrawAComma){

	if (!this->theKey.empty()) {
		this->theStringNew.push_back('\"');
		this->theStringNew += this->theKey;
		this->theStringNew.push_back('\"');
		this->theStringNew.push_back(':');
	}
	this->theStringNew += std::to_string(theValue);
	std::cout << "THE STRING: " << theStringNew << std::endl;
	this->theStringNew = this->theStringNew.substr(0, this->theStringNew.size() - 2);
	return this->theStringNew.data();
}

std::string JsonObjectBase::toString(bool& theValue, bool doWeDrawAComma) {
	if (!this->theKey.empty()) {
		this->theStringNew.push_back('\"');
		this->theStringNew += this->theKey;
		this->theStringNew.push_back('\"');
		this->theStringNew.push_back(':');
	}
	std::stringstream theStream{};
	theStream << std::boolalpha << theValue;
	this->theStringNew += theStream.str();
	std::cout << "THE STRING: " << theStringNew << std::endl;
	this->theStringNew = this->theStringNew.substr(0, this->theStringNew.size() - 2);
	return this->theStringNew.data();
}

std::string JsonObjectBase::toString(int64_t& theValue, bool doWeDrawAComma){

	if (!this->theKey.empty()) {
		this->theStringNew.push_back('\"');
		this->theStringNew += this->theKey;
		this->theStringNew.push_back('\"');
		this->theStringNew.push_back(':');
	}
	this->theStringNew += std::to_string(theValue);
	std::cout << "THE STRING: " << theStringNew << std::endl;
	this->theStringNew = this->theStringNew.substr(0, this->theStringNew.size() - 2);
	return this->theStringNew.data();
}

const char* JsonObjectBase::toString(std::unordered_map<std::string, JsonObjectBase>& theMap) {
	for (auto& [key, value] : theMap) {
		std::cout << "THE TYPE: " << (int)value.theType << std::endl;
		switch (value.theType) {
		case ObjectType::Number_Float: {
			this->theStringNew += this->toString(*(float*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Number_Double: {
			this->theStringNew += this->toString(*(double*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::String: {
			this->theStringNew += this->toString(*(std::string*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Boolean: {
			this->theStringNew += this->toString(*(bool*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Number_Integer: {
			this->theStringNew += this->toString(*(int64_t*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Array: {
			this->theStringNew += this->toString(*(Array*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::JsonObjectBase: {
			this->theStringNew += this->toString(*(JsonObjectBase*)value.thePtr, false);
			return this->theStringNew.data();
		}
		case ObjectType::Number_Unsigned: {
			this->theStringNew += this->toString(*(uint64_t*)value.thePtr, false);
			return this->theStringNew.data();
		}
		}

	}
	
	return this->theStringNew.data();
}

JsonObjectBase::~JsonObjectBase() {}

    struct JsonObject {

        
    protected:
        int32_t currentDepth{ 0 };
    };

	Array::operator std::string() {
		std::string theString{ "\"" };
		theString += this->theKey;
		theString += "\":[";
		bool doWeAddCommas{ false };
		for (uint32_t x = 0; x < this->theVector.size(); ++x) {
			if (x > 0 && x < this->theVector.size() - 1) {
				doWeAddCommas = true;
			}
			switch (this->theVector[x].theType) {
				case ObjectType::Number_Float: {
					this->theStringNew += this->toString(*(float*)theVector[x].thePtr, false);
					return this->theStringNew.data();
				}
				case ObjectType::Number_Double: {
					this->theStringNew += this->toString(*(double*)theVector[x].thePtr, false);
					return this->theStringNew.data();
				}
				case ObjectType::String: {
					this->theStringNew += this->toString(*(std::string*)theVector[x].thePtr, false);
					return this->theStringNew.data();
				}
				case ObjectType::Boolean: {
					this->theStringNew += this->toString(*(bool*)theVector[x].thePtr, false);
					return this->theStringNew.data();
				}
				case ObjectType::Number_Integer: {
					this->theStringNew += this->toString(*(int64_t*)theVector[x].thePtr, false);
					return this->theStringNew.data();
				}
				case ObjectType::Array: {
					this->theStringNew += this->toString(*(Array*)theVector[x].thePtr, false);
					return this->theStringNew.data();
				}
				case ObjectType::JsonObjectBase: {
					this->theStringNew += this->toString(*(JsonObjectBase*)theVector[x].thePtr, false);
					return this->theStringNew.data();
				}
				case ObjectType::Number_Unsigned: {
					this->theStringNew += this->toString(*(uint64_t*)theVector[x].thePtr, false);
					return this->theStringNew.data();
				}
			}
		}
		theString += "]";
		return theString;
	}

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
		JsonObjectBase data{};
        JsonObjectBase theD{};
        JsonObjectBase theProperties{};
		theProperties.append("browser", "DiscordCoreAPI");
		theProperties.append("device", "DiscordCoreAPI");
        Array theShard{};
        theShard.append(static_cast<uint64_t>(this->currentShard));
        theShard.append(static_cast<uint64_t>(this->numberOfShards));
        theD.append("shard", theShard);
        theD.append("large_threshold", static_cast<uint64_t>(250));
        theD.append("intents", static_cast<uint64_t>(this->intents));
        theD.append("compress", false);
        theD.append("token", this->botToken);
        Array theActivities{};
        for (auto& value : this->presence.activities) {
            JsonObjectBase dataNew{};
            if (static_cast<std::string>(value.url) != "") {
                dataNew.append("url", std::string{ value.url });
            }
            dataNew.append("name", std::string{ value.name });
            dataNew.append("type", static_cast<uint64_t>(value.type));
            theActivities.append(dataNew);
        }
        JsonObjectBase dataNewReal{};
        dataNewReal.append("status", this->presence.status);
        dataNewReal.append("since", static_cast<uint64_t>(0));
        dataNewReal.append("afk", this->presence.afk);
        theD.append("presence", dataNewReal);
        data.append("op", static_cast<uint64_t>(2));
#ifdef _WIN32
        theProperties.append("os", "Windows");
#else
		theProperties.append("os", "Linux");
#endif
		theD.append("properties", theProperties);
        data.append("d", theD);
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
