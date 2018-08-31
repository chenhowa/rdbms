


#include "block_buffer.hpp"
#include "catch.hpp"
#include <cstring>
#include <string>
#include <cstdio> // for std::tmpnam
#include <stdio.h>

#include "string_input_stream.hpp"
#include "string_output_stream.hpp"

char* makeBuffer(int size, char value) {
    char* buffer = new char[size];
    memset(buffer, value, size);
    return buffer;
}

TEST_CASE("BlockBuffer: Read from RAM ... ") {
    fruit::Injector<BlockBufferFactory> injector(getTestingBlockBufferComponent);
    BlockBufferFactory bufferFactory(injector);
    
    int buf_size = 100;
    std::unique_ptr<BlockBuffer> buffer = bufferFactory(buf_size);
        
    SECTION("bytes with plenty of space") {
        int src_size = buf_size - 1;
        char* src = makeBuffer(src_size, 'b');
        int bytes_read = buffer->read(src_size, src);
        
        REQUIRE(bytes_read == src_size);
        REQUIRE(buffer->isCount(src_size));
        REQUIRE(buffer->bufferEquals(src_size, src));
    }
    
    SECTION("bytes with exact space of BlockBuffer") {
        int src_size = buf_size;
        char* src = makeBuffer(src_size, 'a');
        int bytes_read = buffer->read(src_size, src);

        REQUIRE(bytes_read == src_size);
        REQUIRE(buffer->isCount(src_size));
        REQUIRE(buffer->isFull());
        REQUIRE(buffer->bufferEquals(src_size, src));
    }
    
    SECTION("bytes - too many for BlockBuffer") {
        int src_size = buf_size + 1;
        char* src = makeBuffer(src_size, 'b');
        int bytes_read = buffer->read(src_size, src);
        char* expected = makeBuffer(buf_size, 'b');
        
        REQUIRE(buffer->isCount(buf_size));
        REQUIRE(buffer->isFull());
        REQUIRE(buffer->bufferEquals(buf_size, expected));
        REQUIRE(bytes_read == buf_size);
    }
}

TEST_CASE("BlockBuffer: Write to RAM ... ") {
    fruit::Injector<BlockBufferFactory> injector(getTestingBlockBufferComponent);
    BlockBufferFactory bufferFactory(injector);
    int buf_size = 100;
    std::unique_ptr<BlockBuffer> buffer = bufferFactory(buf_size);

    char* init_data = makeBuffer(buf_size, 'c');
    buffer->read(buf_size, init_data);
    
    SECTION("just a few bytes") {
        int num_bytes_to_write = buf_size - 1;
        int expected_size = buf_size - num_bytes_to_write;
        char* expected = makeBuffer(expected_size, 'c');
        char* dest = makeBuffer(buf_size, '\0');
        int bytes_written = buffer->write(num_bytes_to_write, dest);
        
        std::string written_expected(num_bytes_to_write, 'c');
        
        REQUIRE(bytes_written == num_bytes_to_write);
        REQUIRE(buffer->isCount(expected_size));
        REQUIRE(buffer->bufferEquals(expected_size, expected));
        REQUIRE(strcmp(dest, written_expected.c_str()) == 0);
    }
    
    SECTION("all bytes in buffer") {
        int num_bytes_to_write = buf_size;
        char* dest = makeBuffer(buf_size + 1, '\0');
        int bytes_written = buffer->write(num_bytes_to_write, dest);
        
        std::string written_expected(num_bytes_to_write, 'c');
        
        REQUIRE(buffer->isEmpty());
        REQUIRE(bytes_written == buf_size);
        REQUIRE(memcmp(init_data, dest, buf_size) == 0);
        REQUIRE(strcmp(dest, written_expected.c_str()) == 0);
    }
    
    SECTION("way too many bytes") {
        int num_bytes_to_write = buf_size + 5;
        char* dest = makeBuffer(num_bytes_to_write + 1, '\0');
        int bytes_written = buffer->write(num_bytes_to_write, dest);
        
        std::string written_expected(buf_size, 'c');
        
        REQUIRE(buffer->isEmpty());
        REQUIRE(bytes_written == buf_size);
        REQUIRE(memcmp(init_data, dest, buf_size) == 0);
        REQUIRE(strcmp(dest, written_expected.c_str()) == 0);
    }
}


TEST_CASE("BlockBuffer integration: Reading and Writing to RAM ... ") {
    // Create a half-full BlockBuffer
    fruit::Injector<BlockBufferFactory> injector(getTestingBlockBufferComponent);
    BlockBufferFactory bufferFactory(injector);
    
    int buf_size = 100;
    std::unique_ptr<BlockBuffer> buffer = bufferFactory(buf_size);

    int init_size = buf_size / 2;
    char* init_data = makeBuffer(init_size, 'a');
    buffer->read(init_size, init_data);
    
    SECTION("half-full => empty => full") {
        
        // write to empty
        char* dest = makeBuffer(buf_size + 1, '\0');
        int bytes_written = buffer->write(init_size, dest);
        REQUIRE(buffer->isEmpty());
        REQUIRE(buffer->isCount(0));
        REQUIRE(buffer->isStart(init_size));
        REQUIRE(buffer->isEnd(init_size));
        REQUIRE(bytes_written == init_size);
        REQUIRE(memcmp(init_data, dest, init_size) == 0);
        
        // read to full
        buffer->read(init_size, init_data);
        buffer->read(init_size, init_data);
        REQUIRE(buffer->isFull());
        REQUIRE(buffer->isCount(buf_size));
        REQUIRE(buffer->isStart(init_size));
        REQUIRE(buffer->isEnd(init_size));
        
        SECTION("check parent section's conditions") {
            REQUIRE(1 == 1);
        }
        
        SECTION(" parent section: => half-full") {
            //write to half-full
            buffer->write(init_size, dest);
            REQUIRE(!buffer->isFull());
            REQUIRE(!buffer->isEmpty());
            REQUIRE(buffer->isCount(buf_size / 2));
            REQUIRE(buffer->isStart(0));
            REQUIRE(buffer->isEnd(init_size));
        }
    }
}

TEST_CASE("BlockBuffer: Stream Reading ... ") {    
    // Create test file containing only the capital alphabet letters.
    int test_length = 26;
    std::string test_vals(test_length, 'A');
    fruit::Injector<IStringInputStream> stream_inj(getIStringInputStream);
    IStringInputStream* test_stream = stream_inj.get<IStringInputStream*>();
    (test_stream)->setContent(test_vals);
    
    SECTION("read with buffer smaller than stream... ") {
        fruit::Injector<BlockBufferFactory> injector(getTestingBlockBufferComponent);
        BlockBufferFactory bufferFactory(injector);
        int buf_size = test_length - 2;
        std::unique_ptr<BlockBuffer> buffer = bufferFactory(buf_size);
        
        REQUIRE(buffer->isEmpty());
        
        SECTION("empty => full") {
            int bytes_read = buffer->read(*test_stream);
            REQUIRE(buffer->isFull());
            REQUIRE(buffer->isCount(buf_size));
            REQUIRE(buffer->isStart(0));
            REQUIRE(buffer->isEnd(0));
            REQUIRE(!buffer->isEmpty());
            REQUIRE(bytes_read == buf_size);
            char* verif = makeBuffer(buf_size, 'A');
            REQUIRE(buffer->bufferEquals(buf_size, verif));
        }
        
        SECTION("half-full => full") {
            int src_size = buf_size / 2;
            char* src = makeBuffer(src_size, 'A');
            buffer->read(src_size, src);
            
            int bytes_read = buffer->read(*test_stream);
            
            REQUIRE(buffer->isFull());
            REQUIRE(buffer->isCount(buf_size));
            REQUIRE(buffer->isStart(0));
            REQUIRE(buffer->isEnd(0));
            REQUIRE(!buffer->isEmpty());
            REQUIRE(bytes_read == buf_size / 2);
            char* verif = makeBuffer(buf_size, 'A');
            REQUIRE(buffer->bufferEquals(buf_size, verif));
        }
    }
    
    SECTION("read with buffer larger than stream...") {
        fruit::Injector<BlockBufferFactory> injector(getTestingBlockBufferComponent);
        BlockBufferFactory bufferFactory(injector);
        int buf_size = test_length + 1;
        std::unique_ptr<BlockBuffer> buffer = bufferFactory(buf_size);
        
        REQUIRE(buffer->isEmpty());
        
        int bytes_read = buffer->read(*test_stream);
        REQUIRE(bytes_read == test_length);
        REQUIRE(!buffer->isFull());
        REQUIRE(buffer->isCount(test_length));
        REQUIRE(buffer->isStart(0));
        REQUIRE(buffer->isEnd(test_length));
        char* verif = makeBuffer(test_length, 'A');
        REQUIRE(buffer->bufferEquals(test_length, verif));
    }
    
    SECTION("read with buffer same size as stream...") {
        fruit::Injector<BlockBufferFactory> injector(getTestingBlockBufferComponent);
        BlockBufferFactory bufferFactory(injector);
        int buf_size = test_length;
        std::unique_ptr<BlockBuffer> buffer = bufferFactory(buf_size);
        
        REQUIRE(buffer->isEmpty());
        
        int bytes_read = buffer->read(*test_stream);
        REQUIRE(buffer->isFull());
        REQUIRE(buffer->isCount(buf_size));
        REQUIRE(buffer->isStart(0));
        REQUIRE(buffer->isEnd(0));
        REQUIRE(!buffer->isEmpty());
        REQUIRE(bytes_read == buf_size);
        char* verif = makeBuffer(buf_size, 'A');
        REQUIRE(buffer->bufferEquals(buf_size, verif));
    }
}


TEST_CASE("BlockBuffer: Stream Writing ... ") {
    // Create starting write stream with a starting number of "A" characters in it.
    
    int test_len = 5;
    std::string test_vals = std::string(test_len, 'A');
    fruit::Injector<IStringOutputStream> stream_inj(getIStringOutputStream);
    IStringOutputStream* test_stream = stream_inj.get<IStringOutputStream*>();
    (test_stream)->setContent(test_vals);
    REQUIRE((test_stream)->getContent() == test_vals);

    
    // Create a BlockBuffer to write things to the stream
    int buf_size = 11;
    
    fruit::Injector<BlockBufferFactory> injector(getTestingBlockBufferComponent);
    BlockBufferFactory bufferFactory(injector);
    std::unique_ptr<BlockBuffer> buffer = bufferFactory(buf_size);
    
    REQUIRE(buffer->isEmpty());
    
    SECTION("full => empty") {
        int src_size = buf_size;
        char* src = makeBuffer(src_size, 'A');
        buffer->read(src_size, src);
        
        REQUIRE(buffer->isFull());
        
        buffer->write(*test_stream);
        
        REQUIRE(buffer->isEmpty());
        REQUIRE(!buffer->isFull());
        REQUIRE(buffer->isCount(0));
        REQUIRE(buffer->isStart(0));
        REQUIRE(buffer->isEnd(0));
        
        std::string verify(test_len + src_size, 'A');
        REQUIRE(verify == (test_stream)->getContent() );
    }
    
    SECTION("half-full => empty") {
        int src_size = buf_size / 2;
        char *src = makeBuffer(src_size, 'A');
        buffer->read(src_size, src);
        
        REQUIRE(!buffer->isFull());
        REQUIRE(!buffer->isEmpty());
        REQUIRE(buffer->isCount(src_size));
        
        buffer->write(*test_stream);
        
        REQUIRE(buffer->isEmpty());
        REQUIRE(buffer->isCount(0));
        REQUIRE(buffer->isStart(src_size));
        REQUIRE(buffer->isEnd(src_size));
        
        std::string verify(test_len + src_size, 'A');
        REQUIRE(verify == (test_stream)->getContent());
    }
    
}

/*
TEST_CASE("BlockBuffer Integration: Reading and Writing to File ... ") {
    // Generate test files.
    
    // TODO : Add additional integration tests
    
    
    // Generate half-full buffer.
    
    
    SECTION("half-full => empty => full") {
        
        
        SECTION("...run parent by itself") {
            
        }
        
        SECTION("parent: => empty") {
            
        }
    }
}


TEST_CASE("BlockBuffer Integration: Reading and Writing to File AND Disk") {
    // Todo : Add additional integration tests.
    
    // Generate test files
    
    SECTION("WHAT TESTS TO RUN???") {
        
    }
}
*/