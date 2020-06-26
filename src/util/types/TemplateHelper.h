#pragma once

#include <cstddef>   // for size_t
#include <array>

namespace alt
{
//----------------------------------------------------------------------------
// Misc common helper templates and definitions
//----------------------------------------------------------------------------

/// \brief make_array: a template to make a std::array for auto declaration.
/// \details
/// Usage example:
/// static auto some_array = make_array(element1, element2, element3, ...);
template<typename... T>
std::array<typename std::common_type<T...>::type, sizeof...(T)>
make_array(T &&...t) {
    return {std::forward<T>(t)...};
}

/// \brief convert the value of T to its underlying type
template <typename T>
constexpr typename std::underlying_type<T>::type to_underlying(T val) noexcept {
    return static_cast<typename std::underlying_type<T>::type>(val);
}

/// \brief all_same_type: a template to check if the first T and all the rest
/// Ts are the same type
template<typename T, typename... Ts>
using all_same_type = std::conjunction<std::is_same<T, Ts>...>;

/// \brief hash_combine: a template for hash key combine.
/// \details
/// Usage example:
/// std::size_t seed=0;
/// hash_combine(seed, key1, key2, key3, ...);
inline void hash_combine(std::size_t& seed) { }
template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest &&... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hash_combine(seed, std::forward<Rest>(rest)...);
}

/// \brief member_addr: returns member address of x.
/// \details
/// Usage example:
/// member_addr(object, object_member_offset);
template <typename T>
inline void * member_addr(T* x, size_t member_offset)
{
    return reinterpret_cast<char*>(x)+member_offset;
}
template <typename T>
inline const void * member_addr(const T* x, size_t member_offset)
{
    return reinterpret_cast<const char*>(x)+member_offset;
}

/// \brief define move only constructor and assignment
#define MOVEONLY(MoveOnly) \
    MoveOnly (MoveOnly&& other) { swap(other); } \
    MoveOnly& operator=(MoveOnly&& other) { swap(other); return *this; } \
    MoveOnly (const MoveOnly& other)=delete; \
    MoveOnly& operator=(const MoveOnly&) = delete;

/// \brief define move only constructor and assignment
#define NONCOPYABLE(NonCopyable) \
    NonCopyable (NonCopyable&& other)=delete; \
    NonCopyable& operator=(NonCopyable&& other)=delete; \
    NonCopyable (const NonCopyable& other)=delete; \
    NonCopyable& operator=(const NonCopyable&) = delete;

}