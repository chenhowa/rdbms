


#include "mock_file_manager.hpp"

#include <boost/filesystem.hpp>


using namespace boost;

extern IFileSystem* g_filesystem;


MockFileManager::MockFileManager() : 
            fs(g_filesystem)
                            {
}

std::string MockFileManager::generateName() {
    std::string name = filesystem::unique_path().string();
    temp_file_names.push_back(name);
    return name;
}

void MockFileManager::makeTempFile(std::string &name) {
    
}

bool MockFileManager::removeFile(std::string &name) {
    if(!fs->fileExists(name)){
        // File not removed. File did not exist.
        return false;
    }
    
    fs->deleteFile(name);
    return true;
}

void MockFileManager::removeTempFiles() {
    while(!temp_file_names.empty()) {
        std::string name_to_remove = temp_file_names.back();
        temp_file_names.pop_back();
        if(fs->fileExists(name_to_remove)) {
            fs->deleteFile(name_to_remove);
        }
    }
}


using namespace fruit;

Component<IFileManager> getMockFileManager() {
    return createComponent()
        .bind<IFileManager, MockFileManager>();
}
