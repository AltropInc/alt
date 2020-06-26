#include <util/storage/RingBuffer.h>
#include <util/string/StrBuffer.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <assert.h>
#include <vector>

TEST_CASE( "RingBufferTest", "[RingBufferTest]" )
{
    alt::RingBuffer buffer(32);
    size_t read_len;
    buffer.write ("0123456789", 10);
    buffer.write ("abcdefghij", 10);
    buffer.write ("klmnopqrst", 10);

    char read_buffer[13];
    read_len = buffer.read (read_buffer, 12);
    REQUIRE(alt::StrRefInLength(read_buffer, read_len)=="0123456789ab");

    buffer.write ("uvwxyz", 6);

    read_len = buffer.read (read_buffer, 12);
    read_buffer[read_len] = '\0';
    REQUIRE(alt::StrRefInLength(read_buffer, read_len)=="cdefghijklmn");

    struct iovec iov_arr[2];
    std::span<iovec,2> iov(iov_arr);
    read_len = buffer.fetch(iov,  10);
    REQUIRE(alt::StrRefInLength((const char*)iov[0].iov_base, iov[0].iov_len)=="opqrstuv");
    REQUIRE(alt::StrRefInLength((const char*)iov[1].iov_base, iov[1].iov_len)=="wx");
    buffer.commitRead(0);

    buffer.write ("0123456789", 10);
    read_len = buffer.read (read_buffer, 12);
    read_buffer[read_len] = '\0';
    REQUIRE(alt::StrRefInLength(read_buffer, read_len)=="yz0123456789");
}


TEST_CASE( "RingMsgBuffer", "[RingMsgBufferTest]" )
{
    alt::RingMsgBuffer<uint16_t> buffer(32);
    size_t read_len;
    buffer.write ("0123456789", 10);
    buffer.write ("abcdefghij", 10);

    char read_buffer[32];
    read_len = buffer.read (read_buffer);
    REQUIRE(alt::StrRefInLength(read_buffer, read_len)=="0123456789");

    buffer.write ("klmno", 5);
    buffer.write ("pqrstuvw", 8);

    read_len = buffer.read (read_buffer);
    REQUIRE(alt::StrRefInLength(read_buffer, read_len)=="abcdefghij");

    buffer.write ("xyz", 3);

    read_len = buffer.read (read_buffer);
    REQUIRE(alt::StrRefInLength(read_buffer, read_len)=="klmno");

    read_len = buffer.read (read_buffer);
    //std::cout << "pqrstuvw " << read_len << " " << read_buffer << std::endl;
    REQUIRE(alt::StrRefInLength(read_buffer, read_len)=="pqrstuvw");

    read_len = buffer.read (read_buffer);
    REQUIRE(alt::StrRefInLength(read_buffer, read_len)=="xyz");

    // buffer.write ("uvwxyz", 6);

}


