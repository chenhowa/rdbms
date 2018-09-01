#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <assert.h>


#include "file_hash.hpp"
#include "i_block_buffer.hpp"
#include "block_buffer.hpp"

int calcBlockSize() {
    struct stat s;
    stat("/", &s);
    printf("Blocksize: %i\n", (int)s.st_blksize);
    return (int)(s.st_blksize);
}

FileHasher::FileHasher() {
    
}

FileHasher::FileHasher(unsigned num_outputs, IBlockBufferFactory fac) {
    this->blocksize = calcBlockSize();
    
    this->input = fac(this->blocksize);
    this->_numOutputBuffers = num_outputs;
    this->outputs.reset(new unique_ptr<IBlockBuffer>[this->_numOutputBuffers]);
    
    for(unsigned i = 0; i < this->_numOutputBuffers; i++) {
        this->outputs[i] = fac(this->blocksize);
    }
}

FileHasher::FileHasher(unsigned num_outputs, unsigned blocksize, IBlockBufferFactory fac) {
    this->blocksize = blocksize;
    this->input = fac(this->blocksize);
    this->_numOutputBuffers = num_outputs;
    this->outputs.reset( new unique_ptr<IBlockBuffer>[this->_numOutputBuffers] );
    
    for(unsigned i = 0; i < this->_numOutputBuffers; i++) {
        this->outputs[i] = fac(this->blocksize);
    }
}

// The IBlockBuffers should all be the same size, and of the 
// same concrete type. Otherwise the behavior of the constructed
// FileHasher is not defined.
FileHasher::FileHasher(IBlockBuffer* in, IBlockBuffer* out_1, IBlockBuffer* out_2) {
    this->blocksize = 0;
    this->input.reset(in);
    this->_numOutputBuffers = 2;
    this->outputs.reset(new unique_ptr<IBlockBuffer>[2]);
    this->outputs[0].reset(out_1);
    this->outputs[1].reset(out_2);
}

unsigned FileHasher::numOutputBuffers() {
    return this->_numOutputBuffers;
}


unsigned FileHasher::getBlockSize() {
    return this->blocksize;
}

FileHasher::~FileHasher() {
   
}

/*
 * This function is a test implementation of the hashing algorithm. 
 * @ file - a handle to the input stream to read data from.
 * @ dests - a vector of handles to output streams to write hashed data to.
 * @@ pre - all stream handles are in good condition.
 * @@ post - data in input file will be hashed and sent to destinations.
 * 
 */
void FileHasher::testHash(IInputStream &in, std::vector<IOutputStream*> &dests) {
    assert(in.good());
    assert(dests.size() <= this->numOutputBuffers());
    assert(dests.size() > 0);
    for(unsigned i = 0; i < dests.size(); i++) {
        assert(dests[i]->good());
    }
    
    //printf("preparing to hash\n");
    
    // While there is more data to read
    while(in.good()) {
        //printf("data to read\n");
        // If the input block is empty, read in a block.
        if(this->input->isEmpty()) {
            //printf("reading from stream\n");
            this->input->read(in);
        }
        
        // while the input buffer has more data to process
        char data;
        int hash_val;
        while(! this->input->isEmpty()) {
            // read the next piece of data.
            this->input->write(1, &data);
            
            //printf("data read: %i\n", data);
            
            // hash the next piece of data.
            hash_val = ((int)data) % this->numOutputBuffers();
            
            //printf("hash val: %i\n", hash_val);
            
            // Send the data to the correct output buffer.
            this->outputs[hash_val]->read(1, &data);
            
            // Check if the output buffer is full. If it is,
            // then flush it, before continuing.
            if(this->outputs[hash_val]->isFull()) {
                //printf("full buffer: writing\n");
                this->outputs[hash_val]->write(*(dests[hash_val]));
            }
        }
    }
    
    //Once the data is exhausted, flush the buffers.
    for(unsigned i = 0; i < dests.size(); i++) {
        this->outputs[i]->write(*dests[i]);
    }
}

fruit::Component<IFileHasherFactory> getIFileHasherFactory() {
    return fruit::createComponent()
        .install(getIBlockBufferFactory)
        .registerFactory<std::unique_ptr<IFileHasher>(
                fruit::Assisted<unsigned>, 
                fruit::Assisted<unsigned>, 
                IBlockBufferFactory)> (
            [](unsigned num_outputs, unsigned blocksize, IBlockBufferFactory fac) {
                return std::unique_ptr<IFileHasher>(
                    new FileHasher(num_outputs, blocksize, fac)
                );
            });
}

