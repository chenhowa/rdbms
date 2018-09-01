





#ifndef I_FILE_HASH_HPP
#define I_FILE_HASH_HPP

#include "i_input_stream.hpp"
#include "i_output_stream.hpp"

class IFileHasher {
public:
    virtual unsigned getBlockSize() = 0;
    
    virtual ~IFileHasher() { }
};


#endif //I_FILE_HASH_HPP