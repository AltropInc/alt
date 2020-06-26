#include "StrScan.h"
#include "StrUtils.h"

#include <assert.h>

using namespace alt;

uint64_t StrScan::getUnsigned ()
{
    if (scan_buffer_.atEnd())
    {
        return 0;
    }
	char const *p = scan_buffer_.curPos();
    if (*p=='0')
    {
        ++p;
        scan_buffer_.advance();
        if (*p=='x' || *p=='X')
        {
            scan_buffer_.advance();
            return getUnsignedFromHex();
        }
        else if (uint64_t(*p - '0') < 8)
        {
            return getUnsignedFromOct();
        }  
    }
    return getUnsignedFromDec();
}

uint64_t StrScan::getUnsignedFromDec ()
{
    uint64_t val = 0;
	uint64_t diff;
    char const *start_p = scan_buffer_.curPos();
    while (scan_buffer_.hasRemaining() && (diff = uint64_t(scan_buffer_.curChar() - '0')) < 10)
    {
      	uint64_t new_val = val * 10 + diff;
        if (new_val < val)
        {
            setErrStatus(Err_IntegerOverflow);
            tv_.end_pos_ = scan_buffer_.curPos();
            while (scan_buffer_.hasRemaining() && isDigit(scan_buffer_.curChar()))
            {
	            scan_buffer_.advance();
            }
            return val;
        }
        else
        {
            val = new_val;
        }
	    scan_buffer_.advance();
    }
    tv_.end_pos_ = scan_buffer_.curPos();
    return val;
}

uint64_t StrScan::getUnsignedFromHex (size_t digits)
{
    uint64_t val = 0;
	char const *start_p = scan_buffer_.curPos();
    uint64_t diff;
    while (scan_buffer_.hasRemaining() && digits)
    {
        const char ch = scan_buffer_.curChar();
        const char digit = hexDigit(ch);
        if (digit==INVALID_HEX_DIGIT)
        {
            break;
        }
        uint64_t new_val = (val << 4) + digit;
        if (new_val < val)
        {
            setErrStatus(Err_IntegerOverflow);
            tv_.end_pos_ = scan_buffer_.curPos();
            while (scan_buffer_.hasRemaining() && isHexDigit(scan_buffer_.curChar()))
            {
	            scan_buffer_.advance();
            }
            return val;
        }
        val = new_val;
	    scan_buffer_.advance();
        --digits;
    }
    tv_.end_pos_ = scan_buffer_.curPos();
    return val;
}

uint64_t StrScan::getUnsignedFromOct ()
{
    uint64_t val = 0;
	char const *start_p = scan_buffer_.curPos();
    uint64_t diff;
    while (scan_buffer_.hasRemaining() && (diff = uint64_t(scan_buffer_.curChar() - '0')) < 10)
    {
        if (diff>=8)
        {
            setErrStatus(Err_InvalidOctDigit);
            scan_buffer_.advance();
            continue;
        }
        uint64_t new_val = (val << 3) + diff;
        if (new_val < val)
        {
            setErrStatus(Err_IntegerOverflow);
            tv_.end_pos_ = scan_buffer_.curPos();
            while (scan_buffer_.hasRemaining() && isDigit(scan_buffer_.curChar()))
            {
	            scan_buffer_.advance();
            }
            return val;
        }
        val = new_val;
	    scan_buffer_.advance();
    }
    tv_.end_pos_ = scan_buffer_.curPos();
    return val;
}

uint64_t StrScan::getUnsignedFromBinary ()
{
    uint64_t val = 0;
	char const *start_p = scan_buffer_.curPos();
    uint64_t diff;
    while (scan_buffer_.hasRemaining() && (diff = uint64_t(scan_buffer_.curChar() - '0')) < 2)
    {
        uint64_t new_val = (val << 1) + diff;
        if (new_val < val)
        {
            setErrStatus(Err_IntegerOverflow);
            tv_.end_pos_ = scan_buffer_.curPos();
            while (scan_buffer_.hasRemaining() && isBinDigit(scan_buffer_.curChar()))
            {
	            scan_buffer_.advance();
            }
            return val;
        }
        val = new_val;
	    scan_buffer_.advance();
    }
    tv_.end_pos_ = scan_buffer_.curPos();
    return val;
}

int64_t StrScan::getInteger ()
{
	char const *p = scan_buffer_.curPos();
	if (*p=='-')
    {
        scan_buffer_.advance();
		return -int64_t(getUnsigned());
    }
	return int64_t(getUnsigned());
}


void StrScan::getLabel()
{
    char ch = scan_buffer_.curChar();
    while ((ch>='A' && ch<='Z') || (ch>='a' && ch<='z'))
    {
        ch = scan_buffer_.nextChar();
        if (scan_buffer_.atEnd())
        {
            break;
        }
    }
}

void StrScan::getNumber()
{
    tv_.integer_ = 0;
    tv_.start_pos_ = scan_buffer_.curPos();
    char ch = *tv_.start_pos_;
    bool integerGot = false;
    bool isNeg = false;
    if (ch=='-')
    {
        ch = scan_buffer_.nextNonWhiteSpace();
        isNeg = true;
    }

    if (ch>='0' && ch<='9')
    {
        bool scanned{false};
        bool nothing_scanned{false};
        const char* start_p{0};
        if (ch=='0')
        {
            char nch = scan_buffer_.peerNext();
            if (nch=='x' || nch=='X')
            {  
                scan_buffer_.advance(2);
                start_p = scan_buffer_.curPos();
                scanned = true;
                tv_.integer_ = getUnsignedFromHex();
                if (start_p == scan_buffer_.curPos()) setErrStatus(Err_MissingHexDigits);
            }
            else if (nch=='b')
            {
                scan_buffer_.advance(2);
                start_p = scan_buffer_.curPos();
                scanned = true;
                tv_.integer_ = getUnsignedFromBinary();
                if (start_p == scan_buffer_.curPos()) setErrStatus(Err_MissingBinaryDigits);
            }
            else if (nch>='0' || nch<='7')
            {
                scan_buffer_.advance(1);
                start_p = scan_buffer_.curPos();
                scanned = true;
                tv_.integer_ = getUnsignedFromOct();
            }
        }
        if (!scanned)
        {
            start_p = scan_buffer_.curPos();
            tv_.integer_ = getUnsignedFromDec();
        }
        if (isNeg)
        {
            tv_.integer_ = -tv_.integer_;
        }
        integerGot = true;
        ch = scan_buffer_.curChar();
    }
    else if (ch!='.')
    {
        size_t savedPos = scan_buffer_.pos();
        const char * saved_start_pos = tv_.start_pos_;
        tv_.start_pos_ = scan_buffer_.curPos();
        getLabel();
        tv_.end_pos_ = scan_buffer_.curPos();
        if (tv_.scanned<8>("Infinity") || tv_.scanned<3>("NaN"))
        {
            tv_.integer_ = isNeg ? std::numeric_limits<int64_t>::min()
                                 : std::numeric_limits<int64_t>::max();
            tv_.vt_ = INT;
            tv_.start_pos_ = saved_start_pos;
            return;
        }
        scan_buffer_.resetPos(savedPos);
        tv_.vt_ = TOKEN;
        tv_.start_pos_ = saved_start_pos;
        tv_.end_pos_ = scan_buffer_.curPos();
        return;
    }

    if (ch=='.')
    {
        ch = scan_buffer_.nextChar();
        if (ch>='0' && ch<='9')
        {
            const char* start_cp = scan_buffer_.curPos();
            double decimal = double(getUnsignedFromDec());
            size_t scanned_digits = size_t(tv_.end_pos_-start_cp);
            tv_.double_ = double(tv_.integer_);
            if (isNeg)
            {
                tv_.double_ -= decimal/s_exp10[scanned_digits];
            }
            else
            {
                tv_.double_ += decimal/s_exp10[scanned_digits];
            }
            tv_.vt_ = DOUBLE;
        }
        else if (integerGot)
        {
            tv_.vt_ = DOUBLE;
        }
        else
        {
            tv_.vt_ = TOKEN;
        }
		ch = scan_buffer_.curChar();
    }
    else
    {
        tv_.vt_ = integerGot ? INT : NONE;
    }

    if (ch=='E' || ch=='e')
    {
        ch = scan_buffer_.nextChar();
        bool isNegExp = ch=='-';
        if (ch=='+' || ch=='-')
        {
            scan_buffer_.advance();
            uint64_t exp = getUnsignedFromDec();
            if (isNegExp)
            {
                if (tv_.vt_==DOUBLE)
                {
                    tv_.double_ = exp<=18 ? tv_.double_/s_exp10[exp] : 0.0;
                }
                else if (tv_.vt_==INT)
                {
                    tv_.double_ = exp<=18 ? double(tv_.integer_)/s_exp10[exp] : 0.0;
                    tv_.vt_==DOUBLE;
                }
            }
            else
            {
                if (tv_.vt_==DOUBLE)
                {
                    tv_.double_ = exp<=18 ? tv_.double_*s_exp10[exp] :
                        isNeg ? std::numeric_limits<double>::min() : std::numeric_limits<double>::max();
                }
                else if (tv_.vt_==INT)
                {
                    tv_.integer_ = exp<=18 ? tv_.integer_*s_exp10[exp] :
                        isNeg ? std::numeric_limits<int64_t>::min() : std::numeric_limits<int64_t>::max();
                }
            }
        }
    }
    /*
    if (tv_.vt_==INT)
    {
        char surffix = scan_buffer_.curChar();
        bool is_unsigned {false};
        bool is_long {false};
        if (surffix=='u' || surffix=='U')
        {
            is_unsigned = true;
            surffix = scan_buffer_.nextChar();
        }
        if (surffix=='l' || surffix=='L')
        {
            is_long = true;
            surffix = scan_buffer_.nextChar();
        }
    }
    */
    tv_.end_pos_ = scan_buffer_.curPos();
}


void StrScan::getString()
{
    tv_.string_.clear();
    char ch = scan_buffer_.nextChar();

    while (ch && ch!='"')
    {
        if (ch=='\\')
        {
            ch = scan_buffer_.nextChar();
            switch (ch)
            {
                case '\\': tv_.string_.push_back(ch); break;
                case '/': tv_.string_.push_back(ch); break;
                case '"': tv_.string_.push_back(ch); break;
                case 'a': tv_.string_.push_back('\a'); break;
                case 'b': tv_.string_.push_back('\b'); break;
                case 'e': tv_.string_.push_back('\e'); break;
                case 'v': tv_.string_.push_back('\v'); break;
                case 'n': tv_.string_.push_back('\n'); break;
                case 't': tv_.string_.push_back('\t'); break;
                case 'r': tv_.string_.push_back('\r'); break;
                case 'f': tv_.string_.push_back('\f'); break;
                case '?': tv_.string_.push_back('\?'); break;
                case 'x':
                case 'X':
                {
                    scan_buffer_.nextChar();
                    const char * start_p = scan_buffer_.curPos();
                    size_t digits = ch=='x' ? 2 : 8;
                    uint64_t code = getUnsignedFromHex(digits);
                    if (size_t(scan_buffer_.curPos() - start_p) < digits)
                    {
                        setErrStatus(Err_CharHexDigitMissing);
                    }
                    tv_.string_.push_back(char(code));
                    break;
                }
                case 'u':
                case 'U':
                {
                    scan_buffer_.nextChar();
                    const char * start_p = scan_buffer_.curPos();
                    size_t digits = ch=='u' ? 4 : 8;
                    uint64_t code = getUnsignedFromHex(digits);
                    if (size_t(scan_buffer_.curPos() - start_p) < digits)
                    {
                        setErrStatus(Err_UCodeHexDigitMissing);
                    }
                    if (code <= 0X7F) // one byte
                    {
                        tv_.string_.push_back(char(code)); 
                    }
                    else if (code <= 0X7FF) // two bytes
                    {
                        tv_.string_.push_back(char(code >> 6) | 0XC0); 
                        tv_.string_.push_back(char(code & 0x3F) | 0X80);
                    }
                    else if (code <= 0XD7FF) // three bytes
                    {
                        tv_.string_.push_back(char(code >> 12) | 0XE0); 
                        tv_.string_.push_back(char((code & 0xFFF) >> 6 | 0X80));
                        tv_.string_.push_back(char(code & 0x3F) | 0X80);
                    }
                    else if (code >= 0xE000 && code <= 0X10FFFF)  // four bytes
                    {
                        tv_.string_.push_back(char(code >> 18) | 0XE0); 
                        tv_.string_.push_back(char((code & 0x3FFFF) >> 12 | 0X80));
                        tv_.string_.push_back(char((code & 0xFFF) >> 6 | 0X80));
                        tv_.string_.push_back(char(code & 0x3F) | 0X80);
                    }
                    else
                    {
                        setErrStatus(Err_UCodeInvlid);
                    }
                    
                    break;
                }
                default: tv_.string_.push_back(ch); break;
            }
        }
        tv_.string_.push_back(ch);
        ch = scan_buffer_.nextChar();
    }
    if (ch!='"')
    {
        setErrStatus(Err_Incomplete);
    }
    else
    {
        scan_buffer_.advance();
    }
    tv_.vt_ = STRING;
    tv_.end_pos_ = scan_buffer_.curPos();
}

void StrScan::getCharValue()
{
    char ch = scan_buffer_.curChar();
    // std::cout << "getCharValue " << scan_buffer_.curPos() << std::endl;
    if (ch=='\\')
    {
        ch = scan_buffer_.nextChar();
        switch (ch)
        {
            case '\\':
            case '\'':
            case '\"':
                      tv_.char_ = alt_char_t(ch); break;
            case 'a': tv_.char_ = alt_char_t('\a'); break;
            case 'b': tv_.char_ = alt_char_t('\b'); break;
            case 'e': tv_.char_ = alt_char_t('\e'); break;
            case 'v': tv_.char_ = alt_char_t('\v'); break;
            case 'n': tv_.char_ = alt_char_t('\n'); break;
            case 't': tv_.char_ = alt_char_t('\t'); break;
            case 'r': tv_.char_ = alt_char_t('\r'); break;
            case 'f': tv_.char_ = alt_char_t('\f'); break;
            case '?': tv_.char_ = alt_char_t('\?'); break;
            case 'x':
            case 'X':
            {
                scan_buffer_.nextChar();
                const char * start_p = scan_buffer_.curPos();
                size_t digits = ch=='x' ? 2 : 8;
                uint64_t code = getUnsignedFromHex(digits);
                if (size_t(scan_buffer_.curPos() - start_p) < digits)
                {
                    setErrStatus(Err_CharHexDigitMissing);
                }
                tv_.char_ = alt_char_t(code);
                break;
            }
            case '#':
            {
                scan_buffer_.nextChar();
                const char * start_p = scan_buffer_.curPos();
                uint32_t code = getUnsignedFromDec();
                //if ((code > 0XD77 && code < 0xE000) || code > 0X10FFFF)
                //{
                //    setErrStatus(Err_UCodeInvlid);
                //}
                ch = scan_buffer_.curChar();
                if (ch!=';')
                {
                    setErrStatus(Err_CharDecDigitMissingEnd);
                }
                else
                {
                    scan_buffer_.nextChar();
                }
                tv_.char_ = alt_char_t(code);             
                break;
            }
            case 'u':
            case 'U':
            {
                scan_buffer_.nextChar();
                const char * start_p = scan_buffer_.curPos();
                size_t digits = ch=='u' ? 4 : 8;
                uint32_t code = getUnsignedFromHex(digits);
                if (size_t(scan_buffer_.curPos() - start_p) < digits)
                {
                    setErrStatus(Err_UCodeHexDigitMissing);
                }
                if ((code > 0XD77 && code < 0xE000) || code > 0X10FFFF)
                {
                    setErrStatus(Err_UCodeInvlid);
                }
                tv_.char_ = alt_char_t(code);
                break;
            }
            default: tv_.char_ = alt_char_t(ch); break;
        }
    }
    else
    {
        size_t bytes = scanUTF8String(scan_buffer_.curPos(), tv_.char_);
        scan_buffer_.advance(bytes);
    }
    tv_.vt_ = CHAR;
}

void StrScan::getChar()
{
    char ch = scan_buffer_.nextChar();
    if (ch && ch!='\'')
    {
        getCharValue();
        char ch = scan_buffer_.curChar();
        if (ch!='\'')
        {
            setErrStatus(Err_IncompleteChar);
        }
        else
        {
            scan_buffer_.advance();
        }
    }
    else
    {
        tv_.char_ = 0;
        tv_.vt_ = CHAR;
        setErrStatus(Err_MissinhgChar);
        if (ch=='\'')
        {
            scan_buffer_.advance();
        }
    }
    tv_.end_pos_ = scan_buffer_.curPos();
}

char StrScan::getSubstring(char end_ch, std::string& substring, bool stop_at_ws)
{
    // skip leading white space
    char ch = skipWhiteSpace();
    while (ch && ch!=end_ch)
    {
        if (isspace(ch) && stop_at_ws)
        {
            break;
        }
        substring.push_back(ch);
        ch = scan_buffer_.nextChar();
    }
    return ch;
}

char StrScan::getSubstring(const char* end_list, std::string& substring)
{
    // skip leading white space
    char ch = skipWhiteSpace();
    while (ch && (!end_list || strchr(end_list, ch)==nullptr))
    {
        substring.push_back(ch);
        ch = scan_buffer_.nextChar();
    }
    return ch;
}


char StrScan::getSubstringQuoted(char end_ch, std::string& text_scanned)
{
    char ch = skipWhiteSpace();
    if (ch=='\"')
    {
        StrScan::getString();
        text_scanned.swap(tv_.string_);
        ch = scan_buffer_.nextNonWhiteSpace();
    }
    else
    {
        ch = getSubstring(end_ch, text_scanned, true /*stop at whitespace*/);
    }
    return ch;
}

//=============================================================================
// StrParser
//=============================================================================
void StrParser::toString ()
{
	char const *p = scan_buffer_.curPos();
	tv_.start_pos_ = p;
	while (*p && !isSeparator_[*p] &&
		   (!splitSeparator_ || *p!=splitSeparator_) &&
		   *p!=terminator_)
    {
        ++p; scan_buffer_.advance();
    }
    tv_.vt_ = STRING;
	tv_.end_pos_ = scan_buffer_.curPos();
}

StrRef StrParser::toStrRef ()
{
    assert(clearSeparator_);
	char const *p = scan_buffer_.curPos();
    char const * startp = p;
	while (*p && !isSeparator(*p))
    {
        ++p; scan_buffer_.advance();
    }
    return StrRef(startp);
}

StrRefInLength StrParser::toStrRefInLength ()
{
	char const *p = scan_buffer_.curPos();
    char const * startp = p;
	while (*p &&  !isSeparator(*p))
    {
        ++p; scan_buffer_.advance();
    }
    return StrRefInLength(startp, size_t(p-startp));
}

bool StrParser::toBool ()
{
	size_t startp = scan_buffer_.pos();
	char const *p = scan_buffer_.curPos();
    char startCh = *p;
	while (*p && !isSeparator(*p))
    {
        scan_buffer_.advance();
        ++p;
    }

    return startCh=='t' || startCh=='T' || startCh=='1';
}

char StrParser::toChar ()
{
    char ch = scan_buffer_.curChar();
	if (ch &&  !isSeparator(ch))
    {
        scan_buffer_.advance();
        return ch;
    }
    return '\0';
}

alt_char_t StrParser::toWChar()
{
    char ch = scan_buffer_.curChar();
    if (ch && !isSeparator(ch))
    {
       getCharValue();
       if (tv_.vt_ == CHAR)
       {
           return tv_.char_;
       }
    }
    return 0;
}

double StrParser::toDouble()
{
    getNumber();
    return tv_.getDouble();
}

bool StrParser::isBlockStartCh(char ch)
{
	return ch=='[' || ch=='{' || ch=='(' || ch=='<';
}

char StrParser::blockEndCh(char block_start)
{
    switch (block_start)
    {
        case '[': return ']';
        case '{': return '}';
        case '(': return ')';
        case '<': return '>';
    }
	return '\0';
}

StrParser StrParser::newBlockParser(char block_start, char new_separator)
{
    if (block_start)
    {
        // skip block start
        scan_buffer_.advance();
     }
    StrParser block_parser(scan_buffer_);
    if (new_separator)
    {
        block_parser.setSplitSeparator(new_separator);
    }
	if (block_start)
	{
    	block_parser.setTerminator(blockEndCh(block_start));
    }
    block_parser.block_start_ = block_start;
    return block_parser;
}

void StrParser::releaseBlockParser(StrParser& block_parser)
{
    if (block_parser.block_start_)
    {
        // skip block end
        scan_buffer_.advance();
    }
}

size_t StrParser::split (std::vector<std::string>& substrings)
{
	size_t scanned(0);
	size_t start_pos(scan_buffer_.pos());
	size_t end_pos(start_pos);
	bool string_started (false);
	while (scan_buffer_.hasRemaining() && scan_buffer_.curChar()!=terminator_)
	{
	    if (scan_buffer_.curChar()==splitSeparator_)
		{
			substrings.emplace_back(scan_buffer_.at(start_pos), end_pos-start_pos);
			string_started = false;
			++scanned;
			start_pos = scan_buffer_.pos()+1;
			end_pos = start_pos;
		}
		else if (isspace(scan_buffer_.curChar()))
		{
			if (skipLeadingSp_ && !string_started)
			{
				++start_pos;
				++end_pos;
			}
			else if (!skipTrailingSp_  && string_started)
			{
				++end_pos;
			}   
		}
		else
		{
			string_started = true;
			++end_pos;
		}
		scan_buffer_.advance();
  	}
	if (end_pos>start_pos)
	{
	    substrings.emplace_back(scan_buffer_.at(start_pos), end_pos-start_pos);
		++scanned;
	}
	return scanned;
}

inline bool StrParser::isSeparator(char ch) const
{
    return isSeparator_[uint8_t(ch)] || (splitSeparator_ && ch==splitSeparator_);
}

void StrParser::skipSeparator()
{
    if (scan_buffer_.hasRemaining() && isSeparator(scan_buffer_.curChar()))
    {
        if (clearSeparator_)
        {
            *(const_cast<char*>(scan_buffer_.curPos())) = '\0';
        }
        scan_buffer_.advance();
    }
}

void StrParser::skipSplitSeparator()
{
    if (scan_buffer_.hasRemaining() && scan_buffer_.curChar()==splitSeparator_)
    {
        if (clearSeparator_)
        {
            *(const_cast<char*>(scan_buffer_.curPos())) = '\0';
        }
        scan_buffer_.advance();
    }
}
