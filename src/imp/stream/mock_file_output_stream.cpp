





#include "mock_file_output_stream.hpp"

MockFileOutputStream::MockFileOutputStream() :
        buffer(std::ios_base::ate), is_open_flag(false)  {
    
}
std::string MockFileOutputStream::getContent() {
    return buffer.str();
}

void MockFileOutputStream::setContent(std::string &s) {
    buffer.str(s);
}

void MockFileOutputStream::open(const std::string& filename,
            std::fstream::ios_base::openmode mode) {
    is_open_flag = true;
}

bool MockFileOutputStream::is_open() const {
    return is_open_flag;
}

void MockFileOutputStream::close() {
    is_open_flag = false;
}

IOutputStream& MockFileOutputStream::put(char c) {
    buffer.put(c);
    return *this;
}

IOutputStream& MockFileOutputStream::write(unsigned n, const char* s) {
    buffer.write(s, n);
    return *this;
}

IOutputStream& MockFileOutputStream::flush() {
    buffer.flush();
    return *this;
}

bool MockFileOutputStream::good() const {
    return buffer.good();
}

bool MockFileOutputStream::eof() const {
    return buffer.eof();
}

bool MockFileOutputStream::bad() const {
    return buffer.bad();
}

bool MockFileOutputStream::operator! () const {
    return !buffer;
}

MockFileOutputStream::operator bool() const {
    bool(buffer);
}

void MockFileOutputStream::clear(std::iostream::iostate state) {
    buffer.clear(state);
}


using namespace fruit;
Component<IMockFileOutputStream> getIMockFileOutputStream() {
    return createComponent()
        .bind<IMockFileOutputStream, MockFileOutputStream>();
}

Component<IMockFileOutputStreamFactory> getIMockFileOutputStreamFactory() {
    return createComponent()
        .registerFactory<std::unique_ptr<IMockFileOutputStream>() >(
            []() {
                return std::unique_ptr<IMockFileOutputStream>(new MockFileOutputStream() );
            }
        ); 
}