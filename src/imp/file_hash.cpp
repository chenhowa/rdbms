#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <vector>


#include "file_hash.hpp"
#include "i_block_buffer.hpp"

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
    
    this->input.reset(fac->make(this->blocksize) );
    this->_numOutputBuffers = num_outputs;
    this->outputs.reset(new unique_ptr<IBlockBuffer>[this->_numOutputBuffers]);
    
    for(int i = 0; i < this->_numOutputBuffers; i++) {
        this->outputs[i].reset(fac->make(this->blocksize) );
    }
}

FileHasher::FileHasher(int num_outputs, int blocksize) {
    this->blocksize = blocksize;
    this->input.reset(fac->make(this->blocksize) );
    this->_numOutputBuffers = num_outputs;
    this->outputs.reset( new unique_ptr<IBlockBuffer>[this->_numOutputBuffers] );
    
    for(int i = 0; i < this->_numOutputBuffers; i++) {
        this->outputs[i].reset( fac->make(this->blocksize) );
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

void FileHasher::testHash(std::string file, std::vector<std::string> dests) {
    // open the input file
    std::ifstream in(file, std::ifstream::binary);
    if(!in) {
        throw "Failure to open input file";
    }
    
    // open the output files. Truncate for testing purposees.
    std::vector<std::ofstream*> hash_outputs;
    for(unsigned i = 0; i < dests.size(); i++) {
        hash_outputs.push_back(new std::ofstream(dests[i],
                std::ofstream::trunc | std::ofstream::binary | std::ofstream::in));
    }
    
    // While there is more data to read
    while(in) {
        // If the input block is empty, read in a block.
        if(this->input->isEmpty()) {
            this->input->read(in);
        }
        
        // while the input buffer has more data to process
        char data;
        int hash_val;
        while(! this->input->isEmpty()) {
            // read the next piece of data.
            this->input->write(1, &data);
            
            // TODO : Ensure that bug in IBlockBuffer is fixed:
            // EOF SHOULDn"T BE READ INTO THE BUFFER
            
            // hash the next piece of data.
            hash_val = ((int)data) % this->numOutputBuffers();
            
            // Send the data to the correct output buffer.
            this->outputs[hash_val]->read(1, &data);
            
            // Check if the output buffer is full. If it is,
            // then flush it, before continuing.
            if(this->outputs[hash_val]->isFull()) {
                this->outputs[hash_val]->write(*(hash_outputs[hash_val]));
            }
        }
    }
}