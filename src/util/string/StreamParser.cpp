#include "StreamParser.h"
#include "StrUtils.h"
#include <sstream>

using namespace alt;

//==============================================================================
// ParserStream
//==============================================================================
ParserStream::ParserStream(std::istream * input)
    : input_stream_(input)
{}

ParserStream::ParserStream(const char * input)
    : input_stream_(new std::stringstream(input, std::ios_base::in))
    , owns_stream_(true)
{}

ParserStream::~ParserStream()
{
    if (input_stream_ && owns_stream_)
    {
        delete input_stream_;
        input_stream_ = nullptr;
    }
}

ParserStream* ParserStream::createFileStream (const char* file_path)
{
    auto file_stream = new std::ifstream(file_path);
    if (file_stream->is_open())
    {
        auto res = new ParserStream(file_stream);
        res->file_path_   = file_path;
        res->owns_stream_ = true;
        return res;
    }
    delete file_stream;
    return nullptr;
}

void ParserStream::registerError(const char* err, size_t pos)
{
    errors_.emplace_back(ErrorInfo{err, line_, pos});
    std::cout << "Error: " << err << " at line " << line_ << ", pos " << pos << std::endl;
}

//==============================================================================
// StreamParser
//==============================================================================
StreamParser::StreamParser(ParserStreamContext& context)
    : StrScan(scan_buffer_)
    , context_(context)
{
    context_.current_parser_ = this;
}

StreamParser::StreamParser(ParserStreamContext& context, const char *str, size_t length)
    : StrScan(scan_buffer_), context_(context),  scan_buffer_(str, length)
{
    context_.current_parser_ = this;
}

StreamParser::StreamParser(ParserStreamContext& context, const char *begin, const char *end)
    : StrScan(scan_buffer_), context_(context), scan_buffer_(begin, end)
{
    context_.current_parser_ = this;
}

StreamParser::StreamParser(ParserStreamContext& context, const std::string &str)
    : StrScan(scan_buffer_), context_(context), scan_buffer_(str)
{
    context_.current_parser_ = this;
}

StreamParser::StreamParser(ParserStreamContext& context, const char *str)
    : StrScan(scan_buffer_), context_(context), scan_buffer_(str)
{
    context_.current_parser_ = this;
}

StreamParser::StreamParser(ParserStreamContext& context, std::istream * input_stream)
    : StrScan(scan_buffer_), context_(context)
{
    context_.current_stream_ = new ParserStream(input_stream);
    context_.current_parser_ = this;
}

char StreamParser::skipWhiteSpace()
{
    char ch = scan_buffer_.curChar();
    while (!ch || isspace(ch))
    {
        if (!ch)
        {
            if (!context_.nextLine())
            {
                return '\0';
            }
            ch = scan_buffer_.curChar();
        }
        else
        {
            ch = scan_buffer_.nextChar();
        }
    }
    return ch;
}

char StreamParser::skipWhiteSpace(std::string& text_scanned)
{
    char ch = scan_buffer_.curChar();
    while (!ch || isspace(ch))
    {
        if (!ch)
        {
            if (!context_.nextLine())
            {
                return '\0';
            }
            text_scanned.push_back('\n');
            ch = scan_buffer_.curChar();
        }
        else
        {
            text_scanned.push_back(ch);
            ch = scan_buffer_.nextChar();
        }
    }
    return ch;
}

char StreamParser::curChar()
{
    char ch = scan_buffer_.curChar();
    if (!ch)
    {
        if (!context_.nextLine())
        {
            return '\0';
        }
        return scan_buffer_.curChar();
    }
    return ch;
}

char StreamParser::curChar(std::string& text_scanned)
{
    char ch = scan_buffer_.curChar();
    if (!ch || ch=='\r')
    {
        if (!context_.nextLine())
        {
            return '\0';
        }
        text_scanned.push_back('\n');
        return scan_buffer_.curChar();
    }
    return ch;
}

char StreamParser::nextChar(std::string& text_scanned)
{
    char ch = scan_buffer_.nextChar();
    if (!ch)
    {
        if (!context_.nextLine())
        {
            return '\0';
        }
        text_scanned.push_back('\n');
        return '\n';
    }
    return ch;
}

char StreamParser::nextChar(bool skip_white_space)
{
    char ch = scan_buffer_.nextChar();
    while (!ch)
    {
        if (!context_.nextLine())
        {
            return '\0';
        }
        ch = scan_buffer_.curChar();
        if (skip_white_space && isspace(ch))
        {
            ch = skipWhiteSpace();
        }
    }
    return ch;
}

char StreamParser::skipToChar(char target)
{
    char ch = scan_buffer_.curChar();
    while (!ch || ch!=target)
    {
        if (!ch)
        {
            if (!context_.nextLine())
            {
                return '\0';
            }
            ch = scan_buffer_.curChar();
        }
        else
        {
            ch = scan_buffer_.nextChar();
        }
        ch = scan_buffer_.curChar();
    }
    return ch;
}

//==============================================================================
// ParserStreamContext
//==============================================================================
ParserStreamContext::ParserStreamContext(size_t line_buffer_sz)
   : line_buffer_ ((char*)::malloc(line_buffer_sz))
   , line_buffer_sz_(line_buffer_sz)
{
}
        
ParserStreamContext::~ParserStreamContext()
{
    if (line_buffer_)
    {
        ::free(line_buffer_);
    }
    if (current_stream_)
    {
        delete current_stream_;
    }
    for (auto stream_context: stream_context_)
    {
        if (stream_context.stream_)
        {
            delete stream_context.stream_;
        }
    }
}

bool ParserStreamContext::nextLine()
{
    //std::cout << "nextline called" << std::endl;

    if (!current_stream_ || !current_stream_->input_stream_ || current_stream_->at_stream_end_)
    {
        return false;
    }

    std::istream * input = current_stream_->input_stream_;

    input->getline(line_buffer_, line_buffer_sz_);
    if (current_parser_)
    {
        current_parser_->scan_buffer_.str_ = line_buffer_;
        current_parser_->scan_buffer_.pos_ = 0;
        current_parser_->scan_buffer_.length_ = input->gcount();
    }
    //std::cout << "nextline line_bytes=" << current_parser_->scan_buffer_.length_ << " contents="
    //          << current_parser_->scan_buffer_.curPos() << std::endl;

    if (input->eof())
    {
        //std::cout << "next line eof"  << std::endl;
        current_stream_->at_stream_end_ = true;
    }
    else if (input->rdstate() & std::ios::failbit)
    {
        //std::cout << "Err_LineIsTooLong "  << input->rdstate() <<  std::endl;
        if (current_parser_)
        {
            current_parser_->setErrStatus(StrScan::Err_LineIsTooLong);
        }
    }
    else if (current_parser_)
    {
        auto & scan_buffer = current_parser_->scan_buffer_;
        if (scan_buffer.length_ > 0 && *(scan_buffer.str_+scan_buffer.length_-1)=='\0')
        {
            // remove the count for eol
            --scan_buffer.length_;
        }
    }
    
    //line_buffer_content_sz_ = scan_buffer.length_;
    ++current_stream_->line_;
    return true;
}

bool ParserStreamContext::pushStream (ParserStream * stream)
{
    if (!stream)
    {
        return false;
    }

    if (current_stream_)
    {
        stream_context_.push_back(StreamContext());
        StreamContext& context = stream_context_.back();

        // Save unsacnned in scan_buffer_ only when we have input_stream_
        if ( current_parser_ &&
             current_stream_->input_stream_ &&
             !current_parser_->scan_buffer_.atEnd())
        {
            context.scan_pos_ = current_parser_->saveUnscanned(
                context.stream_line_buffer_saved_
            );
        }
        context.stream_ = current_stream_;
    }
    current_stream_ = stream;
    if (current_parser_)
    {
        current_parser_->scan_buffer_.reset();
    }
    return true;
}

bool ParserStreamContext::popStream ()
{
    if (stream_context_.empty())
    {
        return false;
    }
    StreamContext& context = stream_context_.back();
    // restore unscanned
    if (!context.stream_line_buffer_saved_.empty())
    {
        ::memcpy(line_buffer_ + context.scan_pos_,
            context.stream_line_buffer_saved_.c_str(),
            context.stream_line_buffer_saved_.length());
        context.stream_line_buffer_saved_.clear();
    }
    delete current_stream_;
    current_stream_ = context.stream_;
    stream_context_.pop_back();
    return true;
}

bool ParserStreamContext::pushFileStream (const char* file_path)
{
    auto stream = ParserStream::createFileStream(file_path);
    if (stream)
    {
        return pushStream(stream);
    }
    return false;
}

bool ParserStreamContext::pushStream (std::istream * stream)
{
    return pushStream(new ParserStream(stream));
}

bool ParserStreamContext::pushStream (const char * input)
{
    return pushStream(new ParserStream(input));
}

bool ParserStreamContext::pushParser (StreamParser * parser)
{
    if (current_parser_)
    {
        parser_context_.push_back(ParserContext());
        ParserContext& context = parser_context_.back();
        parser->scan_buffer_ = current_parser_->scan_buffer_;
        context.parser_ = current_parser_;
    }
    current_parser_ = parser;
    nextLine();
    return true;
};

bool ParserStreamContext::pushParser(StreamParser * parser, std::istream * str)
{
    if (!pushStream(str))
    {
        return false;
    }
    return pushParser(parser);
}

bool ParserStreamContext::pushParser(StreamParser * parser, const char * str)
{
    if (!pushStream(str))
    {
        return false;
    }
    return pushParser(parser);
}

bool ParserStreamContext::pushFileParser(StreamParser * parser, const char * file_path)
{
    if (!pushFileStream(file_path))
    {
        return false;
    }
    return pushParser(parser);
}

bool ParserStreamContext::pushParser(StreamParser * parser, const std::filesystem::path& path)
{
    if (!pushStream(path))
    {
        return false;
    }
    return pushParser(parser);
}

void ParserStreamContext::registerError(const char* err)
{
    if (current_stream_ && current_parser_)
    {
        current_stream_->registerError(err, current_parser_->scan_buffer_.pos());
    }
}

bool ParserStreamContext::popParser ()
{
    if (parser_context_.empty())
    {
        return false;
    }
    ParserContext& context = parser_context_.back();
    // Note: ParserStreamContext does not own parser
    current_parser_ = context.parser_;
    current_parser_->scan_buffer_ = context.parser_->scan_buffer_;
    parser_context_.pop_back();
    return true;
}
