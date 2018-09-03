



#include "string_output_stream.hpp"


StringOutputStream::StringOutputStream() : 
        output(new std::ostringstream(std::ios_base::ate)) {
    
}

bool StringOutputStream::good() const {
    return output->good();
}

bool StringOutputStream::eof() const {
    return output->eof();
}

bool StringOutputStream::bad() const {
    return output->bad();
}

bool StringOutputStream::fail() const {
    return output->fail();
}

bool StringOutputStream::operator! () const {
    return !(*output);
}

StringOutputStream::operator bool() const {
    return bool(*output);
}

void StringOutputStream::clear(std::iostream::iostate state) {
    output->clear(state);
}

IOutputStream& StringOutputStream::put(char c) {
    output->put(c);
    return *this;
    
}

IOutputStream& StringOutputStream::write(unsigned n, const char* s) {
    output->write(s, n);
    return *this;
}

IOutputStream& StringOutputStream::flush() {
    output->flush();
    return *this;
}

std::string StringOutputStream::getContent() {
    return output->str();
}

void StringOutputStream::setContent(const std::string& s) {
    output->str(s);
}


using namespace fruit;
Component<IStringOutputStream> getIStringOutputStream() {
    return createComponent()
        .bind<IStringOutputStream, StringOutputStream>();
}
