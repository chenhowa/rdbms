
#ifndef I_INPUT_STREAM_HPP
#define I_INPUT_STREAM_HPP

#include <iostream>
#include "i_stream.hpp"

class IInputStream : public IStream {
public:
    virtual char get() = 0;
    virtual char peek() = 0;
    virtual IInputStream& ignore( unsigned n, char delim) = 0;
    virtual IInputStream& read(unsigned in, char* s) = 0;
    virtual ~IInputStream() { };
};


#endif // I_INPUT_STREAM_HPP
