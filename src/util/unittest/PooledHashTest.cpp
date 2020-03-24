#include <util/storage/PooledHash.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <assert.h>
#include <vector>

namespace alt
{
    class PooledHashNode: public PooledHashEntryBase
    {
        static int s_instance_cnt_;
        public:
            PooledHashNode( int val) : value_(val)
            {
                ++s_instance_cnt_;
            }
            ~PooledHashNode()
            {
                --s_instance_cnt_;
            }
            int getValue() const { return value_; }
            static int instanceCount() { return s_instance_cnt_; }
        private:
            int value_ { 0 };
    };
    int PooledHashNode::s_instance_cnt_ = 0;
}
TEST_CASE( "PooledHashTest", "[PooledHashTest]" )
{
    alt::PooledHash  testhash;
    alt::PooledHashNode* node = testhash.emplace<alt::PooledHashNode>(1);
}


