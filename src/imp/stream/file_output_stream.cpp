




#include "file_output_stream.hpp"


FileOutputStream::FileOutputStream() :
        output(new std::ofstream() ) {

}

IOutputStream& FileOutputStream::put(char c) {
    output->put(c);
    return *this;
}

IOutputStream& FileOutputStream::write(unsigned n, const char* s) {
    output->write(s, n);
    return *this;
}

IOutputStream& FileOutputStream::flush() {
    output->flush();
    return *this;
}

bool FileOutputStream::good() const {
    return output->good();
}

bool FileOutputStream::eof() const {
    return output->eof();
}

bool FileOutputStream::bad() const {
    return output->bad();
}

bool FileOutputStream::fail() const {
    return output->fail();
}

bool FileOutputStream::operator! () const {
    return !*output;
}

FileOutputStream::operator bool() const {
    return bool(*output);
}

void FileOutputStream::clear(std::iostream::iostate state) {
    output->clear();
}

void FileOutputStream::open(const std::string& filename,
            std::fstream::ios_base::openmode mode) {
    output->open(filename, mode);
}

bool FileOutputStream::is_open() const {
    return output->is_open();
}

void FileOutputStream::close() {
    output->close();
}

using namespace fruit;
Component<IFileOutputStream> getIFileOutputStream () {
    return createComponent()
        .bind<IFileOutputStream, FileOutputStream>();
}

Component<IFileOutputStreamFactory> getIFileOutputStreamFactory() {
    return createComponent()
        .registerFactory<std::unique_ptr<IFileOutputStream>() >(
            []() {
                return std::unique_ptr<IFileOutputStream>(new FileOutputStream() );
            }
        );
}
