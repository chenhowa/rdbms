#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "./imp/stream/mock_file_output_stream.test.cpp"
#include "./imp/stream/mock_file_input_stream.test.cpp"

TEST_CASE( "test") {
    REQUIRE( 1 == 1);
}
