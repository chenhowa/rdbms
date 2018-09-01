




#include "mock_block_buffer.hpp"
#include <string>

MockBlockBuffer::MockBlockBuffer(unsigned size) :
    blocksize(size),
    charsRead(), charsWritten(),
    buffer()
            {
    
}


bool MockBlockBuffer::isEmpty() {
    return buffer.empty();
}

bool MockBlockBuffer::isFull() {
    return buffer.size() == blocksize;
}

unsigned MockBlockBuffer::write(unsigned num_bytes, char* dest) {
    unsigned bytes_written = 0;
    while(!isEmpty() && bytes_written < num_bytes) {
        char writeVal = buffer.front();
        buffer.pop_front();
        dest[bytes_written] = writeVal;
        bytes_written += 1;
        charsWritten += writeVal;
    }
    
    return bytes_written;
}

unsigned MockBlockBuffer::write(IOutputStream &out) {
    unsigned bytes_written = 0;
    while(!isEmpty() && out.good()) {
        char writeVal = buffer.front();
        buffer.pop_front();
        out.put(writeVal);
        bytes_written += 1;
        charsWritten += writeVal;
    }
    
    return bytes_written;
}

unsigned MockBlockBuffer::read(unsigned num_bytes, char* src) {
    unsigned bytes_read = 0;
    while(!isFull() && bytes_read < num_bytes) {
        char readVal = src[bytes_read];
        bytes_read += 1;
        buffer.push_back(readVal);
        charsRead += readVal;
    }
    
    return bytes_read;
}

unsigned MockBlockBuffer::read(IInputStream &in) {
    unsigned bytes_read = 0;
    while(!isFull() && in) {
        char readVal = in.get();
        if(!in.good()) {
            // break if character read was bad.
            break;
        }
        buffer.push_back(readVal);
        bytes_read += 1;
        charsRead += readVal;
    }
    
    return bytes_read;
}

std::string MockBlockBuffer::getCharsRead() {
    return charsRead;
}

std::string MockBlockBuffer::getCharsWritten() {
    return charsWritten;
}

std::string MockBlockBuffer::getBufferContents() {
    std::string contents;
    for(unsigned i = 0; i < buffer.size(); i++) {
        contents += buffer[i];
    }
    
    return contents;
}


using namespace fruit;
fruit::Component<IMockBlockBufferFactory> getIMockBlockBufferFactory() {
    return createComponent()
        .registerFactory<std::unique_ptr<IMockBlockBuffer>(fruit::Assisted<unsigned>)>(
            [](unsigned size) {
                return std::unique_ptr<IMockBlockBuffer>(new MockBlockBuffer(size));
            }
        );
}
