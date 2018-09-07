# Build a library for hashers

srcs = [
    'file_hash.cpp'
]

inc = [
    inc_hash,
    inc_i_hash,
    inc_i_buffer,
    inc_buffer,
    inc_stream,
    inc_i_stream
]

libs = [
    buffer_lib
]

hash_lib = library('hash', srcs,
                    include_directories : inc,
                    link_with : libs)
