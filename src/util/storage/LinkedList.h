#pragma once

#include <util/sysinfo/Platform.h>
#include "Allocator.h"

#include <functional>

namespace alt
{
class LinkedListBase;

//-----------------------------------------------------------------------------
// Double linked node
//-----------------------------------------------------------------------------
/**
 * \struct LinkedNode
 * \ingroup ContainerUtils
 * \brief A doubly linked node
 */
struct ALT_CORE_PUBLIC LinkedNode
{
    using Pair = std::pair<LinkedNode*, LinkedNode*>;

    LinkedNode*         next_ {nullptr};
    LinkedNode*         prev_ {nullptr};

    LinkedNode* next() { return next_; }
    LinkedNode* prev() { return prev_; }
    const LinkedNode* next() const { return next_; }
    const LinkedNode* prev() const { return prev_; }

    /// \brief extract this node from neighbors
    LinkedNode* extract();

    /// \brief extract linked nodes from this to util node
    /// \param util The last node in the link to be extracted. If it is
    /// nullptr, extract util the
    void extract(LinkedNode* util);
    
    /// \brief Get furtherest neighbor node from the given maximum distance
    LinkedNode* neighbor(int distance);

    /// \brief Get neighbor node from distance, if no node at exact distance,
    /// return nullptr
    LinkedNode* neighborAt(int distance);

    /// \brief Get the last node linked by this node
    LinkedNode* tail();

    /// \brief Get the first node linked by this node
    LinkedNode* head();

    /// \brief Get the last node linked by this node
    /// \param distance output parameter for the distance to tail
    LinkedNode* tail(size_t & distance);

    /// \brief Get the first node linked by this node
    /// \param distance output parameter for the distance to head
    LinkedNode* head(size_t & distance);

    /// \brief Get the first node linked by this node
    bool in(const LinkedListBase* list) const;

    /// \brief Count the number as distance from this to node.
    /// \return positive distance if this is behind node, or negative distance
    /// if this is ahead of node. 0 if this==node. std::numeric_limits<int>::max()
    /// if node is not linked with this
    int distance(LinkedNode* node);

    /// \brief Count the number of nodes linked with this node.
    size_t length() const;

    /// \return Range betwwen this node and the node at the given
    /// distance at most. If the distance is negative, the range is
    /// betwwen the node at the distance and this node.
    Pair range(int distance);

    /// \return Range between the node at distance at most before this node
    /// and the node at distance at most after this node
    Pair range(int dist_before, int dist_after);

    virtual ~LinkedNode() {};
};

//-----------------------------------------------------------------------------
// Base class of a list of double linked nodes
//-----------------------------------------------------------------------------
/**
 * \class LinkedListBase
 * \ingroup ContainerUtils
 * \brief A base class doubly linked list to hold a set of doubly linked nodes.
 * Unlike std::list, this list conatins a set of heterogeneous nodes without
 * using pointer polumorphism, which enables a more efficent memory management.
 */ 
class ALT_CORE_PUBLIC LinkedListBase
{
  protected:
    LinkedNode*  head_   {nullptr};
    LinkedNode*  tail_   {nullptr};

  public:

    template <typename PtrT>
    class IteratorT
    {
        PtrT ptr_;
        IteratorT(PtrT ptr) : ptr_(ptr) {}
        friend class LinkedListBase;
      public:
        IteratorT() : ptr_ {nullptr} {} 
        bool operator!=(const IteratorT& itr) const { return ptr_ != itr.ptr_; } 
        PtrT operator*() const { return ptr_; }
        IteratorT operator++() { if (ptr_) ptr_=ptr_->next_;  return IteratorT(ptr_); }
        IteratorT operator++(int) { auto temp = ptr_; if (ptr_) ptr_=ptr_->next_; return IteratorT(temp); }
        IteratorT operator--() { if (ptr_) ptr_=ptr_->prev_;  return IteratorT(ptr_); }
        IteratorT operator--(int) { auto temp = ptr_; if (ptr_) ptr_=ptr_->prev_; return IteratorT(temp); }
    };
 
    using iterator = IteratorT<LinkedNode*>;
    using const_iterator = IteratorT<LinkedNode const*>;

    iterator begin() { return iterator(head_); };
    iterator end() { return iterator(nullptr); };
    iterator rbegin() { return iterator(tail_); };
    iterator rend() { return iterator(nullptr); };
    const_iterator begin() const { return const_iterator(head_); };
    const_iterator end() const { return const_iterator(nullptr); };
    const_iterator rbegin() const { return const_iterator(tail_); };
    const_iterator rend() const { return const_iterator(nullptr); };

    /// \brief Predicate compare function
    using predicate = std::function<bool(LinkedNode const *, LinkedNode const *)>;

    /// \brief Threeway compare function
    // a <=> b :=
    // if a < b then return -1
    // if a = b then return  0
    // if a > b then return  1
    // if a and b are not comparable then return int_null
    using threeway = std::function<int(LinkedNode const *, LinkedNode const *)>;

    /// \brief Dedfault empty constructor
    LinkedListBase() = default;

    /// \brief Move constructor, other will become empty
    LinkedListBase(LinkedListBase && other);

    /// \brief Copy constructor, disabled
    LinkedListBase(LinkedListBase & other) = delete;

    /// \brief swap contents with other
    void swap(LinkedListBase & other);

    /// \return true if the list is empty
    bool empty() const { return head_==nullptr; }

    /// \return the number of nodes contained in the list
    size_t size() const { return tail_ ? tail_->distance(head_)+1 : 0; }

    /// \return if the list contains the node.
    bool has (const LinkedNode* node) const;

    /// \return The first node of the list
    LinkedNode* front() { return head_; }
    const LinkedNode* front() const { return head_; }

    /// \return The last node in the list
    LinkedNode* back() { return tail_; }
    const LinkedNode* back() const { return tail_; }

    ///@{
    /** Push functions */
    /// \brief append one node at the end of the list
    /// \note The node must be already extracted from a list.
    void pushBack(LinkedNode* node);

    /// \brief append linked nodes from first to last in the link
    /// \note The nodes must be already extracted from a list.
    /// first and last must be linked and first must be equal to or ahead
    /// of the last.
    void pushBack(LinkedNode* first, LinkedNode* last);

    /// \brief append linked nodes from note to the end in the link
    /// \note The nodes must be already extracted from a list.  
    void pushBackFrom(LinkedNode* node);

    /// \brief append linked nodes from start util the note in the link
    /// \note The nodes must be already extracted from a list.
    void pushBackTo(LinkedNode* node);

    /// \brief put one node front into the list
    /// \note The node must be already extracted from a list.
    void pushFront(LinkedNode* node);

    /// \brief put linked nodes front from first to last in the link
    /// \note The nodes must be already extracted from a list.
    /// first and last must be linked and first must be equal to or ahead
    /// of the last.
    void pushFront(LinkedNode* first, LinkedNode* last);

    /// \brief put linked nodes front headed by the node to tail in the link
    /// \note The nodes must be already extracted from a list. 
    void pushFrontFrom(LinkedNode* node);

    /// \brief put linked nodes front from head util the node in the link
    /// \note The nodes must be already extracted from a list.
    void pushFrontTo(LinkedNode* node);
    ///@}
    
    ///@{
    /** Append and insert functions */
    /// \brief insert the note at front
    /// \note The nodes must be already extracted from a list.
    void insert(LinkedNode* node) { return pushFront(node); }

    /// \brief insert the note before the position
    /// \note The nodes must be already extracted from a list.   
    void insert(LinkedNode* position, LinkedNode* node);

    /// \brief insert linked nodes from first to last before the position
    /// \note The nodes must be already extracted from a list.
    /// first and last must be linked and first must be equal to or ahead
    /// of the last.   
    void insert(LinkedNode* position, LinkedNode* first, LinkedNode* last);

    /// \brief insert linked nodes headed by the node before the position
    /// \note The nodes must be already extracted from a list.     
    void insertFrom(LinkedNode* position, LinkedNode* node);

    /// \brief insert linked nodes from thr head until the node before the position
    /// \note The nodes must be already extracted from a list.     
    void insertTo(LinkedNode* position, LinkedNode* node);

    /// \brief append the note at tail
    /// \note The nodes must be already extracted from a list.
    void append(LinkedNode* node) { return pushBack(node); }

    /// \brief append the note after the position
    /// \note The nodes must be already extracted from a list. 
    void append(LinkedNode* position, LinkedNode* node);

    /// \brief append linked nodes from first to last after the position
    /// \note The nodes must be already extracted from a list.
    /// first and last must be linked and first must be equal to or ahead
    /// of the last. 
    void append(LinkedNode* position, LinkedNode* first, LinkedNode* last);

    /// \brief append linked nodes headed by the node after the position
    /// \note The nodes must be already extracted from a list. 
    void appendFrom(LinkedNode* position, LinkedNode* node);
    ///@}

    ///@{
    /** extract functions */
    /// \brief append linked nodes from thr head until the node after the position
    /// \note The nodes must be already extracted from a list.   
    void appendTo(LinkedNode* position, LinkedNode* node);

    /// \brief extract all contained notes 
    LinkedNode* extract();

    /// \brief extract head 
    LinkedNode* extractFront();

     /// \brief extract tail 
    LinkedNode* extractBack();

    /// \brief extract the node    
    LinkedNode* extract(LinkedNode* node);

    /// \brief extract nodes from first to last
    LinkedNode* extract(LinkedNode* first, LinkedNode* last);

    /// \brief extract nodes from the given node to the node in the distance from the node
    /// \note if distance is negative, it is the node before the given node
    LinkedNode* extract(LinkedNode* node, int distance);

    /// \brief extract nodes from the node in the distance before the node to
    /// the node in the distance after the node
    LinkedNode::Pair extract(LinkedNode* node, int dist_before, int dist_after);

    /// \brief extract nodes headed by the given node
    void extractFrom(LinkedNode* node);

    /// \brief extract nodes from head until the given node
    LinkedNode::Pair extractTo(LinkedNode* node);
    ///@}

    ///@{
    /** Splice functions */
    /// \brief transfer all nodes in the other list to this list before the position
    void splice(LinkedNode* position, LinkedListBase& other);

    /// \brief transfer the node in the other list to this list before the position
    void splice(LinkedNode* position, LinkedListBase& other, LinkedNode* node);

    /// \brief transfer the nodes from the first to the last in the other list to this list
    /// before the position
    void splice(LinkedNode* position, LinkedListBase& other, LinkedNode* first, LinkedNode* last);

    /// \brief transfer the nodes from the node to the tail in the other list to this list
    /// before the position
    void spliceFrom(LinkedNode* position, LinkedListBase& other, LinkedNode* node);
    
    /// \brief transfer the nodes from the head to the node in the other list to this list
    /// before the position
    void spliceTo(LinkedNode* position, LinkedListBase& other, LinkedNode* node);

    /// \brief transfer all nodes in the other list to this list at front
    void spliceFront(LinkedListBase& other);

    /// \brief transfer the node in the other list to this list at front
    void spliceFront(LinkedListBase& other, LinkedNode* node);

    /// \brief transfer nodes from the first to the last in the other list to this list at front
    void spliceFront(LinkedListBase& other, LinkedNode* first, LinkedNode* last);

    /// \brief transfer nodes from the node to the tail in the other list to this list at front
    void spliceFrontFrom(LinkedListBase& other, LinkedNode* node);

    /// \brief transfer nodes from the head to the node in the other list to this list at front
    void spliceFrontTo(LinkedListBase& other, LinkedNode* node);

    /// \brief transfer all nodes in the other list to this list at end
    void spliceBack(LinkedListBase& other);

    /// \brief transfer the node in the other list to this list at end
    void spliceBack(LinkedListBase& other, LinkedNode* node);

    /// \brief transfer nodes from the first to the last in the other list to this list at end
    void spliceBack(LinkedListBase& other, LinkedNode* begin, LinkedNode* end);

    /// \brief transfer nodes from the node to the tail in the other list to this list at end
    void spliceBackFrom(LinkedListBase& other, LinkedNode* node);

    /// \brief transfer nodes from the head to the node in the other list to this list at end
    void spliceBackTo(LinkedListBase& other, LinkedNode* node);
    ///@}

    ///@{
    /** Change ordering */
    /// \brief lift the node to the first
    bool toTop(LinkedNode* node);

    /// \brief put the node to the last
    bool toBottom(LinkedNode* node);

    /// \brief lift the node toward the front by one postion
    bool lift(LinkedNode* node);

    /// \brief lower the node toward the back by one postion
    bool lower(LinkedNode* node);

    /// \brief place the node before the position
    bool placeBefore(LinkedNode* position, LinkedNode* node);

    /// \brief place the node after the position
    bool placeAfter(LinkedNode* position, LinkedNode* node);

    /// \brief swap the position of two nodes
    bool swapPlace(LinkedNode* node1, LinkedNode* node2);

    /// \brief reverse order
    void reverse() noexcept;
    ///@}

    ///@{
    /** Compare, sort and merge */
    /// \brief compare the position of node1 and node2.
    /// \return 0 - if node1 and node2 are the same node
    ///         1 - if node1 is after node2
    ///        -1 - if node1 is before node2
    ///        int_null - if undetermined (node1 and node2 are not linked)
    int compare(LinkedNode* node1, LinkedNode* node2);

    /// \brief sort linked nodes between node1 and node2 by pred
    void sort(predicate pred, LinkedNode* node1, LinkedNode* node2);

    /// \brief sort all linked nodes by pred
    void sort(predicate pred);

    /// \brief merge the node into the list by pred.
    /// \note The node must be extracted from a list, and the node will be extracted
    /// from its previous link. All nodes need to be sorted.
    void merge(predicate pred, LinkedNode* node);

    /// \brief merge the nodes from the first to the last into the list by pred.
    /// \note The nodes must be extracted from a list. All nodes need to be sorted.
    void merge(predicate pred, LinkedNode* first, LinkedNode* last);

    /// \brief merge the all nodes from other list into this list by pred.
    void merge(predicate pred, LinkedListBase& other);

    /// \brief merge the nodes from the first to the last in the other list into this list by pred.
    void merge(predicate pred, LinkedListBase& other, LinkedNode* first, LinkedNode* lalst);
    ///@}
};

//-----------------------------------------------------------------------------
// LinkedList
//-----------------------------------------------------------------------------
/**
 * \class LinkedList
 * \ingroup ContainerUtils
 * \brief Template for doubly linked list using the given memory allocation type
 * \tparam Alloc type for memory allocator. The default allocator is the one using
 * C mallc/free. see Allocator.h. 
 */ 
template <class Alloc = Allocator>
class LinkedList: public LinkedListBase
{
    Alloc&      allocator_;

  public:

    LinkedList(Alloc& allocator=Alloc::instance()): allocator_(allocator) {};

    LinkedList(LinkedList && other) :
        LinkedListBase(static_cast<LinkedList&&>(other)), allocator_(other.allocator_)
        {}

    LinkedList(LinkedList & other) = delete;

    template <typename T, typename... Args>
    T* emplaceBack(Args&&... args)
    {
        T* node =  tf_pnew(allocator_, T, std::forward<Args>(args)...);
        pushBack(node);
        return node;
    }

    template <typename T, typename... Args>
    T* emplaceFront(Args&&... args)
    {
        T* node =  tf_pnew(allocator_, T, std::forward<Args>(args)...);
        pushFront(node);
        return node;
    }

    template <typename T, typename... Args>
    T* emplace(LinkedNode* position, Args&&... args)
    {
        T* node =  tf_pnew(allocator_, T, std::forward<Args>(args)...);
        insert(position, node);
        return node;
    }

    template <typename T, typename... Args>
    T* emplaceAfter(LinkedNode* position, Args&&... args)
    {
        T* node =  tf_pnew(allocator_, T, std::forward<Args>(args)...);
        append(position, node);
        return node;
    }

    template <typename T, typename... Args>
    T* create(Args&&... args)
    {
       return tf_pnew(allocator_, T, std::forward<Args>(args)...);
    }

    LinkedNode* erase(LinkedNode* node)
    {
        LinkedNode* next = extract(node);
        tf_pdel(allocator_, LinkedNode, node);
        return next;
    }

    // node is already extracted
    void releaseNode(LinkedNode* node)
    {
        LinkedNode* next{nullptr};
        for (auto n = node; n; n = next)
        {
            next = n->next_; // extract(n);
            tf_pdel(allocator_, LinkedNode, n);
        }
    }

    LinkedNode* erase(LinkedNode* from, LinkedNode* to)
    {
        auto next = extract(from, to);
        releaseNode(from);
        return next;
    }

    LinkedNode* erase(LinkedNode* from, size_t n)
    {
        auto next = extract(from, n);
        releaseNode(from);
        return next;
    }

    LinkedNode* popFront() { return erase(front()); }

    void popBack() { erase(back()); }
    
    void clear()
    {
        auto head = extract();
        releaseNode(head);
    }
};

using PooledLinkList = LinkedList<PooledAllocator>;

}

