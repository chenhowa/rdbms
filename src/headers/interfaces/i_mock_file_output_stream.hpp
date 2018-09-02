


#ifndef I_MOCK_FILE_OUTPUT_STREAM_HPP
#define I_MOCK_FILE_OUTPUT_STREAM_HPP

#include "i_file_output_stream.hpp"
#include <string>


class IMockFileOutputStream : public IFileOutputStream {
public:
    virtual std::string getContent() = 0;
    virtual void setContent(std::string &s) = 0;
    
    
    virtual ~IMockFileOutputStream() { };
};


#endif // I_MOCK_FILE_OUTPUT_STREAM_HPP