#include "catch.hpp"
#include "sorter.hpp"
#include "mock_block_buffer.hpp"
#include "mock_file_input_stream.hpp"
#include "mock_file_output_stream.hpp"
#include "string_input_stream.hpp"
#include "string_output_stream.hpp"


/*
TEST_CASE("Sorter initializes correctly...") {
    
    unsigned blocksize = 20;
    unsigned num_workers = 5;
    std::unique_ptr<Sorter> sorter = sorterFac(num_workers, blocksize);    
    REQUIRE(sorter->getBlockSize() == blocksize);
    REQUIRE(sorter->getNumWorkers() == num_workers);
}*/


/*
TEST_CASE("Sorter, with small number of buffers, correctly sorts ... ") {
    fruit::Injector<IMockBlockBufferFactory> buf_inj(getIMockBlockBufferFactory);
    IMockBlockBufferFactory buf_fac(buf_inj);
    fruit::Injector<IMockFileInputStreamFactory> in_inj(getIMockFileInputStreamFactory);
    IMockFileInputStreamFactory in_fac(in_inj);
    fruit::Injector<IMockFileOutputStreamFactory> out_inj(getIMockFileOutputStreamFactory);
    IMockFileOutputStreamFactory out_fac(out_inj);
    fruit::Injector<IStringInputStream> in_str_inj(getIStringInputStream);
    fruit::Injector<IStringOutputStream> out_str_inj(getIStringOutputStream);
    
    unsigned bufferSize = 10;
    IMockBlockBuffer* stage = buf_fac(bufferSize).release();
    unsigned numWorkers = 2;
    std::vector<IBlockBuffer*> workers;
    for(unsigned i = 0; i < numWorkers; i++) {
        workers.push_back(buf_fac(bufferSize).release());
        IMockBlockBuffer* worker = dynamic_cast<IMockBlockBuffer*>(workers[i]);
        REQUIRE(worker->getBlockSize() == bufferSize);
        REQUIRE(worker->getBufferContents().length() == 0);
    }
    unsigned numReaders = numWorkers;
    std::vector<IFileInputStream*> readers;
    for(unsigned i = 0; i < numReaders; i++) {
        readers.push_back(in_fac().release());
    }
    unsigned numWriters = numWorkers;
    std::vector<IFileOutputStream*> writers;
    for(unsigned i = 0; i < numWriters; i++) {
        writers.push_back(out_fac().release());
    }
    
    Sorter sorter(stage, workers, readers, writers);
    
    REQUIRE(sorter.getBlockSize() == 0);
    REQUIRE(sorter.getNumWorkers() == numWorkers);
    
    
    SECTION("input that won't fill even the first buffer") {
        IStringInputStream* in_stream(in_str_inj);
        std::string input("DBCA");
        in_stream->setContent(input);
        REQUIRE(in_stream->getContent() == input);
        
        IStringOutputStream* out_stream(out_str_inj);
        REQUIRE(out_stream->getContent().empty());
        
        sorter.sort(*in_stream, *out_stream);
        
        for(unsigned i = 0; i < numWorkers; i++) {
            IMockBlockBuffer* worker = dynamic_cast<IMockBlockBuffer*>(workers[i]);
            REQUIRE(worker->getBufferContents().empty() );
        }
        /*
        for(unsigned i = 0; i < numReaders; i++) {
            IMockFileInputStream* reader = dynamic_cast<IMockFileInputStream*>(readers[i]);
            REQUIRE(reader->getContent().empty());
        }
        for(unsigned i = 0; i < numWriters; i++) {
            IMockFileOutputStream* writer = dynamic_cast<IMockFileOutputStream*>(writers[i]);
            REQUIRE(writer->getContent().empty());
        }
         * 
        REQUIRE(out_stream->getContent() == std::string("ABCD"));
    }
    
    SECTION("input that fills buffers exactly") {
        
    }
    
    SECTION("input that overfills buffers") {
        
    }
}
*/


TEST_CASE("Sorter, with arbitrarily large number of buffers, "
            "correctly sorts ... " ) {
                
    SECTION("input that won't fill buffers") {
        
    }
    
    SECTION("input that fills buffers exactly") {
        
    }
    
    SECTION("input that overfills buffers") {
        
    }
}