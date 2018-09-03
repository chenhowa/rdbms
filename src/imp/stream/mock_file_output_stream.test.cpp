#include "catch.hpp"

#include "mock_file_output_stream.hpp"
#include "filesystem.hpp"


TEST_CASE("Writing with just one file stream ... ") {
    fruit::Injector<IFileSystem> injector(getIFileSystem);
    IFileSystem* fs(injector);
    
    MockFileOutputStream out(fs);
    
    SECTION("to one file") {
        std::string name("default.txt");
        fs->createFile(name);
        REQUIRE(fs->fileExists(name) == true);
        
        
        
    }
}