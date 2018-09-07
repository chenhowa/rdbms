/*
 * Interface for classes that read/write blocks of data
 * to and from memory/ram
 *
 */
 
#ifndef I_BLOCK_BUFFER_HPP
#define I_BLOCK_BUFFER_HPP
 

#include <fstream>
#include <iostream>
#include <string> // USE THIS. BETTER THAN CHAR*
#include "i_input_stream.hpp"
#include "i_output_stream.hpp"

class IBlockBuffer {
public:
    virtual bool isEmpty() = 0;
    virtual bool isFull() = 0;
    virtual unsigned write(unsigned num_bytes, char* dest) = 0;
    virtual unsigned write(IOutputStream &out) = 0;
    virtual unsigned write(IBlockBuffer &dest) = 0;
    virtual unsigned read(unsigned num_bytes, char* src) = 0;
    virtual unsigned read(IInputStream &in) = 0;
    virtual unsigned getBlockSize() = 0;
    virtual char peek() = 0;
    virtual ~IBlockBuffer() { };
};

#endif // I_BLOCK_BUFFER_HPP