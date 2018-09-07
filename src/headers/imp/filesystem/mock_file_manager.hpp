



#ifndef MOCK_FILE_MANAGER_HPP
#define MOCK_FILE_MANAGER_HPP

#include "i_file_manager.hpp"
#include "i_filesystem.hpp"
#include <fruit/fruit.h>
#include <vector>
#include <string>

class MockFileManager : public IFileManager {
private:
    IFileSystem* fs;
    std::vector<std::string> temp_file_names;
    
public:
    INJECT( MockFileManager() );
    virtual std::string generateName() override;
    virtual void makeTempFile(std::string &name) override;
    virtual bool removeFile(std::string &name) override;
    virtual void removeTempFiles() override;

};


fruit::Component<IFileManager> getMockFileManager();


#endif // MOCK_FILE_MANAGER_HPP
