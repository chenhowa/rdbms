




#ifndef STRING_OUTPUT_STREAM_HPP
#define STRING_OUTPUT_STREAM_HPP

#include "i_string_output_stream.hpp"
#include <sstream>
#include <memory>
#include <string>
#include <fruit/fruit.h>

class StringOutputStream : public IStringOutputStream {
private:
    std::unique_ptr<std::ostringstream> output;

public:
    INJECT( StringOutputStream() );
    virtual bool good() const override;
    virtual bool eof() const override;
    virtual bool bad() const override;
    virtual bool operator! () const override;
    virtual explicit operator bool() const override;
    virtual void clear(std::iostream::iostate state) override;

    virtual IOutputStream& put(char c) override;
    virtual IOutputStream& write(unsigned n, const char* s) override;
    virtual IOutputStream& flush() override;
    
    virtual std::string getContent();
    virtual void setContent(const std::string& s);
    
    virtual ~StringOutputStream() { };
    
};

fruit::Component<IStringOutputStream> getIStringOutputStream();



#endif