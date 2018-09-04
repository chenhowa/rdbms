


#ifndef MOCK_FILE_INPUT_STREAM_HPP
#define MOCK_FILE_INPUT_STREAM_HPP

#include "i_mock_file_input_stream.hpp"
#include <fruit/fruit.h>
#include "i_filesystem.hpp"

class MockFileInputStream : public IMockFileInputStream {
private:
    MockFileInputStream() = default;
    std::string *current_file;
    IFileSystem *filesystem;
    std::string::iterator current_iterator;
    bool is_open_flag;
    bool good_bit;
    bool eof_bit;
    bool fail_bit;
    bool bad_bit;
    std::ios_base::openmode mode;
public:
    MockFileInputStream(IFileSystem* fs);
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
    virtual bool fail() const override;
    virtual bool operator! () const override;
    virtual explicit operator bool() const override;
    virtual void clear(std::iostream::iostate state) override;
    
    virtual ~MockFileInputStream() { }

private:
    bool isTrunc(std::ios_base::openmode mode);
    bool isBinary(std::ios_base::openmode mode);
    bool isIn(std::ios_base::openmode mode);
    bool isOut(std::ios_base::openmode mode);
    bool isAte(std::ios_base::openmode mode);
    bool isApp(std::ios_base::openmode mode);

};

using IMockFileInputStreamFactory = std::function<std::unique_ptr<IMockFileInputStream>()>;
fruit::Component<IMockFileInputStreamFactory> getIMockFileInputStreamFactory();



#endif // MOCK_FILE_INPUT_STREAM_HPP