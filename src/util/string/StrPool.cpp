
#include <thread>
#include "StrPool.h"
#include "StrScan.h"
#include <iostream>

using namespace alt;

void StrPoolBase::newPage()
{
    pages_.emplace_back(page_size_);
    cur_page_ = &pages_.back();
    //std::cout << "StrPoolBase::newPage pages:" << pages_.size() << std::endl;
}

const char* StrPoolBase::push(const char* str, size_t length)
{
    if (page_size_ - cur_page_->pos_ < length+1)
    {
        newPage();
    }
    char* ret = cur_page_->buffer_ + cur_page_->pos_;
    std::memcpy(ret, str, length);
    *(ret+length) = '\0';
    cur_page_->pos_ += length+1;
    return ret;
}

const char* StrPoolBase::insert(const char* str, size_t length)
{
    char* ret;
    if (length < free_spaces_.size() && !free_spaces_[length].empty())
    {
        ret = const_cast<char*>(free_spaces_[length].back());
        free_spaces_[length].pop_back();
        std::memcpy(ret, str, length);
        *(ret+length) = '\0';
        return ret;
    }
    return push(str, length);
}

void StrPoolBase::erase(const char* str, size_t length)
{
    if (length >= free_spaces_.size())
    {
        free_spaces_.resize(length+16);
    }
    auto & free_space = free_spaces_[length];
    if (free_space.capacity()-free_space.size()<=1)
    {
        size_t grow_size = std::max(free_space.capacity()+32, free_space.size()<<1);
        free_space.reserve(grow_size);
    }
    free_space.push_back(str);
}

const char* StrPoolBase::push(
    const std::initializer_list<const char*>& strs,
    const std::initializer_list<size_t>& str_sizes,
    size_t total_length,
    const char* separator)
{
    if (page_size_ - cur_page_->pos_ < total_length+1)
    {
        newPage();
    }
    char * ret = cur_page_->buffer_ + cur_page_->pos_;
    char * cp = ret;
    auto strp = strs.begin();
    auto szp = str_sizes.begin();
    if (strp!=strs.end()) do
    {
        std::memcpy(cp, *strp, *szp);
        cp += *szp;
        ++strp;
        ++szp;
        if (strp == strs.end())
        {
            break;
        }
        if (separator)
        {
            const char * sp = separator;
            while (*sp)
            {
                *(cp++) = *sp++;
            }
        }
    }
    while (1);

    *(cp) = '\0';
    cur_page_->pos_ += total_length + (separator ? strlen(separator) : 0);
    return ret;
}

#ifdef TEST_BUILD
void StrPoolBase::dump()
{
    int i(1);
    for (auto & page: pages_)
    {
        std::cout << "Page " << i << ":\n";
        std::vector<std::string> string_vec;
        StrScan scan(page.buffer_, page.pos_);
        scan.split(string_vec, '\0', false, false);
        for (const auto& str: string_vec)
        {
            std::cout << "  " << str << '\n';
        }
        std::cout << std::endl;
        ++i;
    }
}
#endif
#ifdef TEST_BUILD
int main (void)
{
    StrPoolSpinLocked strpool(24);
    std::cout << "starting first thread ...\n";
    std::thread thread1([&strpool]{ for (int i=0; i<10; ++i) strpool.push("********");});
    std::cout << "starting second thread ...\n";
    std::thread thread2([&strpool]{ for (int i=0; i<10; ++i) strpool.push("........");});
    std::cout << "waiting for first to finish..." << std::endl;
    thread1.join();
    std::cout << "waiting for second to finish..." << std::endl;
    thread2.join();
    strpool.dump();
    return 0;
}

#endif