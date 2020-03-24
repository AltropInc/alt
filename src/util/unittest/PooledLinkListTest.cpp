#include <util/storage/LinkedList.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <assert.h>
#include <vector>

namespace alt
{
    class PooledLinkListNode: public LinkedNode
    {
        static int s_instance_cnt_;
        public:
            PooledLinkListNode( int val) : value_(val)
            {
                ++s_instance_cnt_;
            }
            ~PooledLinkListNode()
            {
                --s_instance_cnt_;
            }
            int getValue() const { return value_; }
            static int instanceCount() { return s_instance_cnt_; }
        private:
            int64_t value_ { 0 };
    };
    int PooledLinkListNode::s_instance_cnt_ = 0;
}
TEST_CASE( "PooledLinkListTest", "[PooledLinkList]" )
{
    alt::PooledLinkList  testlist;

    alt::PooledLinkListNode* node = testlist.emplaceBack<alt::PooledLinkListNode>(1);
    // Make sure the constructor is called
    REQUIRE(alt::PooledLinkListNode::instanceCount() == 1);
    REQUIRE(testlist.size()==1);
    REQUIRE(testlist.front()!=nullptr);
    REQUIRE(testlist.front()==testlist.back());
    auto next = testlist.popFront();
    REQUIRE(testlist.size()==0);
    // Make sure the destructor is called
    REQUIRE(alt::PooledLinkListNode::instanceCount() == 0);
    REQUIRE(testlist.back()==nullptr);
    REQUIRE(testlist.front()==nullptr);

    //std::cout << "testlist.emplaceFront(2)" << std::endl;
    node = testlist.emplaceFront<alt::PooledLinkListNode>(2);
    REQUIRE(testlist.back()==node);
    REQUIRE(testlist.front()==node);
    //std::cout << "testlist.emplaceFront(1)" << std::endl;
    auto node2 = testlist.emplaceFront<alt::PooledLinkListNode>(1);
    REQUIRE(testlist.back()==node);
    REQUIRE(testlist.front()==node2);

    int value = 1;
    for (auto node: testlist)
    {
        REQUIRE(static_cast<alt::PooledLinkListNode*>(node)->getValue() == value);
        ++value;
    }

    value = 2;
    for (auto riter = testlist.rbegin(); riter!=testlist.rend(); --riter)
    {
        REQUIRE(static_cast<alt::PooledLinkListNode*>(*riter)->getValue() == value);
        --value;
    }

    next = testlist.erase(node2);
    REQUIRE(testlist.size()==1);
    REQUIRE(next==node);
    REQUIRE(testlist.back()==node);
    testlist.popBack();
    REQUIRE(testlist.size()==0);
    REQUIRE(alt::PooledLinkListNode::instanceCount() == 0);
}


