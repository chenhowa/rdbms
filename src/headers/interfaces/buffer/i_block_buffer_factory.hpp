


#ifndef I_BLOCK_BUFFER_FACTORY_HPP
#define I_BLOCK_BUFFER_FACTORY_HPP

#include "i_block_buffer.hpp"

class IBlockBufferFactory {
public:
    virtual IBlockBuffer* make(int blocksize) = 0;
    virtual ~IBlockBufferFactory();
};


#endif