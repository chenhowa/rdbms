#include "catch.hpp"
#include "file_hash.hpp"
#include "i_block_buffer.hpp"
#include "block_buffer.hpp"

#include <fakeit.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <sstream>

using namespace fakeit;


fruit::Component<IBlockBufferFactory> getMockBlockBufferComponent() {
    static Mock<IBlockBuffer> mock;
    return fruit::createComponent()
        .registerFactory<std::unique_ptr<IBlockBuffer>(fruit::Assisted<int>)>(
            [](int blocksize) {
                return std::unique_ptr<IBlockBuffer>( &mock.get() );
            });
} 


fruit::Component<FileHasherFactory> getTestFileHasherComponent() {
    return fruit::createComponent()
        .replace(getBlockBufferComponent).with(getMockBlockBufferComponent)
        .install(_getFileHasherComponent);
}


TEST_CASE("file hasher initializes correctly with custom"
            "output number and blocksize") {
    fruit::Injector<FileHasherFactory> injector(getTestFileHasherComponent);
    FileHasherFactory fac(injector);
    
    int blocksize = 20;
    int num_outputs = 5;
    unique_ptr<FileHasher> hasher = fac(num_outputs, blocksize);
    
    REQUIRE(hasher->getBlockSize() == blocksize);
    REQUIRE(hasher->numOutputBuffers() == num_outputs);
}

/*

TEST_CASE("file hasher correctly hashes ... ") {
    
    Mock<IBlockBuffer> mock_in, mock_out_1, mock_out_2;
    Fake(Dtor(mock_in));
    When(OverloadedMethod(mock_in, read, int(std::istream&)))
        .Do((
            [](std::istream& in) -> int {
                in.get();
                return 5;
            }
        ));
    Fake(OverloadedMethod(mock_in, read, int(int, char*)));
    Fake(OverloadedMethod(mock_in, write, int(std::ostream&)));
    When(OverloadedMethod(mock_in, write, int(int, char*)))
        .Do(3_Times(
            [](int num, char* dest) -> int {
                *dest = 'A';
                return 1;
            }
        ))
        .Do(3_Times(
            [](int num, char* dest) -> int {
                *dest = 'B';
                return 1;
            }
        ))
        .Do(3_Times(
            [](int num, char* dest) -> int {
                *dest = 'C';
                return 1;
            }
        ))
        .Do(3_Times(
            [](int num, char* dest) -> int {
                *dest = 'D';
                return 1;
            }
        ));
    When(Method(mock_in, isEmpty)).Return(10_Times(true), false);
        
    Fake(Dtor(mock_out_1));
    Fake(OverloadedMethod(mock_out_1, read, int(std::istream&)));
    Fake(OverloadedMethod(mock_out_1, read, int(int, char*)));
    Fake(OverloadedMethod(mock_out_1, write, int(std::ostream&)));
    Fake(OverloadedMethod(mock_out_1, write, int(int, char*)));
    When(Method(mock_out_1, isFull))
        .Return(
            2_Times(false), true,
            2_Times(false), true
        );
    
    Fake(Dtor(mock_out_2));
    Fake(OverloadedMethod(mock_out_2, read, int(std::istream&)));
    Fake(OverloadedMethod(mock_out_2, read, int(int, char*)));
    Fake(OverloadedMethod(mock_out_2, write, int(std::ostream&)));
    Fake(OverloadedMethod(mock_out_2, write, int(int, char*)));
    When(Method(mock_out_2, isFull))
        .Return(
            2_Times(false), true,
            2_Times(false), true
        );
    
    FileHasher hasher(&mock_in.get(), &mock_out_1.get(), &mock_out_2.get());
    
    REQUIRE(hasher.getBlockSize() == 0);
    REQUIRE(hasher.numOutputBuffers() == 2);
    
    SECTION("simple file, simple function") {        
        std::vector<std::ostream*> dests;
        std::stringstream in, out_1, out_2;
        dests.push_back(&out_1);
        dests.push_back(&out_2);
        hasher.testHash(in, dests);
    }
    
    SECTION("simple file, complex function") {
        
    }
}

*/