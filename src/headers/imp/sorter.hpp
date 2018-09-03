


#ifndef FILE_SORT_HPP
#define FILE_SORT_HPP

#include "i_sorter.hpp"
#include "i_block_buffer.hpp"
#include "i_file_input_stream.hpp"
#include "i_file_output_stream.hpp"
#include <fruit/fruit.h>
#include <vector>


using IBlockBufferFactory = std::function<std::unique_ptr<IBlockBuffer>(unsigned)>;
using IFileInputStreamFactory = std::function<std::unique_ptr<IFileInputStream>()>;
using IFileOutputStreamFactory = std::function<std::unique_ptr<IFileOutputStream>()>;


class Sorter : public ISorter {
private:
    Sorter();
    unsigned blocksize;
    unsigned _numWorkers;
    std::unique_ptr<IBlockBuffer> stage;
    std::vector<std::unique_ptr<IBlockBuffer> > workers;
    IFileInputStreamFactory in_fac;
    IFileOutputStreamFactory out_fac;
    std::vector<std::unique_ptr<IFileInputStream> > in_streams;
    std::vector<std::unique_ptr<IFileOutputStream> > out_streams;
public:
    Sorter(unsigned num_bufs, IBlockBufferFactory fac, 
                IFileInputStreamFactory in_fac, IFileOutputStreamFactory out_fac);
    Sorter(unsigned num_bufs, unsigned blocksize, IBlockBufferFactory fac,
                IFileInputStreamFactory in_fac, IFileOutputStreamFactory out_fac);
    Sorter(IBlockBuffer* stage_buffer,
                    std::vector<IBlockBuffer*> &buffers, 
                    std::vector<IFileInputStream*> &readers, 
                    std::vector<IFileOutputStream*> &writers );
    virtual unsigned getNumWorkers();
    virtual unsigned getBlockSize();
    virtual void sort(IInputStream& in, IOutputStream& out) override;
    virtual ~Sorter() { };
};


using ISorterFactory = std::function<std::unique_ptr<ISorter>(unsigned, unsigned) >;
fruit::Component<ISorterFactory> getISorterFactory();


#endif //FILE_SORT_HPP
