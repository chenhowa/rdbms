



#ifndef STRING_INPUT_STREAM_HPP
#define STRING_INPUT_STREAM_HPP

#include "i_string_input_stream.hpp"
#include <sstream>
#include <string>
#include <memory>

#include <fruit/fruit.h>

class StringInputStream : public IStringInputStream {
private:
    std::unique_ptr<std::istringstream> input;

public:
    INJECT( StringInputStream() );
    virtual char get() override;
    virtual char peek() override;
    virtual IInputStream& ignore( unsigned n, char delim) override;
    virtual IInputStream& read(unsigned in, char* s) override;
    virtual bool good() const override;
    virtual bool eof() const override;
    virtual bool bad() const override;
    virtual bool fail() const override;
    virtual bool operator! () const override;
    virtual explicit operator bool() const override;
    virtual void clear(std::iostream::iostate state) override;
    virtual ~StringInputStream() { };
    virtual void setContent(const std::string &s);
    virtual std::string getContent();
};


fruit::Component<IStringInputStream> getIStringInputStream();


#endif // STRING_INPUT_STREAM_HPP