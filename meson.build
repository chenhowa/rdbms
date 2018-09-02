project('demo-dbms', 'cpp', default_options : ['cpp_std=c++11'])
#add_global_arguments('-lfruit', language: 'cpp')
incdir = include_directories('src/headers/', 'src/headers/imp', 'src/headers/interfaces')
#imps = ['src/imp/file_sort.cpp']
stream_root = 'src/imp/stream/'
utils_src = [   stream_root + 'file_input_stream.cpp',
                stream_root + 'string_input_stream.cpp',
                stream_root + 'file_output_stream.cpp',
                stream_root + 'string_output_stream.cpp',
                stream_root + 'mock_file_input_stream.cpp',
                stream_root + 'mock_file_output_stream.cpp'
            ]

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
sort_mocks = [ buffer_root + 'mock_block_buffer.cpp'] 
sort_src = sort_imps + sort_mocks + ['src/main.sort.test.cpp'] + utils_src
sort_test = executable('sorter.test', sort_src,
                            include_directories: incdir,
                            cpp_args : '-lfruit', link_args : '-lfruit')
test('sorter', sort_test)

# Build executable for PRODUCT
# imps = buffer_imps + file_hash_imps
# src = imps + ['src/main.cpp']
# ex = executable('dbms.ex', src, include_directories : incdir)

