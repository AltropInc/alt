
#include "TreeNode.h"
#include <assert.h>

namespace alt
{

size_t TreeNodeBase::size () const
{
    size_t sz = 1;
    for (auto node: children_)
    {
        sz += static_cast<const TreeNodeBase*>(node)->size();
    }
	return sz;
};

bool TreeNodeBase::isMyOffspring (const TreeNodeBase * n) const
{
	for (auto p=n; p; p = p->parent_)
	{
       if (p==this) return true; 
	}
	return false;
};

bool TreeNodeBase::isMyAncestor (const TreeNodeBase * n) const
{
	return n ? n->isMyOffspring(this) : false;
}

bool TreeNodeBase::isMySibling (const TreeNodeBase * n) const
{
	return n && n->parent_ == parent_;
};

const TreeNodeBase* TreeNodeBase::root() const
{
    const TreeNodeBase* r = this;
    while (r->parent_) r = r->parent_;
    return r;
}

TreeNodeBase* TreeNodeBase::root()
{
    TreeNodeBase* r = this;
    while (r->parent_) r = r->parent_;
    return r;
}

TreeNodeBase * TreeNodeBase::leastCommonAncestor (TreeNodeBase* other, TreeNodeBase* root)
{
    auto n1 = this;
    auto n2 = other;
    while (n1 && n2)
    {
        if (n1==root || n2==root) return root;
        if (n1==n2) return n1;
        if (n2->isMyAncestor(n1)) return n1;
        if (n1->isMyAncestor(n2)) return n2;
        n1 = n1->parent_;
        n2 = n2->parent_;
    }
	return nullptr;
}

std::vector<TreeNodeBase*> TreeNodeBase::getPathDownTo(TreeNodeBase * n)
{
    std::vector<TreeNodeBase*> path;
	if (!n) { return path; }
	size_t distance =0;
	TreeNodeBase * p =n;
	while (p)
	{
		distance++;
		if (p==this) break;
	    p =  p->parent_;
	}
	if (!p) return path;
	p =n;
    path.resize(distance, nullptr);
	for (size_t i=0; i<distance; i++)
	{
		path[i] = p;
	    p =  p->parent_;
	}
	return path;
};

std::vector<TreeNodeBase*> TreeNodeBase::getPathUpTo(TreeNodeBase * n)
{
    return n ? n->getPathDownTo(this) : std::vector<TreeNodeBase*>();
}

int TreeNodeBase::bottomUp(nodefunc f)
{
    for (auto node: children_)
    {
        if (reinterpret_cast<TreeNodeBase*>(node)->bottomUp(f) < 0 ||
            f(reinterpret_cast<TreeNodeBase*>(node)) < 0)
        {
            return -1;
        }
    }
    return f(reinterpret_cast<TreeNodeBase*>(this));
}

int TreeNodeBase::bottomUpBack(nodefunc f)
{
    for (auto iter = children_.rbegin(); iter!=children_.rend(); --iter)
    {
        if (reinterpret_cast<TreeNodeBase*>(*iter)->bottomUpBack(f) < 0 ||
            f(reinterpret_cast<TreeNodeBase*>(*iter)) < 0)
        {
            return -1;
        }
    }
    return f(reinterpret_cast<TreeNodeBase*>(this));
}

int TreeNodeBase::upDown(nodefunc f)
{
    if (f(reinterpret_cast<TreeNodeBase*>(this))<0)
    {
        return -1;
    }
    for (auto node: children_)
    {
        if (f(reinterpret_cast<TreeNodeBase*>(node)) < 0 ||
            reinterpret_cast<TreeNodeBase*>(node)->upDown(f) < 0)
        {
            return -1;
        }
    }
    return 0;
}

int TreeNodeBase::upDownBack(nodefunc f)
{
    if (f(reinterpret_cast<TreeNodeBase*>(this))<0)
    {
        return -1;
    }
    for (auto iter = children_.rbegin(); iter!=children_.rend(); --iter)
    {
        if (f(reinterpret_cast<TreeNodeBase*>(*iter)) < 0 ||
            reinterpret_cast<TreeNodeBase*>(*iter)->upDownBack(f) < 0)
        {
            return -1;
        }
    }
    return 0;
}

std::pair<TreeNodeBase*, TreeNodeBase*> TreeNodeBase::siblingRange(int distance)
{
    auto r = LinkedNode::range(distance);
    return std::make_pair(static_cast<TreeNodeBase*>(r.first),static_cast<TreeNodeBase*>(r.second));
}

std::pair<TreeNodeBase*, TreeNodeBase*> TreeNodeBase::siblingRange(int dist_before, int dist_after) 
{
    auto r = LinkedNode::range(dist_before,dist_after);
    return std::make_pair(static_cast<TreeNodeBase*>(r.first),static_cast<TreeNodeBase*>(r.second));
}

TreeNodeBase * TreeNodeBase::detach()
{
    // root cannot be detatched
    if (parent_)
    {
        TreeNodeBase * n = static_cast<TreeNodeBase*>(parent_->children_.extract(this));
        parent_ = nullptr;
        return n;
    }
    return this;
}

TreeNodeBase * TreeNodeBase::extract()
{
    // root cannot be extracted
    if (parent_)
    {
        auto insert_pos = next_;
        TreeNodeBase * n = static_cast<TreeNodeBase*>(parent_->children_.extract(this));
        for (auto node: children_)
        {
            static_cast<TreeNodeBase*>(node)->parent_ = parent_;
        }
        parent_->children_.splice(insert_pos, children_);
        parent_ = nullptr;
        return n;
    }
    return this;
}

TreeNodeBase* TreeNodeBase::extractChildren()
{
    for (auto node: children_)
    {
        static_cast<TreeNodeBase*>(node)->parent_ = nullptr;
    }
    return static_cast<TreeNodeBase*>(children_.extract());
}

TreeNodeBase* TreeNodeBase::extractFirstChild()
{
    auto node =  static_cast<TreeNodeBase*>(children_.extractFront());
    if (node) node->parent_ = nullptr;
    return node;
}

TreeNodeBase* TreeNodeBase::extractLastChild()
{
    auto node =  static_cast<TreeNodeBase*>(children_.extractBack());
    if (node) node->parent_ = nullptr;
    return node;
}

TreeNodeBase* TreeNodeBase::extractChild(TreeNodeBase* node)
{
    node->parent_ = nullptr;
    return static_cast<TreeNodeBase*>(children_.extract(node));
}

TreeNodeBase* TreeNodeBase::extractChildren(TreeNodeBase* begin, TreeNodeBase* end)
{
    for (auto node=begin; node; node=node->nextSibling())
    {
        node->parent_ = nullptr;
        if (node==end) break;
    }
    return static_cast<TreeNodeBase*>(children_.extract(begin, end));
}

TreeNodeBase* TreeNodeBase::extractChildrenFrom(TreeNodeBase* node)
{
    return extractChildren (node, node->youngestSibling());
};

TreeNodeBase* TreeNodeBase::extractChildrenTo(TreeNodeBase* node)
{
    return extractChildren (node->eldestSibling(), node);
};

TreeNodeBase* TreeNodeBase::extractChildren(TreeNodeBase* node, int distance)
{
    auto range = node->siblingRange(distance);
    return extractChildren (range.first, range.second);
};

TreeNodeBase* TreeNodeBase::extractChildren(TreeNodeBase* node, int dist_before, int dist_after)
{
    auto range = node->siblingRange(dist_before, dist_after);
    return extractChildren (range.first, range.second);
};

void TreeNodeBase::appendChild(TreeNodeBase* node)
{
    children_.append(node);
    node->parent_ = this;
}

void TreeNodeBase::TreeNodeBase::appendChild(TreeNodeBase* position, TreeNodeBase* node)
{
    children_.append(position, node);
    node->parent_ = this;
}

void TreeNodeBase::appendChildren(TreeNodeBase* position, TreeNodeBase* begin, TreeNodeBase* end)
{
    children_.append(position, begin, end);
    for (auto node=begin; node; node=node->nextSibling())
    {
        node->parent_ = this;
        if (node==end) break;
    }
}

void TreeNodeBase::appendChildrenFrom(TreeNodeBase* position, TreeNodeBase* node)
{
    children_.appendFrom(position, node);
    for (auto n=node; n; n=n->nextSibling())
    {
        n->parent_ = this;
    }
}

void TreeNodeBase::appendChildrenTo(TreeNodeBase* position, TreeNodeBase* node)
{
    TreeNodeBase*head = node->eldestSibling();
    appendChildren(position, head, node);
}

void TreeNodeBase::insertChild(TreeNodeBase* node)
{
    children_.insert(node);
    node->parent_ = this;
}

void TreeNodeBase::insertChild(TreeNodeBase* position, TreeNodeBase* node)
{
    children_.insert(position, node);
    node->parent_ = this;
}

void TreeNodeBase::insertChildren(TreeNodeBase* position, TreeNodeBase* begin, TreeNodeBase* end)
{
    children_.insert(position, begin, end);
    for (auto node=begin; node; node=node->nextSibling())
    {
        node->parent_ = this;
        if (node==end) break;
    }
}

void TreeNodeBase::insertChildrenFrom(TreeNodeBase* position, TreeNodeBase* node)
{
    children_.insertFrom(position, node);
    for (auto n=node; n; n=n->nextSibling())
    {
        n->parent_ = this;
    }
}

void TreeNodeBase::insertChildrenTo(TreeNodeBase* position, TreeNodeBase* node)
{
    TreeNodeBase*head = node->eldestSibling();
    insertChildren(position, head, node);
}

void TreeNodeBase::reposition(TreeNodeBase* new_sibling)
{
    assert(!new_sibling || new_sibling->parent_ == parent_);
	if (parent_)
	{
		LinkedNode::extract();
		parent_->children_.insert(new_sibling, this);
	}
}

void TreeNodeBase::reparent(TreeNodeBase* new_parent)
{
	if (new_parent && parent_!=new_parent)
	{
		TreeNodeBase::detach();
		new_parent->appendChild(this);
	}
}

void TreeNodeBase::reparent(TreeNodeBase* new_parent, TreeNodeBase* new_sibling)
{
    assert(!new_sibling || new_sibling->parent_ == new_parent);
	if (new_parent && parent_!=new_parent)
	{
		TreeNodeBase::detach();
		new_parent->insertChild(new_sibling, this);
	}
    else if (parent_ && parent_==new_parent)
    {
        reposition(new_sibling);
    }
}

void TreeNodeBase::foster(TreeNodeBase* node)
{
	if (node && node->parent_!=this)
	{
		node->detach();
		appendChild(node);
	}
}

void TreeNodeBase::foster(TreeNodeBase* node, TreeNodeBase* sibling)
{
    assert(!sibling || sibling->parent_ == this);
	if (node && node->parent_!=this)
	{
		node->detach();
		insertChild(sibling, node);
	}
}

void TreeNodeBase::setChildren()
{
    for (auto node: children_)
    {
        static_cast<TreeNodeBase*>(node)->parent_ = this;
    }
}

void TreeNodeBase::wrap(TreeNodeBase* wrapper)
{
    if (parent_)
    {
        parent_->insertChild(extract(), wrapper);
    }
    wrapper->appendChild(this);
}

void TreeNodeBase::wrapChildren(TreeNodeBase* wrapper)
{
    wrapper->children_.spliceBack(children_);
    wrapper->setChildren();
    appendChild(wrapper);
}

void TreeNodeBase::wrapChild(TreeNodeBase* wrapper, TreeNodeBase* node)
{
    TreeNodeBase* next = static_cast<TreeNodeBase*>(node->next_);
    wrapper->children_.spliceBack(children_, node);
    wrapper->setChildren();
    insertChild(next, wrapper);
}

void TreeNodeBase::wrapChildrenTo(TreeNodeBase* wrapper, TreeNodeBase* node)
{
    TreeNodeBase* next = static_cast<TreeNodeBase*>(node->next_);
    wrapper->children_.spliceBackTo(children_, node);
    wrapper->setChildren();
    insertChild(next, wrapper);
}

void TreeNodeBase::wrapChildrenFrom(TreeNodeBase* wrapper, TreeNodeBase* node)
{
    TreeNodeBase* prev = static_cast<TreeNodeBase*>(node->prev_);
    wrapper->children_.spliceBackFrom(children_, node);
    wrapper->setChildren();
    appendChild(prev, wrapper);
}

void TreeNodeBase::wrapChildren(TreeNodeBase* wrapper, TreeNodeBase* first, TreeNodeBase* last)
{
    if (!last)
    {
        wrapChildrenFrom(wrapper, first);
        return;
    }
    TreeNodeBase* next = static_cast<TreeNodeBase*>(last->next_);
    wrapper->children_.spliceBack(children_, first, last);
    wrapper->setChildren();
    insertChild(next, wrapper);
}

template<> Allocator& TreeNode<Allocator>::allocator_ = Allocator::instance();
template<> PooledAllocator& TreeNode<PooledAllocator>::allocator_ = PooledAllocator::instance();
}
