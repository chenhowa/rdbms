



#ifndef I_STRING_INPUT_STREAM_HPP
#define I_STRING_INPUT_STREAM_HPP


#include "i_input_stream.hpp"

class IStringInputStream : public IInputStream {

public:
    virtual char get() = 0;
    virtual char peek() = 0;
    virtual IInputStream& ignore( unsigned n, char delim) = 0;
    virtual IInputStream& read(unsigned in, char* s) = 0;
    virtual bool good() const = 0;
    virtual bool eof() const = 0;
    virtual bool bad() const = 0;
    virtual bool operator! () const = 0;
    virtual explicit operator bool() const = 0;
    virtual void clear(std::iostream::iostate state) = 0;
    virtual void setContent(const std::string &s) = 0;
    virtual std::string getContent() = 0;
    virtual ~IStringInputStream() { };

};


#endif