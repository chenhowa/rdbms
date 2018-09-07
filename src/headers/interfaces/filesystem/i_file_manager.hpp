




#ifndef I_FILE_MANAGER_HPP
#define I_FILE_MANAGER_HPP

#include <string>

class IFileManager {
public:
    virtual std::string generateName() = 0;
    virtual void makeTempFile(std::string &name) = 0;
    virtual bool removeFile(std::string &name) = 0;
    virtual void removeTempFiles() = 0;
    virtual ~IFileManager() { };
};


#endif // I_FILE_MANAGER_HPP
