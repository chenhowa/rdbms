#include "catch.hpp"
#include "file_hash.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


TEST_CASE("file hasher initializes correctly with custom"
            "output number and blocksize") {
    int blocksize = 20;
    int num_outputs = 5;
    FileHasher hasher(num_outputs, blocksize);
    
    REQUIRE(hasher.getBlockSize() == blocksize);
    REQUIRE(hasher.numOutputBuffers() == num_outputs);
}