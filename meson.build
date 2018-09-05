
project('demo-dbms', 'cpp', default_options : ['cpp_std=c++11'])
# add_global_arguments('-lfruit', language: 'cpp')
incdir = include_directories('src/headers/', 'src/headers/imp',
                            'src/headers/interfaces')
path_boost_headers = '-I/usr/include/boost_1_67_0'
path_boost_links = '-L/usr/include/boost_1_67_0/stage/lib'
boost_system_arg = '-lboost_system'
fruit_arg = '-lfruit'

comp_args = [path_boost_headers, fruit_arg]
linker_args = [path_boost_links, boost_system_arg, fruit_arg]
#imps = ['src/imp/file_sort.cpp']
stream_root = 'src/imp/stream/'
fs_root = 'src/imp/filesystem/'
utils_src = [   stream_root + 'file_input_stream.cpp',
                stream_root + 'string_input_stream.cpp',
                stream_root + 'file_output_stream.cpp',
                stream_root + 'string_output_stream.cpp',
                stream_root + 'mock_file_input_stream.cpp',
                stream_root + 'mock_file_output_stream.cpp',
                fs_root + 'filesystem.cpp',
                'src/imp/globals/g_filesystem.cpp'
            ]
# Build Executable for file system test.
fs_imps = [fs_root + 'filesystem.cpp']
fs_mocks = []
fs_src = fs_imps + fs_mocks + ['src/main.filesystem.test.cpp'] + utils_src
fs_test = executable('fs.test', fs_src, include_directories : incdir,
                            cpp_args : '-lfruit', link_args : '-lfruit')
test('fs', fs_test)


#Build Executable for mock file input stream test.

# Build Executable for mock file output stream test.
mock_file_stream_imps = [stream_root + 'mock_file_output_stream.cpp',
                         stream_root + 'mock_file_input_stream.cpp']
mock_file_stream_mocks = [fs_root + 'filesystem.cpp' ]
mock_file_stream_src = mock_file_stream_imps + mock_file_stream_mocks + ['src/main.mock_file_stream.test.cpp' ] + utils_src
mock_file_stream_test = executable('mock_file_streams.test', mock_file_stream_src,
                            include_directories : incdir,
                            cpp_args : '-lfruit', link_args : '-lfruit')
test('mock_file_streams', mock_file_stream_test)

# Build executable for BlockBuffer unit tests
buffer_root = 'src/imp/buffer/'
buffer_imps = [ buffer_root + 'block_buffer.cpp' ]
buffer_mocks = []
buffer_src = buffer_imps + buffer_mocks + ['src/main.buffer.test.cpp'] + utils_src
buffer_test = executable('buffer.test', buffer_src, include_directories : incdir,
                cpp_args : '-lfruit', link_args : '-lfruit')
test('blockbuffer', buffer_test)

# Build executable for testing Hasher unit tests
hash_root = 'src/imp/hasher/'
hash_imps = [   hash_root + 'file_hash.cpp',
                buffer_root + 'block_buffer.cpp'
            ]
hash_mocks = [ buffer_root + 'mock_block_buffer.cpp']
hash_src = hash_imps + hash_mocks + ['src/main.file_hash.test.cpp'] + utils_src
hash_test = executable('file_hash.test', hash_src,
                                   include_directories : incdir, 
                                   cpp_args : '-lfruit', link_args : '-lfruit')
test('hasher', hash_test)

# Build executable for testing Sorter unit tests
sort_root = 'src/imp/sort/'
sort_imps = [   sort_root + 'sorter.cpp',
                buffer_root + 'block_buffer.cpp'
            ]
sort_mocks = [  buffer_root + 'mock_block_buffer.cpp'
            ]
sort_src = sort_imps + sort_mocks + ['src/main.sort.test.cpp'] + utils_src
sort_test = executable('sorter.test', sort_src,
                            include_directories: incdir,
                            cpp_args : comp_args, link_args : linker_args)
test('sorter', sort_test)

# Build executable for PRODUCT
# imps = buffer_imps + file_hash_imps
# src = imps + ['src/main.cpp']
# ex = executable('dbms.ex', src, include_directories : incdir)

