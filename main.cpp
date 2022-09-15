#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include "ErlPacker.hpp"
#include <stdint.h>

enum class ObjectType : int8_t {
    Object = 0,
    Array = 1,
    String = 2,
    Boolean = 3,
    Number = 4,
    Number_Unsigned = 5,
    Number_Float = 6,
    Number_Double = 7,
    Null = 8
};

class JsonScalarObject;

struct Object {
    std::map<std::string, JsonScalarObject>theMap{};
};

struct Array {
    std::vector<JsonScalarObject> theVector{};
};

struct JsonValueInternal
{
    std::unique_ptr<std::string> theString{ nullptr };
    std::unique_ptr<Object> theObject;
    std::unique_ptr<bool> theBool;
    std::unique_ptr<float> theFloat;
    std::unique_ptr<double> theDouble;
    std::unique_ptr<std::int64_t> theInt;
    std::unique_ptr<std::uint64_t> theUint;
    std::unique_ptr<Array> theArray;

    JsonValueInternal(ObjectType t);

    JsonValueInternal& operator=(bool value) noexcept;

    JsonValueInternal(bool value) noexcept;

    JsonValueInternal& operator=(std::int64_t value) noexcept;

    JsonValueInternal(std::int64_t value) noexcept;

    JsonValueInternal& operator=(std::uint64_t value) noexcept;

    JsonValueInternal(std::uint64_t value) noexcept;

    JsonValueInternal& operator=(double value) noexcept;

    JsonValueInternal(double value) noexcept;

    JsonValueInternal& operator=(float value) noexcept;

    JsonValueInternal(float value) noexcept;

    JsonValueInternal& operator=(std::string& value) noexcept;

    JsonValueInternal(std::string& value) noexcept;

    JsonValueInternal& operator=(Object& value) noexcept;

    JsonValueInternal(Object& value) noexcept;

    JsonValueInternal& operator=(Object&& value) noexcept;

    JsonValueInternal(Object&& value)  noexcept;

    JsonValueInternal& operator=(Array& value) noexcept;

    JsonValueInternal(Array& value) noexcept;

    JsonValueInternal& operator=(Array&& value) noexcept;

    JsonValueInternal(Array&& value) noexcept;

    JsonValueInternal& operator=(const JsonValueInternal& other) noexcept;

    JsonValueInternal(const JsonValueInternal& other) noexcept;

    ~JsonValueInternal() noexcept;
};

struct JsonScalarObject {

    ObjectType type();

    JsonScalarObject& operator=(const JsonScalarObject&);

    JsonScalarObject(const JsonScalarObject&);

    JsonScalarObject& operator=(Array& theArray);

    JsonScalarObject(Array& theArray);

    JsonScalarObject& operator=(float& other);

    JsonScalarObject(float& other);

    JsonScalarObject& operator=(float&& other);

    JsonScalarObject(float&& other);

    JsonScalarObject& operator=(double& other);

    JsonScalarObject(double& other);

    JsonScalarObject& operator=(double&& other);

    JsonScalarObject(double&& other);

    JsonScalarObject& operator=(std::string&& other);

    JsonScalarObject(std::string&& other);

    JsonScalarObject& operator=(std::string& other);

    JsonScalarObject(std::string& other);

    const char* toString(int32_t  depth);

    JsonScalarObject();

    JsonValueInternal theValue{ ObjectType::String };
    std::string theStringNew{};
    std::string theKey{};
    ObjectType theType{};
};

JsonValueInternal::JsonValueInternal(ObjectType t) {
    switch (t)
    {
    case ObjectType::Object:
    {
        theObject = std::make_unique<Object>();
        break;
    }

    case ObjectType::Array:
    {
        theArray = std::make_unique<Array>();
        break;
    }

    case ObjectType::String:
    {
        theString = std::make_unique<std::string>();
        break;
    }

    case ObjectType::Boolean:
    {
        theBool = std::make_unique<bool>();
        break;
    }

    case ObjectType::Number:
    {
        theInt = std::make_unique<int64_t>();
        break;
    }

    case ObjectType::Number_Unsigned:
    {
        theUint = std::make_unique<uint64_t>();
        break;
    }

    case ObjectType::Number_Float:
    {
        theFloat = std::make_unique<float>();
        break;
    }

    case ObjectType::Number_Double:
    {
        theDouble = std::make_unique<double>();
        break;
    }

    case ObjectType::Null:
    {
        theObject = std::make_unique<Object>();
        break;
    }
    }
}

JsonValueInternal& JsonValueInternal::operator=(bool value) noexcept {
    this->theBool = std::make_unique<bool>();
    *this->theBool = value;
    return *this;
};

JsonValueInternal::JsonValueInternal(bool value) noexcept : theBool(std::make_unique<bool>(value)) {
    *this = value;
};

JsonValueInternal& JsonValueInternal::operator=(int64_t value) noexcept {
    this->theInt = std::make_unique<int64_t>();
    *this->theInt = value;
    return *this;
};

JsonValueInternal::JsonValueInternal(int64_t value) noexcept : theInt(std::make_unique<int64_t>()) {
    *this = value;
};

JsonValueInternal& JsonValueInternal::operator=(uint64_t value) noexcept {
    this->theUint = std::make_unique<uint64_t>();
    *this->theUint = value;
    return *this;
};

JsonValueInternal::JsonValueInternal(uint64_t value) noexcept : theUint(std::make_unique<uint64_t>()) {
    *this = value;
};

JsonValueInternal& JsonValueInternal::operator=(double value) noexcept {
    this->theDouble = std::make_unique<double>();
    *this->theDouble = value;
    return *this;
};

JsonValueInternal::JsonValueInternal(double value) noexcept : theDouble(std::make_unique<double>()) {
    *this = value;
};

JsonValueInternal& JsonValueInternal::operator=(float value) noexcept {
    this->theFloat = std::make_unique<float>();
    *this->theFloat = value;
    return *this;
};

JsonValueInternal::JsonValueInternal(float value) noexcept : theFloat(std::make_unique<float>()) {
    *this = value;
};

JsonValueInternal& JsonValueInternal::operator=(std::string& value) noexcept {
    this->theString = std::make_unique<std::string>();
    *this->theString = value;
    return *this;
};

JsonValueInternal::JsonValueInternal(std::string& value) noexcept : theString(std::make_unique<std::string>()) {
    *this = value;
};

JsonValueInternal& JsonValueInternal::operator=(Object& value) noexcept {
    this->theObject = std::make_unique<Object>();
    *this->theObject = value;
    return *this;
};

JsonValueInternal::JsonValueInternal(Object& value)  noexcept : theObject(std::make_unique<Object>()) {
    *this = value;
};

JsonValueInternal& JsonValueInternal::operator=(Object&& value) noexcept {
    this->theObject = std::make_unique<Object>();
    *this->theObject = value;
    return *this;
};

JsonValueInternal::JsonValueInternal(Object&& value)  noexcept : theObject(std::make_unique<Object>()) {
    *this = std::move(value);
};

JsonValueInternal& JsonValueInternal::operator=(Array& value) noexcept {
    this->theArray = std::make_unique<Array>();
    *this->theArray = value;
    return *this;
};

JsonValueInternal::JsonValueInternal(Array& value)  noexcept : theArray(std::make_unique<Array>()) {
    *this = value;
};

JsonValueInternal& JsonValueInternal::operator=(Array&& value) noexcept {
    this->theArray = std::make_unique<Array>();
    *this->theArray = value;
    return *this;
};

JsonValueInternal::JsonValueInternal(Array&& value)  noexcept : theArray(std::make_unique<Array>()) {
    *this = std::move(value);
};

JsonValueInternal& JsonValueInternal::operator=(const JsonValueInternal& other) noexcept {
    if (other.theArray){
        this->theArray = std::make_unique<Array>();
        *this->theArray = *other.theArray;
    }else if (other.theBool){
        this->theBool = std::make_unique<bool>();
        *this->theBool = *other.theBool;
    }
    else if (other.theDouble) {
        this->theDouble = std::make_unique<double>();
        *this->theDouble = *other.theDouble;
    }
    else if (other.theFloat) {
        this->theFloat = std::make_unique<float>();
        *this->theFloat = *other.theFloat;
    }
    else if (other.theInt) {
        this->theInt = std::make_unique<int64_t>();
        *this->theInt = *other.theInt;
    }
    else if (other.theObject) {
        this->theObject = std::make_unique<Object>();
        *this->theObject = *other.theObject;
    }
    else if (other.theString){
        this->theString = std::make_unique<std::string>();
        *this->theString = *other.theString;
    }
    else if (other.theUint) {
        this->theUint = std::make_unique<uint64_t>();
        *this->theUint = *other.theUint;
    }
    return *this;
}

JsonValueInternal::JsonValueInternal(const JsonValueInternal& other)  noexcept {
    *this = other;
}

JsonValueInternal::~JsonValueInternal() {};


    ObjectType JsonScalarObject::type() {
        return this->theType;
    }

    JsonScalarObject& JsonScalarObject::operator=(const JsonScalarObject& other) {
        this->theKey = other.theKey;
        this->theStringNew = other.theStringNew;
        this->theType = other.theType;
        this->theValue = other.theValue;
        return *this;
    }

    JsonScalarObject::JsonScalarObject(const JsonScalarObject&other) {
        *this = other;
    }

    JsonScalarObject& JsonScalarObject::operator=(Array& theArray) {
        this->theValue = theArray;
        this->theType = ObjectType::Array;
        return *this;
    }

    JsonScalarObject::JsonScalarObject(Array& theArray){
        *this = theArray;
    };

    JsonScalarObject& JsonScalarObject::operator=(float& other) {
        this->theValue = other;
        this->theType = ObjectType::Number_Float;
        return *this;
    }

    JsonScalarObject::JsonScalarObject(float& other) {
        *this = other;
    }

    JsonScalarObject& JsonScalarObject::operator=(float&& other) {
        this->theValue = other;
        this->theType = ObjectType::Number_Float;
        return *this;
    }

    JsonScalarObject::JsonScalarObject(float&& other) {
        *this = other;
    }

    JsonScalarObject& JsonScalarObject::operator=(double& other) {
        this->theValue = other;
        this->theType = ObjectType::Number_Double;
        return *this;
    }

    JsonScalarObject::JsonScalarObject(double& other) {
        *this = other;
    }

    JsonScalarObject& JsonScalarObject::operator=(double&& other) {
        this->theValue = other;
        this->theType = ObjectType::Number_Double;
        return *this;
    }

    JsonScalarObject::JsonScalarObject(double&& other) {
        *this = other;
    }

    JsonScalarObject& JsonScalarObject::operator=(std::string&& other) {
        this->theValue = other;
        this->theType = ObjectType::String;
        return *this;
    }

    JsonScalarObject::JsonScalarObject(std::string&& other) {
        *this = other;
    }

    JsonScalarObject& JsonScalarObject::operator=(std::string& other) {
        this->theValue = other;
        this->theType = ObjectType::String;
        return *this;
    }

    JsonScalarObject::JsonScalarObject(std::string& other) {
        *this = other;
    }

    const char* JsonScalarObject::toString(int32_t  depth) {
        switch (this->theType) {
        case ObjectType::Number_Float: {
            if (!this->theKey.empty()) {
                this->theStringNew.push_back('\"');
                this->theStringNew += this->theKey;
                this->theStringNew.push_back('\"');
                this->theStringNew.push_back(':');
            }
            depth++;
            float theFloat = *this->theValue.theFloat.get();
            std::stringstream theStream{};
            theStream << std::setprecision(12) << theFloat;
            this->theStringNew += theStream.str();
            if (depth > 0) {
                this->theStringNew += ",";
            }
            return this->theStringNew.data();
        }
        case ObjectType::Number_Double: {
            if (!this->theKey.empty()) {
                this->theStringNew.push_back('\"');
                this->theStringNew += this->theKey;
                this->theStringNew.push_back('\"');
                this->theStringNew.push_back(':');
            }
            depth++;
            double theFloat = *this->theValue.theDouble.get();
            std::stringstream theStream{};
            theStream << std::setprecision(12) << theFloat;
            this->theStringNew += theStream.str();
            if (depth > 0) {
                this->theStringNew += ",";
            }
            return this->theStringNew.data();
        }
        case ObjectType::String: {
            if (!this->theKey.empty()) {
                this->theStringNew.push_back('\"');
                this->theStringNew += this->theKey;
                this->theStringNew.push_back('\"');
                this->theStringNew.push_back(':');
            }
            depth++;
            this->theStringNew += "\"";
            this->theStringNew += *this->theValue.theString.get();
            this->theStringNew += "\"";
            if (depth > 0) {
                this->theStringNew += ",";
            }
            return this->theStringNew.data();
        }
        case ObjectType::Boolean: {
            if (!this->theKey.empty()) {
                this->theStringNew.push_back('\"');
                this->theStringNew += this->theKey;
                this->theStringNew.push_back('\"');
                this->theStringNew.push_back(':');
            }
            depth++;
            bool theData = *this->theValue.theBool.get();
            std::stringstream theStream{};
            theStream << std::boolalpha << theData;
            this->theStringNew += theStream.str();
            if (depth > 0) {
                this->theStringNew += ",";
            }
            return this->theStringNew.data();

        }
        case ObjectType::Number: {
            if (!this->theKey.empty()) {
                this->theStringNew.push_back('\"');
                this->theStringNew += this->theKey;
                this->theStringNew.push_back('\"');
                this->theStringNew.push_back(':');
            }
            depth++;
            int64_t theData = *this->theValue.theInt.get();
            if (depth > 0) {
                this->theStringNew += ",";
            }
            this->theStringNew += std::to_string(theData);
            return this->theStringNew.data();
        }
        case ObjectType::Array: {
            if (!this->theKey.empty()) {
                this->theStringNew.push_back('\"');
                this->theStringNew += this->theKey;
                this->theStringNew.push_back('\"');
                this->theStringNew.push_back(':');
            }
            this->theStringNew += "[";
            for (auto& value : this->theValue.theArray->theVector) {
                depth++;
                this->theStringNew.append(value.toString(depth));
            }
            this->theStringNew += "]";
            if (this->theStringNew[this->theStringNew.size() - 2] == ',') {
                this->theStringNew.erase(this->theStringNew.begin() + this->theStringNew.size() - 2);
            }
            if (depth > 0) {
                this->theStringNew += ",";
            } 
            depth--;
            return this->theStringNew.data();
        }
        case ObjectType::Object: {
            this->theStringNew += "{";
            for (auto& [key,value]:this->theValue.theObject->theMap) {
                depth++;
                this->theStringNew.append(value.toString(depth));
            }
            this->theStringNew += "}";
            if (depth > 0) {
                this->theStringNew += ",";
            }
            depth--;
            return this->theStringNew.data();
        }
        }
    }

    JsonScalarObject::JsonScalarObject() {};


    struct JsonObject {
        std::unordered_map<std::string, JsonScalarObject> theMap{};

        template<std::same_as<float> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Number_Float;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<double> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Number_Double;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<int64_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Number;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<int32_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Number;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<int16_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Number;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<int8_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Number;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<uint64_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Number_Unsigned;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<uint32_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Number_Unsigned;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<uint16_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Number_Unsigned;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<uint8_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Number_Unsigned;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<Array> JsonObjectType>
        void append(const char* keyName, JsonObjectType  theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Array;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<std::string> JsonObjectType>
        void append(const char* keyName, JsonObjectType  theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::String;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<bool> JsonObjectType>
        void append(const char* keyName, JsonObjectType  theObject) {
            theMap[keyName] = theObject;
            theMap[keyName].theType = ObjectType::Boolean;
            theMap[keyName].theKey = keyName;
        }

        operator std::string() {
            std::string theString{};
            theString.append("{");
            for (auto& [key, value] : this->theMap) {
                theString.append(value.toString(1));
            }
            theString.append("}");
            if (theString[theString.size() - 2] == ',') {
                theString.erase(theString.begin() + theString.size() - 2);
            }
            return theString;
        }
        JsonScalarObject& operator[](const char* key)
        {
            return this->theMap[key];
        }
    };

    int32_t main() {
        try {
            std::vector<JsonScalarObject> theVector{};
            std::string theScalar{ "TESTING FOR REAL" };
            JsonObject theObject{ };
            double theFloat{ 0.05f };
            Array theVectorNew{};
            JsonScalarObject theValue{ theScalar };
            theVectorNew.theVector.push_back(theValue);
            theObject.append("TEST02", theFloat);
            theObject.append("TEST03", theVectorNew);
            std::cout << "WERE HERE THIS IS IT! 0101: " << nlohmann::json::parse((std::string)theObject).dump() << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        }
        catch (...) { DiscordCoreAPI::reportException("main()"); };

        return 0;
    }
