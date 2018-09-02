
#ifndef MOCK_FILE_OUTPUT_STREAM_HPP
#define MOCK_FILE_OUTPUT_STREAM_HPP

#include "i_mock_file_output_stream.hpp"
#include <sstream>
#include <fruit/fruit.h>

class MockFileOutputStream : public IMockFileOutputStream {
private:
    std::ostringstream buffer;
    bool is_open_flag;
public:
    INJECT( MockFileOutputStream() );
    virtual std::string getContent() override;
    virtual void setContent(std::string &s) override;
    virtual void open(const std::string& filename,
                std::fstream::ios_base::openmode mode) override;
    virtual bool is_open() const override;
    virtual void close() override;
    virtual IOutputStream& put(char c) override;
    virtual IOutputStream& write(unsigned n, const char* s) override;
    virtual IOutputStream& flush() override;
    virtual bool good() const override;
    virtual bool eof() const override;
    virtual bool bad() const override;
    virtual bool operator! () const override;
    virtual explicit operator bool() const override;
    virtual void clear(std::iostream::iostate state) override;
    
    
    virtual ~MockFileOutputStream() { };

};

fruit::Component<IMockFileOutputStream> getIMockFileOutputStream();

using IMockFileOutputStreamFactory = std::function<std::unique_ptr<IMockFileOutputStream>()>;
fruit::Component<IMockFileOutputStreamFactory> getIMockFileOutputStreamFactory();

#endif // MOCK_FILE_OUTPUT_STREAM_HPP