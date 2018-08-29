/*
 * Interface for classes that read/write blocks of data
 * to and from memory/ram
 *
 */
 
#ifndef I_BLOCK_BUFFER_HPP
#define I_BLOCK_BUFFER_HPP
 

#include <fstream>
#include <string> // USE THIS. BETTER THAN CHAR*

class IBlockBuffer {
public:
    virtual bool isEmpty() = 0;
    virtual bool isFull() = 0;
    virtual int write(int num_bytes, char* dest) = 0;
    virtual int write(std::ofstream &out) = 0;
    virtual int read(int num_bytes, char* src) = 0;
    virtual int read(std::ifstream &in) = 0;
    virtual ~IBlockBuffer() { };
};

#endif // I_BLOCK_BUFFER_HPP