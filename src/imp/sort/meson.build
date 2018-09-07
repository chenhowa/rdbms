# Build a sorter library

srcs = [
    'sorter.cpp'
]
libs = [
    buffer_lib,
    stream_lib,
    file_manager_lib
]

inc = [
    inc_sort,
    inc_i_sort,
    inc_i_stream,
    inc_i_buffer,
    inc_buffer,
    inc_stream,
    inc_i_filesystem,
    inc_filesystem
]
sort_lib = library('sort', srcs,
                    link_with : libs,
                    include_directories : inc )
