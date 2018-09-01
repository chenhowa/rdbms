


#include "sorter.hpp"
#include "block_buffer.hpp"
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


Sorter::Sorter(unsigned num_bufs, IBlockBufferFactory fac) {
    this->blocksize = calcBlockSize();
    this->_numWorkers = num_bufs;
    this->stage = fac(this->blocksize);
    for(unsigned i = 0; i < this->_numWorkers; i++) {
        workers.push_back(fac(this->blocksize));
    }
    
    assert(workers.size() == this->_numWorkers);
}

Sorter::Sorter(unsigned num_bufs, unsigned blocksize, IBlockBufferFactory fac) {
    this->blocksize = blocksize;
    this->_numWorkers = num_bufs;
    this->stage = fac(this->blocksize);
    for(unsigned i = 0; i < this->_numWorkers; i++) {
        workers.push_back(fac(this->blocksize));
    }
    
    assert(workers.size() == this->_numWorkers);
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
        .registerFactory<std::unique_ptr<ISorter>(
                fruit::Assisted<unsigned>, 
                fruit::Assisted<unsigned>, 
                IBlockBufferFactory)> (
            [](unsigned num_workers, unsigned blocksize, IBlockBufferFactory fac) {
                return std::unique_ptr<ISorter>(
                    new Sorter(num_workers, blocksize, fac)
                );
            });
}
