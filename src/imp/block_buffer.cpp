



#include "block_buffer.hpp"
#include <string>
#include <assert.h>
#include <cstdio>
#include <fstream>
#include <stdio.h>

using namespace std;

BlockBuffer::BlockBuffer() {
    
}

BlockBuffer::BlockBuffer(int blocksize) :
    start_byte(0),
    end_byte(0),
    count(0),
    max_bytes(blocksize) {
    buffer.reset(new char[blocksize]);
    
    assert(blocksize > 0);
}

bool BlockBuffer::isEmpty() {
    return start_byte == end_byte;
}


bool BlockBuffer::isFull() {
    return count == max_bytes;
}

// Buffer overflows are possible with the write. Be
// careful to allocate enough memory for the destination!
// returns number of bytes written
int BlockBuffer::write(int num_bytes, char* dest) {
    int bytes_to_write = count >= num_bytes ? 
                            num_bytes : count;
    
    for(int i = 0; i < bytes_to_write; i++) {
        *dest = buffer[start_byte];
        start_byte = start_byte == max_bytes - 1 ? 0 : start_byte + 1;
        dest += 1;
        count -= 1;
    }
    
    return bytes_to_write;
}

// TODO: FILE MUST BE IN BINARY WRITE MODE.
// HOW TO ENFORCE THAT?
// ALSO MAKE SURE fstream is OPEN
int BlockBuffer::write(ofstream f) {
    // Can I implement a B+ tree by using a sequential
    // file layout? How would I reference the table the
    // B tree refers to? Maybe I still have to reference
    // table files in the disk system BY NAME???
    
    // I will let the file system handle the linked
    // list of blocks.
    
    // But as for this. Write all the bytes to the
    // file stream.
    int bytes_written = 0;
    
    while(count > 0) {
        f.put(buffer[start_byte]);
        count -= 1;
        start_byte = (start_byte + 1) % max_bytes;
    }
    
    assert(start_byte == end_byte);
    assert(count == 0);
    
    return bytes_written;
}

// Buffer overflows are possible with the write. Be
// careful to allocate enough memory for the destination!
// returns number of bytes written
int BlockBuffer::read(int num_bytes, char* src) {
    int bytes_remaining = max_bytes - count;
    int bytes_to_read = bytes_remaining >= num_bytes ?
                          num_bytes : bytes_remaining;
    for(int i = 0; i < bytes_to_read; i++) {
        buffer[end_byte] = *src;
        src += 1;
        end_byte = end_byte == max_bytes - 1 ? 0 : end_byte + 1;
        count += 1;
    }
    
    return bytes_to_read;
}

// TODO:  FILE MUST BE IN BINARY READ MODE.
// HOW TO ENFORCE THAT?
// ALSO MAKE SURE fstream is OPEN
int BlockBuffer::read(ifstream in) {
    // TODO MAKE SURE TO EXIT ON EOF.
    int bytes_read = 0;
    
    while(count < max_bytes && in) {
        buffer[end_byte] = in.get();
        count++;
        end_byte = (end_byte + 1) % max_bytes;
        bytes_read += 1;
    }
    
    assert(start_byte = end_byte);
    assert(count == max_bytes);
    
    return bytes_read;
}

bool BlockBuffer::bufferEquals(int num_bytes, char* src) {
    if(num_bytes != count) {
        // Cannot be equal if not same number of bytes
        return false;
    }
    
    for(int i = 0; i < count; i++) {
        int index = (start_byte + i) % max_bytes;
        if(buffer[index] != *(src + i)) {
            return false;
        }
        
    }
    
    return true;
}

void BlockBuffer::print() {
    for(int i = 0; i < max_bytes; i++) {
        putchar(buffer[i]);
    }
}

bool BlockBuffer::isStart(int index) {
    return index == this->start_byte;
}

bool BlockBuffer::isEnd(int index) {
    return index == this->end_byte;
}

bool BlockBuffer::isCount(int c) {
    return c == this->count;
}

