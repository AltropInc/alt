#include <util/storage/SideBuckets.h>
#include <util/storage/SortedArray.h>
#include <catch2/catch.hpp>
#include <sstream>
#include <iostream>

TEST_CASE( "SortedArray", "[SortedArray]" )
{
    alt::SortedArray<int> sorted_array;
    sorted_array.insert(3);
    int x = 5;
    sorted_array.insert(x, true);
    sorted_array.insert(4, true);
    sorted_array.insert(1, true);
    sorted_array.insert(8, true);
    sorted_array.insert(-1, true);
    sorted_array.insert(4, true);
    
    REQUIRE(sorted_array.find(-1)==6);
    REQUIRE(sorted_array.find(1)==7);
    REQUIRE(sorted_array.find(3)==8);
    REQUIRE(sorted_array.find(4)==9);
    REQUIRE(sorted_array.find(5)==10);
    REQUIRE(sorted_array.find(8)==11);
    // sorted_array.print();
}

TEST_CASE( "SideBuckets", "[SideBuckets]" )
{
    using Price_t = int64_t;     // in number of ticks
    struct QtyCnt
    {
        int64_t      qty_       : 32;
        int64_t      order_cnt_ : 32;
        QtyCnt() : qty_{0}, order_cnt_{0} {}
        QtyCnt(int32_t qty, int32_t cnt) : qty_{qty}, order_cnt_{cnt} {}
        std::string toStr() const
        {
            std::ostringstream ostr; 
            ostr << '(' << qty_ << ',' << order_cnt_ << ')';
            return ostr.str();
        } 
        void add(const QtyCnt& added)
        {
            qty_ += added.qty_;
            if (qty_ < 0) qty_ = 0;
            order_cnt_ += added.order_cnt_;
            if (order_cnt_ < 0) order_cnt_ = 0;
        }
        void update(const QtyCnt& upd)
        {
            qty_ = upd.qty_;
            order_cnt_ = upd.order_cnt_;
        }
        bool reset()
        {
            qty_=0;
            order_cnt_=0;
            return true;
        }
        bool empty() const
        {
            return qty_<=0 && order_cnt_<=0;
        }
        bool isPositive() const
        {
            return qty_>0 || order_cnt_>0;
        }
    };

    bool dump = false;
    alt::SideBuckets<Price_t, QtyCnt, alt::SortedBucketCompareDec<Price_t>> buckets(4, 4);
    buckets.add(10, QtyCnt(100,1));   if (dump) buckets.print();
    buckets.add(11, QtyCnt(200,2));   if (dump) buckets.print();
    buckets.add(9, QtyCnt(10,1));     if (dump) buckets.print();
    buckets.add(9, QtyCnt(20,2));     if (dump) buckets.print();
    buckets.add(15, QtyCnt(200,2));   if (dump) buckets.print();
    buckets.add(13, QtyCnt(200,2));   if (dump) buckets.print();
    buckets.add(9, QtyCnt(-30,-3));   if (dump) buckets.print();
    buckets.add(11, QtyCnt(200,2));   if (dump) buckets.print();
    buckets.add(10, QtyCnt(200,2));   if (dump) buckets.print();
    buckets.add(13, QtyCnt(200,2));   if (dump) buckets.print();
    buckets.add(14, QtyCnt(200,2));   if (dump) buckets.print();
    buckets.add(14, QtyCnt(-100,-1)); if (dump) buckets.print();
    buckets.add(14, QtyCnt(-100,-1)); if (dump) buckets.print();
    buckets.add(9, QtyCnt(10,1));     if (dump) buckets.print();
    buckets.add(8, QtyCnt(10,1));     if (dump) buckets.print();
    buckets.add(12, QtyCnt(10,1));    if (dump) buckets.print();
    buckets.add(9, QtyCnt(-10,-1));   if (dump) buckets.print();
    buckets.add(16, QtyCnt(10,1));    if (dump) buckets.print();
    buckets.add(17, QtyCnt(10,1));    if (dump) buckets.print();
    buckets.add(18, QtyCnt(10,1));    if (dump) buckets.print();
    auto iter = buckets.begin();
    REQUIRE((*iter).first == 18);
    REQUIRE((*iter).second.qty_ == 10);
    REQUIRE((*iter).second.order_cnt_ == 1);
    ++iter;
    REQUIRE((*iter).first == 17);
    REQUIRE((*iter).second.qty_ == 10);
    REQUIRE((*iter).second.order_cnt_ == 1);
    ++iter;
    REQUIRE((*iter).first == 16);
    REQUIRE((*iter).second.qty_ == 10);
    REQUIRE((*iter).second.order_cnt_ == 1);
    ++iter;
    REQUIRE((*iter).first == 15);
    REQUIRE((*iter).second.qty_ == 200);
    REQUIRE((*iter).second.order_cnt_ == 2);
    ++iter;
    REQUIRE((*iter).first == 13);
    REQUIRE((*iter).second.qty_ == 400);
    REQUIRE((*iter).second.order_cnt_ == 4);
    ++iter;
    REQUIRE((*iter).first == 12);
    REQUIRE((*iter).second.qty_ == 10);
    REQUIRE((*iter).second.order_cnt_ == 1);
    ++iter;
    REQUIRE((*iter).first == 11);
    REQUIRE((*iter).second.qty_ == 400);
    REQUIRE((*iter).second.order_cnt_ == 4);
    ++iter;
    REQUIRE((*iter).first == 10);
    REQUIRE((*iter).second.qty_ == 300);
    REQUIRE((*iter).second.order_cnt_ == 3);
    ++iter;
    REQUIRE((*iter).first == 8);
    REQUIRE((*iter).second.qty_ == 10);
    REQUIRE((*iter).second.order_cnt_ == 1);
}


