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

/// \brief helper type for std::visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
#if !defined (ALT_CPP_VERSION_20)
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
#endif

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

/// \brief StructExtension: this is for a extention of a struct with number of
/// bitfields to specify which optional fields will be present in the extension.
/// If a bit is set, the filed will be present. The optional field must appear in
/// the same order of the bit that represents the filed in the bitfield array.
/// \tparam N total number of optional fields
/// \tparam Opts list of selected optional field types for the extention
/// Usage example:
///   struct MsgBase { uint16_t msg_type_; ... };
///   struct MsgBase { uint16_t msg_type_; ... };
///   struct MyExtendedMsg: MsgBase, StructExtension<OptFiled1, OptField9> {};
///   struct OptFiled0 { static constexpr int INDEX = 0; int32_t field1; };
///   struct OptFiled1 { static constexpr int INDEX = 1; double field2; };
///   ...
///   struct MyExtendedMsg: MsgBase, StructExtension<OptFiled1, OptField9> {};
#pragma pack(push, 1)
template <int N, typename... Opts>
struct StructExtension : Opts...
{
    template <typename T>
    struct hasExtention
    {
        static constexpr bool value = (std::is_same<T, Opts>::value || ...);
    };

    template <typename T>
    static constexpr bool setfiled(std::array<uint8_t, N>& field_flags_)
    {
        field_flags_[T::INDEX/8] |= 1 << T::INDEX%8;
        return true;
    }

    std::array<uint8_t, (N-1)/8+1>      field_flags_  = {0};
    
    StructExtension()
    {
        (setfiled<Opts>(field_flags_) && ...);
    }
};
#pragma pack(pop)

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