#pragma once

#include "ValueWrapper.h"

#include <mutex>
#include <memory>
#include <atomic>

namespace alt
{

class SingletonClass
{
  public:  
    static SingletonClass& instance()
    {    
        static SingletonClass s_instance;   
        return s_instance;  
    }
  private:  
    SingletonClass() { /* contruct code if nedded */ };  
    ~SingletonClass() { /* destructor code if nedded */ };  
    SingletonClass(const SingletonClass&)= delete; 
    SingletonClass(const SingletonClass&&)= delete; 
    SingletonClass& operator=(const SingletonClass&)= delete;
    SingletonClass& operator=(const SingletonClass&&)= delete;
};

class LazySingletonClass
{
  public:  
    static LazySingletonClass& instance()
    {    
        static std::unique_ptr<LazySingletonClass> instance;
        static std::atomic<LazySingletonClass*> instance_ptr {nullptr};
        
        LazySingletonClass* ptr = instance_ptr.load(std::memory_order_acquire);
        if (!ptr)
        {
            LazySingletonClass* new_ptr = new LazySingletonClass();
            if (instance_ptr.compare_exchange_strong(ptr, new_ptr, std::memory_order_acq_rel, std::memory_order_relaxed))
            {
                // instance_ptr is sucessfully set to new_ptr. Set it to the unique pointer for cleanup on exit
                instance.reset(new_ptr);
                ptr = new_ptr;
            }
            else
            {
                // someone else already created the instance and sucessfully set the instance_ptr.
                // Delete mine. This case rarely happens.
                delete new_ptr;
            }
        };
  
        return *ptr;
    }

    static LazySingletonClass& instance2()
    {    
        static std::unique_ptr<LazySingletonClass> s_instance;
        static std::mutex  instance_mutex;
        
        if (!s_instance)
        {
            std::scoped_lock<std::mutex> lock (instance_mutex);
            if (!s_instance)
            {
                s_instance.reset(new LazySingletonClass());
            }
        };
  
        return *s_instance.get();
    }

  private:  
    LazySingletonClass() { /* contruct code if nedded */ };  
    ~LazySingletonClass() { /* destructor code if nedded */ };  
    LazySingletonClass(const LazySingletonClass&)= delete; 
    LazySingletonClass(const LazySingletonClass&&)= delete; 
    LazySingletonClass& operator=(const LazySingletonClass&)= delete;
    LazySingletonClass& operator=(const LazySingletonClass&&)= delete;
};

} // namespace alt
