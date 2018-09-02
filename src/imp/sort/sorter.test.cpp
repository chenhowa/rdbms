#include "catch.hpp"
#include "sorter.hpp"
#include "mock_block_buffer.hpp"
#include "mock_file_input_stream.hpp"
#include "mock_file_output_stream.hpp"
#include "string_output_stream.hpp"
#include "string_input_stream.hpp"

using SorterFactory = std::function<std::unique_ptr<Sorter>(unsigned, unsigned) >;
fruit::Component<SorterFactory> getSorterFactory() {
    return fruit::createComponent()
        .install(getIMockBlockBufferFactory)
        .install(getIMockFileInputStreamFactory)
        .install(getIMockFileOutputStreamFactory)
        .registerFactory<std::unique_ptr<Sorter>(
                fruit::Assisted<unsigned>, 
                fruit::Assisted<unsigned>, 
                IMockBlockBufferFactory,
                IMockFileInputStreamFactory,
                IMockFileOutputStreamFactory)> (
            [](unsigned num_workers, unsigned blocksize, IMockBlockBufferFactory fac,
                        IMockFileInputStreamFactory in, IMockFileOutputStreamFactory out) {
                return std::unique_ptr<Sorter>(
                    new Sorter(num_workers, blocksize, fac, in, out)
                );
            });
}

TEST_CASE("Sorter initializes correctly...") {
    fruit::Injector<SorterFactory> injector(getSorterFactory);
    SorterFactory sorterFac(injector);
    
    unsigned blocksize = 20;
    unsigned num_workers = 5;
    std::unique_ptr<Sorter> sorter = sorterFac(num_workers, blocksize);    
    REQUIRE(sorter->getBlockSize() == blocksize);
    REQUIRE(sorter->getNumWorkers() == num_workers);
}


TEST_CASE("Sorter, with small number of buffers, correctly sorts ... ") {
    
    
    
    
    SECTION("input that won't fill buffers") {
        
    }
    
    SECTION("input that fills buffers exactly") {
        
    }
    
    SECTION("input that overfills buffers") {
        
    }
}


TEST_CASE("Sorter, with arbitrarily large number of buffers, "
            "correctly sorts ... " ) {
                
    SECTION("input that won't fill buffers") {
        
    }
    
    SECTION("input that fills buffers exactly") {
        
    }
    
    SECTION("input that overfills buffers") {
        
    }
}