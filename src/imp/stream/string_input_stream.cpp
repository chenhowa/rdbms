



#include "string_input_stream.hpp"



StringInputStream::StringInputStream()     {
    this->input.reset(new std::istringstream() );
}
char StringInputStream::get() {
    return (char)input->get();
}
char StringInputStream::peek() {
    return (char)input->peek();
}
IInputStream& StringInputStream::ignore( unsigned n, char delim) {
    input->ignore(n, delim);
    return *this;
}
IInputStream& StringInputStream::read(unsigned in, char* s) {
    input->read(s, in);
    return *this;
}
bool StringInputStream::good() const {
    return input->good();
}
bool StringInputStream::eof() const {
    return input->eof();
}
bool StringInputStream::bad() const {
    return input->bad();
}
bool StringInputStream::operator! () const {
    return !*input;
}
StringInputStream::operator bool() const {
    return bool(*input);
}
void StringInputStream::clear(std::iostream::iostate state) {
    input->clear();
}

void StringInputStream::setContent(const std::string &s) {
    input->str(s);
}

std::string StringInputStream::getContent() {
    return input->str();
}

using namespace fruit;
Component<IStringInputStream> getIStringInputStream() {
    return createComponent()
        .bind<IStringInputStream, StringInputStream>();
}
