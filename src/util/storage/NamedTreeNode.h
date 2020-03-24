#pragma once

#include "TreeNode.h"
#include "StringHashMap.h"
#include "util/string/StrPrint.h"

namespace alt
{

template <class Alloc = Allocator>
class ALT_CORE_PUBLIC NamedTreeNode: public TreeNode<Alloc>
{
  public:
    typedef TreeNode<Alloc> base_t;
    typedef NamedTreeNode*  pointer_t;
    using NamedTreeNodeHash =
        typename std::conditional<
            std::is_same<Alloc, PooledAllocator>::value,
            StringHashMap<pointer_t,
                          StdFixedPoolAllocator<std::pair<const StrRef, pointer_t>>>,
            StringHashMap<pointer_t,
                          std::allocator<std::pair<const StrRef, pointer_t>>>
        >::type;

    static constexpr size_t MAX_NAME_LENGTH = 128;
    static constexpr size_t ID_LENGTH       = 6;

    struct NameID
    {
        char id_[ID_LENGTH];
        void setID (int32_t id)
        {
            id_[0] = char((id & 0x007f) | 0x80); id>>=7;
            id_[1] = char((id & 0x007f) | 0x80); id>>=7;
            id_[2] = char((id & 0x007f) | 0x80); id>>=7;
            id_[3] = char((id & 0x007f) | 0x80); id>>=7;
            id_[4] = char((id & 0x007f) | 0x80); id>>=7;
            id_[5] = char((id & 0x007f) | 0x80);
        }
        const char* getID() const { return id_; }
    };

    struct NameRegistry
    {
        NamedTreeNodeHash    name_hash_table_;
        int32_t              offsprins_id_ {0};
        int32_t              dummy_id_ {0};
        int32_t generateId () { return offsprins_id_++; }
    };

    struct QualifiedName
    {
        QualifiedName(const NameID& id, const char* name)
        {
            qname_ << std::make_tuple(id.getID(), ID_LENGTH)
                   << name << '\0';
        }
        const char* c_str() const { return qname_.c_str(); }
        StrPrintWithBuffer<MAX_NAME_LENGTH+sizeof(NameID)> qname_;
    };

    NamedTreeNodeHash& nameMap()
    {
        return name_register_->name_registry_->name_hash_table_;
    }

    NamedTreeNodeHash& childNameMap()
    {
        return name_registry_ ? name_registry_->name_hash_table_
                              : name_register_->name_registry_->name_hash_table_;
    }

    const char* name() const { return name_; }
    bool rename (const char * new_name);
    NamedTreeNode* searchDown (const char * name);
    NamedTreeNode* searchUp (const char * name);
    NamedTreeNode* search (const char * name);

    NamedTreeNode (const char *name, NamedTreeNode* parent, bool is_name_register);

  protected:
    NameID         id_;                      ///< my id
    const char*    name_ {nullptr};
    NameRegistry*  name_registry_ {nullptr}; ///< name registry for offsprings
    pointer_t      name_register_ {nullptr}; ///< to ancestor where my name is registered

    static constexpr char s_dummy_name_ = '\0';

    void registerName(const char *name, NamedTreeNode* node);

    NamedTreeNode* parent() { return reinterpret_cast<NamedTreeNode*>(base_t::parent_); }
    const NamedTreeNode* parent() const { return reinterpret_cast<const NamedTreeNode*>(base_t::parent_); }
};

template <class Alloc>
void NamedTreeNode<Alloc>::registerName(const char *name, NamedTreeNode* node)
{
    node->id_.setID(name_registry_->generateId());
    // root does not register its name. The registerd name is the node name 
    // prefixed with its parent id
    if (node->parent())
    {
        QualifiedName qname (node->parent()->id_, name);
        auto [iter, inserted] = name_registry_->name_hash_table_.emplace(qname.c_str(), node);
        node->name_ = iter->first.c_str() + ID_LENGTH;
    }
    else
    {
        node->name_ = name_registry_->name_hash_table_.getStringPool().insert(name);
    }
    
}

template <class Alloc>
NamedTreeNode<Alloc>::NamedTreeNode(const char *name, NamedTreeNode* parent, bool is_name_register)
    : TreeNode<Alloc>(parent)
{
    if (!parent || is_name_register)
    {
        name_registry_ = new NameRegistry();
    }

    if (parent)
    {
        if (parent->name_registry_)
        {
            name_register_ = parent;
        }
        else    
        {
            name_register_ = parent->name_register_;
        }
    }
    else
    {
        name_register_ = this;
    }

    name_register_->registerName(name, this);
};

template <class Alloc>
bool NamedTreeNode<Alloc>::rename (const char * new_name)
{
    // root cannot be renamed
    if (!base_t::parent_) return false;

    QualifiedName qname (parent()->id_, new_name);
    auto& name_map = nameMap();
    QualifiedName oldname (parent()->id_, name_);
    auto [renamed_iter, renamed] = name_map.rename(oldname.c_str(), qname.c_str());
    if (renamed)
    {
        name_ = renamed_iter->first.c_str() + ID_LENGTH;
    }
    return renamed;
}

template <class Alloc>
NamedTreeNode<Alloc>* NamedTreeNode<Alloc>::searchDown (const char * name)
{
    auto& name_map = childNameMap();
    auto iter = name_map.find(QualifiedName(id_,name).c_str());
    return iter==name_map.end() ? nullptr : iter->second;
}

template <class Alloc>
NamedTreeNode<Alloc>* NamedTreeNode<Alloc>::searchUp (const char * name)
{
    auto p = this;
	while (p)
	{
	    if (std::strcmp(name, p->name())==0)
        {
            return p;
        }
	    p = p->parent();
    }
}

template <class Alloc>
NamedTreeNode<Alloc>* NamedTreeNode<Alloc>::search (const char * name)
{
    auto node = searchDown(name);
	if (!node)
	{    
	    return searchUp(name);
    }
    return node;
}

using NamedNode = NamedTreeNode<PooledAllocator>;

}

