


#include "block_buffer.hpp"
#include "catch.hpp"
#include <cstring>

char* makeBuffer(int size, char value) {
    char* buffer = new char[size];
    memset(buffer, value, size);
    return buffer;
}


TEST_CASE("BlockBuffer: Read from RAM ... ") {
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    
    SECTION("bytes with plenty of space") {
        int src_size = buf_size - 1;
        char* src = makeBuffer(src_size, 'b');
        int bytes_read = buffer.read(src_size, src);
        
        REQUIRE(bytes_read == src_size);
        REQUIRE(buffer.isCount(src_size));
        REQUIRE(buffer.bufferEquals(src_size, src));
    }
    
    SECTION("bytes with exact space of BlockBuffer") {
        int src_size = buf_size;
        char* src = makeBuffer(src_size, 'a');
        int bytes_read = buffer.read(src_size, src);

        REQUIRE(bytes_read == src_size);
        REQUIRE(buffer.isCount(src_size));
        REQUIRE(buffer.isFull());
        REQUIRE(buffer.bufferEquals(src_size, src));
    }
    
    SECTION("bytes - too many for BlockBuffer") {
        
    }
}

/*
TEST_CASE("BlockBuffer: read bytes with plenty of space") {
    int buf_size = 101;
    BlockBuffer buffer(buf_size);
    int src_size = 100;
    char* src = new char[src_size];
    memset(src, 'b', src_size);
    int bytes_read = buffer.read(src_size, src);
    
    REQUIRE(bytes_read == src_size);
    REQUIRE(buffer.isCount(src_size));
    REQUIRE(buffer.bufferEquals(src_size, src));
}*/

/*
TEST_CASE("BlockBuffer: read bytes with exact space") {
    int size = 100;
    BlockBuffer buffer(size);
    char* src = new char[size];
    memset(src, 'a', size);
    int bytes_read = buffer.read(size, src);

    REQUIRE(bytes_read == size);
    REQUIRE(buffer.isCount(size));
    REQUIRE(buffer.isFull());
    REQUIRE(buffer.bufferEquals(size, src));

}
*/

TEST_CASE("BlockBuffer: read bytes with insufficient space") {
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    
    int src_size = 101;
    char* src = new char[src_size];
    memset(src, 'b', src_size);
    int bytes_read = buffer.read(src_size, src);
    
    char* result = new char[buf_size];
    memcpy(result, src, buf_size);
    REQUIRE(buffer.isCount(buf_size));
    REQUIRE(buffer.isFull());
    REQUIRE(buffer.bufferEquals(buf_size, result));
    REQUIRE(bytes_read == buf_size);
}

TEST_CASE("BlockBuffer: write some bytes") {
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    int src_size = 100;
    char* src = new char[src_size];
    memset(src, 'c', src_size);
    buffer.read(src_size, src);
    
    int write_size = 99;
    int res_size = buf_size - write_size;
    char* result = new char[res_size];
    memset(result, 'c', res_size);
    int bytes_written = buffer.write(write_size, src);
    REQUIRE(bytes_written == write_size);
    REQUIRE(buffer.isCount(res_size));
    REQUIRE(buffer.bufferEquals(res_size, result));
}

TEST_CASE("BlockBuffer: write all bytes") {
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    char* src = new char[buf_size];
    memset(src, 'd', buf_size);
    buffer.read(buf_size, src);
    
    char* result = new char[buf_size];
    
    int bytes_written = buffer.write(buf_size, result);
    REQUIRE(buffer.isEmpty());
    REQUIRE(bytes_written == buf_size);
    REQUIRE(memcmp(src, result, buf_size) == 0);
}

TEST_CASE("BlockBuffer: write too many bytes") {
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    char* src = new char[buf_size];
    memset(src, 'g', 10);
    memset(src + 10, 'e', buf_size - 10);
    buffer.read(buf_size, src);
    
    int write_size = 101;
    char* result = new char[write_size];
    int bytes_written = buffer.write(write_size, result);
    REQUIRE(buffer.isEmpty());
    REQUIRE(bytes_written == buf_size);
    REQUIRE(memcmp(src, result, buf_size) == 0);
}

TEST_CASE("BlockBuffer: read half-full,"
            " write to empty, read to full") {
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    int src_size = buf_size / 2;
    char* src = new char[src_size];
    memset(src, 'a', src_size);
    buffer.read(src_size, src);
    
    char* result = new char[buf_size];
    int bytes_written = buffer.write(src_size, result);
    
    REQUIRE(buffer.isEmpty());
    REQUIRE(buffer.isCount(0));
    REQUIRE(buffer.isStart(src_size));
    REQUIRE(buffer.isEnd(src_size));
    REQUIRE(bytes_written == src_size);
    REQUIRE(memcmp(src, result, src_size) == 0);
    
    buffer.read(src_size, src);
    buffer.read(src_size, src);
    REQUIRE(buffer.isFull());
    REQUIRE(buffer.isCount(buf_size));
    REQUIRE(buffer.isStart(src_size));
    REQUIRE(buffer.isEnd(src_size));
    
}

/*
TEST_CASE("BlockBuffer: read half-full,"
            " write to empty, read to full,"
            " write to half-full") {
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    int src_size = buf_size / 2;
    char* src = new char[src_size];
    memset(src, 'a', src_size);
    buffer.read(src_size, src);
    
    char* result = new char[buf_size];
    int bytes_written = buffer.write(src_size, result);
    
    REQUIRE(buffer.isEmpty());
    REQUIRE(buffer.isCount(0));
    REQUIRE(buffer.isStart(src_size));
    REQUIRE(buffer.isEnd(src_size));
    REQUIRE(bytes_written == src_size);
    REQUIRE(memcmp(src, result, src_size) == 0);
    
    buffer.read(src_size, src);
    buffer.read(src_size, src);
    REQUIRE(buffer.isFull());
    REQUIRE(buffer.isCount(buf_size));
    REQUIRE(buffer.isStart(src_size));
    REQUIRE(buffer.isEnd(src_size));
    
    bytes_written = buffer.write(src_size, result);
    REQUIRE(!buffer.isFull());
    REQUIRE(!buffer.isEmpty());
    REQUIRE(buffer.isCount(buf_size / 2));
    REQUIRE(buffer.isStart(0));
    REQUIRE(buffer.isEnd(src_size));
}

TEST_CASE("BlockBuffer: empty buffer, read from full file") {
    // First, write the desired file.
    int fd = write_buffer_to_file(test_buf);
    
    //Read the desired file in and begin testing.
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    buffer.read(fd);
    
    REQUIRE(buffer.isFull());
    REQUIRE(buffer.isCount(buf_size));
    REQUIRE(!buffer.isEmpty());
    REQUIRE(buffer.isStart(0));
    REQUIRE(buffer.isEnd(0));
}

TEST_CASE("BlockBuffer: empty buffer, read from half-full file") {
    // First, write the desired file.
    int fd = write_buffer_to_file(test_buf);
    
    //Read the desired file in and begin testing.
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    
    REQUIRE(buffer.isCount(0));
    REQUIRE(buffer.isEmpty());
    
    buffer.read(fd);
    REQUIRE(!buffer.isFull());
    REQUIRE(buffer.isCount(buf_size / 2));
    REQUIRE(!buffer.isEmpty());
    REQUIRE(buffer.isStart(buf_size / 2));
    REQUIRE(buffer.isEnd(buf_size / 2));
    REQUIRE(buffer.bufferEquals(buf_size / 2, result))
}

TEST_CASE("BlockBuffer: full buffer, write to file") {
    // First, open file to write to.
    int fd = open_temp_file_for_write();
    
    // Initialize a full buffer.
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    char* src = new char[buf_size];
    memset(src, 'q', buf_size);
    buffer.read(buf_size, src);
    
    //Write a full buffer to file.
    buffer.write(fd);
    REQUIRE(buffer.isEmpty());
    REQUIRE(!buffer.isFull());
    REQUIRE(buffer.isCount(0));
    REQUIRE(buffer.isStart(0));
    REQUIRE(buffer.isEnd(0));
    
    // TODO: Check that file written is correct!
}

TEST_CASE("BlockBuffer: half-full buffer, write all to file") {
    // First, open file to write to.
    int fd = open_temp_file_for_write();
    
    // Get half-full buffer.
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    
    int src_size = buf_size / 2;
    char* src = new char[src_size];
    memset(src, 'b', src_size);
    buffer.read(src_size, src);
    
    REQUIRE(buffer.isCount(src_size));
    REQUIRE(buffer.bufferEquals(src_size, src));
    
    // Write the half-full buffer.
    buffer.write(fd);
    REQUIRE(buffer.isCount(0));
    REQUIRE(buffer.isEmpty());
    REQUIRE(buffer.isStart(src_size));
    REQUIRE(buffer.isEnd(src_size));
    
    // TODO: Check that file written is correct!
}
*/