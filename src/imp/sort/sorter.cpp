


#include "sorter.hpp"
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

void Sorter::sort(IInputStream& in, IOutputStream& out) {
    
}