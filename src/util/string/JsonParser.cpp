/**
 * @file StreamParer.h
 * @brief Defines StreamParer, ParserStream and  StreamParserConext
 * @author David Shang
 * @version 1.00
 * $Id: $
 */

#include "JsonParser.h"
#include "StrPrint.h"

namespace alt
{

bool JsonParser::parse (NamedNode* parent)
{
    getToken();
    if (tk_ != JsonToken::LCurlyBr)
    {
        context_.registerError("missing left curly bracket bracket");
        return false;
    }

    return parseObject (parent);
}

bool JsonParser::parseObject (NamedNode* parent)
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
        std::string value_name;
        StrPrint spr(value_name);
        spr << index;

        NamedNode* value = parseValue(value_name, array);
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

NamedNode* JsonParser::parseValue(const std::string& value_name, NamedNode* parent)
{
    NamedNode* n {nullptr};
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

bool JsonParser::parseFile(const char* file_path, NamedNode* json_root)
{
    ParserStreamContext context;
    JsonParser parser(context);
    if (!context.pushFileParser (&parser, file_path))
    {
        return false;
    }

    return parser.parse(json_root);
}

void JsonParser::print (std::ostream& sot, NamedNode* json_node)
{
    print (sot, json_node, std::string(), false);
    sot << std::endl;
}

void JsonParser::print (std::ostream& sot, NamedNode* json_node,
     const std::string& indent, bool from_array)
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
            auto n = static_cast<NamedNode*>(child);
            print (sot, n, new_indent, true);
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
        sot << '"' << static_cast<JsonString*>(json_node)->value_ << '"';
    }
    else if (json_node->subCategory()==JSON_NODE_BOOL)
    {
        sot << static_cast<JsonBool*>(json_node)->value_;
    }
    else if (json_node->subCategory()==JSON_NODE_INTEGER)
    {
        sot << static_cast<JsonInteger*>(json_node)->value_;
    }
    else if (json_node->subCategory()==JSON_NODE_DOUBLE)
    {
        sot << static_cast<JsonDouble*>(json_node)->value_;
    }
    else // if (json_node->subCategory()==JSON_OBJECT)
    {
        sot << "{\n";
        std::string new_indent = indent + "  ";
        for (auto child: json_node->children())
        {
            auto n = static_cast<NamedNode*>(child);
            print (sot, n, new_indent, false);
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
}

} // namespace alt
