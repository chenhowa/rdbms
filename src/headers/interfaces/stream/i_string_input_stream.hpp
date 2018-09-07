



#ifndef I_STRING_INPUT_STREAM_HPP
#define I_STRING_INPUT_STREAM_HPP


#include "i_input_stream.hpp"

class IStringInputStream : public IInputStream {

public:
    virtual void setContent(const std::string &s) = 0;
    virtual std::string getContent() = 0;
    virtual ~IStringInputStream() { };

};


#endif