


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

bool verifyFilesAreSame(const char* filename1, const char* filename2, const char* message) {
    // printf("Testing: %s\n", message);
    std::ifstream input_1(filename1, std::ifstream::binary);
    std::ifstream input_2(filename2, std::ifstream::binary);
    
    if(!input_1) {
        perror("input 1 not opened correctly");
    }
    
    if(!input_2) {
        perror("input 2 not opened correctly");
    }
    
    char val_1;
    char val_2;
    
    while(input_1 || input_2) {
        val_1 = input_1.get();
        val_2 = input_2.get();
        /*
         * This is more thorough instrumentation to see the values of each file.
        printf("%i / ", val_1);
        printf("%i\n", val_2);
        */
        
        
        if(val_1 != val_2) {
            perror("values not equal\n");
            printf("val_1: %i val_2: %i\n", val_1, val_2);
            return false;
        }
        
        if( (input_1 && !input_2) || (!input_1 && input_2) ) {
            perror("streams not in same state");
            return false;
        }
    }
    
    return true;
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
    char filename[] = "./test_files/test_read_file.txt";
    std::ofstream test_file(filename, std::ofstream::binary | std::ofstream::trunc);
    if(!test_file) {
        REQUIRE(1 == 2);
    }
    int test_length = 26;
    char* test_chars = makeBuffer(test_length, 'A');
    
    test_file.write(test_chars, test_length);
    test_file.close();
    
    SECTION("read with buffer smaller than file... ") {
        int buf_size = test_length - 2;
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
    
    SECTION("read with buffer larger than file...") {
        int buf_size = test_length + 1;
        BlockBuffer buffer(buf_size);
        
        std::ifstream read_file(filename, std::ofstream::binary);
        if(!read_file) {
            perror("Problem opening file for read\n");
            REQUIRE(1 == 2);
        }
        
        REQUIRE(buffer.isEmpty());
        
        int bytes_read = buffer.read(read_file);
        REQUIRE(bytes_read == test_length);
        REQUIRE(!buffer.isFull());
        REQUIRE(buffer.isCount(test_length));
        REQUIRE(buffer.isStart(0));
        REQUIRE(buffer.isEnd(test_length));
        REQUIRE(buffer.bufferEquals(test_length, test_chars));
    }
    
    SECTION("read with buffer same size as file...") {
        int buf_size = test_length;
        BlockBuffer buffer(buf_size);
        
        std::ifstream read_file(filename, std::ofstream::binary);
        if(!read_file) {
            perror("Problem opening file for read\n");
            REQUIRE(1 == 2);
        }
        
        REQUIRE(buffer.isEmpty());
        
        int bytes_read = buffer.read(read_file);
        REQUIRE(buffer.isFull());
        REQUIRE(buffer.isCount(buf_size));
        REQUIRE(buffer.isStart(0));
        REQUIRE(buffer.isEnd(0));
        REQUIRE(!buffer.isEmpty());
        REQUIRE(bytes_read == buf_size);
    }
}


TEST_CASE("BlockBuffer: File Writing ... ") {
    // Create file with a starting number of "A" characters in it.
    char filename[] = "./test_files/test_write_file.txt";
    std::ofstream test_file(filename, std::ofstream::binary | std::ofstream::trunc);
    if(!test_file) {
        REQUIRE(1 == 2);
    }
    
    int test_len = 5;
    char* test_vals = makeBuffer(test_len, 'A');
    
    test_file.write((char*)test_vals, test_len);
    test_file.close();
    
    // Prepare to write to the file.
    std::ofstream write_file(filename, std::ofstream::binary | std::ofstream::ate | std::ofstream::in);
    if(!write_file) {
        perror("Problem opening file for write\n");
        REQUIRE(1 == 2);
    }
    
    int buf_size = 10;
    BlockBuffer buffer(buf_size);
    
    REQUIRE(buffer.isEmpty());
    
    SECTION("full => empty") {
        int src_size = buf_size;
        char* src = makeBuffer(src_size, 'B');
        buffer.read(src_size, src);
        
        REQUIRE(buffer.isFull());
        
        buffer.write(write_file);
        
        REQUIRE(buffer.isEmpty());
        REQUIRE(!buffer.isFull());
        REQUIRE(buffer.isCount(0));
        REQUIRE(buffer.isStart(0));
        REQUIRE(buffer.isEnd(0));
        
        write_file.close();
        
        REQUIRE(verifyFilesAreSame(filename, "./test_files/expected_full_write.txt", "full => empty"));        
    }
    
    SECTION("half-full => empty") {
        int src_size = buf_size / 2;
        char *src = makeBuffer(src_size, 'B');
        buffer.read(src_size, src);
        
        REQUIRE(!buffer.isFull());
        REQUIRE(!buffer.isEmpty());
        REQUIRE(buffer.isCount(src_size));
        
        buffer.write(write_file);
        
        REQUIRE(buffer.isEmpty());
        REQUIRE(buffer.isCount(0));
        REQUIRE(buffer.isStart(src_size));
        REQUIRE(buffer.isEnd(src_size));
        
        write_file.close();
        
        REQUIRE(verifyFilesAreSame(filename, "./test_files/expected_half_write.txt", "half-full => empty"));
    }
}