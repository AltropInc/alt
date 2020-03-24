
#include "SharedMemory.h"
//#include "Logger.h"
#include <util/numeric/Intrinsics.h>
#include <util/sysinfo/SysConfig.h>

// Must use -D_GNU_SOURCE compiler flags for cygwin
#include <sys/features.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace alt;

SharedMemory::SharedMemory(
    const std::string& name,
    bool is_master
)
    : name_ (name)
    , handle_(-1)
    , is_master_(is_master)
    , total_size_(0)
    , payload_size_ (0)
    , address_ ( nullptr )
    , header_ ( nullptr )
    , payload_ ( nullptr )
    , is_new_ (false)
{        
}

SharedMemory::~SharedMemory()
{
    release();
}

void SharedMemory::setReady(bool ready)
{
    if (!is_master_ || !address_)
    {
        return;
    }
    if (ready)
    {
        header_->flags_.store(1, std::memory_order_release);
    }
    else
    {
        header_->flags_.store(0, std::memory_order_release);
    }
}

void SharedMemory::release()
{
    if (address_)
    {
        if (is_master_)
        {
            setReady(false);
        }
        ::munmap(address_, total_size_);
        address_ = nullptr;
    }
	if (handle_ != -1)
	{
		::close(handle_);
		handle_ = -1;
	}
	//if (!persistent_ && is_master_)
	//{
	//	::shm_unlink(shm_name_.c_str());
	//}
}

int SharedMemory::acquire (SM_Mode mode, SM_Access access, std::size_t size)
{
    size_t header_size = constAlign(sizeof(SM_Header), SysConfig::instance().cache_line_size_);
    payload_size_ = size;
    total_size_ = header_size + payload_size_ + SysConfig::instance().cache_line_size_;

    if (!is_master_ && mode!=SM_Mode::SM_OpenOnly)
    {
        // To avoid multiple processes racing to create a shared memory
        // segment with the same name, we want to have the owner process
        // responsible for creating a segment of name.
        // ERR_LOG("CORE", "Only master can create shared memory " << name_);
        return -1;
    }

    shm_name_ = "/" + name_;

	int flags = 0;
 	if (access == SM_Access::SM_ReadWrite)
    {
		flags |= O_RDWR;
    }
	else
    {
		flags |= O_RDONLY;
    }

    is_new_ = false;
    //DBG_LOG("CORE", "Open Share Memory " << name_);
	handle_ = ::shm_open(shm_name_.c_str(), flags, S_IRUSR | S_IWUSR);

    if (handle_ == -1 && (mode==SM_Mode::SM_OpenOrCreate || mode==SM_Mode::SM_CreateOnly))
    {
        flags |= O_CREAT;
        if (mode==SM_Mode::SM_CreateOnly)
        {
            flags = O_CREAT | O_EXCL;
        }
        //DBG_LOG("CORE", "Create Share Memory " << name_);
	    handle_ = ::shm_open(shm_name_.c_str(), flags, S_IRUSR | S_IWUSR);
        is_new_ = true;
    }

	if (handle_ == -1)
    {
        //SYS_ERR_LOG("CORE",
        //            "Failed to open or create shared memory " << name_);
        return -1;
    }

	if (is_master_)
    {
        if (::ftruncate(handle_, total_size_) != 0)
        {
            ::close(handle_);
            handle_ = -1;
            //::shm_unlink(name.c_str());
            //SYS_ERR_LOG("CORE",
            //            "Failed to set size shared memory " << name_);
        }
    }

	int access_flag = PROT_READ;
	if (access == SM_Access::SM_ReadWrite)
		access_flag |= PROT_WRITE;

	address_ = ::mmap(nullptr, total_size_, access_flag, MAP_SHARED, handle_, 0);
	if (address_ == MAP_FAILED)
    {
        ::close(handle_);
        handle_ = -1;
        address_ = nullptr;
        //SYS_ERR_LOG("CORE",
        //       "Failed to map address to shared memory " << name_);
        return -1;
    }

	char* addr = static_cast<char*>(address_);
    char* aligned_addr = constAlign(addr, SysConfig::instance().cache_line_size_);
    header_ = reinterpret_cast<SM_Header*>(aligned_addr);
    payload_ = aligned_addr + header_size;
    if (is_master_)
    {
        setReady(true);
    }
}


