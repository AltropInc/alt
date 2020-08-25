#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file StreamParer.h
 * @library alt_util
 * @brief definition of ParserStreamContext, ParserStream, and StreamParser.
 * ParserStreamContext maintains a stack of streams and a stack of stream parsers.
 * The parsering process is conducted by the current parser on the current stream.
 * ParserStreamContext owns streams but not parsers. Streams and parsers are
 * pushed and popped independently.
*/

#include "StrScan.h"
#include <filesystem>

namespace alt
{
class ParserStreamContext;

using ParseErrID = int;
struct ErrorInfo
{
    ParseErrID     error_id_;      // error id
    std::string    extra_info_;    // extra info about the error
    size_t         line_ {0};      // line number where the error occurs
    size_t         pos_ {0};       // position in the line where the error occurs

    ErrorInfo (ParseErrID id, const char* extra_info, size_t line, size_t pos)
        :error_id_(id), line_(line), pos_(pos)
    {
        if (extra_info) extra_info_ = extra_info;
    }
};
using ErrorInfoVec = std::vector<ErrorInfo>;

/**
 * \class ParserStream
 * \ingroup StringUtils
 * \brief A stream to be scanned by a parser. The stream can be either an in-memory
 * string stream or a file stream
 */
class ParserStream
{
public:

    /// \brief constructor to create a ParserStream with string stream
    ParserStream(std::istream * input);

    /// \brief constructor to create a ParserStream with a C string for the string stream
    ParserStream(const char * input);

    ~ParserStream();

    /// \brief create a ParserStream with a file stream of the given file path
    /// \return the ParserStream created or nullptr if the creation fails
    static ParserStream* createFileStream (const char* file_path);

     /// \brief create a ParserStream with a file stream of the given file path
    /// \return the ParserStream created or nullptr if the creation fails   
    static ParserStream* createFileStream (const std::filesystem::path& file_path)
    {
        return createFileStream(file_path.string().c_str());
    }

    /// \brief tells if the current parsing position reaches to end
    bool atEnd() { return at_stream_end_; }

    /// \brief register error in parsing
    /// \param err the error string
    /// \param pos the postion in the current line buffer
    void registerError(ParseErrID err, const char* extra_into, size_t pos);

    /// \brief returns regitered error list
    const ErrorInfoVec& getErrors() const { return errors_; }

protected:
    friend class ParserStreamContext;
    size_t                 line_           {0};        // the current line in parsing
    std::istream *         input_stream_   {nullptr};  // the stream in parsing 
    bool                   at_stream_end_  {false};    // indicator when we reaches the stream end 
    std::string            file_path_;                 // file path if it is a file stream
    std::string            original_file_path_;        // original file path if file_path_ is the local cached path
    bool                   owns_stream_    {false};    // whether this object owns the input_stream_
                                                       // If true, input_stream_ needs to be released
    ErrorInfoVec           errors_;                    // registered erros for this stream during parsing
};


class ParserErrorReporter
{
  public:
    virtual void reportError(void* owner, const std::string& file_path, const ErrorInfoVec& errors) =0; 
};

/**
 * \class StreamParser
 * \ingroup StringUtils
 * \brief A stream parser that handles multiple lines from a stream. 
 */
class StreamParser: public StrScan
{
    public:
        StreamParser(ParserStreamContext& context);
        StreamParser(ParserStreamContext& context, const char *str, size_t length);
        StreamParser(ParserStreamContext& context, const char *begin, const char *end);
        StreamParser(ParserStreamContext& context, const std::string &str);
        StreamParser(ParserStreamContext& context, const char *str);
        StreamParser(ParserStreamContext& context, std::istream * input_stream);

        // scan functions that need to take care of new lines
        char skipWhiteSpace();
        char skipWhiteSpace(std::string& text_scanned);
        char curChar();
        char curChar(std::string& text_scanned);
        char nextChar(bool skip_white_space);
        char nextChar(std::string& text_scanned);
        char skipToChar(char target);

        bool hasError() const;
        const ErrorInfoVec& getErrors() const;
        void setErrorReporter(ParserErrorReporter* reporter) { error_reporter_= reporter; }

    protected:
        friend class ParserStreamContext;
        ParserStreamContext &       context_;
        ScanBuffer                  scan_buffer_;
        ParserErrorReporter*        error_reporter_{nullptr};
};

/**
 * \class ParserStreamContext
 * \ingroup StringUtils
 * \brief A stream parser context stack to manage parsers and streams being parsed.
 * Stream and parser can be pushed into context separately, for instance, pushing parser
 * only to use a different parser for the sane stream, and pushing stream only to use
 * the same parser for a different stream.
 */
// Example of usage:
//    ParserStreamContext context;
//    AParserDerivedFromStreanParser parser(context);
//    context.pushParser (&parser, a_file_path);   // push parser and stream the same time
class  ParserStreamContext
{
    public:
        ParserStreamContext(size_t line_buffer_sz=1024*1024*3);
        ~ParserStreamContext();

        bool nextLine();

        bool pushStream (std::istream *);
        bool pushStream (const char *);
        bool pushFileStream (const char* file_path);
        bool pushStream (const std::filesystem::path& path) { return pushFileStream(path.string().c_str()); }
        bool pushStream (const std::filesystem::path& path, const std::filesystem::path& original_path);

        bool popStream();

        // Note: ParserStreamContext does not own parser 
        bool pushParser (StreamParser * parser);
        bool pushParser(StreamParser * parser, std::istream *);
        bool pushParser(StreamParser * parser, const char *);
        bool pushFileParser(StreamParser * parser, const char * file_path);
        bool pushParser(StreamParser * parser, const std::filesystem::path& path);
        bool popParser ();

        StreamParser* parser() { return current_parser_; };
        ParserStream* stream() { return current_stream_; };

        bool atStreamEnd() { return !current_stream_ || current_stream_->atEnd(); }

        size_t line() const { return current_stream_ ? current_stream_->line_ : 0; }
        size_t pos() const { return current_parser_ ? current_parser_->pos() : 0; }
        const std::string& filePath() const { return current_stream_->file_path_; }
        const std::string& origFilePath() const { return current_stream_->original_file_path_; }
        size_t scannedStartPos() { return current_parser_ ? current_parser_->scannedStartPos() : 0; }
        size_t scannedEndPos() { return current_parser_ ? current_parser_->scannedEndPos() : 0; }

        void registerError(ParseErrID err_id, const char* extra_into=nullptr);
        void registerError(const char* err_into);
        
    protected:
        friend class StreamParser;

        // Note: ParserStreamContext creates ParserStream internally and owns it 
        bool pushStream (ParserStream * stream);
        
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
