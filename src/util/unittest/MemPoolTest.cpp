#include <util/storage/Allocator.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <assert.h>
#include <vector>

namespace alt
{
    class MemTest
    {
        static int s_instance_cnt_;
        public:
            MemTest( int val) : value_(val)
            {
                ++s_instance_cnt_;
            }
            ~MemTest()
            {
                --s_instance_cnt_;
            }
            int getValue() const { return value_; }
            static int instanceCount() { return s_instance_cnt_; }
        private:
            int value_ { 0 };
    };
    int MemTest::s_instance_cnt_ = 0;
}
TEST_CASE( "PooledAllocator", "[PooledAllocator]" )
{
    alt::PooledAllocator& mem_pool = alt::PooledAllocator::instance();
    size_t count = mem_pool.getTracker().getTotalCount();
    void * p = alt_malloc(16);
    REQUIRE(p!=nullptr);
    REQUIRE(mem_pool.getTracker().getTotalCount()==count+1);
    //mem_pool.getTracker().reportMostUsed();
    alt_free(p);
    REQUIRE(mem_pool.getTracker().getTotalCount()==count);
    //mem_pool.getTracker().reportMostUsed();
    auto test = alt_new(alt::MemTest, 2);
    REQUIRE(test->getValue()==2);
    REQUIRE(mem_pool.getTracker().getTotalCount()==count+1);
    //mem_pool.getTracker().reportMostUsed();
    REQUIRE(alt::MemTest::instanceCount()==1);

    std::vector<alt::MemTest*> vec(10, nullptr);
    for (int i = 0; i<10; ++i)
    {
        vec[i] = alt_new(alt::MemTest, i);
    }
    REQUIRE(alt::MemTest::instanceCount()==11);
    //mem_pool.getTracker().reportMostUsed();
    REQUIRE(mem_pool.getTracker().getTotalCount()==count+11);
    for (int i = 0; i<10; ++i)
    {
        alt_del(alt::MemTest, vec[i]);
    }
    //mem_pool.getTracker().reportMostUsed();
    REQUIRE(alt::MemTest::instanceCount()==1);
    REQUIRE(mem_pool.getTracker().getTotalCount()==count+1);
    alt_del(alt::MemTest, test);
    REQUIRE(alt::MemTest::instanceCount()==0);
    REQUIRE(mem_pool.getTracker().getTotalCount()==count);

}


