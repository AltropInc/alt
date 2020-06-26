#pragma once

#include "LinkedList.h"
#include <vector>
#include <functional>

namespace alt
{
class ALT_UTIL_PUBLIC TreeNodeBase: public LinkedNode
{
  protected:
    TreeNodeBase*           parent_  {nullptr};
    LinkedListBase          children_;

  public:
  
    /// \brief Predicate compare function
    using nodefunc = std::function<int(TreeNodeBase*)>;

    TreeNodeBase() {}
    TreeNodeBase(TreeNodeBase* parent): parent_(parent)
    {
        if (parent) parent->children_.append(this);
    }

    TreeNodeBase* parent() { return parent_; }
    LinkedListBase& children() { return children_; }
    const TreeNodeBase* parent() const { return parent_; }
    const LinkedListBase& children() const { return children_; }

    TreeNodeBase* root();
    const TreeNodeBase* root() const;

    TreeNodeBase* firstChild() { return static_cast<TreeNodeBase*>(children_.front()); }
    TreeNodeBase* lastChild() { return static_cast<TreeNodeBase*>(children_.back()); }
    const TreeNodeBase* firstChild() const  { return static_cast<const TreeNodeBase*>(children_.front()); }
    const TreeNodeBase* lastChild() const { return static_cast<const TreeNodeBase*>(children_.back()); }
    TreeNodeBase* nthChild(int n) { return static_cast<TreeNodeBase*>(children_.nthNode(n)); }
    const TreeNodeBase* nthChild(int n) const { return static_cast<const TreeNodeBase*>(children_.nthNode(n)); }

    TreeNodeBase* nextSibling() { return static_cast<TreeNodeBase*>(next()); }
    TreeNodeBase* prevSibling() { return static_cast<TreeNodeBase*>(prev()); }
    const TreeNodeBase* nextSibling() const { return static_cast<const TreeNodeBase*>(next()); }
    const TreeNodeBase* prevSibling() const { return static_cast<const TreeNodeBase*>(prev()); }

    TreeNodeBase* youngestSibling()
    { return parent_ ? parent_->lastChild() : static_cast<TreeNodeBase*>(tail()); }
    TreeNodeBase* eldestSibling()
    { return parent_ ? parent_->firstChild() : static_cast<TreeNodeBase*>(head()); }

    size_t childrenNum() const { return children_.size(); }
    size_t siblingNum() const { return length(); }
    size_t size() const;

    const bool isMyChild(const TreeNodeBase *n) const
    { return n && n->parent_==this; }

    const bool isMyParent(const TreeNodeBase *n) const
    { return n && parent_==n; }

    const bool isRoot() const { return parent_==nullptr; }
    const bool isLeaf() const { return children_.empty(); }

    bool isMyOffspring (const TreeNodeBase * n) const;	// not include me!
    bool isMyAncestor (const TreeNodeBase * n) const;	// not include me!
    bool isMySibling (const TreeNodeBase * n) const;	// include me!
    bool isAncestorOf (const TreeNodeBase * n) const { return isMyOffspring(n); }
    TreeNodeBase * leastCommonAncestor (TreeNodeBase * n, TreeNodeBase* root=nullptr);  

    /// \return Range between this node and the node at the given
    /// distance at most. If the distance is negative, the range is
    /// between the node at the distance and this node.
    std::pair<TreeNodeBase*, TreeNodeBase*> siblingRange(int distance);

    /// \return Range between the node at distance at most before this node
    /// and the node at distance at most after this node
    std::pair<TreeNodeBase*, TreeNodeBase*> siblingRange(int dist_before, int dist_after); 

    /// \brief detach subtree rooted by this node from the tree
    /// If this node is root, nothing will happen
    /// \return The next node (sibling), if any. Otherwise, nullptr
    TreeNodeBase* detach();

    /// \brief extract this node only from the tree. Children of this node
    /// will be reparented to the parent of this. If this node is root,
    /// nothing will happen
    /// \return The next node (sibling), if any. Otherwise, nullptr
    TreeNodeBase* extract();

    /// \brief extract all children of this node
    /// \return the head of the extracted children
    TreeNodeBase* extractChildren();
    TreeNodeBase* extractFirstChild();
    TreeNodeBase* extractLastChild();
    TreeNodeBase* extractChild(TreeNodeBase* node);
    TreeNodeBase* extractChildren(TreeNodeBase* begin, TreeNodeBase* end);
    TreeNodeBase* extractChildren(TreeNodeBase* node, int distance);
    TreeNodeBase* extractChildren(TreeNodeBase* node, int dist_before, int dist_after);
    TreeNodeBase* extractChildrenFrom(TreeNodeBase* node);
    TreeNodeBase* extractChildrenTo(TreeNodeBase* node);

    void appendChild(TreeNodeBase* node);
    void appendChild(TreeNodeBase* position, TreeNodeBase* node);
    void appendChildren(TreeNodeBase* position, TreeNodeBase* begin, TreeNodeBase* end);
    void appendChildrenFrom(TreeNodeBase* position, TreeNodeBase* node);
    void appendChildrenTo(TreeNodeBase* position, TreeNodeBase* node);

    void insertChild(TreeNodeBase* node);
    void insertChild(TreeNodeBase* position, TreeNodeBase* node);
    void insertChildren(TreeNodeBase* position, TreeNodeBase* begin, TreeNodeBase* end);
    void insertChildrenFrom(TreeNodeBase* position, TreeNodeBase* node);
    void insertChildrenTo(TreeNodeBase* position, TreeNodeBase* node);

    void reposition(TreeNodeBase* new_sibling);
    ///@}

    ///@{
    /** Splice functions */
    void reparent(TreeNodeBase* new_parent);
    void reparent(TreeNodeBase* new_parent, TreeNodeBase* new_sibling);

    /// \breif Make node be the last child of this
    void foster(TreeNodeBase* node);

    /// \breif Make node be the child before the sibling
    void foster(TreeNodeBase* node, TreeNodeBase* sibling);

    /// \breif Let the new wrapper node foster all children of this node and
    /// make the new wrapper a child of this node.
    void wrapChildren(TreeNodeBase* wrapper);

    /// \breif Let the new wrapper node foster the given node, and the wrapper
    /// will replace the child position of the given node.
    void wrapChild(TreeNodeBase* wrapper, TreeNodeBase* node);

    /// \breif Let the new wrapper node foster children from the first to the last node
    /// of this node and make the new wrapper a child of this node. The wrapper will be
    /// in the position of the first and last before function call
    void wrapChildren(TreeNodeBase* wrapper, TreeNodeBase* first, TreeNodeBase* last);

    /// \breif Let the new wrapper node foster children from the node to the tail
    /// of this node and make the new wrapper a child of this node. The wrapper will be
    /// in the position of the node before function call
    void wrapChildrenTo(TreeNodeBase* wrapper, TreeNodeBase* node);

    /// \breif Let the new wrapper node foster children from the head to the given node
    /// of this node and make the new wrapper a child of this node. The wrapper will be
    /// in the position of the node before function call
    void wrapChildrenFrom(TreeNodeBase* wrapper, TreeNodeBase* node);

    /// \breif Replace this node with a new wrapper node and make the this node
    /// (plus subtree) a child of the new wrapper.
    void wrap(TreeNodeBase* wrapper);
    ///@}
    
    /// \brief Get a list of nodes from this down to the offspring n
    /// \param n The offspring node
    /// \return The list of nodes from this up to the offspring n,
    /// or an empty vector if n is not an offspring.
    std::vector<TreeNodeBase*> getPathDownTo(TreeNodeBase * n);

    /// \brief Get a list of nodes from this up to the ancestor n
    /// \param n The ancestor node
    /// \return The list of nodes from this up to the ancestor n,
    /// or an empty vector if n is not an ancestor.
    std::vector<TreeNodeBase*> getPathUpTo(TreeNodeBase * n);

    /// \brief traverse subtree at this node bottom up and front back
    /// \param f The node function to be applied to each node
    /// \return true if all nodes in the subtree, including this node, is traversed
    /// \note if the node function f returns -1, the traversal will stop
    int bottomUp(nodefunc f);

    /// \brief traverse subtree at this node up down and front back
    /// \param f The node function to be applied to each node
    /// \return true if all nodes in the subtree, including this node, is traversed
    /// \note if the node function f returns -1, the traversal will stop    
    int upDown(nodefunc f);

    /// \brief traverse subtree at this node up down and back front
    /// \param f The node function to be applied to each node
    /// \return true if all nodes in the subtree, including this node, is traversed
    /// \note if the node function f returns -1, the traversal will stop      
    int bottomUpBack(nodefunc f);

    /// \brief traverse subtree at this node up down and back front
    /// \param f The node function to be applied to each node
    /// \return true if all nodes in the subtree, including this node, is traversed
    /// \note if the node function f returns -1, the traversal will stop      
    int upDownBack(nodefunc f);

    virtual ~TreeNodeBase() {};

  private:

    void setChildren();
};

template <class Alloc = Allocator>
class TreeNode: public TreeNodeBase
{
    static Alloc&      allocator_;

    TreeNode(TreeNode && other) = delete;
    TreeNode(TreeNode & other) = delete;
    TreeNode& operator =(TreeNode & other) = delete;

  public:

    TreeNode() {}
    TreeNode(TreeNode* parent): TreeNodeBase(parent) {}

    static Alloc& getAllocator() { return allocator_; }

    template <typename T, typename... Args>
    static T* create(Args&&... args)
    {
        return  alt_pnew(allocator_, T, std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    T* newChild(Args&&... args)
    {
       auto chd = alt_pnew(allocator_, T, std::forward<Args...>(args...));
       appendChild(chd);
       return chd;
    }

    template <typename T, typename... Args>
    T* newChildFront(Args&&... args)
    {
        T* chd =  alt_pnew(allocator_, T, std::forward<Args>(args)...);
        appendChild(nullptr, chd);
        return chd;
    }

    template <typename T, typename... Args>
    T* newChildBefore(TreeNodeBase* position, Args&&... args)
    {
        T* chd =  alt_pnew(allocator_, T, std::forward<Args>(args)...);
        insertChild(position, chd);
        return chd;
    }

    template <typename T, typename... Args>
    T* newChildAfter(TreeNodeBase* position, Args&&... args)
    {
        T* chd =  alt_pnew(allocator_, T, std::forward<Args>(args)...);
        appendChild(position, chd);
        return chd;
    }

    static TreeNode* releaseNode(TreeNode* node)
    {
        TreeNode* next = static_cast<TreeNode*>(node->detach());
        node->eraseChildren();
        alt_pdel(allocator_, TreeNode, node);
        return next;
    }

    // node is already extracted
    static void releaseNodes(TreeNode* node)
    {
        TreeNode* next{nullptr};
        for (auto n = node; n; n = next)
        {
            next = static_cast<TreeNode*>(n->next_);
            n->eraseChildren();
            alt_pdel(allocator_, TreeNode, n);
        }
    }

    void eraseChildren()
    {
        if (children_.empty()) return;
        auto node = static_cast<TreeNode*>(children_.extract());
        releaseNodes(node);
    }

    TreeNode* eraseChild(TreeNode* node)
    {
        TreeNode* next = static_cast<TreeNode*>(extractChild(node));
        node->eraseChildren();
        alt_pdel(allocator_, TreeNode, node);
        return next;
    }

    TreeNode* eraseChildren(TreeNode* from, TreeNode* to)
    {
        auto next = static_cast<TreeNode*>(extractChild(from, to));
        releaseNode(from);
        return next;
    }

    TreeNode* eraseChildren(TreeNode* from, size_t n)
    {
        auto next = extractChild(from, n);
        releaseNode(from);
        return next;
    }
};

using PooledTreeNode = TreeNode<PooledAllocator>;

}

