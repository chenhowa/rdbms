


#ifndef I_OUTPUT_STREAM_HPP
#define I_OUTPUT_STREAM_HPP

#include "i_stream.hpp"

class IOutputStream : public IStream {
public:
    virtual IOutputStream& put(char c) = 0;
    virtual IOutputStream& write(unsigned n, const char* s) = 0;
    virtual IOutputStream& flush() = 0;
    virtual ~IOutputStream() { };
};


#endif // I_OUTPUT_STREAM_HPP