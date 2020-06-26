#pragma once

namespace alt
{
/**
 * \brief implements an opaque pointer to a type to be defined
 * \tparam T the opaque type
 * @note std smart pointers require the type being defined, which make it impossible
 * to have an opaque pointer in header. OpaquePointer is simular to unique_ptr
 * but you cannot use it to access the pointed contents from outside.
 */
template <class T> 
class OpaquePointer
{
  public:
    constexpr OpaquePointer(T* ptr ) noexcept : ptr_(ptr) { }
    explicit   OpaquePointer() noexcept : ptr_(nullptr) { }
    ~OpaquePointer() { delete ptr_; }
    void reset(T* ptr) noexcept { delete ptr_; ptr_ = ptr; }
    const T* operator->() const noexcept { return ptr_; }
    T* operator->() noexcept { return ptr_; }
    const T* get() const noexcept { return ptr_; }
    T* get() noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_!=nullptr; }
  private:
    OpaquePointer(const OpaquePointer&) = delete;
	OpaquePointer(OpaquePointer&&) = delete;
	OpaquePointer& operator = (const OpaquePointer&) = delete;
	OpaquePointer& operator = (OpaquePointer&&) = delete;
    T* ptr_;
};

} // namespace alt