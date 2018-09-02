


#ifndef MOCK_FILE_INPUT_STREAM_HPP
#define MOCK_FILE_INPUT_STREAM_HPP

#include "i_mock_file_input_stream.hpp"
#include <sstream>
#include <fruit/fruit.h>

class MockFileInputStream : public IMockFileInputStream {
private:
    std::istringstream buffer;
    bool is_open_flag;
public:
    INJECT( MockFileInputStream() );
    virtual void setContent(const std::string &s) override;
    virtual std::string getContent() override;
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
    virtual bool operator! () const override;
    virtual explicit operator bool() const override;
    virtual void clear(std::iostream::iostate state) override;
    
    virtual ~MockFileInputStream() { }
};

fruit::Component<IMockFileInputStream> getIMockFileInputStream();

using IMockFileInputStreamFactory = std::function<std::unique_ptr<IMockFileInputStream>()>;
fruit::Component<IMockFileInputStreamFactory> getIMockFileInputStreamFactory();



#endif // MOCK_FILE_INPUT_STREAM_HPP