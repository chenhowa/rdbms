

#ifndef BLOCK_BUFFER_HPP
#define BLOCK_BUFFER_HPP

#include "i_block_buffer.hpp"
#include <fruit/fruit.h>
#include <memory>
#include <string>
#include <assert.h>
#include <cstdio>
#include <stdio.h>
#include "i_input_stream.hpp"
#include "i_output_stream.hpp"

using namespace std;

class BlockBuffer : public IBlockBuffer {
private:
    BlockBuffer();
    unsigned start_byte;
    unsigned end_byte;
    unsigned count;
    unsigned max_bytes;
    unique_ptr<char[]> buffer;
public:
    BlockBuffer( unsigned blocksize);
    bool isEmpty() override;
    bool isFull() override;
    
    // Buffer overflows are possible with the write. Be
    // careful to allocate enough memory for the destination!
    // returns number of bytes written
    unsigned write(unsigned num_bytes, char* dest) override;
    
    // This function attempts to write up to a block of bytes
    // to the output file associated with the ofstream.
    // @ pre - ofstream is opened to a valid file
    // @ pre - ofstream is in binary and read mode.
    // @ pre - ofstream has no error flags.
    // @ post - ofstream will be written by X bytes, where X is 
    //          the size of the BlockBuffer
    // @ post - BlockBuffer will be empty.
    // @ return - returns number of bytes written.
    unsigned write(IOutputStream &out) override;
    
    // Buffer overflows are possible with the write. Be
    // careful to allocate enough memory for the destination!
    // returns number of bytes written
    unsigned read(unsigned num_bytes, char* src) override;
    
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
    unsigned read(IInputStream &in) override;
    
    bool bufferEquals(unsigned num_bytes, char* src);
    
    void print();
    
    bool isStart(unsigned index);
    
    bool isEnd(unsigned index);
    
    bool isCount(unsigned c);
    
    virtual ~BlockBuffer();
    
private:
    unsigned calcNumBytesToWrite(unsigned num_bytes);
    void writeStartByte(char* dest);
    void incrementStart();
    void decrementCount();
    void writeStartByte(IOutputStream &out);
    unsigned calcNumEmptyBytes();
    unsigned calcNumBytesToRead(unsigned num_bytes);
    void readToEndByte(char data);
    void incrementEnd();
    void incrementCount();
    unsigned readAsMuchAsPossible(IInputStream &in);
    bool allBytesMatchBuffer(unsigned num_bytes, char* src);

};

using IBlockBufferFactory = std::function<std::unique_ptr<IBlockBuffer>(unsigned)>;
fruit::Component<IBlockBufferFactory> getIBlockBufferFactory();

using BlockBufferFactory = std::function<std::unique_ptr<BlockBuffer>(unsigned)>;
fruit::Component<BlockBufferFactory> getBlockBufferFactory();


#endif //BLOCK_BUFFER_HPP