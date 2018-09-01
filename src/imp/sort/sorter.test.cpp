#include "catch.hpp"
#include "sorter.hpp"
#include "mock_block_buffer.hpp"
#include "string_output_stream.hpp"
#include "string_input_stream.hpp"

TEST_CASE("Sorter initializes correctly...") {
    fruit::Injector<IMockBlockBufferFactory> injector(getIMockBlockBufferFactory);
    IMockBlockBufferFactory bufferFac(injector);
    
    unsigned blocksize = 20;
    unsigned num_workers = 5;
    Sorter sorter(num_workers, blocksize, bufferFac);    
    REQUIRE(sorter.getBlockSize() == blocksize);
    REQUIRE(sorter.getNumWorkers() == num_workers);
}

