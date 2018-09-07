



#include "file_manager.hpp"

#include <boost/filesystem.hpp>
#include <cstdio>

using namespace boost;

std::string FileManager::generateName() {
    std::string name = filesystem::unique_path().string();
    temp_files.push_back(name);
    return name;
}

void FileManager::makeTempFile(std::string &name) {
    // NO NEED. NO WAY TO DO THIS WELL, DAMMIT
}

bool FileManager::removeFile(std::string &name) {
    filesystem::remove(name);
    return true;
}

void FileManager::removeTempFiles() {
    while(!temp_files.empty()) {
        filesystem::remove(temp_files.back());
        temp_files.pop_back();
    }
}

using namespace fruit;
Component<IFileManager> getIFileManager() {
    return createComponent()
        .bind<IFileManager, FileManager>();
    
}
