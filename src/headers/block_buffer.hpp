

#ifndef BLOCK_BUFFER_HPP
#define BLOCK_BUFFER_HPP

#include "i_block_buffer.hpp"
#include <fruit/fruit.h>
#include <memory>
#include <string>
#include <assert.h>
#include <cstdio>
#include <fstream>
#include <stdio.h>
#include <iostream>

using namespace std;

class BlockBuffer : public IBlockBuffer {
private:
    BlockBuffer();
    int start_byte;
    int end_byte;
    int count;
    int max_bytes;
    unique_ptr<char[]> buffer;
public:
    BlockBuffer( int blocksize);
    bool isEmpty() override;
    bool isFull() override;
    
    // Buffer overflows are possible with the write. Be
    // careful to allocate enough memory for the destination!
    // returns number of bytes written
    int write(int num_bytes, char* dest) override;
    
    // This function attempts to write up to a block of bytes
    // to the output file associated with the ofstream.
    // @ pre - ofstream is opened to a valid file
    // @ pre - ofstream is in binary and read mode.
    // @ pre - ofstream has no error flags.
    // @ post - ofstream will be written by X bytes, where X is 
    //          the size of the BlockBuffer
    // @ post - BlockBuffer will be empty.
    // @ return - returns number of bytes written.
    int write(ostream &out) override;
    
    // Buffer overflows are possible with the write. Be
    // careful to allocate enough memory for the destination!
    // returns number of bytes written
    int read(int num_bytes, char* src) override;
    
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
    int read(istream &in) override;
    
    bool bufferEquals(int num_bytes, char* src);
    
    void print();
    
    bool isStart(int index);
    
    bool isEnd(int index);
    
    bool isCount(int c);
    
    virtual ~BlockBuffer();
};

using IBlockBufferFactory = std::function<std::unique_ptr<IBlockBuffer>(int)>;
fruit::Component<IBlockBufferFactory> getBlockBufferComponent();

using BlockBufferFactory = std::function<std::unique_ptr<BlockBuffer>(int)>;
fruit::Component<BlockBufferFactory> getTestingBlockBufferComponent();


#endif //BLOCK_BUFFER_HPP