# builds a library that contains all stream implementations

srcs = [
    'file_input_stream.cpp',
    'file_output_stream.cpp',
    'mock_file_input_stream.cpp',
    'mock_file_output_stream.cpp',
    'string_input_stream.cpp',
    'string_output_stream.cpp'
]

inc = [
    inc_stream,
    inc_i_stream,
    inc_filesystem,
    inc_i_filesystem
]

libs = [
    g_filesystem_lib
]

stream_lib = library('streams', srcs,
                include_directories : inc,
                link_with : libs)
