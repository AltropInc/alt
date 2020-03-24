#pragma once

#include <cstdlib>   // for size_t
#include <stdint.h>
#include <vector>

namespace alt {

// We have to define CACHE_LINE_SIZE a constant in order to use it for alignment
// SysConfig will check if the cache_line_size_ is actually the epected one
#define EXPECTED_CACHE_LINE_SIZE        64
#define CACHE_LINE_ALIGN                alignas(EXPECTED_CACHE_LINE_SIZE)

struct CPUInfo
{
    enum Vendor {GenuineIntel, AuthenticAMD, Other};
    uint64_t  frequency_;
    size_t    cache_size_;
    size_t    core_id_;
    Vendor    vendor_;
    size_t    cache_alignment_;
};

struct SysConfig
{
    size_t   cache_line_size_;
    size_t   page_size_;
    size_t   line_max_;
    size_t   number_of_procesors_;

    std::vector<CPUInfo>  cpu_info_;

    static SysConfig const & instance();

  private:
    SysConfig();
};

}