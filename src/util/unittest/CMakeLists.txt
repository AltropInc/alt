add_executable (UtilTest
    EnumTest.cpp
    StrScanTest.cpp
    MemPoolTest.cpp
    LinkedListTest.cpp
    PooledLinkListTest.cpp
    StringHashMapTest.cpp
    QueueTest.cpp
    TreeNodeTest.cpp
    NamedTreeNodeTest.cpp
    RingBufferTest.cpp
    SortedArrayTest.cpp
)
target_link_libraries (UtilTest PRIVATE alt_util)
