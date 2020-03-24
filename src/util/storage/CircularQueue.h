#pragma once

#include "Allocator.h"          // for StorageAttrs
#include <util/ipc/SharedMemory.h>
#include <util/sysinfo/Platform.h>
#include <util/types/Enum.h>
#include <util/sysinfo/SysConfig.h>
#include <string>
#include <errno.h>
#include <atomic>
#include <unistd.h>  // for pause

namespace alt {

/**
 * \class ReadSequencer
 * \brief implements a lock-free read syncronizing sequencer fro mutiple readers
 */
class ReadSequencer
{
    CACHE_LINE_ALIGN std::atomic<int64_t>  read_sequence_ {0};

  public:

    /// \brief acquire the current read sequence and advance the sequence for next
    /// \tparam Container the indexed container to check if the entry corresponding to the
    /// sequence in tyhe container is a valid entry (committed by a writer)
    /// \param container the container
    /// \return the sequence for read
    template <class Container>
    int64_t acquire(const Container& container)
    {
        int64_t read_seq = read_sequence_.load();
        while (true)
        {
            if (!container[read_seq].isValid())
            {
                // nothing to read
                return -1;
            }

            // try to advance read position for next acquiring
            if (read_sequence_.compare_exchange_strong(
                    read_seq, read_seq+1,
                    std::memory_order_acq_rel, std::memory_order_relaxed)
               )
            {
                // read_sequence_ advanced, return the current
                return read_seq;
            }
            // some other reader advanced the sequence, try again
            // Not a really pause, just tell the cpu that we are spinning
            pause();
        }
    }
};

/**
 * \class WriteSequencer
 * \brief implements a lock-free write syncronizing sequencer for mutiple writers
 */
class WriteSequencer
{
    CACHE_LINE_ALIGN std::atomic<int64_t>  allocated_ {-1};
    CACHE_LINE_ALIGN std::atomic<int64_t>  committed_ {-1};

  public:
    int64_t getAllocated() const { return allocated_.load(std::memory_order_acquire); }
    int64_t getCommitted() const { return committed_.load(std::memory_order_acquire); }

    /// \brief acquire a number of free slots for writing
    int64_t acquire(int64_t num =1)
    {
        int64_t seq = allocated_.load(std::memory_order_acquire);
        while (!allocated_.compare_exchange_weak(
                seq, seq+num,
                std::memory_order_acq_rel, std::memory_order_relaxed)
                )
        {
            pause();
        }
        return seq;
    }

    /// \brief commit the sequence (index of the slot) in the container
    template <class Container>
    void commit(int64_t seq_to_commit, const Container& container)
    {
        int64_t committed = committed_.load(std::memory_order_acquire);
        if (seq_to_commit > committed)
        {
            int64_t first_undone = committed + 1;
            for (; first_undone<=seq_to_commit; ++first_undone)
            {
                if (!container[first_undone].isValid())
                {
                    break;
                }
            }
            int64_t last_done = first_undone - 1;
            if (last_done > committed)
            {
                while (!committed_.compare_exchange_weak(
                        committed, last_done,
                        std::memory_order_acq_rel, std::memory_order_relaxed)
                    )
                {
                    if (committed >= last_done)
                    {
                        // someone else already updated the committed
                        break;
                    }
                    pause();
                }
            }
        }
    }
};

/**
 * \class CircularQueue
 * \brief implements a lock-free circular queue for multiple writers and multiple readers
 * entry has the fixed size. No derived classes, nor virtual functions, as this may be
 * allocated/reused in shared memory
 */
class ALT_CORE_PUBLIC CircularQueue final
{
    struct QueueHeader
    {
        char *              entry_buffer_ {nullptr};
        bool                owns_buffer_ {false};
        size_t              entry_number_;
        size_t              entry_size_;
        size_t              entry_size_shift_;
        size_t              entry_size_mask_;
        WriteSequencer      write_sn_;
        ReadSequencer       read_sn_;
        size_t              overrun_cnt_ {0};

        QueueHeader (size_t entry_size, size_t entry_number);
        QueueHeader (size_t entry_size, size_t entry_number, char* buffer);
        ~QueueHeader();
    }
    header_;

  public:

    struct EntryHeader
    {
        uint64_t                       sequence_;
        mutable std::atomic<bool>      is_valid_ {false};

        bool isValid() const { return is_valid_.load(std::memory_order_acquire); }
        void release() const { is_valid_.store(false, std::memory_order_release); }
        void setValid() const { is_valid_.store(true, std::memory_order_release); }
    };

    CircularQueue (size_t entry_size, size_t entry_number, char* buffer);
    CircularQueue (size_t entry_size, size_t entry_number);

    static size_t requiredHeaderSize();
    static size_t requiredEntrySize(size_t entry_size);
    static size_t requiredSize(size_t entry_size, size_t entry_number);
    static CircularQueue* create (
        char* addr, const MemoryAttrs& attrs, size_t entry_size, size_t entry_number);

    static SM_Mode getOpenMode(bool is_master)
    { return is_master ? SM_Mode::SM_OpenOrCreate : SM_Mode::SM_OpenOnly; }

    static SM_Access getAccessRequeust(bool is_master) { return SM_Access::SM_ReadWrite; }

    size_t getIndex (int64_t seq) const
    {
        return (seq & header_.entry_size_mask_) << header_.entry_size_shift_;
    }

    EntryHeader& operator[] (int64_t seq)
    {
        return reinterpret_cast<EntryHeader&>(*(header_.entry_buffer_ + getIndex(seq)));
    }

    const EntryHeader& operator[] (int64_t seq) const
    {
        return reinterpret_cast<const EntryHeader&>(*(header_.entry_buffer_ + getIndex(seq)));
    }

    EntryHeader* getNextWriteEntry();

    EntryHeader* getNextNWriteEntry(int num =1);

    bool empty() const
    { return header_.write_sn_.getCommitted()<0; }

    /// \brief read an entry, called for multiple readers
    /// \return pointer to the entry to be read
    /// \note After read is done, call commitRead to release the entry
    const EntryHeader* read();

    /// \brief read an entry, only called for single reader mode
    /// \param seq the sequence of the entry to be retrieved, starting from 0.
    /// \return pointer to the entry to be read
    /// \note After read is done, call commitRead to release the entry
    const EntryHeader* read (int64_t seq);

    void commitWrite(EntryHeader* entry)
    {
        header_.write_sn_.commit(entry->sequence_, *this);
        entry->setValid();
    }

    const void commitRead(const EntryHeader* entry) const
    {
        entry->release();
    }
};

using SharedCircularQueue = SharedContainer<SharedMemory, CircularQueue>;

} // namespace alt

