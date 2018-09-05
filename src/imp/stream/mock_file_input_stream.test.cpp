


#include "catch.hpp"

#include "mock_file_input_stream.hpp"
#include "filesystem.hpp"


void checkInputGood(MockFileInputStream &in, const char* message) {
    //printf("%s\n", message);
    REQUIRE(in.good());
    REQUIRE(!in.bad());
    REQUIRE(!in.fail());
    REQUIRE(!in.eof());
}


TEST_CASE("reading with just one file stream ...") {
    fruit::Injector<IFileSystem> injector(getIFileSystem);
    IFileSystem* fs(injector);
    
    MockFileInputStream in(fs);
    REQUIRE(!in.is_open());
    checkInputGood(in, "check1");
    
    std::string name("default");
    SECTION("read from an empty file") {
        fs->createFile(name);
        in.open(name, std::ios_base::in);
        checkInputGood(in, "check2");
        REQUIRE(in.is_open());
        char c = in.get();
        REQUIRE(c == char(-1));
        CHECK(in.eof());
        CHECK(!in.good());
        CHECK(!in.fail());
        CHECK(!in.bad());
        in.close();
        checkInputGood(in, "check3");
        REQUIRE(!in.is_open());
        
        in.open(name, std::ios_base::in);
        REQUIRE(in.is_open());
        checkInputGood(in, "check4");
        c = in.peek();
        REQUIRE(c == char(-1));
        CHECK(in.eof());
        CHECK(!in.good());
        CHECK(!in.fail());
        CHECK(!in.bad());
        in.close();
        checkInputGood(in, "check5");
        REQUIRE(!in.is_open());
        
        //printf("last one\n");
        in.open(name, std::ios_base::in);
        REQUIRE(in.is_open());
        checkInputGood(in, "check6");
        char* buffer = new char[20];
        memset(buffer, '\0', 20);
        REQUIRE(std::string(buffer).empty());
        in.read(1, buffer);
        REQUIRE(in.eof());
        REQUIRE(!in.good());
        REQUIRE(!in.fail());
        REQUIRE(!in.bad());
        
        
        delete [] buffer;
    }
    
    SECTION("read from beginning of non-empty file") {
        //printf("test beginning non-empy file\n");
        fs->createFile(name);
        fs->set(name, "hello");
        REQUIRE(fs->get(name) == std::string("hello"));
        in.open(name, std::ios_base::in);
        REQUIRE(in.is_open());
        char c = in.get();
        REQUIRE(c == 'h');
        unsigned buf_size = 20;
        char* buffer = new char[buf_size];
        memset(buffer, '\0', buf_size);
        in.read(2, buffer);
        REQUIRE(std::string(buffer) == std::string("el"));
        memset(buffer, '\0', buf_size);
        in.read(100, buffer);
        REQUIRE(std::string(buffer) == std::string("lo"));
        CHECK(in.eof());
        CHECK(!in.good());
        CHECK(!in.fail());
        CHECK(!in.bad());
        
        delete [] buffer;
    }
    
    
    SECTION("read from end of non-empty file") {
        REQUIRE(!fs->fileExists(name));
        REQUIRE(fs->createFile(name) == true);
        fs->set(name, "hello");
        REQUIRE(fs->get(name) == std::string("hello"));
        in.open(name, std::ios_base::ate);
        checkInputGood(in, "check7.5");
        char c = in.get();
        REQUIRE(c == char(-1));
        REQUIRE(in.eof());
        REQUIRE(!in.good());
        REQUIRE(!in.fail());
        REQUIRE(!in.bad());
    }
    
    in.close();
    REQUIRE(!in.is_open());
    checkInputGood(in, "check8");
}


TEST_CASE("reading with two file streams ... ") {
    fruit::Injector<IFileSystem> injector(getIFileSystem);
    IFileSystem* fs(injector);
    
    MockFileInputStream in_1(fs);
    REQUIRE(!in_1.is_open());
    checkInputGood(in_1, "check1");
    
    MockFileInputStream in_2(fs);
    REQUIRE(!in_2.is_open());
    checkInputGood(in_2, "check2");
    
    std::string name("default");
    REQUIRE(!fs->fileExists(name));
    SECTION("simultaneous reading from a single file ... ") {
        fs->createFile(name);
        fs->set(name, "hello");
        unsigned buf_size = 20;
        char* buffer = new char[buf_size];
        char c;
        
        in_1.open(name, std::ios_base::in);
        REQUIRE(in_1.is_open());
        c = in_1.get();
        REQUIRE(c == 'h');
        memset(buffer, '\0', buf_size);
        in_1.read(2, buffer);
        REQUIRE(std::string(buffer) == std::string("el"));
        memset(buffer, '\0', buf_size);
        in_1.read(100, buffer);
        REQUIRE(std::string(buffer) == std::string("lo"));
        CHECK(in_1.eof());
        CHECK(!in_1.good());
        CHECK(!in_1.fail());
        CHECK(!in_1.bad());
        
        in_2.open(name, std::ios_base::in);
        REQUIRE(in_2.is_open());
        c = in_2.get();
        REQUIRE(c == 'h');
        memset(buffer, '\0', buf_size);
        in_2.read(2, buffer);
        REQUIRE(std::string(buffer) == std::string("el"));
        memset(buffer, '\0', buf_size);
        in_2.read(100, buffer);
        REQUIRE(std::string(buffer) == std::string("lo"));
        CHECK(in_2.eof());
        CHECK(!in_2.good());
        CHECK(!in_2.fail());
        CHECK(!in_2.bad());
        
        delete [] buffer;
    }
    
    
    in_1.close();
    REQUIRE(!in_1.is_open());
    checkInputGood(in_1, "check10");
    
    in_2.close();
    REQUIRE(!in_2.is_open());
    checkInputGood(in_2, "check11");
}