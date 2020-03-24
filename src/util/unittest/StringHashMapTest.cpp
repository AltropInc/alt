#include <util/storage/StringHashMap.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <assert.h>
#include <vector>

TEST_CASE( "StringHashMapTest", "[StringHashMapTest]" )
{
    //std::cout << "testhash start" << std::endl;
    alt::StringHashMap<std::pair<int64_t, double>>  testhash;
    //std::cout << "testhash.emplace(\"one\")" << std::endl;
    auto res = testhash.emplace("one", std::make_pair(1,1.1));
    //std::cout << "testhash.emplace(\"two\")" << std::endl;
    auto res2 = testhash.emplace("two", std::make_pair(2,1.2));
    //std::cout << "testhash.emplace(\"three\")" << std::endl;
    auto res3 = testhash.emplace("three", std::make_pair(3,1.3));
    //std::cout << "testhash.find(\"one\")" << std::endl;
    auto fres1 = testhash.find("one");
    //std::cout << "testhash.find(\"two\")" << std::endl;
    auto fres2 = testhash.find("two");
    //std::cout << "testhash.find(\"three\")" << std::endl;
    auto fres3 = testhash.find("three");
    REQUIRE(fres1->second.first==1);
    REQUIRE(fres2->second.first==2);
    REQUIRE(fres3->second.first==3);
    //std::cout << "testhash.erase(\"three\")" << std::endl;
    testhash.erase("three");
    //std::cout << "testhash.find(\"three\")" << std::endl;
    fres3 = testhash.find("three");
    REQUIRE(fres3==testhash.end());
    REQUIRE(2==testhash.size());
    //std::cout << "testhash end" << std::endl;
}
/*
ALLOCATE n=1 hint=0 Tsz=24 rebind_size=0
FixedMemPool::newSlab: malloc size=28800 entry_size=32
New rebindAllocator Tsz=16 Usz=24
DELETE ALLOCATOR Tsz=16
New rebindAllocator Tsz=8 Usz=24
ALLOCATE n=3 hint=0 Tsz=8 rebind_size=8
FixedMemPool::newSlab: malloc size=17600 entry_size=16
DELETE ALLOCATOR Tsz=8

ALLOCATE n=1 hint=0 Tsz=24 rebind_size=0
New rebindAllocator Tsz=16 Usz=24
DELETE ALLOCATOR Tsz=16

ALLOCATE n=1 hint=0 Tsz=24 rebind_size=0
New rebindAllocator Tsz=16 Usz=24
DELETE ALLOCATOR Tsz=16

New rebindAllocator Tsz=8 Usz=24
ALLOCATE n=7 hint=0 Tsz=8 rebind_size=8
DELETE ALLOCATOR Tsz=8

New rebindAllocator Tsz=8 Usz=24
DELETE ALLOCATOR Tsz=8
testhash find 1 by one
testhash find 2 by two
testhash find 3 by three
New rebindAllocator Tsz=16 Usz=24
DELETE ALLOCATOR Tsz=16
New rebindAllocator Tsz=16 Usz=24
DELETE ALLOCATOR Tsz=16
New rebindAllocator Tsz=16 Usz=24
DELETE ALLOCATOR Tsz=16
New rebindAllocator Tsz=8 Usz=24
DELETE ALLOCATOR Tsz=8
DELETE ALLOCATOR Tsz=24
*/
