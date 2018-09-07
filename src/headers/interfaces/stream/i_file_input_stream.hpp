



#ifndef I_FILE_INPUT_STREAM_HPP
#define I_FILE_INPUT_STREAM_HPP

#include "i_input_stream.hpp"
#include <fstream>
#include <string>


class IFileInputStream : public IInputStream {
public:
    virtual void open(const std::string& filename,
                std::fstream::ios_base::openmode mode) = 0;
    virtual bool is_open() const = 0;
    virtual void close() = 0;
    virtual std::string get_file_name() = 0;
    virtual ~IFileInputStream() { }
};


#endif // I_FILE_INPUT_STREAM_HPP