#include "catch.hpp"
#include "file_hash.hpp"


TEST_CASE("file hash") {
    FileHasher hasher;
    REQUIRE(hasher.getBlockSize() == 1);
}