#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file NamedTreeNode.h
 * @library alt_util
 * @brief Defines named tree node. If a tree node has a name, its name is
 * registered in one of its ancestor's hash table. A child can be searched
 * through its parent by name. The node that holds the name hash table is
 * a name register. Other nodes are hanger-ons. The root must be a name register.
 * Therefore, a hanger-on node should normally not be detached from a tree unless
 * it is going to be deleted. However, a hanger-on node can still be detached
 * without unregistering its name in its name register, and in this case, the
 * hanger-on node still belongs to its original parent and it can be fostered by
 * another tree node and the names of the hanger-on and all its hander-on children
 * will be transferred to a new name register.
 */

#include <util/Defs.h>              // for ALT_UTIL_PUBLIC
#include "TreeNode.h"               // for TreeNode
#include "StringHashMap.h"          // for StringHashMap
#include "util/string/StrPrint.h"   // for StrPrinter
#include "util/string/StrUtils.h"   // for StrSplit

namespace alt
{

/**
 * \class NamedTreeNode
 * \brief  Defines named tree node
 * \tparam Alloc: type of allocator to determine how the tree node is allocated 
 */
template <class Alloc = Allocator>
class ALT_UTIL_PUBLIC NamedTreeNode: public TreeNode<Alloc>
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
        StrPrinter<MAX_NAME_LENGTH+sizeof(NameID)> qname_;
    };

    /// \brief A structure to hold multiple search result. Single search result is the
    /// typical case. Separate multiple search result from the single search result so
    /// we do not sacrifice the effciency of getting a single search result.
    template<typename ResultT>
    struct SearchResultT
    {
        struct iterator
        {
          private:
            SearchResultT*  parent_;
            size_t          ix_ {0};
 
          public:
            iterator(const iterator& oth):
                parent_(oth.parent_),
                ix_(oth.ix_)
                {}

            iterator(SearchResultT * p, size_t ix):
                parent_(p), ix_(ix) {}

            bool operator==(const iterator& oth)
            {
                return parent_==oth.parent_ && ix_==oth.ix_;
            }

            bool operator!=(const iterator& oth)
            {
                return !(*this==oth);
            }

            iterator& operator=(const iterator&oth)
            {
                parent_=oth.parent_;
                ix_=oth.ix_;
                return *this;
            }

            iterator& operator++()
            {
                if (ix_ <= parent_->found_list_.size()) ++ix_;
                return *this;
            }

            ResultT operator*() const
            {
                return ix_==0 ? parent_->getFirstFound()
                              : parent_->getRestFound(ix_-1);
            }
        };

        iterator begin() { return found_ ? iterator(this, 0) : iterator(this, 1); }
        iterator end() { return iterator(this, found_list_.size()+1); }

        bool notFound () const { return found_==nullptr; };
        bool found () const { return found_!=nullptr; };
        bool hasRestFound () const { return !found_list_.empty(); };

        ResultT getFirstFound() { return found_; }
        std::vector<ResultT>& getRestFound() { return found_list_; }
        ResultT getRestFound(size_t ix)
        { return ix<found_list_.size() ? found_list_[ix] : nullptr; }

        void addResult (ResultT res) { return addNode(res); }
        void addResultUnique (ResultT res)  { return addNodeUnique(res); }

        void combine (SearchResultT& other)
        {
            addNode(other.found_);
            auto& other_rest_found = other.getRestFound();
            for (auto found: other_rest_found) addNode(found);
        }

        void combineUnique (SearchResultT& other)
        {
            addNodeUnique(other.found_);
            auto& other_rest_found = other.getRestFound();
            for (auto found: other_rest_found) addNodeUnique(found);
        }

        void clear() { found_=nullptr; found_list_.clear(); };

      private:
        ResultT 			        found_ {nullptr};
        std::vector<ResultT> 	found_list_;

        void addNode (ResultT found)
        {
            if (found)
            {
                if (!found_) found_ = found;
                else found_list_.push_back(found);			
            }
        }

        void addNodeUnique (ResultT found)
        {
            if (!found || found==found_) return;
            if (!found_) found_ = found;
            else if (std::find(found_list_.begin(), found_list_.end(), found)==found_list_.end())
                found_list_.push_back(found);	
        }
    };

    using SearchResult = SearchResultT<NamedTreeNode*>;
    using ConstSearchResult = SearchResultT<const NamedTreeNode*>;

    NamedTreeNodeHash& nameMap()
    {
        return name_register_->name_registry_->name_hash_table_;
    }

    NamedTreeNodeHash const & nameMap() const
    {
        return name_register_->name_registry_->name_hash_table_;
    }

    NamedTreeNodeHash& childNameMap()
    {
        return name_registry_ ? name_registry_->name_hash_table_
                              : name_register_->name_registry_->name_hash_table_;
    }

    NamedTreeNodeHash const & childNameMap() const
    {
        return name_registry_ ? name_registry_->name_hash_table_
                              : name_register_->name_registry_->name_hash_table_;
    }

    const char* name() const { return name_; }
    bool isAnonymous() const { return !name_ || !*name_; }
    bool isNameRegister() const { return name_registry_!=nullptr; }
    NamedTreeNode* getNameRegister();

    virtual uint64_t category() const { return 0; }
    virtual uint64_t subCategory() const { return 0; }
    size_t childrenNum(uint64_t cat_filter, uint64_t sub_cat_filter) const;
    size_t childrenNum() const { return TreeNodeBase::childrenNum(); }

    virtual LinkedListBase* getExtraChildNodes() { return nullptr; }

    bool rename (const char * new_name);
    bool reparent(NamedTreeNode* new_parent);
    void detach();
    NamedTreeNode* extract ();
    void fosterRoot(const char* name, NamedTreeNode* node);

    void searchDown (const char * name, ConstSearchResult& res) const;
    void searchUp (const char * name, ConstSearchResult& res) const;
    void search (const char * name, ConstSearchResult& res) const;

    void searchDown (const char * name, SearchResult& res);
    void searchUp (const char * name, SearchResult& res);
    void search (const char * name, SearchResult& res);

    NamedTreeNode* searchDown (const char * name)
    {
        SearchResult res;
        searchDown(name, res);
        return res.getFirstFound();       
    }

    const NamedTreeNode* searchUp (const char * name) const
    {
        ConstSearchResult res;
        searchUp(name, res);
        return res.getFirstFound();       
    }
    
    const NamedTreeNode* search (const char * name) const
    {
        ConstSearchResult res;
        search(name, res);
        return res.getFirstFound();       
    }

    const NamedTreeNode* searchDown (const char * name) const
    {
        ConstSearchResult res;
        searchDown(name, res);
        return res.getFirstFound();       
    }

    NamedTreeNode* searchUp (const char * name)
    {
        SearchResult res;
        searchUp(name, res);
        return res.getFirstFound();       
    }
    
    NamedTreeNode* search (const char * name)
    {
        SearchResult res;
        search(name, res);
        return res.getFirstFound();       
    }

    NamedTreeNode* myChild (const char* name)
    {
        return searchDown(name);
    }

    const NamedTreeNode* myChild (const char* name) const
    {
        return const_cast<NamedTreeNode*>(this)->searchDown(name);
    }

    NamedTreeNode* myOffspring (const char* name);
    NamedTreeNode* myOffspring (std::vector<std::string> const& name_list);

    const NamedTreeNode* myOffspring (const char* name) const
    {
        return const_cast<NamedTreeNode*>(this)->myMyOffspring(name);
    }

    const NamedTreeNode* myOffspring (std::vector<std::string> const& name_list) const
    {
        return const_cast<NamedTreeNode*>(this)->myMyOffspring(name_list);
    }


    //static NamedTreeNode* releaseChild(NamedTreeNode* node);

    NamedTreeNode (const char *name, NamedTreeNode* parent, bool is_name_register);
    NamedTreeNode () : NamedTreeNode(nullptr, nullptr, true) {}
    virtual ~NamedTreeNode();

  protected:
    NameID         id_;                      ///< my id
    const char*    name_ {nullptr};
    NameRegistry*  name_registry_ {nullptr}; ///< name registry for offsprings
    pointer_t      name_register_ {nullptr}; ///< to ancestor where my name is registered

    static constexpr char s_dummy_name_ = '\0';

    void registerName(const char *name, NamedTreeNode* node);
    void unregisterName(NamedTreeNode* node);
    void transferName(NamedTreeNode* node, NamedTreeNode* new_register, const NameID* new_parent_id);

    NamedTreeNode* parent() { return reinterpret_cast<NamedTreeNode*>(base_t::parent_); }
    const NamedTreeNode* parent() const { return reinterpret_cast<const NamedTreeNode*>(base_t::parent_); }
};

//--------------------------------------------------------------------------
// NamedTreeNode<Alloc>
//--------------------------------------------------------------------------
template <class Alloc>
void NamedTreeNode<Alloc>::registerName(const char *name, NamedTreeNode* node)
{
    if (!name || !*name)
    {
        // anonymous name, do not register
        return;
    }
    // root does not register its name. The registered name is the node name 
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
void NamedTreeNode<Alloc>::unregisterName(NamedTreeNode* node)
{
    // unregister node's name if it is not anonymous
    if (!node->isAnonymous())
    {
        // unregister node name in name register
        QualifiedName registered_name (node->parent()->id_, node->name_);
        name_registry_->name_hash_table_.erase(registered_name.c_str());
        node->name_ = nullptr;
    }

    // unregister node children name if node is not name register
    if (!node->isNameRegister())
    {
        for (auto chd: node->children_)
        {
            unregisterName(static_cast<NamedTreeNode*>(chd));
        }
        // check if there are any extra chid nodes not listed in node->children_
        auto extra_chd = node->getExtraChildNodes();
        if (extra_chd)
        {
            for (auto exchd: *extra_chd)
            {
                unregisterName(static_cast<NamedTreeNode*>(exchd));
            }
        }
    }
    // else, all children of node are registered in node name_hash_table_, therefore
    // we do not need to go deeper to unregister children's name for this name
    // register
}

template <class Alloc>
void NamedTreeNode<Alloc>::transferName(
    NamedTreeNode* node, NamedTreeNode* new_register, const NameID* new_parent_id)
{
    // transfer node's name if it is not anonymous
    if (!node->isAnonymous())
    {
        // unregister node name in name register
        QualifiedName registered_name (node->parent()->id_, node->name_);
        name_registry_->name_hash_table_.erase(registered_name.c_str());

        if (new_parent_id)
        {
            // Parent is changed, we need to use new qualified name
            QualifiedName new_qname (*new_parent_id, registered_name.c_str() + ID_LENGTH);
            auto [iter, inserted] = new_register->name_registry_->name_hash_table_.emplace(new_qname.c_str(), node);
            node->name_ = iter->first.c_str() + ID_LENGTH;
        }
        else
        {
            // Parent is not changed, use the original qualified name
            auto [iter, inserted] = new_register->name_registry_->name_hash_table_.emplace(registered_name.c_str(), node);
            node->name_ = iter->first.c_str() + ID_LENGTH;
        }
    }

    // transfer node children name if node is not name register
    if (!node->isNameRegister())
    {
        for (auto chd: node->children_)
        {
            // children's parent is not changed, use nullptr for new_parent_id
            transferName(static_cast<NamedTreeNode*>(chd), new_register, nullptr);
        }
    }
    // else, all children of node are registered in node name_hash_table_, therefore
    // we do not need to go deeper to transfer children's name for this name
    // register
}

template <class Alloc>
NamedTreeNode<Alloc>* NamedTreeNode<Alloc>::getNameRegister()
{
    auto p = parent();
    if (p)
    {
        return p->name_registry_ ? p : p->name_register_;
    }
    return name_registry_ ? this : nullptr;
}

template <class Alloc>
NamedTreeNode<Alloc>::NamedTreeNode(const char *name, NamedTreeNode* parent, bool is_name_register)
    : TreeNode<Alloc>(parent)
{
    NameRegistry * name_registry {nullptr};
    if (!parent || is_name_register)
    {
        name_registry_ = new NameRegistry();
        name_registry = name_registry_;
    }

    name_register_ = getNameRegister();
    if (!name_registry)
    {
        name_registry = name_register_->name_registry_;
    }

    id_.setID(name_registry->generateId());
    name_register_->registerName(name, this);
};

template <class Alloc>
NamedTreeNode<Alloc>::~NamedTreeNode()
{
    delete name_registry_;
}

#if 0
template <class Alloc>
NamedTreeNode<Alloc>* NamedTreeNode<Alloc>::releaseNode(NamedTreeNode* node)
{
    name_register_->unregisterName(this);
    return static_cast<NamedTreeNode*>(TreeNode::releaseNode(node));
}
#endif

template <class Alloc>
void NamedTreeNode<Alloc>::detach()
{
    name_register_->unregisterName(this);
    TreeNodeBase::detach();
}

template <class Alloc>
bool NamedTreeNode<Alloc>::reparent (NamedTreeNode * new_parent)
{
    // If name register is changed, we need to transfers name of this node and
    // names of children to the new register
    if (name_register_!=new_parent && name_register_!=new_parent->name_register_)
    {
        NamedTreeNode* new_name_register = new_parent->isNameRegister()
                                            ? new_parent : new_parent->name_register_;
        name_register_->transferName(this, new_name_register, &new_parent->id_);
    }
    TreeNodeBase::reparent(new_parent);
    return true;
}

template <class Alloc>
void NamedTreeNode<Alloc>::fosterRoot(const char* name, NamedTreeNode* node)
{
    assert(node->isRoot());
    TreeNodeBase::foster(node);
    NamedTreeNode * name_registor = name_registry_ ? this : parent()->name_register_;
    //id_.setID(name_registor->name_registry->generateId());
    name_registor->registerName(name, node);
}

template <class Alloc>
NamedTreeNode<Alloc>* NamedTreeNode<Alloc>::extract()
{
    if (!parent)
    {
        // root cannot be extracted
        return this;
    }
    // If this node is name register, we need to transfer names of all children
    if (isNameRegister())
    {
        NamedTreeNode* new_register = parent()->name_register_;
        NameID& parent_id = parent()->id_;
        for (auto chd: TreeNodeBase::children_)
        {
            transferName(static_cast<NamedTreeNode*>(chd), new_register, &parent_id);
        }
    }
    return static_cast<NamedTreeNode*>(TreeNodeBase::extract());
}

template <class Alloc>
size_t NamedTreeNode<Alloc>::childrenNum(uint64_t cat_filter, uint64_t sub_cat_filter) const
{
    size_t num = 0;
    for (auto child: this->children())
    {
        if ((cat_filter==0 ||
             static_cast<NamedTreeNode const*>(child)->category()==cat_filter) &&
            (sub_cat_filter==0 ||
             static_cast<NamedTreeNode const*>(child)->subCategory()==sub_cat_filter)
        )
        {
            ++num;
        }
    }
    return num;
}

template <class Alloc>
bool NamedTreeNode<Alloc>::rename (const char * new_name)
{
    // root cannot be renamed
    if (!base_t::parent_) return false;

    auto& name_map = nameMap();

    if (!name_)
    {
        if (new_name && *new_name)
        {
            // rename from anonymous, add to name table
            QualifiedName qname (parent()->id_, new_name);
            auto [iter, inserted] = name_map.emplace(qname.c_str(), this);
            if (inserted)
            {
                name_ = iter->first.c_str() + ID_LENGTH;
            }
            return inserted;
        }
    }

    QualifiedName oldname (parent()->id_, name_);

    if (!new_name || !*new_name)
    {
        // rename to anonymous, remove entry in name table
        name_map.erase(oldname.c_str());
        name_ = nullptr;
        return true;
    }

    // change name of the entry in name table
    QualifiedName qname (parent()->id_, new_name);
    auto [renamed_iter, renamed] = name_map.rename(oldname.c_str(), qname.c_str());
    if (renamed)
    {
        name_ = renamed_iter->first.c_str() + ID_LENGTH;
    }
    return renamed;
}

template <class Alloc>
void NamedTreeNode<Alloc>::searchDown (const char * name , ConstSearchResult& res) const
{
    auto const & name_map = childNameMap();
    auto iter = name_map.find(QualifiedName(id_,name).c_str());
    if (iter!=name_map.end())
    {
        res.addResult(iter->second);
    }
}

template <class Alloc>
NamedTreeNode<Alloc>* NamedTreeNode<Alloc>::myOffspring (const char * name)
{
    std::vector<std::string> name_list;
    size_t res = strSplit(name, name_list, 0, ',');
    NamedTreeNode* node = this;
     NamedTreeNode* found = nullptr;
    for (auto& n: name_list)
    {
        auto const & name_map = node->childNameMap();
        auto iter = name_map.find(QualifiedName(node->id_, name).c_str());
        if (iter==name_map.end())
        {
            found = nullptr;
            break;
        }
        node = iter->second;
        found = node;
    }
    return found;
}


template <class Alloc>
NamedTreeNode<Alloc>* NamedTreeNode<Alloc>::myOffspring (std::vector<std::string> const& name_list)
{
    NamedTreeNode* node = this;
    NamedTreeNode* found = nullptr;
    for (auto& n: name_list)
    {
        auto const & name_map = node->childNameMap();
        auto iter = name_map.find(QualifiedName(node->id_, n.c_str()).c_str());
        if (iter==name_map.end())
        {
            found = nullptr;
            break;
        }
        node = iter->second;
        found = node;
    }
    return found;
}

template <class Alloc>
void NamedTreeNode<Alloc>::searchUp (const char * name, ConstSearchResult& res) const
{
    const auto p = this;
	while (p)
	{
	    if (std::strcmp(name, p->name())==0)
        {
            res.addResult(p);
            return;
        }
	    p = p->parent();
    }
}

template <class Alloc>
void NamedTreeNode<Alloc>::search (const char * name, ConstSearchResult& res) const
{
    searchDown(name, res);
	if (res.notFound())
	{    
	    searchUp(name, res);
    }
}

template <class Alloc>
void NamedTreeNode<Alloc>::searchDown (const char * name , SearchResult& res)
{
    auto const & name_map = childNameMap();
    auto iter = name_map.find(QualifiedName(id_,name).c_str());
    if (iter!=name_map.end())
    {
        res.addResult(iter->second);
    }
}

template <class Alloc>
void NamedTreeNode<Alloc>::searchUp (const char * name, SearchResult& res)
{
    auto p = this;
	while (p)
	{
	    if (std::strcmp(name, p->name())==0)
        {
            res.addResult(p);
            return;
        }
	    p = p->parent();
    }
}

template <class Alloc>
void NamedTreeNode<Alloc>::search (const char * name, SearchResult& res)
{
    searchDown(name, res);
	if (res.notFound())
	{    
	    searchUp(name, res);
    }
}

using PooledNamedNode = NamedTreeNode<PooledAllocator>;

}

