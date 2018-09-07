# Builds a library containing all IBlockBuffer implementations


srcs = [
    'block_buffer.cpp',
    'mock_block_buffer.cpp'
]

inc = [
    inc_buffer,
    inc_i_buffer,
    inc_i_stream
]

libs = [

]


buffer_lib = library('buffers', srcs,
                        include_directories : inc,
                        link_with : libs)
