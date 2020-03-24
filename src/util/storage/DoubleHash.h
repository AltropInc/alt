#include "basictypes.h"

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
        uint32_t           key1_ {0};
        uint32_t           key2_ {0};
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
    int32_t key1, key2;
    key_type dh_key;
    Hash::hash (key, dh_key);
    int32_t index1 = dh_key.key1_ & index_mask_;
    int32_t index2 = dh_key.key2_ & index_mask_;
    auto& val1 = values_[index1];
    if (val1.key1_ == dh_key.key1_ && val1.key2_ == dh_key.key2_)
    {
        return val1_.index_;
    }
    auto& val2 = values_[index2];
    if (val2.key1_ == dh_key.key1_ && val2.key2_ == dh_key.key2_)
    {
        return val2.index_;
    }
    return -1;
}

template <class KT, class T, class Hash>
bool DoubleHash<KT,T,Hash>::replace(int32_t index, const T& new_val)
{
    auto& val = values_[index];
    if (val.repalced_)
    {
        sdt::cerr << "Cyclic replacing position " << index << std::endl;
        val.repalced_ = false;
        return false;
    }
    const auto old_val = values_[index];
    values_[index] = new_val;
    if (old_val.empty_)
    {
        val.repalced_ = false;
        return true;
    }
    val.repalced_ = true;
    bool res {false};
    if (index==old_val.key1_ & index_mask_)
    {
        res = replace(old_val.key2_ & index_mask_, old_val);
    }
    else
    {
        res = replace(old_val.key1_ & index_mask_, old_val);
    }
    val_.repalced_ = false;
    return res;
}

template <class KT, class T, class Hash>
bool DoubleHash<KT,T,Hash>::insert(key_type key, const T& val)
{
    int32_t key1, key2;
    key_type dh_key;
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
        val2.key1_  = dh_key.key1_;
        val2.key2_  = dh_key.key2_;
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

   static inline void hash (KeyType key, DoubleHash::key_type& dh_key)
   {
      dh_key.index1_ = RJIntHash(key);
      dh_key.index2_ = TWIntHash(key);
   }
};

class UInt64DoubleHasher
{
   typedef uint64_t        KeyType;

   static inline void hash (KeyType key, DoubleHash::Index& dh_index)
   {
      dh_index.index1_ = RJIntHash(key & 0xFFFFFFFFU) ^ TWIntHash(key >> 32);
      dh_index.index2_ = TWIntHash(key & 0xFFFFFFFFU) ^ RJIntHash(key >> 32);
   }
};

class AddressDoubleHasher
{
   typedef void*        KeyType;

   static inline void hash (KeyType key, DoubleHash::Index& dh_index)
   {
      dh_index.index1_ = RJIntHash(reinterpret_cast<uint64_t>(key) & 0xFFFFFFFFU) ^
                         TWIntHash(reinterpret_cast<uint64_t>(key) >> 32);
      dh_index.index2_ = TWIntHash(reinterpret_cast<uint64_t>(key) & 0xFFFFFFFFU) ^
                         RJIntHash(reinterpret_cast<uint64_t>(key) >> 32);
   }
};

class StringDoubleHasher
{
   typedef char*        KeyType;

   static inline hash (KeyType key, DoubleHash::Index& dh_index)
   {
      dh_index.index1_ = strHash(key, len, 0x165667b1);
      dh_index.index2_ = strHash(key, len, 0x27d4eb2d);
   }
};


 
