



#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include "i_file_manager.hpp"
#include <fruit/fruit.h>
#include <vector>
#include <string>

class FileManager : public IFileManager {
private:
    std::vector<std::string> temp_files;

public:
    INJECT( FileManager() ) = default;
    virtual std::string generateName() override;
    virtual void makeTempFile(std::string &name) override;
    virtual ~FileManager() { };
    virtual bool removeFile(std::string &name) override;
    virtual void removeTempFiles() override;
};

fruit::Component<IFileManager> getIFileManager();



#endif // FILE_MANAGER_HPP
