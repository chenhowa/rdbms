#include "catch.hpp"

#include "mock_file_output_stream.hpp"
#include "filesystem.hpp"


void checkGood(MockFileOutputStream &out) {
    REQUIRE(out.good());
    REQUIRE(!out.bad());
    REQUIRE(!out.fail());
    REQUIRE(!out.eof());
}

TEST_CASE("Writing with just one file stream ... ") {
    fruit::Injector<IFileSystem> injector(getIFileSystem);
    IFileSystem* fs(injector);
    
    MockFileOutputStream out(fs);
    REQUIRE(!out.is_open());
    checkGood(out);
    
    SECTION("to one empty file ... ") {
        std::string name("default.txt");
        fs->createFile(name);
        REQUIRE(fs->fileExists(name) == true);

        
        SECTION("write at beginning") {
            out.open(name, std::ios_base::out);
            REQUIRE(out.is_open());
            out.put('a');
            
            REQUIRE(out.getContent() == std::string("a") );
            REQUIRE( (*fs)[name] == std::string("a"));
            out.write(3, "bcdef");
            REQUIRE(out.getContent() == std::string("abcd") );
            
            checkGood(out);
        }
        
        SECTION("write at end ... ") {
            out.open(name, std::ios_base::ate);
            REQUIRE(out.is_open());
            out.put('a');
            REQUIRE(out.getContent() == std::string("a"));
            REQUIRE( (*fs)[name] == std::string("a"));
            out.write(3, "bcdef");
            REQUIRE(out.getContent() == std::string("abcd"));
        }
        
        out.close();
        REQUIRE(!out.is_open());
        checkGood(out);
        
        fs->deleteFile(name);
        REQUIRE(fs->fileExists(name) == false);
        REQUIRE(fs->fileCount() == 0);
    }
    
    SECTION("to one non-empty file ... ") {
        std::string name("default.txt");
        fs->createFile(name);
        (*fs)[name] = std::string("zzz");
        REQUIRE(fs->fileExists(name) == true);
        REQUIRE( (*fs)[name] == std::string("zzz"));
        
        SECTION("write at beginning") {
            out.open(name, std::ios_base::out);
            REQUIRE(out.is_open());
            out.put('a');
            REQUIRE(out.getContent() == std::string("azz"));
            REQUIRE( (*fs)[name] == std::string("azz"));
            out.write(3, "bcdef");
            REQUIRE(out.getContent() == std::string("abcd"));
        }
        
        SECTION("write at end ... ") {
            out.open(name, std::ios_base::ate);
            REQUIRE(out.is_open());
            out.put('a');
            REQUIRE(out.getContent() == std::string("zzza"));
            REQUIRE( (*fs)[name] == std::string("zzza"));
            out.write(3, "bcdef");
            REQUIRE(out.getContent() == std::string("zzzabcd"));
        }
        
        out.close();
        REQUIRE(!out.is_open());
        checkGood(out);
        
        fs->deleteFile(name);
        REQUIRE(fs->fileExists(name) == false);
        REQUIRE(fs->fileCount() == 0);
    }
    
    SECTION("perform invalid operations") {
        std::string invalid_name("oops.txt");
        out.open(invalid_name, std::ios_base::out);
        REQUIRE(!out.is_open());
        REQUIRE(!out.good());
        REQUIRE(out.fail());
        REQUIRE(!out.bad());
        REQUIRE(!out.eof());
    }
}

TEST_CASE("Writing with two file streams ... ") {
    // TODO : Add any tests that make sense for
    // two file streams .... Are there any, even?
}