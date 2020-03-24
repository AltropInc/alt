#pragma once

#include "Allocator.h"
#include "util/string/StrBuffer.h"
#include "util/string/StrPool.h"
#include <unordered_map>

namespace alt
{

/**
 * \brief implements a string key hash using fixed pools to reduce heap allocation at
 * run time. The key is a char pointer that points to the position in a string pool
 * where the space is not freed for new string. Therefore, this hash map is suitable
 * for the usage where erase seldomly happens during the lifetime of the map.
 */
template <typename T, class Allocator = StdFixedPoolAllocator< std::pair<const StrRef, T> > >
class ALT_CORE_PUBLIC StringHashMap
{
  public:
    using key_type          = StrRef;
    using mapped_type       = T;
    using value_type        = std::pair<const StrRef, T>;
    using size_type         = size_t;

    using map_type    =
          std::unordered_map<StrRef,
                       T,
                       std::hash<StrRef>,
                       std::equal_to<StrRef>,
                       Allocator
                      >;

    using iterator           = typename map_type::iterator;
    using const_iterator     = typename map_type::const_iterator;
    using insert_return_type = typename map_type::insert_return_type;

  protected:
    map_type                          hash_map_;
    StrPool                           string_pool_;

  public:
    iterator begin() noexcept { return hash_map_.begin(); }
    const_iterator begin() const noexcept  { return hash_map_.begin(); }
    const_iterator cbegin() const noexcept  { return hash_map_.cbegin(); }
    iterator end() noexcept { return hash_map_.end(); }
    const_iterator end() const noexcept { return hash_map_.end(); }
    const_iterator cend() const noexcept { return hash_map_.cend(); }

    bool empty() const noexcept { return hash_map_.empty(); }
    size_type size() const noexcept { return hash_map_.size(); }
    size_type max_size() const noexcept { return hash_map_.max_size(); }

    template <typename KeyT>
    iterator find(const KeyT& key) { return hash_map_.find(StrRef(key)); }

    template <typename KeyT>
    const_iterator find(const KeyT& key) const { return hash_map_.find(StrRef(key)); }

    std::pair<iterator, bool> insert(const value_type& value)
    {
        const auto [iter, inserted] = hash_map_.insert(value);
        if (inserted)
        {
            // replace key with pointer in string pool
            *const_cast<StrRef*>(&iter->first) = StrRef(string_pool_.insert(value.first.c_str()));
        }
        return std::make_pair(iter, inserted);
    }

    std::pair<iterator, bool> emplace(char const* key, mapped_type value)
    {
        return insert(std::make_pair(StrRef(key), value));
    }

    template <typename... Args>
    std::pair<iterator, bool> try_emplace(char const* key, Args&&... args)
    {
        return insert(std::make_pair(StrRef(key), T(std::forward<Args>(args)...)));      
    }

    iterator erase(const_iterator pos)
    {
        string_pool_.erase(pos->first.c_str());
        return hash_map_.erase(pos);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        for (auto iter=first; iter!=last;)
        {
           iter = erase(iter);
        }
    }

    size_type erase(char const* key)
    {
        auto iter = hash_map_.find(StrRef(key));
        if (iter==hash_map_.end()) return 0;
        erase(iter);
        return 1;
    }

    std::pair<iterator, bool> rename(const char* old_name, const char* new_name)
    {
        auto pos = hash_map_.find(StrRef(old_name));
        if (pos==hash_map_.end())
        {
            return std::make_pair(pos, false);
        }
        auto node = hash_map_.extract(pos);
        string_pool_.erase(pos->first.c_str());
        const char* new_pooled_str = string_pool_.insert(new_name);
        node.key() = StrRef(new_pooled_str);
        auto insert_status = hash_map_.insert(move(node));
        return std::make_pair(insert_status.position, insert_status.inserted);
    }

    T& operator[](char const* key) { return hash_map_[StrRef(key)]; }
    T& at(char const* key) { return hash_map_.at(StrRef(key)); }
    const T& at(char const* key) const { return hash_map_.at(StrRef(key)); }

    template <typename KeyT>
    bool contains(const KeyT& key) const { return find(key)!=hash_map_.end(); }

    size_type bucket_count() const { return hash_map_.bucket_count(); }
    size_type max_bucket_count() const { return hash_map_.max_bucket_count(); }
    size_type bucket_size(size_type n) const { return hash_map_.bucket_size(n); }
    size_type bucket(const char* key) const { return hash_map_.bucket(StrRef(key)); }
    float load_factor() const { return hash_map_.load_factor(); }
    float max_load_factor() const { return hash_map_.max_load_factor(); }
    void max_load_factor(float ml) { hash_map_.max_load_factor(ml); }
    void reserve(size_type count) { return hash_map_.reserve(count); }
    void rehash(size_type count) { return hash_map_.rehash(count); }

    StrPool & getStringPool() { return string_pool_; }
};

}

