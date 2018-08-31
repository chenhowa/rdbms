





#ifndef I_FILE_HASH_HPP
#define I_FILE_HASH_HPP

class IFileHasher {
public:
    virtual unsigned getBlockSize() = 0;
    
    virtual ~IFileHasher() { }
};


#endif //I_FILE_HASH_HPP