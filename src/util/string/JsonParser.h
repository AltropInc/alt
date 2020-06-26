/**
 * @file JsonParser.h
 * @brief Json Parser
 * @author David Shang
 * @version 1.00
 * $Id: $
 */

#pragma once

#include "StreamParser.h"
#include <util/storage/NamedTreeNode.h>

namespace alt
{

enum JsonNodeType
{
    JSON_NODE_OBJECT=1,
    JSON_NODE_ARRAY,
    JSON_NODE_STRING,
    JSON_NODE_INTEGER,
    JSON_NODE_DOUBLE,
    JSON_NODE_BOOL,
};

class JsonObject: public NamedNode
{
    public:
    JsonObject(const std::string& name, NamedNode* parent) :
        NamedNode(name.c_str(), parent, false)
    {}
    uint64_t subCategory() const override { return JSON_NODE_OBJECT; }
};

class JsonArray: public NamedNode
{
    public:
    JsonArray(const std::string& name, NamedNode* parent) :
        NamedNode(name.c_str(), parent, false)
    {}
    uint64_t subCategory() const override { return JSON_NODE_ARRAY; }
    std::vector<const NamedNode*>   value_;
};

class JsonString: public NamedNode
{
    public:
    JsonString(const std::string& name, NamedNode* parent) :
        NamedNode(name.c_str(), parent, false)
    {}
    uint64_t subCategory() const override { return JSON_NODE_STRING; }
    std::string value_;
};

class JsonBool: public NamedNode
{
    public:
    JsonBool(const std::string& name, NamedNode* parent, bool value) :
        NamedNode(name.c_str(), parent, false), value_(value)
    {}
    uint64_t subCategory() const override { return JSON_NODE_BOOL; }
    bool      value_ {0};
};

class JsonInteger: public NamedNode
{
    public:
    JsonInteger(const std::string& name, NamedNode* parent) :
        NamedNode(name.c_str(), parent, false)
    {}
    uint64_t subCategory() const override { return JSON_NODE_INTEGER; }
    int64_t      value_ {0};
};

class JsonDouble: public NamedNode
{
    public:
    JsonDouble(const std::string& name, NamedNode* parent) :
        NamedNode(name.c_str(), parent, false)
    {}
    uint64_t subCategory() const override { return JSON_NODE_DOUBLE; }
    double      value_ {0};
};

class JsonParser: public StreamParser
{
  public:
    using StreamParser::StreamParser;

    bool parse (NamedNode* json_root);

    static bool parseFile(const char* file_path, NamedNode* json_root);

    static void print (std::ostream& sot, NamedNode* json_node);

  protected:

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
    bool parseObject (NamedNode* parent);
    bool parseArray (JsonArray* array);
    NamedNode* parseValue(const std::string& value_name, NamedNode* parent);

    static void print (
        std::ostream& sot,
        NamedNode* json_root,
        const std::string& indent,
        bool from_array);

    JsonToken     tk_ {JsonToken::Unknown};
};

inline const std::string& getJsonAttrString(
    const NamedNode& json_node,
    const char* attr_name,
    const std::string default_value=std::string())
{
    auto attr = json_node.myChild(attr_name);
    return attr && attr->subCategory()==JSON_NODE_STRING
        ? static_cast<const JsonString*>(attr)->value_ : default_value;
}

inline int64_t getJsonAttrInteger(
    const NamedNode& json_node,
    const char* attr_name,
    int64_t default_value = 0
    )
{
    auto attr = json_node.myChild(attr_name);
    return attr && attr->subCategory()==JSON_NODE_INTEGER
        ? static_cast<const JsonInteger*>(attr)->value_ : default_value;
}

inline double getJsonAttrDouble(
    const NamedNode& json_node,
    const char* attr_name,
    double default_value = 0
    )
{
    auto attr = json_node.myChild(attr_name);
    return attr && attr->subCategory()==JSON_NODE_DOUBLE
        ? static_cast<const JsonDouble*>(attr)->value_ : default_value;
}

inline bool getJsonAttrBool(
    const NamedNode& json_node,
    const char* attr_name,
    bool default_value = false
    )
{
    auto attr = json_node.myChild(attr_name);
    return attr && attr->subCategory()==JSON_NODE_BOOL
        ? static_cast<const JsonBool*>(attr)->value_ : default_value;
}

inline const NamedNode * getJsonAttrObject(
    const NamedNode& json_node,
    const char* attr_name,
    const NamedNode * default_value = nullptr
    )
{
    auto attr = json_node.myChild(attr_name);
    return attr && attr->subCategory()==JSON_NODE_OBJECT
        ? static_cast<const JsonObject*>(attr) : default_value;
}

inline const std::vector<const NamedNode*>& getJsonAttrArray(
    const NamedNode& json_node,
    const char* attr_name
    )
{
    static std::vector<const NamedNode*> s_empty_vec_;
    auto attr = json_node.myChild(attr_name);
    return attr && attr->subCategory()==JSON_NODE_ARRAY ?
        static_cast<const JsonArray*>(attr)->value_ : s_empty_vec_;
}

inline void getJsonAttrStringArray(
    const NamedNode& json_node,
    const char* attr_name,
    std::vector<std::string> & string_vec
    )
{
    auto array = getJsonAttrArray(json_node, attr_name);
    for (auto ele: array)
    {
        if (ele->subCategory()==JSON_NODE_STRING)
        {
            string_vec.push_back(static_cast<const JsonString*>(ele)->value_);
        }
    }
}

} // namespace alt
