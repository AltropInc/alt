#include <util/storage/TreeNode.h>
#include <util/storage/NamedTreeNode.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <assert.h>
#include <vector>

namespace alt
{
    class MyTreeNode: public PooledTreeNode
    {
        static int s_instance_cnt_;
        public:
            MyTreeNode(const std::string & val) : value_(val)
            {
                ++s_instance_cnt_;
            }
            ~MyTreeNode()
            {
                --s_instance_cnt_;
            }
            const std::string& getValue() const { return value_; }
            static int instanceCount() { return s_instance_cnt_; }
        private:
            std::string value_;
    };
    int MyTreeNode::s_instance_cnt_ = 0;
}

TEST_CASE( "PooledTreeNodeTest", "[PooledTreeNode]" )
{
 

    //alt::PooledAllocator& mem_pool = alt::PooledAllocator::instance();
    alt::MyTreeNode*  root = alt::PooledTreeNode::create<alt::MyTreeNode>("root");
    //REQUIRE(alt::MyTreeNode::instanceCount() == 1);
    REQUIRE(root->getValue()=="root");
    //mem_pool.getTracker().reportMostUsed();
    alt::MyTreeNode* chd1 = root->newChild<alt::MyTreeNode>("chd1");
    alt::MyTreeNode* chd3 = root->newChild<alt::MyTreeNode>("chd3");
    alt::MyTreeNode* chd2 = root->newChild<alt::MyTreeNode>("chd2");
    //REQUIRE(alt::MyTreeNode::instanceCount() == 4);
    REQUIRE(root->childrenNum() == 3);
    REQUIRE(chd3->siblingNum() == 3);
    REQUIRE(root->size() == 4);
    REQUIRE(chd3->eldestSibling() == chd1);
    REQUIRE(chd3->youngestSibling() == chd2);
    REQUIRE(chd3->prevSibling() == chd1);
    REQUIRE(chd3->parent() == root);
    alt::MyTreeNode* chd5 = root->newChildFront<alt::MyTreeNode>("chd5");
    REQUIRE(root->firstChild() == chd5);
    alt::MyTreeNode* chd6 = root->newChildAfter<alt::MyTreeNode>(root->lastChild(), "chd6");
    REQUIRE(root->lastChild() == chd6);
    root->children().lift(chd1);
    REQUIRE(root->firstChild() == chd1);
    REQUIRE(chd1->nextSibling() == chd5);
    REQUIRE(chd5->prevSibling() == chd1);
    REQUIRE(chd5->nextSibling() == chd3);
    root->children().lower(chd3);
    REQUIRE(chd3->prevSibling() == chd2);
    REQUIRE(chd2->nextSibling() == chd3);
    chd5->reposition(chd6);  // place chd5 before chd6
    REQUIRE(chd6->prevSibling() == chd5);
    REQUIRE(chd5->nextSibling() == chd6);
    REQUIRE(chd5->prevSibling() == chd3);
    auto range1 = chd2->siblingRange(2);
    REQUIRE(range1.first == chd2);
    REQUIRE(range1.second == chd5);

    alt::MyTreeNode* chd31 = chd3->newChild<alt::MyTreeNode>("chd31");
    alt::MyTreeNode* chd32 = chd3->newChild<alt::MyTreeNode>("chd32");
    alt::MyTreeNode* chd33 = chd3->newChild<alt::MyTreeNode>("chd33");

    alt::MyTreeNode* chd51 = chd5->newChild<alt::MyTreeNode>("chd51");
    alt::MyTreeNode* chd52 = chd5->newChild<alt::MyTreeNode>("chd52");
    alt::MyTreeNode* chd53 = chd5->newChild<alt::MyTreeNode>("chd53");

    alt::MyTreeNode* chd511 = chd51->newChild<alt::MyTreeNode>("chd511");
    alt::MyTreeNode* chd512 = chd51->newChild<alt::MyTreeNode>("chd512");
    REQUIRE(chd512->root() == root);
    REQUIRE(chd512->isMyParent(chd51) == true);
    REQUIRE(chd51->isMyChild(chd512) == true);
    REQUIRE(chd512->isLeaf() == true);
    REQUIRE(chd5->isMyOffspring(chd512) == true);
    REQUIRE(chd3->isMyOffspring(chd512) == false);
    REQUIRE(chd511->isMyAncestor(chd5) == true);
    REQUIRE(chd53->isMySibling(chd51) == true);
    REQUIRE(chd512->isMySibling(chd512) == true);
    REQUIRE(((alt::MyTreeNode*)chd512->leastCommonAncestor(chd53))->getValue() == "chd5");
    REQUIRE(((alt::MyTreeNode*)chd31->leastCommonAncestor(chd511))->getValue() == "root");

    alt::MyTreeNode* chd513 = chd33->newChild<alt::MyTreeNode>("chd513");
    chd513->reparent(chd51, nullptr);
    REQUIRE(chd513->parent() == chd51);
    REQUIRE(chd512->nextSibling() == chd513);
    REQUIRE(chd513->prevSibling() == chd512);

    chd3->extract();
    REQUIRE(chd31->parent() == root);
    REQUIRE(chd31->prevSibling() == chd2);
    REQUIRE(chd33->nextSibling() == chd5);
    root->wrapChildren(chd3, chd31, chd33);
    REQUIRE(chd31->parent() == chd3);
    REQUIRE(chd3->firstChild() == chd31);
    REQUIRE(chd3->lastChild() == chd33);
    REQUIRE(chd3->prevSibling() == chd2);
    REQUIRE(chd3->nextSibling() == chd5);

    alt::PooledTreeNode::releaseNode(root);
    REQUIRE(alt::MyTreeNode::instanceCount() == 0);

}


