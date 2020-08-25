#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file DoubleHash.h
 * @library alt_util
 * @brief Implements an open-addressing hash table to resolve hash collisions
 * by using a secondary hash of the key as an offset when a collision occurs.
 * DoubleHash has a much faster search but may be slower in inserting. Therefore,
 * it is suitable to use in case we have a fixed number of things to hash during
 * initialization.  During inserting, if collision cannot be resolved, resize the
 * table and rehash the whole entries are required.
 * TODO: to be tested and more work required to make it a good hash table to use
 */

#include <util/numeric/Intrinsics.h>    // RJIntHash and TWIntHash
#include <util/string/StrUtils.h>       // for StrHash

namespace alt
{

struct DoubleHashKey
{
    uint32_t           key1_ {0};
    uint32_t           key2_ {0};
};

template <class KT, class T, class Hash>
class DoubleHash
{
  public:
    typedef T   mapped_type;
    typedef KT  key_type;

    struct value_type
    {
        key_type           key_;
        mapped_type        value_;
        DoubleHashKey      dh_key_;
        bool               replaced_ {false};
        bool               empty_ {true};
    };

    DoubleHash(uint32_t init_size) : value_buffer_ (NULL)
    {
        initialize(init_size);
    }

    bool insert(key_type key, const T& val);
    int32_t find(key_type key);

   private:

      void initialize(uint32_t init_size);
      bool replace(int32_t index, const T& new_val);

      std::vector<value_type>          values_;
      uint32_t                         index_mask_;
};

template <class KT, class T, class Hash>
void DoubleHash<KT,T,Hash>::initialize(uint32_t init_size)
{
    uint32_t adjusted_size = 1 << bsr(init_size);

    if (adjusted_size < init_size)
    {
        adjusted_size <<= 1;
    }

    index_mask_ = adjusted_size - 1;
    value_buffer_.resize(adjusted_size);
}

template <class KT, class T, class Hash>
int32_t DoubleHash<KT,T,Hash>::find(key_type key)
{
    DoubleHashKey dh_key;
    Hash::hash (key, dh_key);
    int32_t index1 = dh_key.key1_ & index_mask_;
    int32_t index2 = dh_key.key2_ & index_mask_;
    auto& val1 = values_[index1];
    if (val1.dh_key_ == dh_key)
    {
        return val1_.index_;
    }
    auto& val2 = values_[index2];
    if (val1.dh_key_ == dh_key)
    {
        return val2.index_;
    }
    return -1;
}

template <class KT, class T, class Hash>
bool DoubleHash<KT,T,Hash>::replace(int32_t index, const T& new_val)
{
    auto& val = values_[index];
    if (val.replaced_)
    {
        sdt::cerr << "Cyclic replacing position " << index << std::endl;
        val.replaced_ = false;
        return false;
    }
    const auto old_val = values_[index];
    values_[index] = new_val;
    if (old_val.empty_)
    {
        val.replaced_ = false;
        return true;
    }
    val.replaced_ = true;
    bool res {false};
    if (index==old_val.dh_key_.key1_ & index_mask_)
    {
        res = replace(old_val.dh_key_.key2_ & index_mask_, old_val);
    }
    else
    {
        res = replace(old_val.dh_key_.key1_ & index_mask_, old_val);
    }
    val_.replaced_ = false;
    return res;
}

template <class KT, class T, class Hash>
bool DoubleHash<KT,T,Hash>::insert(key_type key, const T& val)
{
    int32_t key1, key2;
    DoubleHashKey dh_key;
    Hash::hash (key, dh_key);
    int32_t index1 = dh_key.key1_ & index_mask_;
    int32_t index2 = dh_key.key2_ & index_mask_;

    auto& val1 = values_[index1];
    if (val1.key_ == key && !val1.empty_)
    {
        // this entry is already inserted
        return false;
    }

    auto& val2 = values_[index2];
    if (val2.key_ == key && !val2.empty_)
    {
        // this entry is already inserted
        return false;
    }

    if (val1.empty_)
    {
        val1.empty_ = false;
        val1.replaced_ = false;
        val1.key_   = key;
        val1.key1_  = dh_key.key1_;
        val1.key2_  = dh_key.key2_;
        return true;
    }

    if (val2.empty_)
    {
        val2.empty_ = false;
        val2.replaced_ = false;
        val2.key_   = key;
        val2.dh_key_  = dh_key;
        return true;
    }
 
    if (!replace(index1, val))
    {
        return false;
    }

    return true;
}

class UInt32DoubleHasher
{
   typedef uint32_t        KeyType;

   static inline void hash (KeyType key, DoubleHashKey& dh_key)
   {
      dh_key.key1_ = RJIntHash(key);
      dh_key.key2_ = TWIntHash(key);
   }
};

class UInt64DoubleHasher
{
   typedef uint64_t        KeyType;

   static inline void hash (KeyType key, DoubleHashKey& dh_index)
   {
      dh_index.key1_ = RJIntHash(key & 0xFFFFFFFFU) ^ TWIntHash(key >> 32);
      dh_index.key2_ = TWIntHash(key & 0xFFFFFFFFU) ^ RJIntHash(key >> 32);
   }
};

class AddressDoubleHasher
{
   typedef void*        KeyType;

   static inline void hash (KeyType key, DoubleHashKey& dh_index)
   {
      dh_index.key1_ = RJIntHash(reinterpret_cast<uint64_t>(key) & 0xFFFFFFFFU) ^
                         TWIntHash(reinterpret_cast<uint64_t>(key) >> 32);
      dh_index.key2_ = TWIntHash(reinterpret_cast<uint64_t>(key) & 0xFFFFFFFFU) ^
                         RJIntHash(reinterpret_cast<uint64_t>(key) >> 32);
   }
};

class StringDoubleHasher
{
   typedef char*        KeyType;

   static inline void hash (KeyType key, DoubleHashKey& dh_index)
   {
      dh_index.key1_ = strHash(key, 0x165667b1);
      dh_index.key2_ = strHash(key, 0x27d4eb2d);
   }
};

} // namespace alt
 
