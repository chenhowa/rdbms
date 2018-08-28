#include "catch.hpp"
#include "file_hash.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <string>

bool verifyFilesAreSame(const std::string filename1, const std::string filename2, const std::string message) {
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



TEST_CASE("file hasher initializes correctly with custom"
            "output number and blocksize") {
    int blocksize = 20;
    int num_outputs = 5;
    FileHasher hasher(num_outputs, blocksize);
    
    REQUIRE(hasher.getBlockSize() == blocksize);
    REQUIRE(hasher.numOutputBuffers() == num_outputs);
}

TEST_CASE("file hasher correctly hashes ... ") {
    
    int blocksize = 20;
    int num_outputs = 5;
    FileHasher hasher(num_outputs, blocksize);
    
    REQUIRE(hasher.getBlockSize() == blocksize);
    REQUIRE(hasher.numOutputBuffers() == num_outputs);
    
    SECTION("simple file, simple function") {
        std::string root = "./test_files/hash/";
        
        std::vector<string> outputs({
            root + "test_simple_1.txt",
            root + "test_simple_2.txt",
            root + "test_simple_3.txt",
            root + "test_simple_4.txt",
            root + "test_simple_5.txt"
        });
        hasher.testHash(root + "simple_vals.txt", outputs);
        
        REQUIRE( verifyFilesAreSame(outputs[0], root + "simple_sol_1.txt", "TEST 1") );      
        REQUIRE( verifyFilesAreSame(outputs[1], root + "simple_sol_2.txt", "TEST 2") );
        REQUIRE( verifyFilesAreSame(outputs[2], root + "simple_sol_3.txt", "TEST 3") );
        REQUIRE( verifyFilesAreSame(outputs[3], root + "simple_sol_4.txt", "TEST 4") );
        REQUIRE( verifyFilesAreSame(outputs[4], root + "simple_sol_5.txt", "TEST 5") );
    }
    
    SECTION("simple file, complex function") {
        
    }
}