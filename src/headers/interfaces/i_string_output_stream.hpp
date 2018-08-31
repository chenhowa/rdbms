



#ifndef I_STRING_OUTPUT_STREAM_HPP
#define I_STRING_OUTPUT_STREAM_HPP

#include "i_output_stream.hpp"


class IStringOutputStream : public IOutputStream {

public:
    virtual bool good() const = 0;
    virtual bool eof() const = 0;
    virtual bool bad() const = 0;
    virtual bool operator! () const = 0;
    virtual explicit operator bool() const = 0;
    virtual void clear(std::iostream::iostate state) = 0;

    virtual IOutputStream& put(char c) = 0;
    virtual IOutputStream& write(unsigned n, const char* s) = 0;
    virtual IOutputStream& flush() = 0;
    
    virtual std::string getContent() = 0;
    virtual void setContent(const std::string& s) = 0;
    
    virtual ~IStringOutputStream() { };
    
};


#endif //I_STRING_OUTPUT_STREAM_HPP