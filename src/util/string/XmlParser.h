#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file XmlParser.h
 * @library alt_util
 * @brief Implements an XML parser to covert XML text stream into XML nodes
 * in a tree structure where XML values can be retrived.
 * @note XmlParser supports encoding UTF-8 only
 * TODO: needs to be polished when it becomes in use
 */

#include "StreamParser.h"
#include <util/storage/NamedTreeNode.h>

namespace alt
{
// Identifiers for XML node, returned by the node's subCategory
constexpr uint64_t XML_ELEMENT     = 1;
constexpr uint64_t XML_NODE        = 2;
constexpr uint64_t XML_ATTRIBUTE   = 3;

class XmlNode: public PooledNamedNode
{
  public:
    XmlNode(const std::string& name, PooledNamedNode* parent) :
        PooledNamedNode(name.c_str(), parent, false)
    {}
    XmlNode() :
        PooledNamedNode(nullptr, nullptr, true)
    {}
    uint64_t subCategory() const override { return XML_NODE; }

    void print (std::ostream& sot) const; 

  protected:
    friend class XmlParser;
    std::string text_;
};

class XmlElement: public PooledNamedNode
{
  public:
    XmlElement(const std::string& name, PooledNamedNode* parent) :
        PooledNamedNode(name.c_str(), parent, false)
    {}
    uint64_t subCategory() const override { return XML_ELEMENT; }
    const XmlNode* operator[] (size_t index) const
    {
        return index >= nodes_.size() ? nullptr : nodes_[index];
    }

  protected:
    friend class XmlParser;
    friend class XmlNode;
    std::vector<XmlNode*>    nodes_;
};

class XmlAttribute: public PooledNamedNode
{
    public:
    XmlAttribute(const std::string& name, PooledNamedNode* parent) :
        PooledNamedNode(name.c_str(), parent, false)
    {}
    uint64_t subCategory() const override { return XML_ATTRIBUTE; }

  protected:
    friend class XmlParser;
    friend class XmlNode;
    std::string value_;
};

/**
 * \class XmlParser
 * \ingroup StringUtils
 * \usage XmlParser::parseFile(file_path, root), where root can be any class derived from
 * PooledNamedNode.
 */
class XmlParser: public StreamParser
{
  public:
    using StreamParser::StreamParser;

    XmlNode* parse ();
    static XmlNode* parseFile(const char* file_path);

  protected:

    virtual PooledNamedNode* createXmlNode(
        const std::string& name,
        PooledNamedNode* parent);

    virtual void setNodeText(
        std::string& text,
        PooledNamedNode* node);

    virtual PooledNamedNode* createXmlAttribute(
        const std::string& name,
        PooledNamedNode* parent,
        std::string& value);

    void scanChar(char ch, std::string& text_scanned);
    char getText(char end_ch, std::string& text_scanned);
    bool parseClosingTag(PooledNamedNode* node);
    bool parseCData(std::string& node_text);
    bool parseComment();
    void ignoreDeclaration();
    PooledNamedNode* parseOpeningTag(PooledNamedNode* parent, bool& closed);
    bool parseNode (PooledNamedNode* parent);
    bool parseNodes (PooledNamedNode* parent, std::string* node_text);
};

} // namespace alt
