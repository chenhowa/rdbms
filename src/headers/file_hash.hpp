

#ifndef FILE_HASH_HPP
#define FILE_HASH_HPP

#include <string>

using namespace std;

class FileHasher {
    // Files that FileHasher can hash
    // must provide metadata at the top of the file
    // so FileHasher understands the table layout
    // so it can hash on the read values
    
    // Does it have to be this way? Perhaps not.
    // But it simplifies things greatly if it is.
    
    private:
        int blocksize;
        int _numOutputBuffers;
        BlockBuffer* input;
        BlockBuffer** outputs;
    public:
        FileHasher();
        int numOutputBuffers();
        int getBlockSize();
        void hash(int col);
        ~FileHasher();
        
};


#endif //FILE_HASH_HPP
