#pragma once

#include <util/system/Platform.h>
#include <util/Defs.h>              // for ALT_UTIL_PUBLIC
#include <util/types/Enum.h>
#include <string>
#include <errno.h>
#include <atomic>

namespace alt {

class SharedMemoryImpl;
class File;

ENUM (SM_Mode, uint8_t, SM_CreateOnly, SM_OpenOrCreate, SM_OpenOnly);
ENUM (SM_Access, uint8_t, SM_ReadOnly, SM_ReadWrite);
ENUM (SM_LifeSpan, uint8_t, SM_LifeProcess, SM_LifeSystem);
ENUM (SM_ShareMode, uint8_t, SM_ShareIPC, SM_ShareLocal);

struct MemoryAttrs
{
    bool  is_new_ {true};
    bool  is_for_owner_ {true};
    bool  is_expandable_ {false};
    bool  is_shared_ {false};
};

/**
 * \class SharedMemory
 * \ingroup IPC
 * \brief Shared Memory used for inter process communication
 * SM_Mode: SM_CreateOnly - create a shared memory with a given name
 *               it will fail if the one with the same name exists
 *          SM_OpenOrCreate - open a shared memory with a given name
 *               if it does not exist, create a new
 *          SM_OpenOnly - open a shared memory with a given name,
 *               fail if it does not exist
 * \note Only the owner can use SM_CreateOnly or SM_OpenOrCreate mode.
 * 
 */
class ALT_UTIL_PUBLIC SharedMemory
{
public:

    struct SM_Header
    {
        // A flag to indicate the status of the shared memory
        // Only the master can update the status. 
        std::atomic<uint64_t>   flags_; 
    };

	SharedMemory(const std::string& name, bool is_master);
    
    /// No copy and moving constructor
	SharedMemory(SharedMemory&& moved) = delete;
    SharedMemory(SharedMemory& moved) = delete;
    SharedMemory& operator=(SharedMemory&& moved) = delete;

    SM_LifeSpan lifeSpan() const { return SM_LifeSpan::SM_LifeSystem; }
    SM_ShareMode shareMode() const { return SM_ShareMode::SM_ShareIPC; }

	~SharedMemory();

	int acquire (SM_Mode mode,
                 SM_Access permision,
                 std::size_t size);
    void release ();

    /// Set the ready flag by the owner to indicate the memory is ready
    /// for clients to read
    void setReady(bool ready);

    const char *getName() const { return name_.c_str(); }
    int getHandle() const { return handle_; }

	//void swap(SharedMemory& other);

	char* ptr() const { return payload_; }
	const char* ptr() { return payload_; }
	size_t size() const { return payload_size_; }
	const SM_Header* getHeader() const { return header_; }
    bool isNew() const { return is_new_; }
    bool isMaster() const { return is_master_; }

private:

    std::string    name_;
    std::string    shm_name_;
    //const bool   persistent_;
    int            handle_;
    const bool     is_master_;
    size_t         total_size_;
    size_t         payload_size_;
    void*          address_;
    SM_Header*     header_;
    char*          payload_;
    bool           is_new_;
};

class ALT_UTIL_PUBLIC LocalMemory
{
public:
	LocalMemory(const std::string& name, bool is_master);
    
    /// No copy and moving constructor
	LocalMemory(LocalMemory&& moved) = delete;
    LocalMemory(LocalMemory& moved) = delete;
    LocalMemory& operator=(LocalMemory&& moved) = delete;

    SM_LifeSpan leftSpan() const { return SM_LifeSpan::SM_LifeProcess; }
    SM_ShareMode shareMode() const { return SM_ShareMode::SM_ShareLocal; }

	~LocalMemory();

	int acqire (const std::string& name,
               SM_Mode mode,
               SM_Access permision,
               std::size_t size);
    void release ();

    const char *getName() const;

	char* ptr() const;
	const char* ptr();
	char* HeaderPtr() const;
	const char* HeaderPtr();
	size_t size() const;
    bool isNew() const { return is_new_; }
    bool isMaster() const { return is_master_; }

private:
    std::string    name_;
    int            handle_;
    bool           is_master_;
    size_t         total_size_;
    size_t         payload_size_;
    void*          address_;
    void*          header_;
    char*          payload_;
    bool           is_new_;
};

template <class StorageT, class ContainerT>
class ALT_UTIL_PUBLIC SharedContainer
{
    StorageT            storage_;
    ContainerT*         container_;
  public:
	SharedContainer(
        const std::string& name,
        bool is_master
    ) : storage_ (name, is_master)
      , container_ (nullptr)
    {
    }

    ContainerT* getContainer() { return container_; }

    template <typename... Args>
    int init(Args&&... args)
    {
        bool is_master = storage_.isMaster();
        size_t size = ContainerT::requiredSize(std::forward<Args>(args)...);
        SM_Mode mode = ContainerT::getOpenMode(is_master);
        SM_Access access = ContainerT::getAccessRequeust(is_master);
        if (storage_.acquire(mode, access, size) < 0)
        {
            return -1;
        }
        MemoryAttrs attrs;
        attrs.is_new_ = storage_.isNew();
        attrs.is_for_owner_ = is_master;
        container_ = ContainerT::create(storage_.ptr(), attrs, std::forward<Args>(args)...);
        return 0;
    }
 };
 
} // namespace alt

