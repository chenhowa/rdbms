


#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include "i_filesystem.hpp"
#include <map>
#include <string>
#include <fruit/fruit.h>

class FileSystem : public IFileSystem {
private:
    std::map<std::string, std::string> fs;

public:
    INJECT( FileSystem() );
    virtual bool fileExists(const std::string &name) override;
    virtual unsigned fileCount() override;
    virtual std::string& operator[] (const std::string &name) override;
    virtual bool createFile(const std::string &name) override;
    virtual bool deleteFile(const std::string &name) override;
    virtual bool renameFile(const std::string &name,
                    const std::string &newName) override;
    virtual unsigned getCharCount(const std::string &name) override;
    
    virtual ~FileSystem() { };

};


fruit::Component<IFileSystem> getIFileSystem();

#endif // FILESYSTEM_HPP