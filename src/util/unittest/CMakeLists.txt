add_executable (UtilTest
    EnumTest.cpp
    StrScanTest.cpp
    MemPoolTest.cpp
    LinkedListTest.cpp
    PooledLinkListTest.cpp
    StringHashMapTest.cpp
    CoQueueTest.cpp
    TimerQueueTest.cpp
    TreeNodeTest.cpp
    NamedTreeNodeTest.cpp
    RingBufferTest.cpp
    SortedArrayTest.cpp
    JsonParserTest.cpp
    XmlParserTest.cpp
)
target_link_libraries (UtilTest PRIVATE alt_util)
