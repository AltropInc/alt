add_library (alt_util
    types/Enum.cpp
    sysInfo/SysConfig.cpp
    numeric/Intrinsics.cpp
    ipc/SharedMemory.cpp
    storage/FixedMemPool.cpp
    storage/Allocator.cpp
    storage/LinkedList.cpp
    storage/TreeNode.cpp
    storage/RingBuffer.cpp
    storage/Queue.cpp
    storage/CircularQueue.cpp
    string/StrUtils.cpp
    string/StrScan.cpp
    string/StreamParser.cpp
    string/StrPool.cpp
)

add_subdirectory(unittest)


