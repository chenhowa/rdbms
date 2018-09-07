
#ifndef MOCK_FILE_OUTPUT_STREAM_HPP
#define MOCK_FILE_OUTPUT_STREAM_HPP

#include "i_mock_file_output_stream.hpp"
#include <fruit/fruit.h>
#include "i_filesystem.hpp"

class MockFileOutputStream : public IMockFileOutputStream {
private:
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
    MockFileOutputStream();
    MockFileOutputStream(IFileSystem *fs);
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
    virtual bool fail() const override;

    virtual bool operator! () const override;
    virtual explicit operator bool() const override;
    // This function will only be used to clear state to GOOD.
    virtual void clear(std::iostream::iostate state = std::iostream::goodbit) override;
    
    
    virtual ~MockFileOutputStream() { };

public:
    virtual IFileSystem* getFileSystem() override;
    virtual void setFileSystem(IFileSystem *fs) override;
    
private:
    bool isTrunc(std::ios_base::openmode mode);
    bool isBinary(std::ios_base::openmode mode);
    bool isIn(std::ios_base::openmode mode);
    bool isOut(std::ios_base::openmode mode);
    bool isAte(std::ios_base::openmode mode);
    bool isApp(std::ios_base::openmode mode);



};

using IMockFileOutputStreamFactory = std::function<std::unique_ptr<IMockFileOutputStream>()>;
fruit::Component<IMockFileOutputStreamFactory> getIMockFileOutputStreamFactory();

fruit::Component<fruit::Required<IFileSystem>, 
        IMockFileOutputStreamFactory>  getIMockFileOutputStreamFactory_req_fs();

// using IFileOutputStreamFactory = std::function<std::unique_ptr<IFileOutputStream>()>;
// fruit::Component<IFileOutputStreamFactory> getIFileOutputStream();

#endif // MOCK_FILE_OUTPUT_STREAM_HPP