


#ifndef I_FILESYSTEM_HPP
#define I_FILESYSTEM_HPP

#include <string>

class IFileSystem {
public:
    virtual bool fileExists(const  std::string &name) = 0;
    virtual unsigned fileCount() = 0;
    virtual std::string& operator[] (const  std::string &name) = 0;
    virtual bool createFile(const  std::string &name) = 0;
    virtual bool deleteFile(const  std::string &name) = 0;
    virtual bool renameFile(const  std::string &name,
                     const std::string &newName) = 0;
    virtual unsigned getCharCount( const std::string &name) = 0;
    virtual std::string& get(const std::string &name) = 0;
    virtual bool set(const std::string &name, const std::string &val) = 0;
    virtual void reset() = 0;
    
    virtual ~IFileSystem() { };
};



#endif // I_FILESYSTEM_HPP