/**
 * @file StreamParer.h
 * @brief Defines StreamParer, ParserStream and  StreamParserConext
 * @author David Shang
 * @version 1.00
 * $Id: $
 */

#include "XmlParser.h"
#include "StrPrint.h"

#define XML_PARSER_DEBUG      0
#if XML_PARSER_DEBUG
#define XMLP_DBG(x)      x
#else
#define XMLP_DBG(x)
#endif

namespace alt
{

void XmlParser::scanChar(char ch, std::string& name_scanned)
{
    if (ch=='&')
    {
        const char* pos = scan_buffer_.curPos();
        const char* cp = std::strchr(pos, ';');
        if (cp)
        {
            if (strEqual<4>(pos, "&amp"))
            {
                name_scanned.push_back('&');
                scan_buffer_.advance(5);
            }
            else if (strEqual<4>(pos, "&lt;"))
            {
                name_scanned.push_back('<');
                scan_buffer_.advance(4);
            }
            else if (strEqual<4>(pos, "&gt;"))
            {
                name_scanned.push_back('>');
                scan_buffer_.advance(4);
            }
            else if (strEqual<4>(pos+1, "apos"))
            {
                name_scanned.push_back('\'');
                scan_buffer_.advance(6);
            }
            else if (strEqual<4>(pos+1, "quot"))
            {
                name_scanned.push_back('"');
                scan_buffer_.advance(6);
            }
        }
        else
        {
            name_scanned.push_back('&');
            scan_buffer_.advance(6);  
        }
    }
    else
    {
        name_scanned.push_back(ch);
        scan_buffer_.advance();    
    }
}

char XmlParser::getText(char end_ch, std::string& text_scanned)
{
    char ch = curChar(text_scanned);
    while (ch && ch!=end_ch)
    {
        scanChar(ch, text_scanned);
        ch = curChar(text_scanned);
    }
    return ch;
}

bool XmlParser::parseClosingTag(PooledNamedNode* node)
{
	scan_buffer_.advance();
    std::string node_name;
    char ch = getSubstring('>', node_name, false);
    XMLP_DBG((std::cout << "node end " << node_name << std::endl));
    if (!ch)
    {
        context_.registerError("missing '>' in closing tag");
        return false;
    }

	if (node_name!=node->name())
    {
        context_.registerError("unmatched closing tag name");
        XMLP_DBG((std::cout << "node_name=" << node_name << " expect " << node->name() << std::endl));
        return false;
    }
    scan_buffer_.advance();
    return true;
}

inline bool isXMLNameStartChar(unsigned char ch)
{
    if ( (ch>='A' && ch<='Z') || (ch>='a' && ch<='z') || ch=='_' || ch>=128)
    {
        return true;
    }
    return false;
}

PooledNamedNode* XmlParser::createXmlNode(
    const std::string& name,
    PooledNamedNode* parent)
{
    XmlElement* element = static_cast<XmlElement*>(parent->myChild(name.c_str()));
    if (!element)
    {
        element = PooledTreeNode::create<XmlElement>(name, parent);
    }
    size_t node_index = element->childrenNum();
    StrPrinter<16> node_name;
    node_name << '_' << node_index;
    XmlNode* node = PooledTreeNode::create<XmlNode>(node_name.c_str(), element);
    element->nodes_.push_back(node);
    return node;
}

void XmlParser::setNodeText(
    std::string& text,
    PooledNamedNode* node)
{
    reinterpret_cast<XmlNode*>(node)->text_ = strTrimCpy(text);
}


PooledNamedNode* XmlParser::createXmlAttribute(
    const std::string& name,
    PooledNamedNode* parent,
    std::string& value)
{
    XmlAttribute* attr = PooledTreeNode::create<XmlAttribute>(name, parent);
    attr->value_.swap(value);
    return attr;
}

PooledNamedNode* XmlParser::parseOpeningTag(PooledNamedNode* parent, bool& closed)
{
    closed = false;

    // '<' is already scanned. The currect position is at the beginning of the node name
    std::string node_name;
    char ch = getSubstring('>', node_name, true /*stop at a whitesapce*/);
    if (!ch)
    {
        context_.registerError("expect XML tag '<'");
        return nullptr;
    }
    XMLP_DBG((std::cout << "node " << node_name << std::endl));

    const char* name_str = node_name.c_str();

    if (!isXMLNameStartChar(static_cast<unsigned char>(*name_str)))
    {
        context_.registerError("XML name cannot start with any number or punctuation character");
        return nullptr;
    }

    PooledNamedNode* node = createXmlNode(node_name, parent);

    // Here we either at '>' or a white space
     ch = skipWhiteSpace();

    // parse attributes, if any
	while (ch != '>')
    {
		if (ch == '/')
        {
            // Tag closes here
            closed = true;
            ch = scan_buffer_.nextChar();
            if (ch!='>')
            {
                context_.registerError("missing '>' in closing tag");
                return nullptr;
            }
            XMLP_DBG((std::cout << "   Tag close here" << std::endl));
            break; 
        }
		
        // parse an attribute
        std::string attribute_name;
        ch = getSubstring('=', attribute_name, false);
        XMLP_DBG((std::cout << "   attrubute name=" << attribute_name << std::endl));
        if (!ch)
        {
            context_.registerError("missing '=' in attribute");
            return nullptr;
        }
        attribute_name = strTrim(attribute_name);

        char quote = nextChar(true/*skip_white_space*/);
        if (quote!='"' && ch!='\'')
        {
            context_.registerError("missing an open quote for an attribute value");
            return nullptr;
        }
        scan_buffer_.advance();
        std::string attribute_value;
        ch = getText(quote, attribute_value);
        XMLP_DBG((std::cout << "   attrubute value=" << attribute_value << std::endl));
        if (ch!=quote)
        {
            context_.registerError("missing closing quote for an attribute value");
            return nullptr;
        }

        createXmlAttribute(attribute_name, node, attribute_value);

        ch = nextChar(true/*skip_white_space*/);

        if (!ch)
        {
            context_.registerError("missing right tag bracket '>'");
            return nullptr;
        }
	}
    scan_buffer_.advance();
    return node;
}

bool XmlParser::parseCData(std::string& node_text)
{
    // CData starts with <![CDATA[ and ends with ]]>
    char ch = curChar(node_text);
    while (ch)
    {
        if (ch==']')
        {
            const char* pos = scan_buffer_.curPos();
            if (strEqual<2>(pos+1, "]>"))
            {
                scan_buffer_.advance(3);
                return true;
            }
        }
        node_text.push_back(ch);
        ch = nextChar(node_text);
    }

    context_.registerError("missing \"]]>\" in CData section");
    return false;
}

bool XmlParser::parseComment()
{
    // CData starts with <![CDATA[ and ends with ]]>
    char ch = curChar();
    while (ch)
    {
        if (ch=='-')
        {
            const char* pos = scan_buffer_.curPos();
            if (strEqual<2>(pos+1, "->"))
            {
                scan_buffer_.advance(3);
                return true;
            }
        }
        ch = nextChar(false);
    }

    context_.registerError("missing \"-->\" for comment ending");
    return false;
}

XmlNode* XmlParser::parse ()
{
    XmlNode* root = PooledTreeNode::create<XmlNode>();
    parseNodes(root, nullptr);
    return root;
}

bool XmlParser::parseNodes (PooledNamedNode* node, std::string* node_text)
{
    while (true)
    {
        char ch;

        if (node_text)
        {
            ch = getText('<', *node_text);
        }
        else
        {
            ch = skipWhiteSpace();
        }

        if (!ch)
        {
            // reach to end
            return true;
        }

        if (ch!='<')
        {
            context_.registerError("expect XML open tag '<...>'");
            return false;
        }

        ch = nextChar(false/*ship_white_space*/);

        if (ch=='?')
        {
            // ignore xml decaltation, such as <?xml version="1.0" encoding="UTF-8"?>
            ch = skipToChar('>');
            if (ch!='>')
            {
                context_.registerError("missing right tag bracket '>' in XML decaltation");
                return false;
            }
            scan_buffer_.advance();
            //XMLP_DBG((std::cout << "End of XML Decl. ch=" << scan_buffer_.curChar() << std::endl));
            continue;
        }

    	if (ch=='!')
        {
            const char* pos = scan_buffer_.curPos();
            // CData starts with <![CDATA[
            if (strEqual<8>(pos, "![CDATA["))
            {
                if (!node_text)
                {
                    context_.registerError("get CDATA section outside an XML element");
                    return false;
                }
                scan_buffer_.advance(8);
                if (!parseCData(*node_text))
                {
                    return false;
                }
                //XMLP_DBG((std::cout << "End of XML CDATA. ch=" << scan_buffer_.curChar() << std::endl));
                continue;
            }

            if (strEqual<2>(pos+1, "--"))
            {
                scan_buffer_.advance(3);
                if (!parseComment())
                {
                    return false;
                }
                //XMLP_DBG((std::cout << "End of XML Comment. ch=" << scan_buffer_.curChar() << std::endl));
                continue;
            }

            context_.registerError("not a valid XML segment");
            return false;
        }

        if (ch=='/')
        {
            XMLP_DBG((std::cout << "Closing Tag node=" 
                      << static_cast<PooledNamedNode*>(node->TreeNode::parent())->name()  << std::endl));
            // closing tag
            if (!node_text)
            {
                context_.registerError("closing tag without macting opening tag");
                return false; 
            }

            if (!parseClosingTag(static_cast<PooledNamedNode*>(node->TreeNode::parent())))
            {
                return false;
            }
            XMLP_DBG((std::cout << "SetNodeText node=" 
                      << static_cast<PooledNamedNode*>(node->TreeNode::parent())->name()
                      << " text=" << *node_text << std::endl));
            setNodeText(*node_text, node);

            //XMLP_DBG((std::cout << "End of XML Node. ch=" << scan_buffer_.curChar() << std::endl));

            // end of this node
            return true;
        }

        bool closed;
        PooledNamedNode* new_node = parseOpeningTag(node, closed);

        if (!new_node)
        {
            context_.registerError("malformat in XML opening tag");
            return false;
        }

        if (closed)
        {
            //XMLP_DBG((std::cout << "Closed Node. ch=" << scan_buffer_.curChar() << std::endl));
            continue;
        }
        
        if (!parseNode(new_node))
        {
            return false;
        }
        //XMLP_DBG((std::cout << "End of parse Node. ch=" << scan_buffer_.curChar() << std::endl));
    }

    return true;
}

bool XmlParser::parseNode (PooledNamedNode* node)
{
    std::string node_text;
    if (!parseNodes (node, &node_text))
    {
        return false;
    }
    return true;
}

XmlNode* XmlParser::parseFile(const char* file_path)
{
    ParserStreamContext context;
    if (!context.pushFileStream (file_path))
    {
        return nullptr;
    }
    XmlParser parser(context);
    if (!context.pushParser (&parser))
    {
        return nullptr;
    }
    return parser.parse();
}

void XmlNode::print (std::ostream& sot) const
{
    static std::function<void(const PooledNamedNode*, const std::string& indent)> print_node =
    [&sot](const PooledNamedNode* xml_node, const std::string& indent)
    {
        if (xml_node->subCategory()==XML_ATTRIBUTE)
        {
            sot << xml_node->name() << '=' << static_cast<const XmlAttribute*>(xml_node)->value_;
        }
        else if (xml_node->subCategory()==XML_NODE)
        {
            const char* element_name = nullptr;
            if (xml_node->TreeNode::parent())
            {
                element_name =
                    static_cast<const PooledNamedNode*>(xml_node->TreeNode::parent())->name();
                sot << indent << '<' << element_name;
            }
            for (auto child: xml_node->children())
            {
                auto n = static_cast<const PooledNamedNode*>(child);
                if (n->subCategory()==XML_ATTRIBUTE)
                {
                    sot << ' ';
                    print_node (n, "");
                }
            }
            if (element_name) sot << '>';
            if (xml_node->childrenNum(0, XML_ELEMENT)>0)
            {
                sot << '\n';
                std::string new_indent = indent + "  ";
                for (auto child: xml_node->children())
                {
                    auto n = static_cast<const PooledNamedNode*>(child);
                    if (n->subCategory()==XML_ELEMENT)
                    {
                        print_node (n, new_indent);
                    }
                }
                if (!static_cast<const XmlNode*>(xml_node)->text_.empty())
                {
                    sot << new_indent << static_cast<const XmlNode*>(xml_node)->text_ << '\n';
                }
                if (element_name)
                {
                    sot << indent << "</" << element_name << ">\n";
                }
            }
            else
            {
                if (!static_cast<const XmlNode*>(xml_node)->text_.empty())
                {
                    sot << static_cast<const XmlNode*>(xml_node)->text_;
                }
                if (element_name)
                {
                    sot << "</" << element_name << ">\n";
                }
            } 
        }
        else if (xml_node->subCategory()==XML_ELEMENT)
        {
            for (auto child: xml_node->children())
            {
                auto n = static_cast<const PooledNamedNode*>(child);
                if (n->subCategory()==XML_NODE)
                {
                    print_node (n, indent);
                }
            }
        }
        else
        {
            for (auto child: xml_node->children())
            {
                auto n = static_cast<const PooledNamedNode*>(child);
                if (n->subCategory()==XML_ELEMENT)
                {
                    sot << '\n';
                    print_node (n, indent);
                }
            }
        }
    };

    print_node(this, "");
}

} // namespace alt
