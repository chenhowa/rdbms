


#include "sorter.hpp"
#include "block_buffer.hpp"
#include "file_input_stream.hpp"
#include "file_output_stream.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

unsigned calcBlockSize() {
    struct stat s;
    stat("/", &s);
    printf("Blocksize: %i\n", (int)s.st_blksize);
    return (unsigned)(s.st_blksize);
}


Sorter::Sorter(unsigned num_bufs, IBlockBufferFactory fac,
                IFileInputStreamFactory in, IFileOutputStreamFactory out) {
    this->blocksize = calcBlockSize();
    this->_numWorkers = num_bufs;
    this->stage = fac(this->blocksize);
    for(unsigned i = 0; i < this->_numWorkers; i++) {
        workers.push_back(fac(this->blocksize));
        in_streams.push_back(in());
        out_streams.push_back(out());
    }
    
    this->in_fac = in;
    this->out_fac = out;
    
    assert(workers.size() == this->_numWorkers);
    assert(in_streams.size() == workers.size());
    assert(out_streams.size() == workers.size());
}

Sorter::Sorter(unsigned num_bufs, unsigned blocksize, IBlockBufferFactory fac,
                    IFileInputStreamFactory in, IFileOutputStreamFactory out) {
    this->blocksize = blocksize;
    this->_numWorkers = num_bufs;
    this->stage = fac(this->blocksize);
    for(unsigned i = 0; i < this->_numWorkers; i++) {
        workers.push_back(fac(this->blocksize));
        in_streams.push_back(in());
        out_streams.push_back(out());
    }
    
    this->in_fac = in;
    this->out_fac = out;
    
    assert(workers.size() == this->_numWorkers);
    assert(in_streams.size() == workers.size());
    assert(out_streams.size() == workers.size());
}

Sorter::Sorter(std::vector<IBlockBuffer*> &buffers,
        std::vector<IFileInputStream*> &readers, std::vector<IFileOutputStream*> &writers ) {
    assert(buffers.size() >= 3);
    assert(buffers.size() - 1 == readers.size());
    assert(buffers.size() - 1 == writers.size());
    
    this->blocksize = 0;
    this->_numWorkers = buffers.size() - 1;
    this->stage.reset(buffers[buffers.size()]);
    
    for(unsigned i = 0; i < readers.size(); i++) {
        workers.push_back(std::unique_ptr<IBlockBuffer>(buffers[i]) );
        in_streams.push_back(std::unique_ptr<IFileInputStream>(readers[i]) );
        out_streams.push_back(std::unique_ptr<IFileOutputStream>(writers[i]) );
    }
}

unsigned Sorter::getNumWorkers() {
    assert(workers.size() == this->_numWorkers);
    return this->_numWorkers;
}

unsigned Sorter::getBlockSize() {
    return this->blocksize;
}

void Sorter::sort(IInputStream& in, IOutputStream& out) {
    // THIS WILL BE TOUGH. YOU'LL HAVE TO
    // LOCK SOME RESOURCES TO DO THE SORT IN PARALLEL
}


using namespace fruit;
Component<ISorterFactory> getISorterFactory() {
    return createComponent()
        .install(getIBlockBufferFactory)
        .install(getIFileInputStreamFactory)
        .install(getIFileOutputStreamFactory)
        .registerFactory<std::unique_ptr<ISorter>(
                fruit::Assisted<unsigned>, 
                fruit::Assisted<unsigned>, 
                IBlockBufferFactory,
                IFileInputStreamFactory,
                IFileOutputStreamFactory)> (
            [](unsigned num_workers, unsigned blocksize, IBlockBufferFactory fac,
                        IFileInputStreamFactory in, IFileOutputStreamFactory out) {
                return std::unique_ptr<ISorter>(
                    new Sorter(num_workers, blocksize, fac, in, out)
                );
            });
}
