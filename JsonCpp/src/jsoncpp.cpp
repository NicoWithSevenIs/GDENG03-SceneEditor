// JsonCpp amalgamated source - simplified version
// This is a minimal implementation to get your scene system working

#include "json/json.h"
#include <sstream>
#include <iostream>
#include <iomanip>

namespace Json {

// ValueType implementation
const char* valueTypeNames[arrayValue + 1] = {
    "null", "int", "uint", "real", "string", "boolean", "array", "object"
};

// Value implementation
Value::Value(ValueType type) : type_(type) {
    switch (type) {
    case nullValue:
        break;
    case intValue:
        value_.int_ = 0;
        break;
    case uintValue:
        value_.uint_ = 0;
        break;
    case realValue:
        value_.real_ = 0.0;
        break;
    case stringValue:
        value_.string_ = new std::string();
        break;
    case booleanValue:
        value_.bool_ = false;
        break;
    case arrayValue:
        value_.array_ = new ArrayType();
        break;
    case objectValue:
        value_.object_ = new ObjectType();
        break;
    }
}

Value::Value() : type_(nullValue) {}

Value::Value(int value) : type_(intValue) {
    value_.int_ = value;
}

Value::Value(unsigned int value) : type_(uintValue) {
    value_.uint_ = value;
}

Value::Value(double value) : type_(realValue) {
    value_.real_ = value;
}

Value::Value(float value) : type_(realValue) {
    value_.real_ = static_cast<double>(value);
}

Value::Value(const char* value) : type_(stringValue) {
    value_.string_ = new std::string(value);
}

Value::Value(const std::string& value) : type_(stringValue) {
    value_.string_ = new std::string(value);
}

Value::Value(bool value) : type_(booleanValue) {
    value_.bool_ = value;
}

Value::Value(const Value& other) : type_(other.type_) {
    switch (type_) {
    case stringValue:
        value_.string_ = new std::string(*other.value_.string_);
        break;
    case arrayValue:
        value_.array_ = new ArrayType(*other.value_.array_);
        break;
    case objectValue:
        value_.object_ = new ObjectType(*other.value_.object_);
        break;
    default:
        value_ = other.value_;
        break;
    }
}

Value::~Value() {
    switch (type_) {
    case stringValue:
        delete value_.string_;
        break;
    case arrayValue:
        delete value_.array_;
        break;
    case objectValue:
        delete value_.object_;
        break;
    default:
        break;
    }
}

Value& Value::operator=(const Value& other) {
    if (this != &other) {
        Value temp(other);
        swap(temp);
    }
    return *this;
}

void Value::swap(Value& other) {
    ValueType tempType = type_;
    ValueUnion tempValue = value_;
    type_ = other.type_;
    value_ = other.value_;
    other.type_ = tempType;
    other.value_ = tempValue;
}

ValueType Value::type() const {
    return type_;
}

bool Value::isNull() const { return type_ == nullValue; }
bool Value::isBool() const { return type_ == booleanValue; }
bool Value::isInt() const { return type_ == intValue; }
bool Value::isUInt() const { return type_ == uintValue; }
bool Value::isIntegral() const { return type_ == intValue || type_ == uintValue; }
bool Value::isDouble() const { return type_ == realValue; }
bool Value::isNumeric() const { return isIntegral() || isDouble(); }
bool Value::isString() const { return type_ == stringValue; }
bool Value::isArray() const { return type_ == arrayValue; }
bool Value::isObject() const { return type_ == objectValue; }

int Value::asInt() const {
    switch (type_) {
    case intValue: return value_.int_;
    case uintValue: return static_cast<int>(value_.uint_);
    case realValue: return static_cast<int>(value_.real_);
    case stringValue: return std::stoi(*value_.string_);
    case booleanValue: return value_.bool_ ? 1 : 0;
    default: return 0;
    }
}

unsigned int Value::asUInt() const {
    switch (type_) {
    case intValue: return static_cast<unsigned int>(value_.int_);
    case uintValue: return value_.uint_;
    case realValue: return static_cast<unsigned int>(value_.real_);
    case stringValue: return static_cast<unsigned int>(std::stoi(*value_.string_));
    case booleanValue: return value_.bool_ ? 1 : 0;
    default: return 0;
    }
}

double Value::asDouble() const {
    switch (type_) {
    case intValue: return static_cast<double>(value_.int_);
    case uintValue: return static_cast<double>(value_.uint_);
    case realValue: return value_.real_;
    case stringValue: return std::stod(*value_.string_);
    case booleanValue: return value_.bool_ ? 1.0 : 0.0;
    default: return 0.0;
    }
}

float Value::asFloat() const {
    return static_cast<float>(asDouble());
}

bool Value::asBool() const {
    switch (type_) {
    case booleanValue: return value_.bool_;
    case intValue: return value_.int_ != 0;
    case uintValue: return value_.uint_ != 0;
    case realValue: return value_.real_ != 0.0;
    case stringValue: return !value_.string_->empty();
    case arrayValue: return !value_.array_->empty();
    case objectValue: return !value_.object_->empty();
    default: return false;
    }
}

std::string Value::asString() const {
    switch (type_) {
    case stringValue: return *value_.string_;
    case intValue: return std::to_string(value_.int_);
    case uintValue: return std::to_string(value_.uint_);
    case realValue: return std::to_string(value_.real_);
    case booleanValue: return value_.bool_ ? "true" : "false";
    case nullValue: return "null";
    default: return "";
    }
}

ArrayIndex Value::size() const {
    switch (type_) {
    case arrayValue: return static_cast<ArrayIndex>(value_.array_->size());
    case objectValue: return static_cast<ArrayIndex>(value_.object_->size());
    default: return 0;
    }
}

bool Value::empty() const {
    return size() == 0;
}

Value& Value::operator[](ArrayIndex index) {
    if (type_ != arrayValue) {
        *this = Value(arrayValue);
    }
    if (index >= value_.array_->size()) {
        value_.array_->resize(index + 1);
    }
    return (*value_.array_)[index];
}

const Value& Value::operator[](ArrayIndex index) const {
    static Value null;
    if (type_ != arrayValue || index >= value_.array_->size()) {
        return null;
    }
    return (*value_.array_)[index];
}

Value& Value::operator[](const char* key) {
    return (*this)[std::string(key)];
}

const Value& Value::operator[](const char* key) const {
    return (*this)[std::string(key)];
}

Value& Value::operator[](const std::string& key) {
    if (type_ != objectValue) {
        *this = Value(objectValue);
    }
    return (*value_.object_)[key];
}

const Value& Value::operator[](const std::string& key) const {
    static Value null;
    if (type_ != objectValue) {
        return null;
    }
    auto it = value_.object_->find(key);
    return (it != value_.object_->end()) ? it->second : null;
}

Value Value::get(const std::string& key, const Value& defaultValue) const {
    if (type_ != objectValue) {
        return defaultValue;
    }
    auto it = value_.object_->find(key);
    return (it != value_.object_->end()) ? it->second : defaultValue;
}

bool Value::isMember(const std::string& key) const {
    if (type_ != objectValue) {
        return false;
    }
    return value_.object_->find(key) != value_.object_->end();
}

Value& Value::append(const Value& value) {
    if (type_ != arrayValue) {
        *this = Value(arrayValue);
    }
    value_.array_->push_back(value);
    return *this;
}

// Simple iterator implementation
Value::iterator Value::begin() {
    if (type_ == arrayValue) {
        return iterator(value_.array_->begin());
    } else if (type_ == objectValue) {
        return iterator(value_.object_->begin());
    }
    return iterator();
}

Value::iterator Value::end() {
    if (type_ == arrayValue) {
        return iterator(value_.array_->end());
    } else if (type_ == objectValue) {
        return iterator(value_.object_->end());
    }
    return iterator();
}

Value::const_iterator Value::begin() const {
    if (type_ == arrayValue) {
        return const_iterator(value_.array_->begin());
    } else if (type_ == objectValue) {
        return const_iterator(value_.object_->begin());
    }
    return const_iterator();
}

Value::const_iterator Value::end() const {
    if (type_ == arrayValue) {
        return const_iterator(value_.array_->end());
    } else if (type_ == objectValue) {
        return const_iterator(value_.object_->end());
    }
    return const_iterator();
}

// Stream operators
std::ostream& operator<<(std::ostream& out, const Value& value) {
    switch (value.type()) {
    case nullValue:
        out << "null";
        break;
    case booleanValue:
        out << (value.asBool() ? "true" : "false");
        break;
    case intValue:
        out << value.asInt();
        break;
    case uintValue:
        out << value.asUInt();
        break;
    case realValue:
        out << std::setprecision(17) << value.asDouble();
        break;
    case stringValue:
        out << "\"" << value.asString() << "\"";
        break;
    case arrayValue:
        out << "[";
        for (ArrayIndex i = 0; i < value.size(); ++i) {
            if (i > 0) out << ",";
            out << value[i];
        }
        out << "]";
        break;
    case objectValue:
        out << "{";
        bool first = true;
        for (auto it = value.begin(); it != value.end(); ++it) {
            if (!first) out << ",";
            out << "\"" << it.key().asString() << "\":" << *it;
            first = false;
        }
        out << "}";
        break;
    }
    return out;
}

std::istream& operator>>(std::istream& in, Value& value) {
    // Simple JSON parsing - just handle basic cases for now
    std::string line;
    std::getline(in, line);
    
    // This is a very basic parser - you might want to enhance it
    if (line == "null") {
        value = Value();
    } else if (line == "true") {
        value = Value(true);
    } else if (line == "false") {
        value = Value(false);
    } else if (line[0] == '"' && line[line.length()-1] == '"') {
        value = Value(line.substr(1, line.length()-2));
    } else {
        try {
            if (line.find('.') != std::string::npos) {
                value = Value(std::stod(line));
            } else {
                value = Value(std::stoi(line));
            }
        } catch (...) {
            value = Value(line);
        }
    }
    
    return in;
}

} // namespace Json
