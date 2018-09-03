



#include "filesystem.hpp"
#include <assert.h>
#include <string>


FileSystem::FileSystem(): fs() {
    
}

bool FileSystem::fileExists(const std::string &name) {
    return fs.find(name) != fs.end();
}

unsigned FileSystem::fileCount() {
    return fs.size();
}

std::string& FileSystem::operator[] (const std::string &name) {
    return fs[name];
}

bool FileSystem::createFile(const std::string &name) {
    if(fileExists(name)) {
        return false;
    }
    
    fs.insert( std::pair<std::string, std::string>(name.c_str(), std::string().c_str()) );
    return true;
}

bool FileSystem::deleteFile(const std::string &name) {
    if(!fileExists(name)) {
        return false;
    }
    
    unsigned num_erased = fs.erase(name);
    assert(num_erased == 1);
    
    return true;
}

bool FileSystem::renameFile(const std::string &name,
                const std::string &newName) {
    if(!fileExists(name)) {
        return false;
    }
    
    if(fileExists(newName)) {
        return false;
    }
    
    fs.insert(std::pair<std::string, std::string>(newName.c_str(), fs[name].c_str()) );
    deleteFile(name);
    
    return true;
}

unsigned FileSystem::getCharCount(const std::string &name) {
    if(fileExists(name)) {
        return fs[name].length();
    }
    
    return 0;
}

using namespace fruit;
Component<IFileSystem> getIFileSystem() {
    return createComponent()
        .bind<IFileSystem, FileSystem>();
}