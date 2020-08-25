
//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

#include "JsonParser.h"
#include "StrPrint.h"
#include <util/types/TemplateHelper.h>    // for overloaded

namespace alt
{

JsonObject* JsonParser::parse()
{
    getToken();
    if (tk_ != JsonToken::LCurlyBr)
    {
        context_.registerError("missing left curly bracket bracket");
        return nullptr;
    }

    JsonObject* root = PooledTreeNode::create<JsonObject>();
    parseObject (root);
    return root;
}

bool JsonParser::parseObject (PooledNamedNode* parent)
{
    getToken();
    if (tk_ == JsonToken::RCurlyBr)
    {
        return true;
    }
    while (true)
    {
        if ((tk_ != JsonToken::Value_String))
        {
            context_.registerError("expect a string");
            return false;
        }
        std::string value_name;
        fetchValue(value_name);

        getToken();
        if ((tk_ != JsonToken::Colon))
        {
            context_.registerError("expect colon");
            return false;
        }

        getToken();
        if (!parseValue(value_name, parent))
        {
            return false;
        }

        getToken();
        if (tk_ == JsonToken::RCurlyBr)
        {
            return true;
        }

        if (tk_ != JsonToken::Comma)
        {
            context_.registerError("expect comma or right curly bracket");
            return false;
        }
        getToken();
    }

    return true;
}

bool JsonParser::parseArray (JsonArray* array)
{
    getToken();
    if (tk_ == JsonToken::RBracket)
    {
        return true;
    }
    size_t index = 0;
    while (true)
    {
        // array children are anonymous
        PooledNamedNode* value = parseValue("", array);
        if (!value)
        {
            return false;
        }
        array->value_.push_back(value);
        getToken();
        if (tk_ == JsonToken::RBracket)
        {
            break;
        }
        if (tk_ != JsonToken::Comma)
        {
            context_.registerError("expect comma or right bracket");
            return false;
        }
        getToken();
    }

    return true;
}

PooledNamedNode* JsonParser::parseValue(const std::string& value_name, PooledNamedNode* parent)
{
    PooledNamedNode* n {nullptr};
    if ((tk_ == JsonToken::LBracket))
    {
        JsonArray* node = PooledTreeNode::create<JsonArray>(value_name, parent);
        if (!parseArray (node))
        {
            return nullptr;
        }
        n = node;
    }
    else if ((tk_ == JsonToken::LCurlyBr))
    {
        JsonObject* node = PooledTreeNode::create<JsonObject>(value_name, parent);
        if (!parseObject (node))
        {
            return nullptr;
        }
        n = node;
    }
    else if ((tk_ == JsonToken::Value_String))
    {
        JsonString* node = PooledTreeNode::create<JsonString>(value_name, parent);
        fetchValue(node->value_);
        n = node;
    }
    else if ((tk_ == JsonToken::Value_Double))
    {
        JsonDouble* node = PooledTreeNode::create<JsonDouble>(value_name, parent);
        fetchValue(node->value_);
        n = node;
    }
    else if ((tk_ == JsonToken::Value_Integer))
    {
        JsonInteger* node = PooledTreeNode::create<JsonInteger>(value_name, parent);
        fetchValue(node->value_);
        n = node;
    }
    else if ((tk_ == JsonToken::Value_Name))
    {
        if (scanned<4>("true"))
        {
            n=PooledTreeNode::create<JsonBool>(value_name, parent, true);
        }
        else if (scanned<5>("false"))
        {
            n=PooledTreeNode::create<JsonBool>(value_name, parent, false);
        }
        else if (scanned<4>("null"))
        {
            // null value, ignore
        }
    }
    else
    {
        context_.registerError("Unknow json element");
        return nullptr;
    }

    return n;
}

JsonParser::JsonToken JsonParser::getToken ()
{
    tv_.vt_ = NONE;

    // std::cout << "getToken atEnd=" << int(scan_buffer_.atEnd())
    //           << " length=" << scan_buffer_.length()
    //           << " pos=" << scan_buffer_.pos_ << std::endl;

    char ch = skipWhiteSpace();

    if (!ch)
    {
        tk_ = JsonToken::Done;
        return tk_;
    }

    tk_ = JsonToken::Unknown;

    //std::cout << "STARTPOS: " << tv_.start_pos_ << std::endl;
    switch (ch)
    {
        case '\0':
            tk_ = JsonToken::Done;
            break;

        case '0': case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': case '-':
            StrScan::getNumber();
            tk_ = tv_.vt_ == DOUBLE ? JsonToken::Value_Double : JsonToken::Value_Integer;
            break;

        case '\"':
            // Note: string scanned will be converted into UTF-8 format. The string format
            // used by StrScan::getString() is compatible to Json string, but is a super
            // set. It handles more control characters, especially, \U for unicode that 
            // requires more than 4 bytes.
            StrScan::getString();
            tk_ = JsonToken::Value_String;
            break;

        case '{': tk_= JsonToken::LCurlyBr; scan_buffer_.advance(); break;
        case '}': tk_= JsonToken::RCurlyBr; scan_buffer_.advance(); break;
        case '[': tk_= JsonToken::LBracket; scan_buffer_.advance(); break;
        case ']': tk_= JsonToken::RBracket; scan_buffer_.advance(); break;
        case ',': tk_= JsonToken::Comma; scan_buffer_.advance(); break;
        case ':': tk_= JsonToken::Colon; scan_buffer_.advance(); break;
        case 't':
        case 'f':
        case 'n':
        {
            scan_buffer_.advance();
            ch = scan_buffer_.curChar();
            while ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'))
            {
                ch = scan_buffer_.nextChar();
            }
            tk_ = JsonToken::Value_Name;
        }

        default:
        {
            tk_ = JsonToken::Unknown;
            scan_buffer_.advance();
        }
    };
    return tk_;
}

JsonObject* JsonParser::parseFile(const char* file_path)
{
    ParserStreamContext context;
    JsonParser parser(context);
    if (!context.pushFileParser (&parser, file_path))
    {
        return nullptr;
    }

    return parser.parse();
}


JsonArray::JsonArray(const std::string& name, PooledNamedNode* parent, std::vector<JsonValue>& val_vec)
    :  JsonObject(name.c_str(), parent)
{
    for (auto val: val_vec)
    {
        PooledNamedNode* child {nullptr};
        std::visit(overloaded
        {
            [](auto arg) { },
            [this, &child](double arg) { child = PooledTreeNode::create<JsonDouble>(nullptr, this, arg); },
            [this, &child](int64_t arg) { child = PooledTreeNode::create<JsonInteger>(nullptr, this, arg); },
            [this, &child](bool arg) { child = PooledTreeNode::create<JsonBool>(nullptr, this, arg); },
            [this, &child](const char* arg) { child = PooledTreeNode::create<JsonString>(nullptr, this, arg); },
            [this, &child](const JsonArray* arg)
            { child = const_cast<JsonArray*>(arg); child->rename(nullptr); child->reparent(this); },
            [this, &child](const JsonObject* arg)
            { child = const_cast<JsonObject*>(arg); child->rename(nullptr); child->reparent(this); }
        }, val);
        if (child)
        {
            value_.push_back(child);
        }
    }
}

bool JsonObject::newValue(const char* name, JsonValue value)
{
    bool res = true;
    std::visit(overloaded
    {
        [&res](auto arg) { res=false; },
        [&name,this](double arg) { PooledTreeNode::create<JsonDouble>(name, this, arg); },
        [&name,this](int64_t arg) { PooledTreeNode::create<JsonInteger>(name, this, arg); },
        [&name,this](bool arg) { PooledTreeNode::create<JsonBool>(name, this, arg); },
        [&name,this](const char* arg) { PooledTreeNode::create<JsonString>(name, this, arg); },
    }, value);
    return res;
}

template<typename JNodeType, typename ValueType>
bool JsonObject::newArrayT(const char* name, const std::vector<ValueType>& vec)
{
    JsonArray * arr = PooledTreeNode::create<JsonArray>(name, this);
    for (const auto& val: vec)
    {
        arr->PooledTreeNode::create<JNodeType>(name, arr, val); 
    }
    return true;  
}

bool JsonObject::newArray(const char* name, const std::vector<std::string>& vec)
{
    return newArrayT<JsonString, std::string>(name, vec);
}

bool JsonObject::newArray(const char* name, const std::vector<int64_t>& vec)
{
    return newArrayT<JsonInteger, int64_t>(name, vec);
}

bool JsonObject::newArray(const char* name, const std::vector<double>& vec)
{
    return newArrayT<JsonDouble, double>(name, vec);
}

bool JsonObject::newArray(const char* name, const std::vector<bool>& vec)
{
    return newArrayT<JsonBool,bool>(name, vec);
}

JsonObject* JsonObject::newChildObject(const char* name)
{
    return PooledTreeNode::create<JsonObject>(name, this);
}

JsonArray* JsonObject::newArray(const char* name)
{
    return PooledTreeNode::create<JsonArray>(name, this);
}

void JsonObject::print (std::ostream& sot) const
{
    // note: a lambda function cannot be recursively called if it is local or uses auto
    // So here type is explicitly defined and static storage is used
    static std::function<void(const PooledNamedNode*, const std::string& indent, bool)> print_node =
    [&sot](const PooledNamedNode* json_node, const std::string& indent, bool from_array)
    {
        if (!from_array)
        {
            sot << indent;
            const char* name = json_node->name();
            if (name)
            {
                sot << '"' << name << "\": ";
            }
        }
        if (json_node->subCategory()==JSON_NODE_ARRAY)
        {
            sot << '[';
            std::string new_indent = indent + "  ";
            for (auto child: json_node->children())
            {
                auto n = static_cast<const PooledNamedNode*>(child);
                print_node (n, new_indent, true);
                if (child == json_node->lastChild())
                {
                    sot << ']';
                }
                else
                {
                    sot << ", ";
                }
            }
        }
        else if (json_node->subCategory()==JSON_NODE_STRING)
        {
            sot << '"' << static_cast<const JsonString*>(json_node)->getValue() << '"';
        }
        else if (json_node->subCategory()==JSON_NODE_BOOL)
        {
            sot << static_cast<const JsonBool*>(json_node)->getValue();
        }
        else if (json_node->subCategory()==JSON_NODE_INTEGER)
        {
            sot << static_cast<const JsonInteger*>(json_node)->getValue();
        }
        else if (json_node->subCategory()==JSON_NODE_DOUBLE)
        {
            sot << static_cast<const JsonDouble*>(json_node)->getValue();
        }
        else // if (json_node->subCategory()==JSON_OBJECT)
        {
            sot << "{\n";
            std::string new_indent = indent + "  ";
            for (auto child: json_node->children())
            {
                auto n = static_cast<const PooledNamedNode*>(child);
                print_node (n, new_indent, false);
                if (child == json_node->lastChild())
                {
                    sot << '\n' << indent << '}';
                }
                else
                {
                    sot << ",\n";
                }
            }
        }
    };

    print_node (this, std::string(), false);
    sot << std::endl;
}

uint64_t JsonObject::subCategory() const
{
    return JSON_NODE_OBJECT;
}

std::string JsonObject::getStringValue(
    const char* name,
    const std::string& default_value) const
{
    auto attr = myChild(name);
    return attr && attr->subCategory()==JSON_NODE_STRING
        ? static_cast<const JsonString*>(attr)->getValue() : default_value;
}

int64_t JsonObject::getIntegerValue(
    const char* attr_name,
    int64_t default_value
    ) const
{
    auto attr = myChild(attr_name);
    return attr && attr->subCategory()==JSON_NODE_INTEGER
        ? static_cast<const JsonInteger*>(attr)->getValue() : default_value;
}

double JsonObject::getDoubleValue(
    const char* attr_name,
    double default_value
    ) const
{
    auto attr = myChild(attr_name);
    return attr && attr->subCategory()==JSON_NODE_DOUBLE
        ? static_cast<const JsonDouble*>(attr)->getValue() : default_value;
}

bool JsonObject::getBoolValue(
    const char* attr_name,
    bool default_value
    ) const
{
    auto attr = myChild(attr_name);
    return attr && attr->subCategory()==JSON_NODE_BOOL
        ? static_cast<const JsonBool*>(attr)->getValue() : default_value;
}

const JsonObject * JsonObject::getChildObject(
    const char* attr_name,
    const JsonObject * default_value
    ) const
{
    auto attr = myChild(attr_name);
    return attr && attr->subCategory()==JSON_NODE_OBJECT
        ? static_cast<const JsonObject*>(attr) : default_value;
}

const std::vector<JsonValue> JsonObject::getArray(
    const char* name
    ) const
{
    std::vector<JsonValue> res;
    auto node = myChild(name);
    if (node && node->subCategory()==JSON_NODE_ARRAY)
    {
        static_cast<const JsonArray*>(node)->getValue(res);
    }
    return res;
}

void JsonArray::getValue(std::vector<JsonValue>& res) const
{
    for (const auto ele: value_)
    {
        switch (ele->subCategory())
        {
            case JSON_NODE_BOOL:
                res.emplace_back(static_cast<const JsonBool*>(ele)->getValue());
                break;
            case JSON_NODE_STRING:
                res.emplace_back(static_cast<const JsonString*>(ele)->getValue());
                break;
            case JSON_NODE_INTEGER:
                res.emplace_back(static_cast<const JsonInteger*>(ele)->getValue());
                break;
            case JSON_NODE_DOUBLE:
                res.emplace_back(static_cast<const JsonDouble*>(ele)->getValue());
                break;
            case JSON_NODE_OBJECT:
                res.emplace_back(static_cast<const JsonObject*>(ele));
                break;
            case JSON_NODE_ARRAY:
                res.emplace_back(static_cast<const JsonArray*>(ele));
                break;
        }
    }
}

#define GET_ARRAY_OF_T(type, Type, TYPE) \
std::vector<type> JsonObject::get##Type##Array(const char* name) const \
{ \
    std::vector<type> res; \
    auto node = myChild(name); \
    if (node && node->subCategory()==JSON_NODE_ARRAY) \
    { \
        for (auto ele: static_cast<const JsonArray*>(node)->value_) \
        {\
            if (ele->subCategory()==JSON_NODE_##TYPE) \
            { \
                res.push_back(static_cast<const Json##Type*>(ele)->getValue()); \
            } \
        } \
    } \
    return res; \
} \

GET_ARRAY_OF_T(int64_t, Integer, INTEGER)
GET_ARRAY_OF_T(double, Double, DOUBLE)
GET_ARRAY_OF_T(bool, Bool, BOOL)
GET_ARRAY_OF_T(const char*, String, STRING)
GET_ARRAY_OF_T(const JsonObject*, Object, OBJECT)

void JsonObject::getStringArray(const char* name, std::vector<std::string>& string_vec) const
{
    std::vector<const char*> str_vec = getStringArray(name);
    for (auto str: str_vec)
    {
        string_vec.emplace_back(str);
    }
}

#undef GET_ARRAY_OF_T

} // namespace alt
