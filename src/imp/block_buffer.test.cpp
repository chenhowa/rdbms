


#include "block_buffer.hpp"
#include "catch.hpp"
#include <cstring>
#include <string>
#include <cstdio> // for std::tmpnam
#include <stdio.h>

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
        int src_size = buf_size + 1;
        char* src = makeBuffer(src_size, 'b');
        int bytes_read = buffer.read(src_size, src);
        char* expected = makeBuffer(buf_size, 'b');
        
        REQUIRE(buffer.isCount(buf_size));
        REQUIRE(buffer.isFull());
        REQUIRE(buffer.bufferEquals(buf_size, expected));
        REQUIRE(bytes_read == buf_size);
    }
}

TEST_CASE("BlockBuffer: Write to RAM ... ") {
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    char* init_data = makeBuffer(buf_size, 'c');
    buffer.read(buf_size, init_data);
    
    SECTION("just a few bytes") {
        int num_bytes_to_write = buf_size - 1;
        int expected_size = buf_size - num_bytes_to_write;
        char* expected = makeBuffer(expected_size, 'c');
        char* dest = makeBuffer(buf_size, '\0');
        int bytes_written = buffer.write(num_bytes_to_write, dest);
        
        std::string written_expected(num_bytes_to_write, 'c');
        
        REQUIRE(bytes_written == num_bytes_to_write);
        REQUIRE(buffer.isCount(expected_size));
        REQUIRE(buffer.bufferEquals(expected_size, expected));
        REQUIRE(strcmp(dest, written_expected.c_str()) == 0);
    }
    
    SECTION("all bytes in buffer") {
        int num_bytes_to_write = buf_size;
        char* dest = makeBuffer(buf_size + 1, '\0');
        int bytes_written = buffer.write(num_bytes_to_write, dest);
        
        std::string written_expected(num_bytes_to_write, 'c');
        
        REQUIRE(buffer.isEmpty());
        REQUIRE(bytes_written == buf_size);
        REQUIRE(memcmp(init_data, dest, buf_size) == 0);
        REQUIRE(strcmp(dest, written_expected.c_str()) == 0);
    }
    
    SECTION("way too many bytes") {
        int num_bytes_to_write = buf_size + 5;
        char* dest = makeBuffer(num_bytes_to_write + 1, '\0');
        int bytes_written = buffer.write(num_bytes_to_write, dest);
        
        std::string written_expected(buf_size, 'c');
        
        REQUIRE(buffer.isEmpty());
        REQUIRE(bytes_written == buf_size);
        REQUIRE(memcmp(init_data, dest, buf_size) == 0);
        REQUIRE(strcmp(dest, written_expected.c_str()) == 0);
    }
}


TEST_CASE("BlockBuffer integration: Reading and Writing to RAM ... ") {
    // Create a half-full BlockBuffer
    int buf_size = 100;
    BlockBuffer buffer(buf_size);
    int init_size = buf_size / 2;
    char* init_data = makeBuffer(init_size, 'a');
    buffer.read(init_size, init_data);
    
    
    SECTION("half-full => empty => full") {
        
        // write to empty
        char* dest = makeBuffer(buf_size + 1, '\0');
        int bytes_written = buffer.write(init_size, dest);
        REQUIRE(buffer.isEmpty());
        REQUIRE(buffer.isCount(0));
        REQUIRE(buffer.isStart(init_size));
        REQUIRE(buffer.isEnd(init_size));
        REQUIRE(bytes_written == init_size);
        REQUIRE(memcmp(init_data, dest, init_size) == 0);
        
        // read to full
        buffer.read(init_size, init_data);
        buffer.read(init_size, init_data);
        REQUIRE(buffer.isFull());
        REQUIRE(buffer.isCount(buf_size));
        REQUIRE(buffer.isStart(init_size));
        REQUIRE(buffer.isEnd(init_size));
        
        SECTION("check parent section's conditions") {
            REQUIRE(1 == 1);
        }
        
        SECTION(" parent section: => half-full") {
            //write to half-full
            buffer.write(init_size, dest);
            REQUIRE(!buffer.isFull());
            REQUIRE(!buffer.isEmpty());
            REQUIRE(buffer.isCount(buf_size / 2));
            REQUIRE(buffer.isStart(0));
            REQUIRE(buffer.isEnd(init_size));
        }
    }
}

TEST_CASE("BlockBuffer: File Reading ... ") {
    // Create test file containing only the capital alphabet letters.
    char filename[] = "./test_files/test_file.txt";
    std::ofstream test_file(filename, std::ofstream::binary | std::ofstream::trunc);
    if(!test_file) {
        REQUIRE(1 == 2);
    }
    int ascii_A = 65;
    int alphabet_length = 27;
    int* test_nums = new int[alphabet_length];
    for(int i = 0; i < 27; i++) {
        test_nums[i] = ascii_A + i;
    }
    
    test_file.write((char*)test_nums, alphabet_length*sizeof(int));
    test_file.close();
    
    SECTION("read ... ") {
        int buf_size = 20;
        BlockBuffer buffer(buf_size);
        
        std::ifstream read_file(filename, std::ofstream::binary);
        if(!read_file) {
            perror("Problem opening file for read\n");
            REQUIRE(1 == 2);
        }
        
        REQUIRE(buffer.isEmpty());
        
        SECTION("empty => full") {
            int bytes_read = buffer.read(read_file);
            REQUIRE(buffer.isFull());
            REQUIRE(buffer.isCount(buf_size));
            REQUIRE(buffer.isStart(0));
            REQUIRE(buffer.isEnd(0));
            REQUIRE(!buffer.isEmpty());
            REQUIRE(bytes_read == buf_size);
        }
        
        SECTION("half-full => full") {
            int src_size = buf_size / 2;
            char* src = makeBuffer(src_size, 'a');
            buffer.read(src_size, src);
            
            int bytes_read = buffer.read(read_file);
            
            REQUIRE(buffer.isFull());
            REQUIRE(buffer.isCount(buf_size));
            REQUIRE(buffer.isStart(0));
            REQUIRE(buffer.isEnd(0));
            REQUIRE(!buffer.isEmpty());
            REQUIRE(bytes_read == buf_size / 2);
        }
        
        read_file.close();
    }
}


/*

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