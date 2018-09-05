#include "catch.hpp"
#include "sorter.hpp"
#include "mock_block_buffer.hpp"
#include "mock_file_input_stream.hpp"
#include "mock_file_output_stream.hpp"
#include "string_input_stream.hpp"
#include "string_output_stream.hpp"
#include "filesystem.hpp"

using namespace fruit;

extern IFileSystem *g_filesystem;

TEST_CASE("Testing initialization ... ") {    
    Injector<IMockFileOutputStreamFactory> out_inj
            (getIMockFileOutputStreamFactory);
    IMockFileOutputStreamFactory out_fac(out_inj);
            
    Injector<IMockFileInputStreamFactory> in_inj
            (getIMockFileInputStreamFactory);
    IMockFileInputStreamFactory in_fac(in_inj);
    
    Injector<IMockBlockBufferFactory> buf_inj(getIMockBlockBufferFactory);
    IMockBlockBufferFactory buf_fac(buf_inj);
    
    // reset file system.
    g_filesystem->reset();
    
    SECTION("check file system") {
        std::string name("default");
        g_filesystem->createFile(name);
        g_filesystem->set(name, "hello");
        REQUIRE(g_filesystem->fileCount() == 1);
        REQUIRE(g_filesystem->fileExists(name));
        REQUIRE(g_filesystem->get(name) == std::string("hello"));
        
        std::unique_ptr<IMockFileInputStream> input = in_fac();
        std::unique_ptr<IMockFileOutputStream> output = out_fac();
        REQUIRE(input->getFileSystem() == output->getFileSystem());
        REQUIRE(input->getFileSystem() == g_filesystem);
        input->open(name, std::ios_base::in);
        CHECK(input->good());
        CHECK(!input->fail());
        REQUIRE(input->is_open() == true);
        
        REQUIRE(input->get() == 'h');
        REQUIRE(input->get() == 'e');
        input->close();
        
        output->open(name, std::ios_base::out);
        REQUIRE(output->is_open() == true);
        output->put('o');
        output->close();
        REQUIRE(g_filesystem->get(name) == std::string("oello"));
        
        input->open(name, std::ios_base::in);
        REQUIRE(input->get() == 'o');
        REQUIRE(input->get() == 'e');        
    }
    
    SECTION("check sorter initialization") {
        unsigned num_bufs = 5;
        unsigned blocksize = 10;
        Sorter sorter(num_bufs, blocksize, buf_fac, in_fac, out_fac);
        
        REQUIRE(sorter.getNumWorkers() == num_bufs);
        REQUIRE(sorter.getBlockSize() == blocksize); 
    }
}

TEST_CASE("Sorter, with small number of buffers, correctly sorts ... ") {
    g_filesystem->reset();
    
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
        IMockFileInputStream *in = dynamic_cast<IMockFileInputStream*>(readers[i]);
        REQUIRE(in->good());
        REQUIRE(!in->is_open());
    }

    unsigned numWriters = numWorkers;
    std::vector<IFileOutputStream*> writers;
    for(unsigned i = 0; i < numWriters; i++) {
        writers.push_back(out_fac().release());
        IMockFileOutputStream *out = dynamic_cast<IMockFileOutputStream*>(writers[i]);
        REQUIRE(out->good());
        REQUIRE(!out->is_open());
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
        
        REQUIRE(stage->getBufferContents().empty());
        
        for(unsigned i = 0; i < numWorkers; i++) {
            IMockBlockBuffer* worker = dynamic_cast<IMockBlockBuffer*>(workers[i]);
            CHECK(worker->getBufferContents().empty() );
        }
        
        for(unsigned i = 0; i < numReaders; i++) {
            IMockFileInputStream* reader = dynamic_cast<IMockFileInputStream*>(readers[i]);
            CHECK_THROWS(reader->getContent().empty());
            CHECK(!reader->is_open());
        }
        for(unsigned i = 0; i < numWriters; i++) {
            IMockFileOutputStream* writer = dynamic_cast<IMockFileOutputStream*>(writers[i]);
            CHECK_THROWS(writer->getContent());
            CHECK(!writer->is_open());
        }
        
        REQUIRE(out_stream->getContent() == std::string("ABCD"));
        
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