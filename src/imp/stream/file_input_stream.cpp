



#include "file_input_stream.hpp"

FileInputStream::FileInputStream():
        input() {    
}

void FileInputStream::open(const std::string& filename, std::fstream::ios_base::openmode mode) {
    this->input.open(filename, mode);
    if(is_open() && good()) {
        name = filename;
    }
}

bool FileInputStream::is_open() const {
    return this->input.is_open();
}

void FileInputStream::close() {
    return this->input.close();
}

char FileInputStream::get() {
    return (char)this->input.get();
}

char FileInputStream::peek() {
    return (char)this->input.peek();
}

IInputStream& FileInputStream::ignore( unsigned n, char delim) {
    this->input.ignore(n, delim);
    return *this;
}

IInputStream& FileInputStream::read(unsigned in, char* s) {
    this->input.read(s, in);
    return *this;
}

bool FileInputStream::good() const {
    return this->input.good();
}

bool FileInputStream::eof() const {
    return this->input.eof();
}

bool FileInputStream::bad() const {
    return this->input.bad();
}

bool FileInputStream::fail() const {
    return this->input.fail();
}

bool FileInputStream::operator !() const {
    return !(this->input);
}

FileInputStream::operator bool() const {
    return bool(this->input);
}

void FileInputStream::clear(std::iostream::iostate state) {
    this->input.clear(state);
}

std::string FileInputStream::get_file_name() {
    if(is_open()) {
        return name;
    }
    
    return std::string();
}

using namespace fruit;
Component<IFileInputStream> getIFileInputStream() {
    return createComponent()
        .bind<IFileInputStream, FileInputStream>();
}
/*
Component<IInputStream> getInputStream_File() {
    return createComponent()
        .bind<IInputStream, FileInputStream>();
}
*/

Component<IFileInputStreamFactory> getIFileInputStreamFactory() {
    return createComponent()
        .registerFactory<std::unique_ptr<IFileInputStream>() >(
            []() {
                return std::unique_ptr<IFileInputStream>( new FileInputStream() );
            }
        );
}
