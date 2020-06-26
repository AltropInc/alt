add_library (alt_util
    types/Enum.cpp
    types/Clock.cpp
    system/SysConfig.cpp
    system/EventPoller.cpp
    system/Reactor.cpp
    system/TimerQueue.cpp
    system/OS.cpp
    numeric/Intrinsics.cpp
    ipc/SharedMemory.cpp
    storage/FixedMemPool.cpp
    storage/Allocator.cpp
    storage/LinkedList.cpp
    storage/TreeNode.cpp
    storage/RingBuffer.cpp
    storage/CoQueue.cpp
    storage/CircularQueue.cpp
    string/StrUtils.cpp
    string/StrScan.cpp
    string/StreamParser.cpp
    string/JsonParser.cpp
    string/XmlParser.cpp
    string/StrPool.cpp
    net/IPAddress.cpp
    net/SocketAddress.cpp
    net/Socket.cpp
    net/StreamConnection.cpp
    ipc/Thread.cpp


)

add_subdirectory(unittest)


