



#include "mock_file_input_stream.hpp"


MockFileInputStream::MockFileInputStream() : 
        buffer(), is_open_flag(false) {
}


void MockFileInputStream::setContent(const std::string &s) { 
    buffer.str(s);
}

std::string MockFileInputStream::getContent() { 
    return buffer.str();
}

void MockFileInputStream::open(const std::string& filename,
            std::fstream::ios_base::openmode mode) {
    is_open_flag = true;
}

bool MockFileInputStream::is_open() const {
    return this->is_open_flag;
}

void MockFileInputStream::close() {
    is_open_flag = false;
}

char MockFileInputStream::get() {
    return buffer.get();
}

char MockFileInputStream::peek() {
    return buffer.peek();
}

IInputStream& MockFileInputStream::ignore( unsigned n, char delim) {
    buffer.ignore(n, delim);
    return *this;
}

IInputStream& MockFileInputStream::read(unsigned in, char* s) {
    buffer.read(s, in);
    return *this;
}

bool MockFileInputStream::good() const {
    return buffer.good();
}

bool MockFileInputStream::eof() const { 
    return buffer.eof();
}

bool MockFileInputStream::bad() const {
    return buffer.bad();
}

bool MockFileInputStream::operator! () const {
    return !(buffer);
}

MockFileInputStream::operator bool() const { 
    bool(buffer);
}

void MockFileInputStream::clear(std::iostream::iostate state) {
    return buffer.clear(state);
}

using namespace fruit;
Component<IMockFileInputStream> getIMockFileInputStream() {
    return createComponent()
        .bind<IMockFileInputStream, MockFileInputStream>();
}

Component<IMockFileInputStreamFactory> getIMockFileInputStreamFactory() {
    return createComponent()
        .registerFactory<std::unique_ptr<IMockFileInputStream>()>(
            []() {
                return std::unique_ptr<IMockFileInputStream>(new MockFileInputStream() );
            }
        );
}

