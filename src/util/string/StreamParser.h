/**
 * @file StreamParer.h
 * @brief Defines StreamParer, ParserStream and  StreamParserConext
 * @author David Shang
 * @version 1.00
 * $Id: $
 */

#pragma once

#include "StrScan.h"

namespace alt
{
class ParserStreamContext;

class ParserStream
{
public:
    ParserStream(std::istream * input);
    ~ParserStream();

    static ParserStream* createFileStream (const char* file_path);
    bool atEnd() { return at_stream_end_; }

protected:
    friend class ParserStreamContext;
    size_t                 line_           {0};
    std::istream *         input_stream_   {nullptr};
    bool                   at_stream_end_  {false};
    std::string            file_path_;
    bool                   owns_stream_    {false};
};

class StreamParser: public StrScan
{
    public:
        StreamParser(ParserStreamContext& context);
        StreamParser(ParserStreamContext& context, const char *str, size_t length);
        StreamParser(ParserStreamContext& context, const char *begin, const char *end);
        StreamParser(ParserStreamContext& context, const std::string &str);
        StreamParser(ParserStreamContext& context, const char *str);
        StreamParser(ParserStreamContext& context, std::istream * input_stream);

    protected:
        friend class ParserStreamContext;
        ParserStreamContext & context_;
        ScanBuffer scan_buffer_;

};

class  ParserStreamContext
{
    public:
        ParserStreamContext(size_t line_buffer_sz=1024*1024*3);
           // : StrScan(scan_buffer_), stream_(stream) {}
            /*
        StreamParser(const char *str, size_t length)
            : StrScan(scan_buffer_), scan_buffer_(str, length) {}
        StreamParser(const char *begin, const char *end)
            : StrScan(scan_buffer_), scan_buffer_(begin, end) {}
        explicit StreamParser(const std::string &str)
            : StrScan(scan_buffer_), scan_buffer_(str) {}
        explicit StreamParser(const char *str)
            : StrScan(scan_buffer_), scan_buffer_(str) {}
*/
        ~ParserStreamContext();

        bool nextLine();

        bool pushStream (ParserStream * stream);
        bool pushStream (std::istream *);
        bool pushFileStream (const char* file_path);
        bool popStream();

        bool pushParser (StreamParser * parser);
        bool popParser ();

        StreamParser* parser() { return current_parser_; };
        ParserStream* stream() { return current_stream_; };

        bool atStreamEnd() { return !current_stream_ || current_stream_->atEnd(); }

    protected:
        friend class StreamParser;
        
        ParserStream *          current_stream_ {nullptr};
        StreamParser *          current_parser_ {nullptr};

        // line buffer for the current stream
        char*                   line_buffer_    {nullptr};
        size_t                  line_buffer_sz_ {0};

        struct StreamContext
        {
            ParserStream * stream_ { nullptr };
            std::string stream_line_buffer_saved_;
            size_t scan_pos_  {0};
        };
        struct ParserContext
        {
            StreamParser * parser_ { nullptr };
        };
        std::vector<StreamContext>    stream_context_;
        std::vector<ParserContext>    parser_context_;
};

} // namespace alt
