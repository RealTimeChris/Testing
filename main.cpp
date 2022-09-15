#include <discordcoreapi/Index.hpp>
#include <simdjson.h>
#include <unordered_set>
#include "ErlPacker.hpp"
#define JSON_HEDLEY_RETURNS_NON_NULL
#include <nlohmann/thirdparty/hedley/hedley.hpp>
#include <xmemory>
#include <type_traits>

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
    std::unordered_map<std::string, JsonScalarObject>theMap{};
};
struct Array {
    std::vector<JsonScalarObject> theVector{};
};

/// helper for exception-safe theScalar creation
template<typename T, typename... Args>
static T* create(Args&& ... args)
{
    struct Deleter {
        void operator()(T* obj) {
            delete obj;
        }
    };
    std::unique_ptr<T, Deleter> obj(new T{});
    return obj.release();
}

struct JsonScalarObject {
    union JsonValueInternal
    {
        /// theScalar (stored with pointer to save storage)
        void* theScalar{ nullptr };
        Object* theObject;
        Array* theArray;

        JsonValueInternal(ObjectType t)
        {
            switch (t)
            {
            case ObjectType::Object:
            {
                theObject = new Object{};
                break;
            }

            case ObjectType::Array:
            {
                theArray = create<Array>();
                break;
            }

            case ObjectType::String:
            {
                theScalar = new std::string{};
                break;
            }

            case ObjectType::Boolean:
            {
                theScalar = new bool{};
                break;
            }

            case ObjectType::Number:
            {
                theScalar = new int64_t{};
                break;
            }

            case ObjectType::Number_Unsigned:
            {
                theScalar = new uint64_t{};
                break;
            }

            case ObjectType::Number_Float:
            {
                theScalar = new float{};
                break;
            }

            case ObjectType::Number_Double:
            {
                theScalar = new double{};
                break;
            }

            case ObjectType::Null:
            {
                theScalar = new Object{};
                break;
            }
            }
        }

        /// default constructor (for null values)
        JsonValueInternal() noexcept = default;
        /// constructor for booleans
        JsonValueInternal(bool value) noexcept : theScalar(new bool{ value }) {}
        /// constructor for numbers (integer)
        JsonValueInternal(int64_t value) noexcept : theScalar(new int64_t{ value }) {};
        /// constructor for numbers (unsigned)
        JsonValueInternal(uint64_t value) noexcept : theScalar(new uint64_t{ value }) {};
        /// constructor for numbers (floating-point)
        JsonValueInternal(double value) noexcept : theScalar(new double{ value }) {};
        /// constructor for empty values of a given type

        /// constructor for strings
        JsonValueInternal(const std::string& value) : theScalar(new std::string{ value }) {};

        /// constructor for rvalue strings
        JsonValueInternal(std::string&& value) : theScalar(new std::string{ std::move(value) }) {};

        /// constructor for theScalars
        JsonValueInternal(const Object& value) : theScalar(new Object{ value }) {}

        /// constructor for rvalue theScalars
        JsonValueInternal(Object&& value) : theScalar(new Object{ std::move(value) }) {};

        /// constructor for arrays
        JsonValueInternal(const Array& value) : theScalar(new Array{ value }) {};

        /// constructor for rvalue arrays
        JsonValueInternal(Array&& value) : theScalar(new Array{ std::move(value) }) {};
        ~JsonValueInternal() {};
    };



    ObjectType type() {
        return this->theType;
    }

    
    JsonScalarObject& operator=(std::vector<JsonScalarObject> theArray) {
        this->theValue.theArray = create<Array>(theArray.begin(), theArray.end());
        this->theValue.theArray->theVector.insert(this->theValue.theArray->theVector.begin(), theArray.begin(), theArray.end());
        this->theType = ObjectType::Array;
        return *this;
    }

    JsonScalarObject(std::vector<JsonScalarObject> theArray) {
        *this=theArray;
    };

    JsonScalarObject& operator=(float& other) {
        this->theValue.theScalar = new float{};
        *(float*)this->theValue.theScalar = other;
        this->theType = ObjectType::Number_Float;
        return *this;
    }

    JsonScalarObject(float& other) :theValue(new float{ other }) {
        *this = other;
    }

    JsonScalarObject& operator=(Array& other) {
        this->theValue.theArray = new Array{};
        *this->theValue.theArray = other;
        this->theType = ObjectType::Array;
        return *this;
    }

    JsonScalarObject(Array& other) {
        *this = other;
    }

    JsonScalarObject& operator=(double& other) {
        this->theValue.theScalar = new double{};
        *(double*)this->theValue.theScalar = other;
        this->theType = ObjectType::Number_Float;
        return *this;
    }

    JsonScalarObject(double& other) :theValue(new double{ other }) {
        *this = other;
    }

    JsonScalarObject& operator=(std::string&& other) {
        this->theValue.theScalar = new std::string{};
        *(std::string*)this->theValue.theScalar = other;
        this->theType = ObjectType::String;
        return *this;
    }

    JsonScalarObject(std::string&& other) :theValue(ObjectType::String) {
        *this = other;
    }

    JsonScalarObject& operator=(std::string& other) {
        this->theValue.theScalar = new std::string{};
        *(std::string*)this->theValue.theScalar = other;
        this->theType = ObjectType::String;
        return *this;
    }

    JsonScalarObject(std::string& other) :theValue(ObjectType::String) {
        *this = other;
    }

    const char* toString(int32_t  depth) {
        switch (this->theType) {
        case ObjectType::Number_Float: {
            if (!this->theKey.empty()) {
                this->theStringNew.push_back('\"');
                this->theStringNew += this->theKey;
                this->theStringNew.push_back('\"');
                this->theStringNew.push_back(':');
            }
            depth++;
            float theFloat = *(float*)this->theValue.theScalar;
            std::stringstream theStream{};
            theStream << std::setprecision(12) << theFloat;
            this->theStringNew += theStream.str();
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
            double theFloat = *(double*)this->theValue.theScalar;
            std::stringstream theStream{};
            theStream << std::setprecision(12) << theFloat;
            this->theStringNew += theStream.str();
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
            this->theStringNew += (*(std::string*)this->theValue.theScalar).data();
            this->theStringNew += "\"";
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
            bool theData = *(bool*)this->theValue.theScalar;
            std::stringstream theStream{};
            theStream << std::boolalpha << theData;
            this->theStringNew += theStream.str();
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
            int64_t theData = *(int64_t*)this->theValue.theScalar;
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
            if (depth > 0) {
                this->theStringNew += ",";
            }
            this->theStringNew += this->theStringNew;
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

    JsonScalarObject() {};

    JsonValueInternal theValue{};
    std::string theStringNew{};
    std::string theKey{};
    ObjectType theType{};
};

    struct JsonObject {
        std::unordered_map<std::string, JsonScalarObject> theMap{};

        template<std::same_as<float> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Number_Float;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<double> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Number_Double;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<int64_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Number;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<int32_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Number;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<int16_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Number;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<int8_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Number;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<uint64_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Number_Unsigned;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<uint32_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Number_Unsigned;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<uint16_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Number_Unsigned;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<uint8_t> JsonObjectType>
        void append(const char* keyName, JsonObjectType theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Number_Unsigned;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<Array> JsonObjectType>
        void append(const char* keyName, JsonObjectType  theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Array;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<std::string> JsonObjectType>
        void append(const char* keyName, JsonObjectType  theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::String;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<bool> JsonObjectType>
        void append(const char* keyName, JsonObjectType  theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Boolean;
            theMap[keyName].theKey = keyName;
        }

        template<std::same_as<std::vector<JsonScalarObject>> JsonVectorType>
        void append(const char* keyName, JsonVectorType theObject) {
            theMap[keyName] = JsonScalarObject{ theObject };
            theMap[keyName].theType = ObjectType::Array;
            theMap[keyName].theKey = keyName;
        }
        operator std::string() {
            std::string theString{};
            theString.append("{");
            for (auto& [key, value] : this->theMap) {
                theString.append(value.toString(0));
            }
            theString.append("}");
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
            std::vector<JsonScalarObject> theVectorNew{};
            theVectorNew.push_back(JsonScalarObject{ theScalar });
            theObject.append("TEST02", theFloat);
            theObject.append("TEST03", theVectorNew);
            std::cout << "WERE HERE THIS IS IT! 0101: " << (std::string)theObject << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds{ 3 });

        }
        catch (...) { DiscordCoreAPI::reportException("main()"); };

        return 0;
    }
