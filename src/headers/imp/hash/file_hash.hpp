

#ifndef FILE_HASH_HPP
#define FILE_HASH_HPP

#include <fruit/fruit.h>
#include <string>
#include <vector>
#include <memory>
#include "i_block_buffer.hpp"
#include "i_file_hash.hpp"

using namespace std;


using IBlockBufferFactory = std::function<std::unique_ptr<IBlockBuffer>(unsigned)>;

class FileHasher : public IFileHasher {
    
private:
    FileHasher();
    unsigned blocksize;
    unsigned _numOutputBuffers;
    unique_ptr<IBlockBuffer> input;
    vector<unique_ptr<IBlockBuffer>> outputs;
public:
    FileHasher(unsigned num_outputs, IBlockBufferFactory fac);
    FileHasher(unsigned num_outputs, unsigned blocksize, IBlockBufferFactory fac);
    FileHasher(IBlockBuffer* in, IBlockBuffer* out_1, IBlockBuffer* out_2);
    unsigned numOutputBuffers();
    unsigned getBlockSize() override;
    ~FileHasher();
    
    void testHash(IInputStream &file, std::vector<IOutputStream*> &dests);
    
private:
    void readToInputBlockIfEmpty(IInputStream &in);
    void hashAndWriteInputBlockToOutput(std::vector<IOutputStream*> &dests);
    void flushAnyRemainingInputToOutput(std::vector<IOutputStream*> &dests);
};

using IFileHasherFactory = std::function<std::unique_ptr<IFileHasher>(unsigned, unsigned)>;
fruit::Component<IFileHasherFactory> getIFileHasherFactory();




#endif //FILE_HASH_HPP
