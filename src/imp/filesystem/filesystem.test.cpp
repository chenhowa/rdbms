#include "catch.hpp"
#include "filesystem.hpp"


TEST_CASE("File System correctly executes ... ") {
    FileSystem fs;
    REQUIRE(fs.fileCount() == 0);
    
    SECTION("detects whether file exists") {
        std::string name("default");
        REQUIRE(fs.fileExists(name) == false);
        REQUIRE(fs.getCharCount(name) == 0);
        
        REQUIRE(fs.createFile(name) == true);
        REQUIRE(fs.fileCount() == 1);
        REQUIRE(fs.createFile(name) == false);
        REQUIRE(fs.fileExists(name) == true);
        REQUIRE(fs.getCharCount(name) == 0);
        
        REQUIRE(fs.deleteFile(name) == true);
        REQUIRE(fs.deleteFile(name) == false);
        REQUIRE(fs.fileExists(name) == false);
        REQUIRE(fs.getCharCount(name) == 0);
        REQUIRE(fs.fileCount() == 0);
    }
    
    SECTION("renames files") {
        std::string name1("1");
        std::string name2("2");
        REQUIRE(fs.fileExists(name1) == false);
        REQUIRE(fs.getCharCount(name1) == 0); 
        REQUIRE(fs.fileExists(name2) == false);
        REQUIRE(fs.getCharCount(name2) == 0); 
        
        REQUIRE(fs.renameFile(name1, name2) == false);
        REQUIRE(fs.createFile(name2) == true);
        REQUIRE(fs.fileCount() == 1);
        REQUIRE(fs.renameFile(name1, name2) == false);
        REQUIRE(fs.deleteFile(name2) == true);
        REQUIRE(fs.fileCount() == 0);
        REQUIRE(fs.createFile(name1) == true);
        REQUIRE(fs.fileCount() == 1);
        REQUIRE(fs.renameFile(name1, name2) == true);
        REQUIRE(fs.fileCount() == 1);
        REQUIRE( (!fs.fileExists(name1) && fs.fileExists(name2) ) == true);
    }       
    
}

TEST_CASE("File System fails to execute ... ") {
    FileSystem fs;
    REQUIRE(fs.fileCount() == 0);
    
    // TODO : Add tests to prove that file system does not
    // execute an operation when it should not.
}