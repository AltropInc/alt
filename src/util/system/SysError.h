#pragma once

#include <util/string/StrUtils.h>       // For strToNameId and strFromNameId
#include <iostream>                     // For std::cout
#include <sstream>                      // For stringstream
#include <exception>                    // for exception
#include <string>                       // for stirng
#include <array>                        // for array
#include <string.h>                     // for strerror
#include <error.h>                      // errno

namespace alt {

/**
 * \class Logger
 * \ingroup Utils
 */
class Logger
{
  public:
    static std::ostream& logError() { return std::cerr; }
    static std::ostream& logInfo() { return std::cout; }
};

constexpr static uint64_t AltExceptionID(std::array<char,8> name)
{
    uint64_t res = 0;
    for (auto ch: name)
    {
        if (!ch) break;
        res = (res<<8) + uint8_t(ch);
    }
    return res;
}

/**
 * \class AltException
 * \brief base class for exceptions in the ALT class library.
 */
template <uint64_t ExceptionId>
class AltException: public std::exception
{
  public:

	AltException(const std::string& msg) : what_msg_(msg) { }
	const char* what() const throw()  override {  return what_msg_.c_str(); }
    static const char* const name() throw()
    {
        if (name_[0]=='\0') name_ = strFromNameId(ExceptionId);
        return std::begin(name_);
    }

  private:

	std::string                          what_msg_;
    inline static std::array<char,9>     name_ {0};
};

// Common system error exceptions
using SysException = AltException<strToNameId({'S','Y','S'})>;
using NetException = AltException<strToNameId({'N','E','T'})>;
using IOException = AltException<strToNameId({'I','O'})>;
using TimeoutException = AltException<strToNameId({'T','I','M','E','O','U','T'})>;
using FileException = AltException<strToNameId({'F','I','L','E'})>;
using ThreadException = AltException<strToNameId({'T','H','R','E','A','D'})>;

} // namespace alt

#define ERR_LOG(API, TEXT) \
   alt::Logger::logError() << "ERROR [" << API << "]: " \
   << TEXT << " in " << __FUNCTION__ << ", "  << __FILE__ << ':' << __LINE__ \
   << std::endl

#define SYS_ERR_LOG_0() \
   alt::Logger::logError() << "ERROR [" << errno << "]: " << strerror(errno) \
   << " in " << __FUNCTION__ << ", "  << __FILE__ << ':' << __LINE__ \
   << std::endl

#define SYS_ERR_LOG_1(API) \
   alt::Logger::logError() << "ERROR [" << API << "] " \
   << '(' << errno << "): " << strerror(errno) \
   << " in " << __FUNCTION__ << ", "  << __FILE__ << ':' << __LINE__ \
   << std::endl

#define SYS_ERR_LOG_2(API, TEXT) \
   alt::Logger::logError() << "ERROR [" << API << "]: " \
   << TEXT << " on error (" << errno << "): " << strerror(errno) \
   << " in " << __FUNCTION__ << ", "  << __FILE__ << ':' << __LINE__ \
   << std::endl

#define SYS_ERR_LOG_N(N,API,TEXT,LOG_MACRO, ...)  LOG_MACRO

#define SYS_ERR_LOG(...)            SYS_ERR_LOG_N(,##__VA_ARGS__,\
                                        SYS_ERR_LOG_2(__VA_ARGS__),\
                                        SYS_ERR_LOG_1(__VA_ARGS__), \
                                        SYS_ERR_LOG_0(__VA_ARGS__) \
                                    )

#define SYS_ERR_THROW_0() {\
   std::stringstream ss; ss << "EXCEPTION (" << errno << "): " << strerror(errno); \
   alt::Logger::logError() << ss.str() << " in " << __FUNCTION__ << ", "  << __FILE__ << ':' << __LINE__ << std::endl; \
   throw std::runtime_error(ss.str()); \
}

#define SYS_ERR_THROW_1(EXCEPTION) {\
   std::stringstream ss; ss << "EXCEPTION <" << EXCEPTION::name() << "> " \
   << '(' << errno << "): " << strerror(errno); \
   alt::Logger::logError() << ss.str() << " in " << __FUNCTION__ << ", "  << __FILE__ << ':' << __LINE__ << std::endl; \
   throw EXCEPTION(ss.str()); \
}

#define SYS_ERR_THROW_2(EXCEPTION, TEXT) {\
   std::stringstream ss; ss << "EXCEPTION <" <<  EXCEPTION::name() << "> " \
   << TEXT << " on error (" << errno << "): " << strerror(errno); \
   alt::Logger::logError() << ss.str() << " in " << __FUNCTION__ << ", "  << __FILE__ << ':' << __LINE__ << std::endl; \
   throw EXCEPTION(ss.str()); \
}

#define SYS_ERR_THROW_3(EXCEPTION, TEXT, IS_SYS_ERR) {\
   std::stringstream ss; \
   ss << "EXCEPTION <" <<  EXCEPTION::name() << "> " << TEXT; \
   if (IS_SYS_ERR) ss <<  " on error (" << errno << "): " << strerror(errno); \
   alt::Logger::logError() << ss.str() << " in " << __FUNCTION__ << ", "  << __FILE__ << ':' << __LINE__ << std::endl; \
   throw EXCEPTION(ss.str()); \
}

#define SYS_ERR_THROW_N(N,EXCEPTION,TEXT,IS_SYS_ERR,THROW_MACRO, ...)  THROW_MACRO

#define SYS_ERR_THROW(...)          SYS_ERR_THROW_N(,##__VA_ARGS__,\
                                        SYS_ERR_THROW_3(__VA_ARGS__), \
                                        SYS_ERR_THROW_2(__VA_ARGS__), \
                                        SYS_ERR_THROW_1(__VA_ARGS__), \
                                        SYS_ERR_THROW_0(__VA_ARGS__) \
                                    )

#define DBG_LOG(API, TEXT) \
   Logger::logInfo() << "INFO [" << API << "]: " \
   << TEXT << " in " << __FUNCTION__ << ", "  << __FILE__ << ':' << __LINE__ \
   << std::endl
