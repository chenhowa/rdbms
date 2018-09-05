


#include "sorter.hpp"
#include "block_buffer.hpp"
#include "file_input_stream.hpp"
#include "file_output_stream.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <string>
#include <cstring>
#include <cstdio>

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

Sorter::Sorter(IBlockBuffer* stage_buffer, 
                            std::vector<IBlockBuffer*> &buffers,
                            std::vector<IFileInputStream*> &readers, 
                            std::vector<IFileOutputStream*> &writers ) {
    assert(buffers.size() >= 2);
    assert(buffers.size() == readers.size());
    assert(buffers.size() == writers.size());
    
    this->blocksize = 0;
    this->_numWorkers = buffers.size();
    this->stage.reset(stage_buffer);
    
    for(unsigned i = 0; i < buffers.size(); i++) {
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
    // While more input remains.
    unsigned nextWorker = 0;
    while(in.good()) {
        // read in next batch of bytes.
        unsigned bytes_read = this->stage->read(in);
        if(bytes_read == 0) {
            // If didn't read any bytes, we are
            // done; EOF has been reached.
            break;
        }
        
        // Send bytes to next worker, for potentially
        // parallel writing to temporary file.
        this->stage->write( *(this->workers[nextWorker]) );
        
        // In that next worker, somehow sort all the
        // bytes in place according to some STRATEGY.
        unsigned size = this->workers[nextWorker]->getBlockSize() + 1;
        char* temp = new char[size];
        memset(temp, '\0', size);
        this->workers[nextWorker]->write(size - 1, temp);
        assert(this->workers[nextWorker]->isEmpty());
        std::string sort_stage(temp);
        std::sort(sort_stage.begin(), sort_stage.end());
        std::strcpy(temp, sort_stage.c_str() );
        this->workers[nextWorker]->read(sort_stage.length(), temp);
        
        // this->workers[nextWorker]->write(out);
        // WRONG : REFACTOR AND TEST.
        // REFACTOR COMMON IMPLEMENTATION OUT AS WELL.
        delete [] temp;
        
        // TO DO : Once sorted, write the bytes to a temporary file,
        // and save the temporary file's HANDLE or 
        // LOCATION.
        std::string path = unique_path();
        
        nextWorker += 1;
    }
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
