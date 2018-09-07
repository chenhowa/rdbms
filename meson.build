# This is the overall project build
project('demo-dbms', 'cpp', default_options : ['cpp_std=c++11'])

add_project_arguments('-lfruit', '-I/usr/include/boost_1_67_0', '-Isrc/headers', 
                        '-DBOOST_LOG_DYN_LINK', language: 'cpp')
add_project_link_arguments(  '-L/usr/include/boost_1_67_0/stage/lib',
                            '-lboost_system', '-lboost_filesystem', 
                            '-lboost_log', '-lpthread',
                            '-lfruit', language: 'cpp')

subdir('src')


# Build Executable for file system test.
fs_src = 'src/main.filesystem.test.cpp'
fs_libs = [filesystem_lib]
fs_inc = [inc_testing, inc_filesystem, inc_i_filesystem]
fs_test = executable('fs.test', fs_src, link_with : fs_libs,
                        include_directories : fs_inc)
test('fs', fs_test)


# Build Executable for mock file stream tests.
mock_fs_src = 'src/main.mock_file_stream.test.cpp'
mock_fs_libs = [stream_lib, filesystem_lib]
mock_fs_inc = [
    inc_testing,
    inc_stream,
    inc_i_stream,
    inc_i_filesystem,
    inc_filesystem
]
mock_fs_test = executable('mock_file_streams.test', mock_fs_src,
                           link_with : mock_fs_libs,
                           include_directories : mock_fs_inc)


# Build executable for buffer tests
buffer_src = 'src/main.buffer.test.cpp'
buffer_libs = [
    buffer_lib,
    stream_lib
]
buffer_inc = [
    inc_testing,
    inc_buffer,
    inc_i_buffer,
    inc_i_stream,
    inc_stream
]
buffer_test = executable('buffer.test', buffer_src,
                        link_with : buffer_libs,
                        include_directories : buffer_inc)

# Build executable for testing Hasher unit tests
hash_src = 'src/main.file_hash.test.cpp'
hash_libs = [
    stream_lib,
    hash_lib,
    buffer_lib
]
hash_inc = [
    inc_testing,
    inc_hash,
    inc_i_buffer,
    inc_i_stream,
    inc_i_hash,
    inc_buffer,
    inc_stream
]
hash_test = executable('hash.test', hash_src,
                        include_directories : hash_inc,
                        link_with : hash_libs)


# Build executable for testing Sorter unit tests

sort_src = [ 'src/main.sort.test.cpp' ]
sort_libs = [
    stream_lib,
    g_filesystem_lib,
    sort_lib,
    buffer_lib,
    file_manager_lib
]
sort_inc = [
    inc_testing,
    inc_sort,
    inc_i_sort,
    inc_i_stream,
    inc_i_buffer,
    inc_i_filesystem,
    inc_buffer,
    inc_stream,
    inc_filesystem
]
sort_test = executable('sort.test', sort_src, 
                            include_directories: sort_inc,
                            link_with : sort_libs)


