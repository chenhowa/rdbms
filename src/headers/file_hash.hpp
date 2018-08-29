

#ifndef FILE_HASH_HPP
#define FILE_HASH_HPP

#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include "i_block_buffer.hpp"
#include "i_block_buffer_factory.hpp"

using namespace std;

class FileHasher {
    // Files that FileHasher can hash
    // must provide metadata at the top of the file
    // so FileHasher understands the table layout
    // so it can hash on the read values
    
    // Does it have to be this way? Perhaps not.
    // But it simplifies things greatly if it is.
    
private:
    FileHasher();
    int blocksize;
    int _numOutputBuffers;
    unique_ptr<IBlockBuffer> input;
    unique_ptr< unique_ptr<IBlockBuffer>[]> outputs;
public:
    FileHasher(int num_outputs, IBlockBufferFactory* fac);
    FileHasher(int num_outputs, int blocksize, IBlockBufferFactory* fac);
    int numOutputBuffers();
    int getBlockSize();
    ~FileHasher();
    
    void testHash(std::string file, std::vector<std::string> dests);

        
};


#endif //FILE_HASH_HPP
