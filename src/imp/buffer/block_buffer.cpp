


#include <memory>
#include <string>
#include <assert.h>
#include <cstdio>
#include <stdio.h>

#include "block_buffer.hpp"

using namespace std;

BlockBuffer::BlockBuffer() {
    
}

BlockBuffer::BlockBuffer( unsigned blocksize) :
        start_byte(0),
        end_byte(0),
        count(0),
        max_bytes(blocksize) {
    buffer.reset(new char[blocksize]);
    
    assert(blocksize > 0);
}

bool BlockBuffer::isEmpty() {
    return start_byte == end_byte && count == 0;
}

bool BlockBuffer::isFull() {
    return count == max_bytes;
}

// Buffer overflows are possible with the write. Be
// careful to allocate enough memory for the destination!
// returns number of bytes written
unsigned BlockBuffer::write(unsigned num_bytes, char* dest) {
    unsigned bytes_to_write = count >= num_bytes ? 
                            num_bytes : count;
    
    for(unsigned i = 0; i < bytes_to_write; i++) {
        *dest = buffer[start_byte];
        start_byte = start_byte == max_bytes - 1 ? 0 : start_byte + 1;
        dest += 1;
        count -= 1;
    }
    
    return bytes_to_write;
}

// This function attempts to write up to a block of bytes
// to the output file associated with the ofstream.
// @ pre - ofstream is opened to a valid file
// @ pre - ofstream is in binary and read mode.
// @ pre - ofstream has no error flags.
// @ post - ofstream will be written by X bytes, where X is 
//          the size of the BlockBuffer
// @ post - BlockBuffer will be empty.
// @ return - returns number of bytes written.
unsigned BlockBuffer::write(IOutputStream &out) {
    assert(out.good());
    unsigned bytes_written = 0;
    
    while(count > 0) {
        out.put(buffer[start_byte]);
        count -= 1;
        start_byte = (start_byte + 1) % max_bytes;
    }
    
    if(out) {
        assert(start_byte == end_byte);
        assert(count == 0);
    } else {
        throw "Output file is in invalid condition";
    }
    
    return bytes_written;
}

// Buffer overflows are possible with the write. Be
// careful to allocate enough memory for the destination!
// returns number of bytes written
unsigned BlockBuffer::read(unsigned num_bytes, char* src) {
    unsigned bytes_remaining = max_bytes - count;
    unsigned bytes_to_read = bytes_remaining >= num_bytes ?
                          num_bytes : bytes_remaining;
    for(unsigned i = 0; i < bytes_to_read; i++) {
        buffer[end_byte] = *src;
        src += 1;
        end_byte = end_byte == max_bytes - 1 ? 0 : end_byte + 1;
        count += 1;
    }
    
    return bytes_to_read;
}

// This function attempts to read up to a block of bytes
// from the input file associated with the ifstream.
// @ pre - ifstream is opened to a valid file
// @ pre - ifstream is in binary and read mode.
// @ pre - ifstream has no error flags.
// @ post - ifstream will be in one of 2 conditions
//      1. At end of file
//      2. Advanced by X bytes, where X is the number of bytes the BlockBuffer
//          read to be full
// @ post - BlockBuffer will be full.
// @ return - returns number of bytes read.
unsigned BlockBuffer::read(IInputStream &in) {
    // assert(in.is_open());
    assert(in.good());
    
    unsigned bytes_read = 0;
    
    // Read until full, or until the file ends.
    while(count < max_bytes && in) {
        char val = (char)in.get();
        if(!in) {
            break; // if we have reached the end of file, stop.
        }
        buffer[end_byte] = val;
        count++;
        end_byte = (end_byte + 1) % max_bytes;
        bytes_read += 1;
    }
    
    if(in) {
        assert(start_byte == end_byte);
        assert(count == max_bytes);
    }
    
    return bytes_read;
}

bool BlockBuffer::bufferEquals(unsigned num_bytes, char* src) {
    if(num_bytes != count) {
        // Cannot be equal if not same number of bytes
        return false;
    }
    
    for(unsigned i = 0; i < count; i++) {
        unsigned index = (start_byte + i) % max_bytes;
        if(buffer[index] != *(src + i)) {
            return false;
        }
        
    }
    
    return true;
}

void BlockBuffer::print() {
    for(unsigned i = 0; i < max_bytes; i++) {
        putchar(buffer[i]);
    }
}

bool BlockBuffer::isStart(unsigned index) {
    return index == this->start_byte;
}

bool BlockBuffer::isEnd(unsigned index) {
    return index == this->end_byte;
}

bool BlockBuffer::isCount(unsigned c) {
    return c == this->count;
}

BlockBuffer::~BlockBuffer() {}


fruit::Component<IBlockBufferFactory> getIBlockBufferFactory() {
    return fruit::createComponent()
        .bind<IBlockBuffer, BlockBuffer>()
        .registerFactory<std::unique_ptr<IBlockBuffer>(fruit::Assisted<unsigned>)>(
            [](unsigned blocksize) {
            return std::unique_ptr<IBlockBuffer>(new BlockBuffer(blocksize));
        });
}

fruit::Component<BlockBufferFactory> getBlockBufferFactory() {
    return fruit::createComponent()
        .registerFactory<std::unique_ptr<BlockBuffer>(fruit::Assisted<unsigned>)>(
            [](unsigned blocksize) {
            return std::unique_ptr<BlockBuffer>(new BlockBuffer(blocksize));
        });
}

