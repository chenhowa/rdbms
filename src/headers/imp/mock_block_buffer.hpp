



#ifndef MOCK_BLOCK_BUFFER_HPP
#define MOCK_BLOCK_BUFFER_HPP

#include "i_mock_block_buffer.hpp"
#include <string>
#include <deque>
#include <fruit/fruit.h>


class MockBlockBuffer : public IMockBlockBuffer {
private:
    MockBlockBuffer();
    unsigned blocksize;
    std::string charsRead;
    std::string charsWritten;
    std::deque<char> buffer;
public:
    MockBlockBuffer(unsigned size);
    virtual bool isEmpty() override;
    virtual bool isFull() override;
    virtual unsigned write(unsigned num_bytes, char* dest) override;
    virtual unsigned write(IOutputStream &out) override;
    virtual unsigned read(unsigned num_bytes, char* src) override;
    virtual unsigned read(IInputStream &in) override;
    virtual ~MockBlockBuffer() { };
    
public:
    virtual std::string getCharsRead() override;
    virtual std::string getCharsWritten() override;
    virtual std::string getBufferContents() override;
    virtual unsigned getBlockSize() override;

};

using IMockBlockBufferFactory = std::function<std::unique_ptr<IMockBlockBuffer>(unsigned)>;
fruit::Component<IMockBlockBufferFactory> getIMockBlockBufferFactory();

#endif // MOCK_BLOCK_BUFFER_HPP