


#ifndef I_FILE_OUTPUT_STREAM_HPP
#define I_FILE_OUTPUT_STREAM_HPP

#include "i_output_stream.hpp"
#include <fstream>
#include <string>
#include <boost/log/trivial.hpp>

class IFileOutputStream : public IOutputStream {
public:
    virtual void open(const std::string& filename,
                std::fstream::ios_base::openmode mode) = 0;
    virtual bool is_open() const = 0;
    virtual void close() = 0;
    virtual std::string get_file_name() = 0;
    virtual ~IFileOutputStream() { }
};



#endif // I_FILE_OUTPUT_STREAM_HPP