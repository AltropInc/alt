#include <util/storage/CoQueue.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <assert.h>
#include <vector>

namespace alt
{
    class MyQueueEntry: public CoQueueBase::EntryBase
    {
        static int s_instance_cnt_;
        public:
            MyQueueEntry( int val) : value_(val)
            {
                ++s_instance_cnt_;
            }
            ~MyQueueEntry()
            {
                --s_instance_cnt_;
            }
            int getValue() const { return value_; }
            static int instanceCount() { return s_instance_cnt_; }
        private:
            int value_ { 0 };
    };
    int MyQueueEntry::s_instance_cnt_ = 0;
    class OtherQueueEntry
    {   public:
            OtherQueueEntry( int val) : value_(val)
            {
            }
        private:
            int value_ { 0 };
    };
}
TEST_CASE( "QueueTest", "[Queue]" )
{
    alt::CoQueue  testQueue;
    testQueue.enqueue<alt::MyQueueEntry>(1);
    //testQueue.enqueue<alt::OtherQueueEntry>(1);
}


