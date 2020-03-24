#include "SysConfig.h"

#include <unistd.h>
#include <assert.h>

namespace alt
{

SysConfig::SysConfig()
{
    cache_line_size_ = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    page_size_ = sysconf(_SC_PAGESIZE);
    line_max_ = sysconf(_SC_LINE_MAX);
    number_of_procesors_ = sysconf(_SC_NPROCESSORS_CONF);
    assert(cache_line_size_==EXPECTED_CACHE_LINE_SIZE);
};

SysConfig const & SysConfig::instance()
{
    static SysConfig s_sys_config;
    return s_sys_config;
}

}