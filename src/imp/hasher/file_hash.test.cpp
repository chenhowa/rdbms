#include "catch.hpp"
#include "file_hash.hpp"
#include "mock_block_buffer.hpp"
#include "string_output_stream.hpp"
#include "string_input_stream.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <sstream>



TEST_CASE("file hasher initializes correctly with custom"
            "output number and blocksize") {
    fruit::Injector<IMockBlockBufferFactory> injector(getIMockBlockBufferFactory);
    IMockBlockBufferFactory bufferFac(injector);
    
    unsigned blocksize = 20;
    unsigned num_outputs = 5;
    FileHasher hasher(num_outputs, blocksize, bufferFac);    
    REQUIRE(hasher.getBlockSize() == blocksize);
    REQUIRE(hasher.numOutputBuffers() == num_outputs);
}

TEST_CASE("file hasher, with small number of buffers, correctly hashes ... ") {
    fruit::Injector<IMockBlockBufferFactory> injector(getIMockBlockBufferFactory);
    IMockBlockBufferFactory bufferFac(injector);
    
    // Create buffers
    unsigned blocksize = 10;
    IMockBlockBuffer* in_buf = bufferFac(blocksize).release();
    IMockBlockBuffer* out_buf_1 = bufferFac(blocksize).release();
    IMockBlockBuffer* out_buf_2 = bufferFac(blocksize).release();
    REQUIRE(in_buf->isEmpty());
    REQUIRE(out_buf_1->isEmpty());
    REQUIRE(out_buf_2->isEmpty());
    
    // Create hasher
    FileHasher hasher(in_buf, out_buf_1, out_buf_2);
    REQUIRE(hasher.getBlockSize() == 0);
    REQUIRE(hasher.numOutputBuffers() == 2);
    
    SECTION("input smaller than total output blocksize... ") {   
        fruit::Injector<IStringOutputStream> output_injector_1(getIStringOutputStream);
        fruit::Injector<IStringOutputStream> output_injector_2(getIStringOutputStream);
        fruit::Injector<IStringInputStream> input_injector(getIStringInputStream);
        
        std::vector<IOutputStream*> dests;
        IStringInputStream *in_stream(input_injector);
        std::string input("AABBCCDD");
        
        REQUIRE(input.length() < blocksize);
        
        in_stream->setContent(input);
        REQUIRE(in_stream->getContent() == input);
        
        IStringOutputStream *out_stream_1(output_injector_1), *out_stream_2(output_injector_2);
        REQUIRE(out_stream_1 != out_stream_2);
        dests.push_back(out_stream_1);
        dests.push_back(out_stream_2);
        hasher.testHash(*in_stream, dests);
        
        /*
        printf("in_stream_content: %s\n", in_stream->getContent().c_str());
        printf("out_stream_1 content: %s\n", out_stream_1->getContent().c_str());
        printf("out_stream_2 content: %s\n", out_stream_2->getContent().c_str());
        printf("in_buf_content: %s\n", in_buf->getBufferContents().c_str());
        printf("out_buf_1 content: %s\n", out_buf_1->getBufferContents().c_str());
        printf("out_buf_2 content: %s\n", out_buf_2->getBufferContents().c_str());
        */

        
        REQUIRE(in_buf->getBufferContents().empty());
        REQUIRE(in_buf->getCharsRead() == std::string("AABBCCDD"));
        REQUIRE(in_buf->getCharsWritten() == std::string("AABBCCDD"));
        
        REQUIRE(out_buf_1->getBufferContents().empty());
        REQUIRE(out_buf_1->getCharsRead() == std::string("BBDD"));
        REQUIRE(out_buf_1->getCharsWritten() == std::string("BBDD"));
        
        REQUIRE(out_buf_2->getBufferContents().empty());
        REQUIRE(out_buf_2->getCharsRead() == std::string("AACC"));
        REQUIRE(out_buf_2->getCharsWritten() == std::string("AACC"));
        
        REQUIRE(! (*in_stream) );
        REQUIRE(!in_stream->good());
        REQUIRE(in_stream->eof());

        REQUIRE(out_stream_1->getContent() == std::string("BBDD"));
        REQUIRE(out_stream_2->getContent() == std::string("AACC"));
        
        
    }
    
    
    SECTION("input same size as output buffer size") {
        fruit::Injector<IStringOutputStream> output_injector_1(getIStringOutputStream);
        fruit::Injector<IStringOutputStream> output_injector_2(getIStringOutputStream);
        fruit::Injector<IStringInputStream> input_injector(getIStringInputStream);
        
        std::vector<IOutputStream*> dests;
        IStringInputStream *in_stream(input_injector);
        std::string input = std::string(blocksize, 'A')
                            + std::string(blocksize, 'B')
                            + std::string(blocksize, 'C')
                            + std::string(blocksize, 'D');
        
        REQUIRE(input.length() == 4 * blocksize);
        
        in_stream->setContent(input);
        REQUIRE(in_stream->getContent() == input);
        
        IStringOutputStream *out_stream_1(output_injector_1), *out_stream_2(output_injector_2);
        REQUIRE(out_stream_1 != out_stream_2);
        dests.push_back(out_stream_1);
        dests.push_back(out_stream_2);
        hasher.testHash(*in_stream, dests);
        
        /*
        printf("in_stream_content: %s\n", in_stream->getContent().c_str());
        printf("out_stream_1 content: %s\n", out_stream_1->getContent().c_str());
        printf("out_stream_2 content: %s\n", out_stream_2->getContent().c_str());
        printf("in_buf_content: %s\n", in_buf->getBufferContents().c_str());
        printf("out_buf_1 content: %s\n", out_buf_1->getBufferContents().c_str());
        printf("out_buf_2 content: %s\n", out_buf_2->getBufferContents().c_str());
        */
        
        REQUIRE(in_buf->getBufferContents().empty());
        REQUIRE(in_buf->getCharsRead() == input );
        REQUIRE(in_buf->getCharsWritten() == input );
        
        REQUIRE(out_buf_1->getBufferContents().empty());
        std::string res_1 = std::string(blocksize, 'B') + std::string(blocksize, 'D');
        REQUIRE(out_buf_1->getCharsRead() == res_1);
        REQUIRE(out_buf_1->getCharsWritten() == res_1);
        
        REQUIRE(out_buf_2->getBufferContents().empty());
        std::string res_2 = std::string(blocksize, 'A') + std::string(blocksize, 'C');
        REQUIRE(out_buf_2->getCharsRead() == res_2);
        REQUIRE(out_buf_2->getCharsWritten() == res_2);
        
        REQUIRE(! (*in_stream) );
        REQUIRE(!in_stream->good());
        REQUIRE(in_stream->eof());

        REQUIRE(out_stream_1->getContent() == res_1);
        REQUIRE(out_stream_2->getContent() == res_2);
    }
    
    SECTION("input larger than buffer size") {
        fruit::Injector<IStringOutputStream> output_injector_1(getIStringOutputStream);
        fruit::Injector<IStringOutputStream> output_injector_2(getIStringOutputStream);
        fruit::Injector<IStringInputStream> input_injector(getIStringInputStream);
        
        std::vector<IOutputStream*> dests;
        IStringInputStream *in_stream(input_injector);
        unsigned input_size = 2 * blocksize;
        std::string input = std::string(input_size, 'A')
                            + std::string(input_size, 'B')
                            + std::string(input_size, 'C')
                            + std::string(input_size, 'D');
        REQUIRE(input.length() == 8 * blocksize);
        
        in_stream->setContent(input);
        REQUIRE(in_stream->getContent() == input);
    
        
        IStringOutputStream *out_stream_1(output_injector_1), *out_stream_2(output_injector_2);
        REQUIRE(out_stream_1 != out_stream_2);
        dests.push_back(out_stream_1);
        dests.push_back(out_stream_2);
        hasher.testHash(*in_stream, dests);
        
        /*
        printf("in_stream_content: %s\n", in_stream->getContent().c_str());
        printf("out_stream_1 content: %s\n", out_stream_1->getContent().c_str());
        printf("out_stream_2 content: %s\n", out_stream_2->getContent().c_str());
        printf("in_buf_content: %s\n", in_buf->getBufferContents().c_str());
        printf("out_buf_1 content: %s\n", out_buf_1->getBufferContents().c_str());
        printf("out_buf_2 content: %s\n", out_buf_2->getBufferContents().c_str());
        */
        
        REQUIRE(in_buf->getBufferContents().empty());
        REQUIRE(in_buf->getCharsRead() == input );
        REQUIRE(in_buf->getCharsWritten() == input );
        
        REQUIRE(out_buf_1->getBufferContents().empty());
        std::string res_1 = std::string(input_size, 'B') + std::string(input_size, 'D');
        REQUIRE(out_buf_1->getCharsRead() == res_1);
        REQUIRE(out_buf_1->getCharsWritten() == res_1);
        
        REQUIRE(out_buf_2->getBufferContents().empty());
        std::string res_2 = std::string(input_size, 'A') + std::string(input_size, 'C');
        REQUIRE(out_buf_2->getCharsRead() == res_2);
        REQUIRE(out_buf_2->getCharsWritten() == res_2);
        
        REQUIRE(! (*in_stream) );
        REQUIRE(!in_stream->good());
        REQUIRE(in_stream->eof());

        REQUIRE(out_stream_1->getContent() == res_1);
        REQUIRE(out_stream_2->getContent() == res_2);
    }
}

TEST_CASE("file hasher, with arbitrarily large number of buffers, "
            "correctly hashes ... ") {
    // TODO : Use the second constructor for this one.
    fruit::Injector<IMockBlockBufferFactory> injector(getIMockBlockBufferFactory);
    IMockBlockBufferFactory bufferFac(injector);
    
    // Initialize the hasher
    unsigned blocksize = 10;
    unsigned num_outputs = 10;
    FileHasher hasher(num_outputs, blocksize, bufferFac);
    
    // Initialize the streams
    fruit::Injector<IStringOutputStream> output_injector_1(getIStringOutputStream);
    fruit::Injector<IStringOutputStream> output_injector_2(getIStringOutputStream);
    fruit::Injector<IStringInputStream> input_injector(getIStringInputStream);
    
    std::vector<IOutputStream*> dests;
    IStringInputStream *in_stream(input_injector);
    unsigned input_size = blocksize;
    std::string input = std::string(input_size, 'A')
                        + std::string(input_size, 'B')
                        + std::string(input_size, 'C')
                        + std::string(input_size, 'D');
    REQUIRE(input.length() == 4 * blocksize);
    
    in_stream->setContent(input);
    REQUIRE(in_stream->getContent() == input);

    
    IStringOutputStream *out_stream_1(output_injector_1), *out_stream_2(output_injector_2);
    REQUIRE(out_stream_1 != out_stream_2);
    dests.push_back(out_stream_1);
    dests.push_back(out_stream_2);
    hasher.testHash(*in_stream, dests);
    
    /*
    printf("in_stream_content: %s\n", in_stream->getContent().c_str());
    printf("out_stream_1 content: %s\n", out_stream_1->getContent().c_str());
    printf("out_stream_2 content: %s\n", out_stream_2->getContent().c_str());
    */
    
    REQUIRE(! (*in_stream) );
    REQUIRE(!in_stream->good());
    REQUIRE(in_stream->eof());

    std::string res_1 = std::string(blocksize, 'B') + std::string(blocksize, 'D');
    REQUIRE(out_stream_1->getContent() == res_1);
    
    std::string res_2 = std::string(blocksize, 'A') + std::string(blocksize, 'C');
    REQUIRE(out_stream_2->getContent() == res_2);

}