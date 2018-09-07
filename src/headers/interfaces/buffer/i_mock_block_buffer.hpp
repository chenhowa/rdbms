



#ifndef I_MOCK_BLOCK_BUFFER_HPP
#define I_MOCK_BLOCK_BUFFER_HPP

#include "i_block_buffer.hpp"


class IMockBlockBuffer : public IBlockBuffer {
public:
    virtual std::string getCharsRead() = 0;
    virtual std::string getCharsWritten() = 0;
    virtual std::string getBufferContents() = 0;
    
    virtual ~IMockBlockBuffer() { }
};


#endif // I_MOCK_BLOCK_BUFFER_HPP