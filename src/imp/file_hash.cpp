#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>


#include "file_hash.hpp"
#include "block_buffer.hpp"

// USE UNIQUE POINTERS. REMOVE ALLOCATION BUGS
// FROM YOUR PROGRAM.

int calcBlockSize() {
    struct stat s;
    stat("/", &s);
    printf("Blocksize: %i\n", (int)s.st_blksize);
    return (int)(s.st_blksize);
}

FileHasher::FileHasher() {
    
}

FileHasher::FileHasher(int num_outputs) {
    this->blocksize = calcBlockSize();
    
    this->input.reset(new BlockBuffer(this->blocksize) );
    this->_numOutputBuffers = num_outputs;
    this->outputs.reset(new unique_ptr<BlockBuffer>[this->_numOutputBuffers]);
    
    for(int i = 0; i < this->_numOutputBuffers; i++) {
        this->outputs[i].reset(new BlockBuffer(this->blocksize) );
    }
}

FileHasher::FileHasher(int num_outputs, int blocksize) {
    this->blocksize = blocksize;
    this->input.reset(new BlockBuffer(this->blocksize) );
    this->_numOutputBuffers = num_outputs;
    this->outputs.reset( new unique_ptr<BlockBuffer>[this->_numOutputBuffers] );
    
    for(int i = 0; i < this->_numOutputBuffers; i++) {
        this->outputs[i].reset( new BlockBuffer(this->blocksize) );
    }
    
}

int FileHasher::numOutputBuffers() {
    return this->_numOutputBuffers;
}


int FileHasher::getBlockSize() {
    return this->blocksize;
}

FileHasher::~FileHasher() {
   
}

void testHash(char* file) {
    // open the file
    std::ifstream in(file, std::ifstream::binary);
    if(!in) {
        throw "Failure to open input file";
    }
    
    
    
    // Read a block into input
    this->input.read(in);
    
    // For every value in the file:
    char c;
    int hash_val;
    while(in) {
        // Read in a value to input
        c = (char)in.get();

        // Hash the value.
        hash_val = ((int)c) %
    
        // Based on the hash value, send it to the appropriate
        // output buffer.
        
        // Check if that output buffer is full. If it is
        // flush it, and then continue.
    }
}