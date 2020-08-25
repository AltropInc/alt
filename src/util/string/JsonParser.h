#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file JsonParser.h
 * @library alt_util
 * @brief Implements a Json parser to covert Json text stream into json nodes
 * in a tree structure where json values can be retrived. Json node tree can
 * also be constructed buy add new values and nodes and print back to Json
 * text stream.
 */

#include "StreamParser.h"
#include <util/storage/NamedTreeNode.h>
#include <variant>

namespace alt
{

class JsonObject;
class JsonArray;

using JsonValue = std::variant<
    bool,
    int64_t,
    double,
    const char*,
    const JsonObject*,
    const JsonArray*>;

// Identifiers for json node, returned by the node's subCategory
constexpr uint64_t JSON_NODE_OBJECT  = 1;
constexpr uint64_t JSON_NODE_ARRAY   = 2;
constexpr uint64_t JSON_NODE_STRING  = 3;
constexpr uint64_t JSON_NODE_INTEGER = 4;
constexpr uint64_t JSON_NODE_DOUBLE  = 5;
constexpr uint64_t JSON_NODE_BOOL    = 6;
/**
 * 
 * \class JsonObject
 * \ingroup StringUtils
 * \brief A Json object node in a tree structure.
 * \note The node is created in fixed pool and can only be released by
 * PooledTreeNode::releaseNode. Do not use delete!
 */
class JsonArray;
class JsonObject: public PooledNamedNode
{
  public:

    /// \brief constructs a jason object node
    JsonObject(const std::string& name, PooledNamedNode* parent) :
        PooledNamedNode(name.c_str(), parent, false)
    {}

    /// \brief constructs a jason root object node
    JsonObject() :
        PooledNamedNode(nullptr, nullptr, true)
    {}
    
    /// \brief overrides PooledNamedNode::subCategory for identifier of this node
    uint64_t subCategory() const override;

    /// \brief gets the string constained in this node
    /// \param name the name of the value
    /// \return the string value
    std::string getStringValue(
        const char* name,
        const std::string& default_value=std::string()) const;

    /// \brief gets the integer constained in this node
    /// \param name the name of the value
    /// \return the value
    int64_t getIntegerValue(
        const char* name,
        int64_t default_value = 0) const;

    /// \brief gets the floating point number constained in this node
    /// \param name the name of the value
    /// \return the value
    double getDoubleValue(
        const char* name,
        double default_value = 0) const;

    /// \brief gets the boolean value constained in this node
    /// \param name the name of the value
    /// \return the boolean value
    bool getBoolValue(
        const char* name,
        bool default_value = false) const;

    /// \brief gets child object constained in this node
    /// \param name the name of the child object
    /// \return pointer to the object node
    const JsonObject * getChildObject(
        const char* name,
        const JsonObject * default_value = nullptr) const;

    /// \brief gets the array in this node
    /// \param name the name of the value
    /// \return the array of json values
    const std::vector<JsonValue> getArray(const char* name) const;

    /// \brief gets the string elements of an array constained in this node
    /// \param name the name of the value
    /// \return the array of string
    std::vector<const char*> getStringArray(const char* name) const;
    void getStringArray(const char* name, std::vector<std::string>& string_vec) const;

    /// \brief gets the integer elements of an array constained in this node
    /// \param name the name of the value
    /// \return the array of integer
    std::vector<int64_t> getIntegerArray(const char* name) const;

    /// \brief gets the double elements of an array constained in this node
    /// \param name the name of the value
    /// \return the array of double
    std::vector<double> getDoubleArray(const char* name) const;

    /// \brief gets the boolean elements of an array constained in this node
    /// \param name the name of the value
    /// \return the array of boolen
    std::vector<bool> getBoolArray(const char* name) const;

    /// \brief gets the object elements of an array constained in this node
    /// \param name the name of the value
    /// \return the array of object
    std::vector<const JsonObject*> getObjectArray(const char* name) const;

    /// \brief prints the Json node tree rooted by this node into Json text
    void print (std::ostream& sot) const ;

    /// \brief creates Json value as a child node
    /// \param name the name of the value
    /// \param value the value. The value must be in simple type
    /// Use newChildObject to create a child object and  newArray to create an empty array
    /// \return true if the value is created, otherwise false
    bool newValue(const char* name, JsonValue value);

    /// \brief creates a Json child object
    /// \param name the name of the value
    /// \return the node of the object created
    JsonObject* newChildObject(const char* name);

    /// \brief creates an empty Json array
    /// \param name the name of the array
    /// \return the node of the array created
    JsonArray* newArray(const char* name);

    /// \brief creates a Json array
    /// \param name the name of the array
    /// \param strs list of the values
    /// \return true if the array is created, otherwise false
    bool newArray(const char* name, const std::vector<std::string>& strs);
    bool newArray(const char* name, const std::vector<int64_t>& strs);
    bool newArray(const char* name, const std::vector<double>& strs);
    bool newArray(const char* name, const std::vector<bool>& strs);

  private:

    // This is used internally by getObjectArray(name)
    const JsonObject* getValue() const { return this; }

    template<typename JNodeType, typename ValueType>
    bool newArrayT(const char* name, const std::vector<ValueType>& strs);

};

class JsonArray: public JsonObject
{
  public:
    JsonArray(const std::string& name, PooledNamedNode* parent) :
        JsonObject(name.c_str(), parent)
    {}
    JsonArray(const std::string& name, PooledNamedNode* parent, std::vector<JsonValue>& val_vec);
    uint64_t subCategory() const override { return JSON_NODE_ARRAY; }
    void getValue(std::vector<JsonValue>& res) const;
  protected:
    friend class JsonParser;
    friend class JsonObject;
    std::vector<const PooledNamedNode*>   value_;
};

class JsonString: public PooledNamedNode
{
  public:
    JsonString(const std::string& name, PooledNamedNode* parent) :
        PooledNamedNode(name.c_str(), parent, false)
    {}
    JsonString(const std::string& name, PooledNamedNode* parent, const char* val) :
        PooledNamedNode(name.c_str(), parent, false), value_(val)
    {}
    JsonString(const std::string& name, PooledNamedNode* parent, const std::string& val) :
        PooledNamedNode(name.c_str(), parent, false), value_(val)
    {}
    uint64_t subCategory() const override { return JSON_NODE_STRING; }
    const char* getValue() const { return value_.c_str(); };
  protected:
    friend class JsonParser;
    std::string value_;
};

class JsonBool: public PooledNamedNode
{
    public:
    JsonBool(const std::string& name, PooledNamedNode* parent, bool value) :
        PooledNamedNode(name.c_str(), parent, false), value_(value)
    {}
    uint64_t subCategory() const override { return JSON_NODE_BOOL; }
    bool getValue() const { return value_; };
  protected:
    friend class JsonParser;
    bool      value_ {0};
};

class JsonInteger: public PooledNamedNode
{
    public:
    JsonInteger(const std::string& name, PooledNamedNode* parent) :
        PooledNamedNode(name.c_str(), parent, false)
    {}
    JsonInteger(const std::string& name, PooledNamedNode* parent, uint64_t val) :
        PooledNamedNode(name.c_str(), parent, false), value_(val)
    {}
    uint64_t subCategory() const override { return JSON_NODE_INTEGER; }
    int64_t getValue() const { return value_; };
  protected:
    friend class JsonParser;
    int64_t      value_ {0};
};

class JsonDouble: public PooledNamedNode
{
    public:
    JsonDouble(const std::string& name, PooledNamedNode* parent) :
        PooledNamedNode(name.c_str(), parent, false)
    {}
    JsonDouble(const std::string& name, PooledNamedNode* parent, double val) :
        PooledNamedNode(name.c_str(), parent, false), value_(val)
    {}
    uint64_t subCategory() const override { return JSON_NODE_DOUBLE; }
    double getValue() const { return value_; };
  protected:
    friend class JsonParser;
    double      value_ {0};
};

/**
 * \class JsonParser
 * \ingroup StringUtils
 * \brief A Json string/file parser to convert Json text into a tree structure
 */ 
class JsonParser: public StreamParser
{
  public:
    using StreamParser::StreamParser;

    /// \brief parse the json text stream. A text stream must be set before
    /// calling this function. Text stream can be set i constructor. See StreamParser
    JsonObject* parse ();

    /// \brief parse the json file. If there is a text stream already set, this will
    /// push the existing stream into a stack and make this file stream for the current
    /// stream if the file stream is created successfully.
    static JsonObject* parseFile(const char* file_path);

  private:

    enum class JsonToken : uint16_t
    {
        Unknown,
        Colon,      // :
        LCurlyBr,   // {
        RCurlyBr,   // }
        LBracket,   // [
        RBracket,   // ]
        Comma,      // ,
        Value_String,
        Value_Integer,
        Value_Double,
        Value_Name,   // true, false, or null
        Done
    };
    
    JsonToken getToken ();
    bool parseObject (PooledNamedNode* parent);
    bool parseArray (JsonArray* array);
    PooledNamedNode* parseValue(const std::string& value_name, PooledNamedNode* parent);

    JsonToken     tk_ {JsonToken::Unknown};
};

} // namespace alt
