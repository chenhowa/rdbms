


#ifndef FILE_INPUT_STREAM_HPP
#define FILE_INPUT_STREAM_HPP

#include "i_file_input_stream.hpp"
#include <fstream>

#include <fruit/fruit.h>


class FileInputStream : public IFileInputStream {
private:
    std::fstream input;
    std::string name;

public:
    INJECT(FileInputStream() );
    virtual void open(const std::string& filename,
                std::fstream::ios_base::openmode mode) override;
    virtual bool is_open() const override;
    virtual void close() override;
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
    virtual std::string get_file_name() override;
    
};

fruit::Component<IFileInputStream> getIFileInputStream();

//fruit::Component<IInputStream> getInputStream_File();

using IFileInputStreamFactory = std::function<std::unique_ptr<IFileInputStream>()>;
fruit::Component<IFileInputStreamFactory> getIFileInputStreamFactory();


#endif // FILE_INPUT_STREAM_HPP