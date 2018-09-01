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
    
    SECTION("input smaller than blocksize... ") {   
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
    
    
    SECTION("input larger than buffer size") {
        fruit::Injector<IStringOutputStream> output_injector_1(getIStringOutputStream);
        fruit::Injector<IStringOutputStream> output_injector_2(getIStringOutputStream);
        fruit::Injector<IStringInputStream> input_injector(getIStringInputStream);
        
        std::vector<IOutputStream*> dests;
        IStringInputStream *in_stream(input_injector);
        std::string input("ABCDABCDABCDABCD");
        
        REQUIRE(input.length() > blocksize);
        
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
        REQUIRE(out_buf_1->getCharsRead() == std::string("BDBDBDBD"));
        REQUIRE(out_buf_1->getCharsWritten() == std::string("BDBDBDBD"));
        
        REQUIRE(out_buf_2->getBufferContents().empty());
        REQUIRE(out_buf_2->getCharsRead() == std::string("ACACACAC"));
        REQUIRE(out_buf_2->getCharsWritten() == std::string("ACACACAC"));
        
        REQUIRE(! (*in_stream) );
        REQUIRE(!in_stream->good());
        REQUIRE(in_stream->eof());

        REQUIRE(out_stream_1->getContent() == std::string("BDBDBDBD"));
        REQUIRE(out_stream_2->getContent() == std::string("ACACACAC"));
    }
    
    SECTION("input the same size as buffer size") {
        fruit::Injector<IStringOutputStream> output_injector_1(getIStringOutputStream);
        fruit::Injector<IStringOutputStream> output_injector_2(getIStringOutputStream);
        fruit::Injector<IStringInputStream> input_injector(getIStringInputStream);
        
        std::vector<IOutputStream*> dests;
        IStringInputStream *in_stream(input_injector);
        std::string input("ABCDABCDAB");
        REQUIRE(input.length() == blocksize);
        
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
        REQUIRE(out_buf_1->getCharsRead() == std::string("BDBDB"));
        REQUIRE(out_buf_1->getCharsWritten() == std::string("BDBDB"));
        
        REQUIRE(out_buf_2->getBufferContents().empty());
        REQUIRE(out_buf_2->getCharsRead() == std::string("ACACA"));
        REQUIRE(out_buf_2->getCharsWritten() == std::string("ACACA"));
        
        REQUIRE(! (*in_stream) );
        REQUIRE(!in_stream->good());
        REQUIRE(in_stream->eof());

        REQUIRE(out_stream_1->getContent() == std::string("BDBDB"));
        REQUIRE(out_stream_2->getContent() == std::string("ACACA"));
    }
}

TEST_CASE("file hasher, with arbitrarily large number of buffers, "
            "correctly hashes ... ") {
    // TODO : Use the second constructor for this one.

}