



#ifndef I_STREAM_HPP
#define I_STREAM_HPP

#include <iostream>

class IStream {
public:
    virtual bool good() const = 0;
    virtual bool eof() const = 0;
    virtual bool fail() const = 0;
    virtual bool bad() const = 0;
    virtual bool operator! () const = 0;
    virtual explicit operator bool() const = 0;
    virtual void clear(std::iostream::iostate state) = 0;

    virtual ~IStream() {}
};


#endif //I_STREAM_HPP