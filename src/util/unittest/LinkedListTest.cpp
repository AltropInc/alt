#include <util/storage/LinkedList.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <assert.h>
#include <vector>

namespace alt
{
    class LinkedListNode: public LinkedNode
    {
        static int s_instance_cnt_;
        public:
            LinkedListNode( int val) : value_(val)
            {
                ++s_instance_cnt_;
            }
            ~LinkedListNode()
            {
                --s_instance_cnt_;
            }
            int getValue() const { return value_; }
            static int instanceCount() { return s_instance_cnt_; }
        private:
            int value_ { 0 };
    };
    int LinkedListNode::s_instance_cnt_ = 0;
}
TEST_CASE( "Basic LinkedListBase Test", "[LinkedListBase]" )
{
    alt::LinkedListBase  testlist;
    alt::LinkedListNode* node = new alt::LinkedListNode(1);
    testlist.pushBack(node);
    // Make sure the constructor is called
    REQUIRE(alt::LinkedListNode::instanceCount() == 1);
    REQUIRE(testlist.size()==1);
    REQUIRE(testlist.front()!=nullptr);
    REQUIRE(testlist.front()==testlist.back());
    auto front = testlist.extractFront();
    REQUIRE(testlist.size()==0);
    REQUIRE(front==node);
    delete front;
    REQUIRE(testlist.back()==nullptr);
    REQUIRE(testlist.front()==nullptr);

    node = new alt::LinkedListNode(2);
    testlist.pushBack(node);
    REQUIRE(testlist.back()==node);
    REQUIRE(testlist.front()==node);
    auto node2 = new alt::LinkedListNode(1);
    testlist.pushFront(node2);
    REQUIRE(testlist.back()==node);
    REQUIRE(testlist.front()==node2);

    int value = 1;
    for (auto node: testlist)
    {
        REQUIRE(static_cast<alt::LinkedListNode*>(node)->getValue() == value);
        ++value;
    }

    value = 2;
    for (auto riter = testlist.rbegin(); riter!=testlist.rend(); --riter)
    {
        REQUIRE(static_cast<alt::LinkedListNode*>(*riter)->getValue() == value);
        --value;
    }

    auto next = testlist.extract(node2);
    REQUIRE(testlist.size()==1);
    REQUIRE(next==node);
    REQUIRE(testlist.back()==node);
    delete node2;
    auto back = testlist.extractBack();
    REQUIRE(back==node);
    REQUIRE(testlist.size()==0);
    delete back;
    REQUIRE(alt::LinkedListNode::instanceCount()==0);
}

TEST_CASE( "LinkedListBase Insert Append Test", "[LinkedListBase]" )
{
    alt::LinkedListBase  testlist;
    alt::LinkedListNode* node1 = new alt::LinkedListNode(1);
    alt::LinkedListNode* node2 = new alt::LinkedListNode(2);
    alt::LinkedListNode* node3 = new alt::LinkedListNode(3);
    alt::LinkedListNode* node4 = new alt::LinkedListNode(4);
    testlist.pushBack(node1);
    testlist.pushBack(node2);
    testlist.pushBack(node3);
    testlist.insert(node2, node4);
    REQUIRE(node1->next_==node4);
    REQUIRE(node4->prev_==node1);
    REQUIRE(node4->next_==node2);
    REQUIRE(node2->prev_==node4);
    REQUIRE(node1->distance(node3)==-3);
    REQUIRE(node3->distance(node1)==3);

    testlist.extract(node4);
    REQUIRE(node1->next_==node2);
    REQUIRE(node2->prev_==node1);

    testlist.append(node2, node4);
    REQUIRE(node2->next_==node4);
    REQUIRE(node4->prev_==node2);
    REQUIRE(node4->next_==node3);
    REQUIRE(node3->prev_==node4);

    testlist.extract(node4);
    REQUIRE(node2->next_==node3);
    REQUIRE(node3->prev_==node2);

    testlist.insert(node1, node4);
    REQUIRE(testlist.front()==node4);
    REQUIRE(node4->prev_==nullptr);
    REQUIRE(node4->next_==node1);
    REQUIRE(node1->prev_==node4);

    testlist.extract(node4);
    REQUIRE(testlist.front()==node1);

    testlist.insert(nullptr, node4);
    REQUIRE(testlist.back()==node4);
    REQUIRE(node4->prev_==node3);
    REQUIRE(node4->next_==nullptr);
    REQUIRE(node3->next_==node4);

    testlist.extract(node4);
    REQUIRE(testlist.back()==node3);

    testlist.append(node3, node4);
    REQUIRE(testlist.back()==node4);
    REQUIRE(node4->prev_==node3);
    REQUIRE(node4->next_==nullptr);
    REQUIRE(node3->next_==node4);

    testlist.extract(node4);
    REQUIRE(testlist.back()==node3);

    testlist.append(nullptr, node4);
    REQUIRE(testlist.front()==node4);
    REQUIRE(node4->prev_==nullptr);
    REQUIRE(node4->next_==node1);
    REQUIRE(node1->prev_==node4);

    testlist.extract(node4);
    REQUIRE(testlist.front()==node1);

    alt::LinkedListNode* node5 = new alt::LinkedListNode(5);
    testlist.append(node4);
    testlist.append(node5);
    testlist.extract(node4, node5);
    REQUIRE(node4->prev_==nullptr);
    REQUIRE(node4->next_==node5);
    REQUIRE(node5->prev_==node4);
    REQUIRE(node5->next_==nullptr);

    testlist.insert(node2, node4, node5);
    REQUIRE(node1->next_==node4);
    REQUIRE(node4->prev_==node1);
    REQUIRE(node4->next_==node5);
    REQUIRE(node5->prev_==node4);
    REQUIRE(node5->next_==node2);
    REQUIRE(node2->prev_==node5);

    testlist.extract(node1, node5);
    REQUIRE(testlist.front()==node2);
    REQUIRE(node1->prev_==nullptr);
    REQUIRE(node1->next_==node4);
    REQUIRE(node4->next_==node5);
    REQUIRE(node5->prev_==node4);
    REQUIRE(node5->next_==nullptr);

    testlist.append(node3, node1, node5);
    REQUIRE(testlist.back()==node5);
    REQUIRE(node3->next_==node1);
    REQUIRE(node1->prev_==node3);

    testlist.extract(node1, node5);
    REQUIRE(testlist.back()==node3);
    testlist.insert(node2, node1, node5);
    REQUIRE(testlist.front()==node1);
    REQUIRE(node1->prev_==nullptr);
    REQUIRE(node5->next_==node2);
    REQUIRE(node2->prev_==node5);

    testlist.extract(node1, node4);
    REQUIRE(testlist.front()==node5);
    testlist.insert(nullptr, node1, node4);
    REQUIRE(testlist.back()==node4);
    REQUIRE(node1->prev_==node3);
    REQUIRE(node3->next_==node1);
    REQUIRE(node4->next_==nullptr);

    testlist.extract(node1, node4);
    REQUIRE(testlist.back()==node3);
    testlist.append(nullptr, node1, node4);
    REQUIRE(testlist.front()==node1);
    REQUIRE(node4->next_==node5);
    REQUIRE(node5->prev_==node4);
    REQUIRE(node1->prev_==nullptr);
    delete node5;
    delete node4;
    delete node3;
    delete node2;
    delete node1;
}

TEST_CASE( "LinkedListBase Sort Merge Test", "[LinkedListBase]" )
{
    alt::LinkedListBase  testlist;
    alt::LinkedListNode* node1 = new alt::LinkedListNode(1);
    alt::LinkedListNode* node2 = new alt::LinkedListNode(2);
    alt::LinkedListNode* node3 = new alt::LinkedListNode(3);
    alt::LinkedListNode* node4 = new alt::LinkedListNode(4);
    testlist.pushFront(node3);
    testlist.pushFront(node2);
    testlist.pushFront(node4);
    testlist.pushFront(node1);
    testlist.sort([](alt::LinkedNode const* n1, alt::LinkedNode const* n2) -> bool
                    { return static_cast<alt::LinkedListNode const*>(n1)->getValue() <
                             static_cast<alt::LinkedListNode const*>(n2)->getValue(); }
    );

    int value = 1;
    for (auto node: testlist)
    {
        REQUIRE(static_cast<alt::LinkedListNode*>(node)->getValue() == value);
        ++value;
    }
    testlist.sort([](alt::LinkedNode const* n1, alt::LinkedNode const* n2) -> bool
                    { return static_cast<alt::LinkedListNode const*>(n1)->getValue() >
                             static_cast<alt::LinkedListNode const*>(n2)->getValue(); }
    );
    value = 4;
    for (auto node: testlist)
    {
        //std::cout << static_cast<alt::LinkedListNode*>(node)->getValue() << std::endl;
        REQUIRE(static_cast<alt::LinkedListNode*>(node)->getValue() == value);
        --value;
    }

    testlist.extract(node1);
    testlist.extract(node2);
    testlist.extract(node3);
    testlist.extract(node4);

    REQUIRE(testlist.empty());

    alt::LinkedListNode* node5 = new alt::LinkedListNode(5);
    alt::LinkedListNode* node6 = new alt::LinkedListNode(6);
    alt::LinkedListNode* node7 = new alt::LinkedListNode(7);

    alt::LinkedListBase  testlist2;
    testlist.pushBack(node1);
    testlist.pushBack(node3);
    testlist.pushBack(node5);
    testlist.pushBack(node7);
    testlist2.pushBack(node2);
    testlist2.pushBack(node4);
    testlist2.pushBack(node6);
    REQUIRE(testlist.size()==4);
    REQUIRE(testlist2.size()==3);

    testlist.merge(
        [](alt::LinkedNode const* n1, alt::LinkedNode const* n2) -> bool
                    { return static_cast<alt::LinkedListNode const*>(n1)->getValue() <
                             static_cast<alt::LinkedListNode const*>(n2)->getValue(); },
        testlist2
    );
    REQUIRE(testlist2.empty());

    value = 1;
    for (auto node: testlist)
    {
        //std::cout << static_cast<alt::LinkedListNode*>(node)->getValue() << std::endl;
        REQUIRE(static_cast<alt::LinkedListNode*>(node)->getValue()==value++);
    }

    testlist2.splice(nullptr, testlist, node5, node7);
    REQUIRE(testlist.size()==4);
    REQUIRE(testlist2.size()==3);

}
