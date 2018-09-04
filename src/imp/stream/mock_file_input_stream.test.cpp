


#include "catch.hpp"

#include "mock_file_input_stream.hpp"
#include "filesystem.hpp"


void checkInputGood(MockFileInputStream &in) {
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
    checkInputGood(in);
}