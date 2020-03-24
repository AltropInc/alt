
#include "LinkedList.h"

namespace alt
{

LinkedNode *LinkedNode::extract()
{
    if (next_) next_->prev_ = prev_;
    if (prev_) prev_->next_ = next_;
    next_ = nullptr;
    prev_ = nullptr;
}

void LinkedNode::extract(LinkedNode* util)
{
    // extract node link from first to last
    if (prev_) prev_->next_ = util->next_;
    if (!util)
    {
        util = tail();
    }
    if (util->next_) util->next_->prev_ = prev_;
}

LinkedNode* LinkedNode::tail()
{
    LinkedNode* last = this;
    while (last->next_) last = last->next_;
    return last;
}

LinkedNode* LinkedNode::head()
{
    LinkedNode* first = this;
    while (first->prev_) first = first->prev_;
    return first;
}

LinkedNode* LinkedNode::tail(size_t& distance)
{
    distance = 0;
    LinkedNode* last = this;
    while (last->next_) { distance++; last = last->next_; }
    return last;
}

LinkedNode* LinkedNode::head(size_t& distance)
{
    distance = 0;
    LinkedNode* first = this;
    while (first->prev_) { distance++; first = first->prev_; }
    return first;
}

bool LinkedNode::in(const LinkedListBase* list) const
{
    return list ? list->has(this) : false;
}

LinkedNode* LinkedNode::neighborAt(int distance)
{
    if (distance==0) return this;
    LinkedNode* n = this;
    if (distance < 0)
    {
        while (n->prev_ && distance++ < 0) n = n->prev_;
        return distance==0 ? n : nullptr;
    }
    while (n->next_ && distance-- > 0) n = n->next_;
    return distance==0 ? n : nullptr;
}

LinkedNode* LinkedNode::neighbor(int distance)
{
    if (distance==0) return this;
    LinkedNode* n = this;
    if (distance < 0)
    {
        while (n->prev_ && distance++ < 0) n = n->prev_;
        return n;
    }
    while (n->next_ && distance-- > 0) n = n->next_;
    return n;
}

LinkedNode::Pair LinkedNode::range(int dist_before, int dist_after)
{
    LinkedNode * last = this;
    LinkedNode * first = this;
    LinkedNode * node = this;
    while (dist_after-- > 0)
    {
        node = node->next_;
        if (!node) break;
        last = node;
    }
    while (dist_before++ < 0)
    {
        node = node->prev_;
        if (!node) break;
        first = node;
    }
    return std::make_pair(first, last);
};

LinkedNode::Pair LinkedNode::range(int distance)
{
    if (distance==0) return std::make_pair(this, this);
    if (distance > 0)
    {
        return range(0, distance);
    }
    return range(-distance, 0);
};

int LinkedNode::distance(LinkedNode* node)
{
    LinkedNode *current = this;
    int num = 0;
    while (current && current != node)
    {  
        --num;              
        current = current->next_;  
    }
    if (current==node) return num;

    current = node;
    num = 0;
    while (current && current != this)
    {  
        ++num;              
        current = current->next_;  
    }
    if (current==this) return num;

    return std::numeric_limits<int>::max();
}

size_t LinkedNode::length() const
{
    const LinkedNode *current = this;
    size_t num = 0;
    while (current)
    {  
        ++num;              
        current = current->next_;  
    }

    current = prev_;
    while (current)
    {  
        ++num;              
        current = current->prev_;  
    }

    return num;
}

LinkedListBase::LinkedListBase(LinkedListBase && other)
    : head_(other.head_)
    , tail_(other.tail_)
{
    other.head_  = nullptr;
    other.tail_  = nullptr;
}

void LinkedListBase::swap(LinkedListBase & other)
{
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
}

bool LinkedListBase::has (const LinkedNode* node) const
{
    auto n = head_;
    while (n)
    {
        if (n==node) return true;
        n = n->next_;
    }
    return false;
}

void LinkedListBase::pushBack(LinkedNode* node)
{
    pushBack(node, node);
}

void LinkedListBase::pushBackFrom(LinkedNode* node)
{
    pushBack(node, node->tail());
}

void LinkedListBase::pushBackTo(LinkedNode* node)
{
    pushBack(node->head(), node);
}

void LinkedListBase::pushBack(LinkedNode* first, LinkedNode* last)
{
    first->extract(last);

    if (!tail_)
    {
        head_ = first;
        tail_ = last;
        return;
    }

    tail_->next_ = first;
    first->prev_ = tail_;
    tail_ = last;
    last->next_ = nullptr;
}

void LinkedListBase::pushFrontFrom(LinkedNode* node)
{
    pushFront(node, node->tail());
}

void LinkedListBase::pushFrontTo(LinkedNode* node)
{
    pushFront(node->head(), node);
}

void LinkedListBase::pushFront(LinkedNode* node)
{
    pushFront(node, node);
}

void LinkedListBase::pushFront(LinkedNode* first, LinkedNode* last)
{
    first->extract(last);
    if (!head_)
    {
        head_ = first;
        tail_ = last;
        return;
    }
    head_->prev_ = last;
    last->next_ = head_;
    head_ = first;
    first->prev_ = nullptr;
}

void LinkedListBase::insertFrom(LinkedNode* position, LinkedNode* node)
{
    insert(position, node, node->tail());
}

void LinkedListBase::insertTo(LinkedNode* position, LinkedNode* node)
{
    insert(position, node->head(), node);
}

void LinkedListBase::insert(LinkedNode* position, LinkedNode* node)
{
    insert(position, node, node);
}

void LinkedListBase::insert(LinkedNode* position, LinkedNode* first, LinkedNode* last)
{
    if (!position)
    {
        pushBack(first, last);
        return;
    }
    LinkedNode * prev = position->prev_;
    first->prev_ = prev;
    last->next_ = position;
    position->prev_ = last;
    if (prev)
    {
        prev->next_ = first;
    }
    else
    {
        head_ = first;
    }
}

void LinkedListBase::appendFrom(LinkedNode* position, LinkedNode* node)
{
    append(position, node, node->tail());
}

void LinkedListBase::appendTo(LinkedNode* position, LinkedNode* node)
{
    append(position, node->head(), node);
}

void LinkedListBase::append(LinkedNode* position, LinkedNode* node)
{
    append(position, node, node);
}
void LinkedListBase::append(LinkedNode* position, LinkedNode* first, LinkedNode* last)
{
    if (!position)
    {
        pushFront(first, last);
        return;
    }
    LinkedNode * next = position->next_;
    first->prev_ = position;
    last->next_ = next;
    position->next_ = first;
    if (next)
    {
        next->prev_ = last;
    }
    else
    {
        tail_ = last;
    }
}

LinkedNode* LinkedListBase::extractFront()
{
    if (head_)
    {
        LinkedNode* front = head_;
        head_ = head_->next_;
        if (!head_) tail_ = nullptr;
        front->next_ = nullptr;
        return front;
    }
    return nullptr;
}

LinkedNode* LinkedListBase::extractBack()
{
    if (tail_)
    {
        LinkedNode* back = tail_;
        tail_ = tail_->prev_;
        if (!tail_) head_ = nullptr;
        back->prev_ = nullptr;
        return back;
    }
    return nullptr;
}

LinkedNode* LinkedListBase::extract(LinkedNode* begin, LinkedNode* end)
{
    if (end->next_)
    {
        end->next_->prev_ = begin->prev_;
    }
    if (begin->prev_)
    {
        begin->prev_->next_ = end->next_;
    }
    if (head_==begin)
    {
        head_ = end->next_;
    }
    if (tail_==end)
    {
        tail_ = begin->prev_;
    }
    LinkedNode* next = end->next_;
    end->next_ = nullptr;
    begin->prev_ = nullptr;
    return next;
}

LinkedNode* LinkedListBase::extract(LinkedNode* node)
{
#if 0
    if (node->next_)
    {
        node->next_->prev_ = node->prev_;
    }
    if (node->prev_)
    {
        node->prev_->next_ = node->next_;
    }
    if (head_==node)
    {
        head_ = node->next_;
    }
    if (tail_==node)
    {
        tail_ = node->prev_;
    }
    LinkedNode* next = node->next_;
    node->next_ = nullptr;
    node->prev_ = nullptr;
#endif
    return extract(node, node);
}

void LinkedListBase::extractFrom(LinkedNode* node)
{
    extract (node, tail_);
};

LinkedNode::Pair LinkedListBase::extractTo(LinkedNode* node)
{
    auto head = head_;
    auto next = extract (head, node);
    return std::make_pair(head, next);
};

LinkedNode* LinkedListBase::extract(LinkedNode* node, int distance)
{
    auto range = node->range(distance);
    return extract (range.first, range.second);
};

LinkedNode::Pair LinkedListBase::extract(LinkedNode* node, int dist_before, int dist_after)
{
    auto range = node->range(dist_before, dist_after);
    auto next = extract (range.first, range.second);
    return std::make_pair(range.first,next);
};

LinkedNode* LinkedListBase::extract()
{
    LinkedNode * node = head_;
    head_  = nullptr;
    tail_  = nullptr;
    return node;
}

void LinkedListBase::splice(LinkedNode* position, LinkedListBase& other)
{
    insertFrom(position, other.extract());    
}

void LinkedListBase::splice(LinkedNode* position, LinkedListBase& other, LinkedNode* node)
{
    other.extract(node);
    insert(position, node);
}

void LinkedListBase::spliceFrom(LinkedNode* position, LinkedListBase& other, LinkedNode* node)
{
    LinkedNode* other_tail = other.tail_;
    other.extract(node, other_tail);
    insert(position, node, other_tail);
}

void LinkedListBase::spliceTo(LinkedNode* position, LinkedListBase& other, LinkedNode* node)
{
    LinkedNode* other_head = other.head_;
    other.extract(other_head, node);
    insertFrom(position, other_head);
}

void LinkedListBase::splice(LinkedNode* position, LinkedListBase& other, LinkedNode* begin, LinkedNode* end)
{
    other.extract(begin, end);
    insert(position, begin, end);
}

void LinkedListBase::spliceFront(LinkedListBase& other)
{
    if (!other.empty())
    {
        pushFront(other.extract());
    }
}

void LinkedListBase::spliceBack(LinkedListBase& other)
{
    if (!other.empty())
    {
        pushBack(other.extract());
    }
}

void LinkedListBase::spliceFront(LinkedListBase& other, LinkedNode* node)
{
    other.extract(node);
    pushFront(node);
}

void LinkedListBase::spliceBack(LinkedListBase& other, LinkedNode* node)
{
    other.extract(node);
    pushBack(node);
}

void LinkedListBase::spliceFront(LinkedListBase& other, LinkedNode* begin, LinkedNode* end)
{
    other.extract(begin, end);
    pushFront(begin, end);
}

void LinkedListBase::spliceBack(LinkedListBase& other, LinkedNode* begin, LinkedNode* end)
{
    other.extract(begin, end);
    pushBack(begin, end);
}

void LinkedListBase::spliceFrontFrom(LinkedListBase& other, LinkedNode* node)
{
    LinkedNode* tail = other.tail_;
    other.extractFrom(node);
    pushFront(node, tail);
}

void LinkedListBase::spliceBackFrom(LinkedListBase& other, LinkedNode* node)
{
    LinkedNode* tail = other.tail_;
    other.extractFrom(node);
    pushBack(node, tail);
}


void LinkedListBase::spliceFrontTo(LinkedListBase& other, LinkedNode* node)
{
    LinkedNode* head = other.head_;
    other.extractTo(node);
    pushFront(head, node);
}

void LinkedListBase::spliceBackTo(LinkedListBase& other, LinkedNode* node)
{
    LinkedNode* head = other.head_;
    other.extractTo(node);
    pushBack(head, node);
}

bool LinkedListBase::toTop(LinkedNode* node)
{
    if (node!=head_)
    {
         node->prev_->next_ = node->next_;
        if (node->next_)
        {
            node->next_->prev_ = node->prev_;
        }
        if (tail_==node)
        {
            tail_ = node->prev_;
        }
        node->next_ = head_;
        node->prev_ = nullptr;
        head_ = node;
        return true;
    }
    return false;
}

bool LinkedListBase::toBottom(LinkedNode* node)
{
    if (node!=tail_)
    {
         node->next_->prev_ = node->prev_;
        if (node->prev_)
        {
            node->prev_->next_ = node->next_;
        }
        if (head_==node)
        {
            head_ = node->next_;
        }
        node->prev_ = tail_;
        node->next_ = nullptr;
        tail_ = node;
        return true;
    }
    return false;
}

bool LinkedListBase::lift(LinkedNode* node)
{
    if (node!=head_)
    {
         node->prev_->next_ = node->next_;
        if (node->next_)
        {
            node->next_->prev_ = node->prev_;
        }
        if (tail_==node)
        {
            tail_ = node->prev_;
        }
        insert(node->prev_, node);
        return true;
    }
    return false;
}

bool LinkedListBase::lower(LinkedNode* node)
{
    if (node!=tail_)
    {
         node->next_->prev_ = node->prev_;
        if (node->prev_)
        {
            node->prev_->next_ = node->next_;
        }
        if (head_==node)
        {
            head_ = node->next_;
        }
        append(node->next_, node);
        return true;
    }
    return false;
}

bool LinkedListBase::placeBefore(LinkedNode* position, LinkedNode* node)
{
    if (node!=position)
    {
        extract(node);
        insert(position, node);
        return true;
    }
    return false;
}

bool LinkedListBase::placeAfter(LinkedNode* position, LinkedNode* node)
{
    if (node!=position)
    {
        extract(node);
        append(position, node);
        return true;
    }
    return false;
}

bool LinkedListBase::swapPlace(LinkedNode* node1, LinkedNode* node2)
{
    if (node1==node2) return false;
    if (node1->next_==node2) return placeBefore(node1, node2);
    if (node2->next_==node1) return placeBefore(node2, node1);
    LinkedNode* position = node1->next_;
    extract(node1);
    insert(node2, node1);
    extract(node2);
    insert(position, node2);
    return true;
}

int LinkedListBase::compare(LinkedNode* node1, LinkedNode* node2)
{
    if (node1==node2) return 0;
    if (node1==head_) return -1;
    if (node1==tail_) return 1;
    for (LinkedNode* node = head_; node; node = node->next_)
    {
        if (node==node1) return -1;
        if (node==node2) return 1;
    }
    return std::numeric_limits<int>::min();
}

void LinkedListBase::sort(predicate pred, LinkedNode* from, LinkedNode* to)
{
    for (LinkedNode* node1 = from; node1 && node1!=to; node1 = node1->next_)
    {
        LinkedNode* node2 = node1->next_;
        while (node2 && node2!=to)
        {
            if (pred(node2, node1))
            {
                LinkedNode* next = extract(node2);
                insert(node1,node2);
                node1 = node2;
                node2 = next;
            }
            else
            {
                node2 = node2->next_;
            }
        }
    }
}

void LinkedListBase::sort(predicate pred)
{
    sort(pred, head_, nullptr);
}

void LinkedListBase::merge(predicate pred, LinkedNode* node)
{
    LinkedNode* n = head_;
    while (node && pred(n, node)) n=n->next_;
    if (node->next_)
    {
        node->next_->prev_ = node->prev_;
    }
    if (node->prev_)
    {
        node->prev_->next_ = node->next_;
    }
    insert(n,node);
}

void LinkedListBase::merge(predicate pred, LinkedNode* from, LinkedNode* to)
{
    if (!head_)
    {
        head_ = from;
        if (to)
        {
            tail_ = to;
        }
        else
        {
            tail_ = from;
            while (tail_->next_) tail_ = tail_->next_;
        }
        return;
    }
    LinkedNode* n1 = head_;
    LinkedNode* n2 = from;
    while (n1 && n2)
    {
        if (pred(n2, n1))
        {
            auto next = n2->next_;
            insert(n1, n2);
            n2 = next;
        }
        else
        {
            n1=n1->next_;
        }
    }
}

void LinkedListBase::merge(predicate pred, LinkedListBase& other)
{
    auto node = other.extract();
    merge(pred, node, nullptr);
}

void LinkedListBase::merge(predicate pred, LinkedListBase& other, LinkedNode* from, LinkedNode* to)
{
    auto node = other.extract(from, to);
    merge(pred, node, nullptr);
}

void LinkedListBase::reverse() noexcept
{
    LinkedNode *current = head_;  
    std::swap(head_, tail_);

    while (current != nullptr)  
    {  
        LinkedNode * temp = current->prev_;  
        current->prev_ = current->next_;  
        current->next_ = temp;              
        current = current->prev_;  
    }  
}

#if 0
/// func ----------------------- remove  ---------------------------
/// \brief to take out this node from the list.
/// \return  the next node after remove in the list
void PooledLinkNode::extract ()
{
	PooledLinkNode *hr;

	if (!this->next_) return nullptr;
    if (next_)
    {
        next_->prev_ = prev_;
    }
    if (prev_)
    {
        prev_->next_ = next_;
    }
    next_ = nullptr;
    prev_ = nullptr;
	return hr;
}

// func ----------------------- NRemove  ---------------------------
// purpose: to take out some nodes from this node in the ring.
// input:   nodes: number of nodes to delete
// output:  the first node after remove in the ring
RingNode * RingNode::NRemove (int nodes)
{
    RingNode * node = this;
    RingNode * nodeTemp;

    do
    {  nodeTemp = node;
       node = node->next;
       nodes--;
    } while (nodes>0 && node && node != this);

    if (node==this) node=0;   // the ring becomes empty
    else                      // adjust the links of the remaining ring
    {   this->prev->next = nodeTemp->next;
	nodeTemp->next->prev = this->prev;
    }

    this->prev = nodeTemp;
    nodeTemp->next = this;

    return node;
};


// func ----------------------- Append  ---------------------------
// purpose: to append new ring into the ring headed by this node.
// input:   new_node: the first node in the new ring
void RingNode::Append (RingNode *new_node)
{      RingNode * temp;

       this->prev->next = new_node;
       new_node->prev->next = this;
       temp = this->prev;
       this->prev = new_node->prev;
       new_node->prev = temp;
};

// func ----------------------- Insert  ---------------------------
// purpose: to insert a new ring into the ring headed by this node.
// input:   new_node: the first node in the new ring
// output:  the new head in the ring
RingNode * RingNode::Insert (RingNode *new_node)
{
	if (!new_node) return this;
	Append (new_node);
	return new_node;
};

// func ----------------------- NodeNumber  -------------------------
// purpose: to count the node number in the ring.
// output:  the node number in the ring
cardinal RingNode::NodeNumber ()
{     RingNode * h =this;
      cardinal num =0;
      do
      {	 h = h->next;
	 num++;
      }  while (h != this);
      return num;
};

// func ----------------------- Free  ----------------------------
// purpose: to free all the nodes in the ring headed by the given node.
void RingNode::Free (RingNode * node)
{
	node->Clearance();
	delete node;
}

// func ----------------------- FreeChain  ----------------------------
// purpose: to free all the nodes in the ring headed by the given node.
void RingNode::FreeChain (RingNode * head)
{
	RingNode * node;
	RingNode * temp;

	head = head->prev;
	node = head;
	do
	{ temp = node->prev;
	  RingNode::Free (node);
	  node = temp;
	} while (node!=head);
}
#endif

}
