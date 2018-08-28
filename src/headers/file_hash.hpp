

#ifndef FILE_HASH_HPP
#define FILE_HASH_HPP

#include <string>
#include "block_buffer.hpp"

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
    unique_ptr<BlockBuffer> input;
    unique_ptr< unique_ptr<BlockBuffer>[]> outputs;
public:
    FileHasher(int num_outputs);
    FileHasher(int num_outputs, int blocksize);
    int numOutputBuffers();
    int getBlockSize();
    void hash(int col);
    ~FileHasher();
        
};


#endif //FILE_HASH_HPP
