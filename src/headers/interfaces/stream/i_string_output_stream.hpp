



#ifndef I_STRING_OUTPUT_STREAM_HPP
#define I_STRING_OUTPUT_STREAM_HPP

#include "i_output_stream.hpp"


class IStringOutputStream : public IOutputStream {

public:
    
    virtual std::string getContent() = 0;
    virtual void setContent(const std::string& s) = 0;
    
    virtual ~IStringOutputStream() { };
    
};


#endif //I_STRING_OUTPUT_STREAM_HPP