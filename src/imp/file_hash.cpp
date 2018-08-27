#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "file_hash.hpp"
#include "block_buffer.hpp"

// USE UNIQUE POINTERS. REMOVE ALLOCATION BUGS
// FROM YOUR PROGRAM.

FileHasher::FileHasher() {
    struct stat s;
    stat("/", &s);
    this->blocksize = (int)(s.st_blksize);
    
    this->input_buffer = new BlockBuffer(this->blocksize);
    this->_numOutputBuffers = this->numOutputBuffers();
    this->output_buffers = new BlockBuffer[this->_numOutputBuffers];
    
    for(int i = 0; i < this->_numOutputBuffers; i++) {
        this->output_buffers[i] = new BlockBuffer(this->blocksize);
    }
    
    

}

int FileHasher::numOutputBuffers() {
    return 15;
}


int FileHasher::getBlockSize() {
    return this->blocksize;
}

FileHasher::~FileHasher() {
    delete this->input_buffer;
    
    for(int i = 0; i < this->_numOutputBuffers; i++) {
        delete this->output_buffers[i];
    }
    
    delete [] this->output_buffers;
    
}
