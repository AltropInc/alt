#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file StrScan.h
 * @library alt_util
 * @brief definition of fast string scanners:
 *    - ScanBuffer: a wrapper to hold a reference to the string to be scanned
 *    - StrScan: a base class for other string parsers 
 *    - StrParser: a string parser using operator >> and handling seperators
 *      for string split
*/

#include "StrBuffer.h"

#include <util/system/Platform.h>
#include <util/types/EnumSet.h>

#include <stddef.h>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <functional>

namespace alt
{
/**
 * \class ScanBuffer
 * \ingroup StringUtils
 * \brief A buffer pointer to string to be scanned 
 */
struct ScanBuffer
{
    const char *str_{nullptr};  ///< start position of the string to scan
    size_t length_{0};          ///< length of the string to scan
    size_t pos_{0};             ///< current scanning position

    ScanBuffer() = default;
    ScanBuffer(const char *str, size_t length) : str_(str), length_(length) {}
    ScanBuffer(const char *begin, const char *end) : str_(begin), length_(size_t(end - begin)) {}
    explicit ScanBuffer(const std::string &str) : str_(str.c_str()), length_(str.length()) {}
    explicit ScanBuffer(const char *str) : str_(str), length_(strlen(str)) {}
    ScanBuffer(ScanBuffer &&oth) : str_(oth.str_), length_(oth.length_), pos_(oth.pos_)  { oth.reset(); };
    ScanBuffer(ScanBuffer &oth) : str_(oth.str_), length_(oth.length_), pos_(oth.pos_)  { oth.reset(); };

    ScanBuffer& operator = (ScanBuffer &oth)
    {
        str_ = oth.str_;
        length_ = oth.length_;
        pos_ = oth.pos_;
        oth.reset();
        return *this;
    }

    void reset(const char *str, size_t length)
    {
        str_ = str;
        length_ = length;
        pos_ = 0;
    }
    void reset(const char *begin, const char *end) { reset(begin, size_t(end - begin)); }
    void reset(const std::string &str) { reset(str.c_str()), str.length(); }
    void reset(const char *str) { reset(str, strlen(str)); }
    void reset() { pos_=0; }

    const size_t length() const { return length_; }
    const size_t pos() const { return pos_; }
    const void resetPos(size_t pos) { pos_ = pos; }
    const char *head() const { return str_; }
    bool empty() const { return str_ == nullptr; }
    char curChar() const { return *(str_ + pos_); }
    char nextChar() { return *(str_ + ++pos_); }
    char peerNext() const { return *(str_ + pos_ + 1); }
    char const *curPos() const { return str_ + pos_; }
    char const *at(size_t pos) const { return str_ + pos; }
    bool hasRemaining() const { return pos_ < length_; }
    bool atEnd() const { return pos_ >= length_; }
    size_t remaining() const { return hasRemaining() ? length_ - pos_ : 0; }
    void advance() { ++pos_; }
    void back() { --pos_; }
    void advance(size_t steps) { pos_ += steps; }
    void back(size_t steps) { pos_ -= steps; }
    const char *skipWhiteSpace();
    char nextNonWhiteSpace();
};

ALT_INLINE const char *ScanBuffer::skipWhiteSpace()
{
    char ch = curChar();
    while (ch && isspace(ch))
    {
        ch = atEnd() ? '\0' : nextChar();
    }
    return curPos();
}

ALT_INLINE char ScanBuffer::nextNonWhiteSpace()
{
    char ch = nextChar();
    while (ch && isspace(ch))
    {
        ch = atEnd() ? '\0' : nextChar();
    }
    return ch;
}

class ParserStream;
class ParserStreamContext;

/**
 * \class StrScan
 * \ingroup util
 * \brief String scan helper converting string into values in various formats
 */
class StrScan
{
public:
    enum ValueType
    {
        NONE,
        TOKEN,
        CHAR,
        INT,
        INT64,
        UINT64,
        DOUBLE,
        STRING,
        IDENTIFIER
    };

    explicit StrScan(ScanBuffer &buffer) : scan_buffer_(buffer) {}
    StrScan(StrScan &&oth) = default;
    StrScan(const StrScan &oth) = default;

    const char *c_str() const { return scan_buffer_.str_; }
    const char *nextScanStr() const { return scan_buffer_.str_ + scan_buffer_.pos(); }
    size_t length() const { return scan_buffer_.length_; }
    bool empty() const { return scan_buffer_.empty(); }
    bool done() const { return scan_buffer_.atEnd(); }
    size_t pos() const { return scan_buffer_.pos(); }

    char skipWhiteSpace();
    char nextNonWhiteSpace();
    void advance() { scan_buffer_.advance(); }
    void advance(size_t steps) {scan_buffer_.advance(steps); }
    void back() { scan_buffer_.back(); }
    void back(size_t steps) {scan_buffer_.back(steps); }
    char curChar() const { return scan_buffer_.curChar(); }
    char nextChar() { return scan_buffer_.nextChar(); }

    /// \brief get a substring from the current position until the terminator
    /// end_ch or to the null terminator, or to a whitespace if stop_at_ws is
    /// true
    /// \note if stop_at_ws is false, substring will include whitespaces in the
    /// midle or at the end. The caller is responsible to remove trailing
    // whitespaces if it is desired. Leading whitespace is skipped anyway.
    char getSubstring(char end_ch, std::string& substring, bool stop_at_ws);

    /// \brief get a substring from the current position until the terminator
    /// listed in end_list or to the null terminator
    char getSubstring(const char* end_list, std::string& substring);

    /// \brief get a substring either quoted or terminated by the end_ch from the
    /// current position
    char getSubstringQuoted(char end_ch, std::string& substring);

    enum ErrorStatus
    {
        Err_MissingHexDigits,
        Err_MissingBinaryDigits,
        Err_IntegerOverflow,
        Err_Incomplete,
        Err_IncompleteChar,
        Err_MissinhgChar,
        Err_CharOverflow,
        Err_CharHexDigitMissing,
        Err_UCodeHexDigitMissing,
        Err_CharDecDigitMissingEnd,
        Err_InvalidOctDigit,
        Err_UCodeInvlid,
        Err_LineIsTooLong,
    };

    struct ScannedValue
    {
        ValueType vt_;
        union {
            alt_char_t char_;
            int64_t integer_;
            double double_;
        };

        const char *start_pos_{nullptr};
        const char *end_pos_{nullptr};
        std::string string_;
        size_t      start_line_{0};

        ScannedValue() { string_.reserve(1000); }

        void reset()
        {
            start_pos_ = nullptr;
            end_pos_ = nullptr;
            integer_ = 0;
            vt_ = NONE;
        }

        size_t scannedLength() const { return size_t(end_pos_ - start_pos_); }
        const char *scannedStart() const { return start_pos_; }
        const char *scannedEnd() const { return end_pos_; }

        template <int N>
        bool scanned(const char *str) const
        {
            //std::cout << "scannedLength=" << scannedLength() << " scanned:" << scannedStart() << std::endl;
            return scannedLength() == N && strEqual<N>(scannedStart(), str);
        }

        bool scanned(char ch) const
        {
            return scannedLength() == 1 && *start_pos_ == ch;
        }

        bool isNumber() const { return vt_ == DOUBLE || vt_ == INT; }

        double getDouble() const
        {
            return vt_ == DOUBLE
                    ? double_
                    : vt_ == INT
                        ? (integer_== std::numeric_limits<int64_t>::max()
                             ? std::numeric_limits<double>::max()
                             : integer_== std::numeric_limits<int64_t>::min()
                                ? std::numeric_limits<double>::min() : static_cast<double>(integer_)
                          )
                        : 0.0;
        }
        int64_t getInteger() const
        {
            return vt_ == INT
                    ? integer_
                    : vt_ == DOUBLE
                        ? (double_== std::numeric_limits<double>::max()
                             ? std::numeric_limits<int64_t>::max()
                             : double_== std::numeric_limits<double>::min()
                                ? std::numeric_limits<int64_t>::min() : static_cast<int64_t>(double_)
                          )
                        : int64_t(0.0);
        }
    };

    // Helper to get null terminated string from the scanned buffer
    struct GetScannedString
    {
        GetScannedString(const ScannedValue & scanned_value) : scanned_value_(scanned_value) {}
        const char* c_str()
        {
            if (!saved_end_char)
            {
                saved_end_char = *scanned_value_.end_pos_;
                if (saved_end_char)
                {
                    *const_cast<char*>(scanned_value_.end_pos_) = '\0';
                }
            }
            return scanned_value_.start_pos_;
        }
        ~GetScannedString()
        {
            if (saved_end_char)
            {
                *const_cast<char*>(scanned_value_.end_pos_) = saved_end_char;
            }
        }
      private:
        const ScannedValue& scanned_value_;
        char saved_end_char {'\0'};
    };

    template <int N>
    bool scanned(const char *str) const { return tv_.scanned<N>(str); }
    size_t scannedLength() const { return tv_.scannedLength(); }
    const char* scannedStart() const { return tv_.scannedStart(); }
    bool scanned(char ch) const { return tv_.scanned(ch); }
    size_t scannedStartPos() const { return  size_t(tv_.scannedStart()-scan_buffer_.head()); }
    size_t scannedEndPos() const { return  size_t(tv_.scannedEnd()-scan_buffer_.head()); }

    ValueType scannedValueType() const { return tv_.vt_; }
    bool isNumber() const { return tv_.isNumber(); }
    double getScannedDouble() const { return tv_.getDouble(); }
    int64_t geScannedInteger() const { return tv_.getInteger(); }
    std::string const &  getScannedString() const
    { static std::string empty_string; return tv_.vt_ == STRING ? tv_.string_ : empty_string; }
    alt_char_t  getScannedChar() const
    { return tv_.vt_ == CHAR ? tv_.char_ : 0; }

    void processScanned (std::function<void(const char*)> f)
    {
        GetScannedString scanned_str (tv_);
        f(scanned_str.c_str());
    }
    
    void fetchScanned (std::string& val) { val.append(tv_.scannedStart(), tv_.scannedLength()); }
    bool fetchValue (std::string& val) { tv_.string_.swap(val); return tv_.vt_ == STRING; }
    bool fetchValue (double& val) { val=tv_.double_; return tv_.vt_ == DOUBLE; }
    bool fetchValue (int64_t& val) { val=tv_.integer_; return tv_.vt_ == INT; }
    bool fetchValue (alt_char_t& val) { val=tv_.char_; return tv_.vt_ == CHAR; }

    uint64_t getUnsigned();
    int64_t getInteger();
    uint64_t getUnsignedFromDec();
    uint64_t getUnsignedFromHex(size_t digits = std::numeric_limits<size_t>::max());
    uint64_t getUnsignedFromOct();
    uint64_t getUnsignedFromBinary();

    void getLabel();
    void getNumber();
    void getChar();
    void getCharValue();
    void getString();

    void setErrStatus(ErrorStatus err) { err_ = err; }

    size_t saveUnscanned(std::string & savebuffer)
    {
        savebuffer.append(
            scan_buffer_.curPos(), scan_buffer_.length_ - scan_buffer_.pos_);
        return scan_buffer_.pos_;
    }

protected:
    friend class ParserStream;
    friend class ParserStreamContext;

    ScanBuffer &scan_buffer_;

    ScannedValue tv_;
    ErrorStatus err_;
};

inline char StrScan::skipWhiteSpace()
{
    char ch = scan_buffer_.curChar();
    while (isspace(ch)) ch = scan_buffer_.nextChar();
    return ch;
}

inline char StrScan::nextNonWhiteSpace()
{
    char ch = scan_buffer_.nextChar();
    while (isspace(ch)) ch = scan_buffer_.nextChar();
    return ch;
}

/**
 * \class StrParser
 * \ingroup util
 * \brief A string parser using operator >> and handles seperators for string splits
 */
class StrParser : public StrScan
{
public:
    StrParser (ScanBuffer &buffer) : StrScan(buffer) {}
    StrParser(const char *str, size_t length) : StrScan(scan_buffer_), scan_buffer_(str, length) {}
    StrParser(const char *begin, const char *end) : StrScan(scan_buffer_), scan_buffer_(begin, end) {}
    explicit StrParser(const std::string &str) : StrScan(scan_buffer_), scan_buffer_(str) {}
    explicit StrParser(const char *str) : StrScan(scan_buffer_), scan_buffer_(str) {}
    StrParser(StrParser &&oth) = default;
    StrParser(StrParser &oth) = default;

    void reset(const char *str, size_t length) { scan_buffer_.reset(str, length); }
    void reset(const char *begin, const char *end) { scan_buffer_.reset(begin, end); }
    void reset(const std::string &str) { scan_buffer_.reset(str); }
    void reset(const char *str) { scan_buffer_.reset(str); }
    void reset() { scan_buffer_.reset(); }

    // For any type that has static method fromString
    template <typename T>
    T toType()
    {
        toString();
        GetScannedString scanned_str (tv_);
        return T::fromString(scanned_str.c_str());
    }

    void toString();
    StrRef toStrRef();
    double toDouble();
    bool toBool();
    char toChar();
    alt_char_t toWChar();

    StrRefInLength toStrRefInLength();

    size_t split(std::vector<std::string> &substrings);

    StrParser &operator>>(int8_t &n)
    {
        n = int8_t(getInteger());
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(int16_t &n)
    {
        n = int16_t(getInteger());
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(int32_t &n)
    {
        n = int32_t(getInteger());
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(int64_t &n)
    {
        n = int64_t(getInteger());
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(char &n)
    {
        n = toChar();
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(alt_char_t &n)
    {
        n = toWChar();
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(uint8_t &n)
    {
        n = uint8_t(getUnsigned());
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(uint16_t &n)
    {
        n = uint16_t(getUnsigned());
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(uint32_t &n)
    {
        n = uint32_t(getUnsigned());
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(uint64_t &n)
    {
        n = uint64_t(getUnsigned());
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(double &n)
    {
        n = toDouble();
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(bool &n)
    {
        n = toBool();
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(std::string &n)
    {
        toString();
        n.assign(tv_.scannedStart(), tv_.scannedLength());
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(StrRef &n)
    {
        n = toStrRef();
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(StrRefInLength &n)
    {
        n = toStrRefInLength();
        skipSeparator();
        return *this;
    };
    StrParser &operator>>(std::vector<std::string> &strings)
    {
        split(strings);
        skipSeparator();
        return *this;
    }

    // For any type that has static method fromString
    template <typename T>
    StrParser &operator>>(T &value)
    {
        toString();
        {
            GetScannedString scanned_str (tv_);
            value = T::fromString(scanned_str.c_str());
        }
        skipSeparator();
        return *this;
    }

    // For EnumSet using refective Enum type only. Not work for standard c++ enum
    template <typename T>
    StrParser &operator>>(EnumSet<T> &value);

    // Create and release a block parser for a block using different format
    StrParser newBlockParser(char blockStart = 0, char newSeparator = 0);
    void releaseBlockParser(StrParser &blockParser);

    void setTerminator(char ch) { terminator_ = ch; }
    char getTerminator() const { return terminator_; }
    void setSplitSeparator(char ch) { splitSeparator_ = ch; }
    char getSplitSeparator() const { return splitSeparator_; }
    void setSkipLeadingSp(bool skip) { skipLeadingSp_ = skip; }
    void setSkipTrailingSp(bool skip) { skipTrailingSp_ = skip; }
    bool getSkipLeadingSp() const { return skipLeadingSp_; }
    bool getSkipTrailingSp() const { return skipTrailingSp_; }

    void setClearSeparator(bool clear) { clearSeparator_ = clear; }
    bool getClearSeparator() const { return clearSeparator_; }

    void addSeparator(char ch) { isSeparator_[uint8_t(ch)] = true; }
    void remSeparator(char ch) { isSeparator_[uint8_t(ch)] = false; }

    bool atValueEnd()
    {
        char ch = scan_buffer_.curChar();
        return scan_buffer_.atEnd() || !ch || isSeparator(ch) ||
               ch == terminator_;
    }

protected:
    void skipSeparator();
    void skipSplitSeparator();

    static bool isBlockStartCh(char ch);
    static char blockEndCh(char start_ch);

    bool isSeparator(char ch) const;

    ScanBuffer scan_buffer_;

    // Parer options
    char terminator_{'\0'};

    // When set, separator will be replaced by 0 after parsing
    bool clearSeparator_{false};

    // Options in split
    char splitSeparator_{','};
    bool skipLeadingSp_{false};
    bool skipTrailingSp_{false};

    char block_start_{'\0'};

    std::vector<bool> isSeparator_{std::vector<bool>(256, false)};
};

template <typename T>
StrParser &StrParser::operator>>(EnumSet<T> &value)
{
    char start_ch = scan_buffer_.curChar();
    char terminator = getTerminator();
    if (isBlockStartCh(start_ch))
    {
        setTerminator(blockEndCh(start_ch));
        scan_buffer_.nextChar(); // skip block start_ch
    }
    while (!atValueEnd())
    {
        T ev = toType<T>();
        if (T::isValid(ev))
            value.set(ev);
        skipSplitSeparator();
    }
    if (isBlockStartCh(start_ch))
    {
        setTerminator(terminator);
        scan_buffer_.nextChar(); // skip blockEndCh
    }
    skipSeparator();
    return *this;
}

} // namespace alt
