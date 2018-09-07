

#ifndef FILE_OUTPUT_STREAM_HPP
#define FILE_OUTPUT_STREAM_HPP

#include "i_file_output_stream.hpp"
#include <fstream>
#include <memory>
#include <fruit/fruit.h>

class FileOutputStream : public IFileOutputStream {
private:
    std::unique_ptr<std::ofstream> output;
public:
    INJECT( FileOutputStream() );
    virtual IOutputStream& put(char c) override;
    virtual IOutputStream& write(unsigned n, const char* s) override;
    virtual IOutputStream& flush() override;
    
    virtual bool good() const override;
    virtual bool eof() const override;
    virtual bool bad() const override;
    virtual bool fail() const override;
    virtual bool operator! () const override;
    virtual explicit operator bool() const override;
    virtual void clear(std::iostream::iostate state) override;
    
    virtual void open(const std::string& filename,
                std::fstream::ios_base::openmode mode) override;
    virtual bool is_open() const override;
    virtual void close() override;
    
    virtual ~FileOutputStream() { }
};

fruit::Component<IFileOutputStream> getIFileOutputStream();

using IFileOutputStreamFactory = std::function<std::unique_ptr<IFileOutputStream>()>;
fruit::Component<IFileOutputStreamFactory> getIFileOutputStreamFactory();

#endif // FILE_OUTPUT_STREAM_HPP
