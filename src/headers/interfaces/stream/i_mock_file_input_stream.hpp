



#ifndef I_MOCK_FILE_INPUT_STREAM_HPP
#define I_MOCK_FILE_INPUT_STREAM_HPP

#include "i_file_input_stream.hpp"
#include "i_filesystem.hpp"
#include <sstream>
#include <string>

class IMockFileInputStream : public IFileInputStream {
public:
    virtual void setContent(const std::string &s) = 0;
    virtual std::string getContent() = 0;
    virtual IFileSystem* getFileSystem() = 0;
    virtual void setFileSystem(IFileSystem *fs) = 0;

    virtual ~IMockFileInputStream() { }
};


#endif // I_MOCK_FILE_INPUT_STREAM_HPP