

#ifndef FILE_HASH_HPP
#define FILE_HASH_HPP

#include <fruit/fruit.h>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "i_block_buffer.hpp"
#include "i_file_hash.hpp"

using namespace std;


using IBlockBufferFactory = std::function<std::unique_ptr<IBlockBuffer>(int)>;

class FileHasher : public IFileHasher {
    
private:
    FileHasher();
    unsigned blocksize;
    unsigned _numOutputBuffers;
    unique_ptr<IBlockBuffer> input;
    unique_ptr< unique_ptr<IBlockBuffer>[]> outputs;
public:
    FileHasher(int num_outputs, IBlockBufferFactory fac);
    FileHasher(int num_outputs, int blocksize, IBlockBufferFactory fac);
    FileHasher(IBlockBuffer* in, IBlockBuffer* out_1, IBlockBuffer* out_2);
    unsigned numOutputBuffers();
    unsigned getBlockSize() override;
    ~FileHasher();
    
    void testHash(std::istream &file, std::vector<std::ostream*> &dests);
};

using IFileHasherFactory = std::function<std::unique_ptr<IFileHasher>(int, int)>;
fruit::Component<IFileHasherFactory> getFileHasherComponent();


using FileHasherFactory = std::function<std::unique_ptr<FileHasher>(int, int)>;
fruit::Component<FileHasherFactory> _getFileHasherComponent();



#endif //FILE_HASH_HPP
