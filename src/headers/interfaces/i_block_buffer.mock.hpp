




#ifndef I_BLOCK_BUFFER_MOCK_HPP
#define I_BLOCK_BUFFER_MOCK_HPP

#include "i_block_buffer.hpp"
#include <vector>

/*
class IBlockBufferMock : public IBlockBuffer {
public:
    int max_count;
    int count;
    int isEmpty_count;
    int isFull_count;
    int writeRAM_count;
    int writeFile_count;
    int readRAM_count;
    int readFile_count;

    IBlockBufferMock(int blocksize) :
        max_count(blocksize),
        count(0), isEmpty_count(0), isFull_count(0),
        writeRAM_count(0), writeFile_count(0),
        readRAM_count(0), readFile_count(0)
    {
        
    }

    virtual bool isEmpty() {
        return this->count == 0;
    }
    virtual bool isFull() {
        return this->count == this->max_count;
    }
    virtual int write(int num_bytes, char* dest) {
        
    }
    virtual int write(std::ostream &out) {
        
    }
    virtual int read(int num_bytes, char* src) {
        
    }
    virtual int read(std::istream &in) {
        
    }
    virtual ~IBlockBuffer() { };
    
};

using IBlockBufferFactory = std::function<std::unique_ptr<IBlockBuffer>(int)>;

*/

#endif // I_BLOCK_BUFFER_MOCK_HPP