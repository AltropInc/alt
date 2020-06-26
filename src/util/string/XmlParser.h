/**
 * @file XmlParser.h
 * @brief Xml Parser
 * @author David Shang
 * @version 1.00
 * $Id: $
 */

#pragma once

#include "StreamParser.h"
#include <util/storage/NamedTreeNode.h>

namespace alt
{

enum XmlNodeType
{
    XML_ELEMENT =1,
    XML_NODE,
    XML_ATTRIBUTE,
};

class XmlNode: public NamedNode
{
    public:
    XmlNode(const std::string& name, NamedNode* parent) :
        NamedNode(name.c_str(), parent, false)
    {}
    uint64_t subCategory() const override { return XML_NODE; }
    std::string text_;
};

class XmlElement: public NamedNode
{
    public:
    XmlElement(const std::string& name, NamedNode* parent) :
        NamedNode(name.c_str(), parent, false)
    {}
    uint64_t subCategory() const override { return XML_ELEMENT; }
    const XmlNode* operator[] (size_t index) const
    {
        return index >= nodes_.size() ? nullptr : nodes_[index];
    }
    std::vector<XmlNode*>    nodes_;
};

class XmlAttribute: public NamedNode
{
    public:
    XmlAttribute(const std::string& name, NamedNode* parent) :
        NamedNode(name.c_str(), parent, false)
    {}
    uint64_t subCategory() const override { return XML_ATTRIBUTE; }
    std::string value_;
};

/**
 * \class XmlParser
 * \ingroup StringUtils
 * \usage XmlParser::parseFile(filr_path, root), where root can be any class derived from
 * NamedNode.
 * \note XmlParser supports encoding UTF-8 only
 */
class XmlParser: public StreamParser
{
  public:
    using StreamParser::StreamParser;

    bool parse (NamedNode* node);
    static bool parseFile(const char* file_path, NamedNode* xml_root);
 
    static void print (
        std::ostream& sot,
        NamedNode* xml_root,
        const std::string& indent=std::string());

  protected:

    virtual NamedNode* createXmlNode(
        const std::string& name,
        NamedNode* parent);

    virtual void setNodeText(
        std::string& text,
        NamedNode* node);

    virtual NamedNode* createXmlAttribute(
        const std::string& name,
        NamedNode* parent,
        std::string& value);

    void scanChar(char ch, std::string& text_scanned);
    char getText(char end_ch, std::string& text_scanned);
    bool parseClosingTag(NamedNode* node);
    bool parseCData(std::string& node_text);
    bool parseComment();
    void ignoreDeclaration();
    NamedNode* parseOpeningTag(NamedNode* parent, bool& closed);
    bool parseNode (NamedNode* parent);
    bool parseNodes (NamedNode* parent, std::string* node_text);
};

} // namespace alt
