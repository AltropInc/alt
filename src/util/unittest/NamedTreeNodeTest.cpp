#include <util/storage/NamedTreeNode.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <assert.h>
#include <vector>

namespace alt
{
    class MyNamedNode: public PooledNamedNode
    {
        static int s_instance_cnt_;
        public:
            MyNamedNode(const char *name, PooledNamedNode* parent, bool is_name_register,
                        const std::string & val)
                    : PooledNamedNode(name,parent,is_name_register)
                    , value_(val)
            {
                ++s_instance_cnt_;
            }
            ~MyNamedNode()
            {
                --s_instance_cnt_;
            }
            const std::string& getValue() const { return value_; }
            static int instanceCount() { return s_instance_cnt_; }
        private:
            std::string value_;
    };
    int MyNamedNode::s_instance_cnt_ = 0;
}

TEST_CASE( "NamedNodeTest", "[PooledNamedNode]" )
{
    //alt::PooledAllocator& mem_pool = alt::PooledAllocator::instance();
    //std::cout << "NamedNodeTest create root" << std::endl;
    alt::MyNamedNode*  root = alt::PooledTreeNode::create<alt::MyNamedNode>
        ("root", nullptr, true, "ROOT");
    //REQUIRE(alt::MyNamedNode::instanceCount() == 1);
    REQUIRE(root->getValue()=="ROOT");
    REQUIRE(std::string(root->name())=="root");
    //mem_pool.getTracker().reportMostUsed();
    //std::cout << "NamedNodeTest create chd1" << std::endl;
    alt::MyNamedNode*  chd1 = alt::PooledTreeNode::create<alt::MyNamedNode>
        ("chd1", root, false, "CHD1");
    //::cout << "NamedNodeTest create chd2" << std::endl;
    alt::MyNamedNode*  chd2 = alt::PooledTreeNode::create<alt::MyNamedNode>
        ("chd2", root, false, "CHD2");
    //std::cout << "NamedNodeTest create chd3" << std::endl;
    alt::MyNamedNode*  chd3 = alt::PooledTreeNode::create<alt::MyNamedNode>
        ("chd3", root, false, "CHD3");
    alt::PooledNamedNode*  parent = root->search("root");
    REQUIRE(parent == root);
    //std::cout << "NamedNodeTest create chd4" << std::endl;
    alt::MyNamedNode*  chd4 = root->create<alt::MyNamedNode>
        ("chdx", root, false, "CHD4");
    chd4->rename("chd4");
    alt::PooledNamedNode*  chd = root->search("chd4");
    REQUIRE(chd == chd4);
    REQUIRE(root->childrenNum() == 4);
}


