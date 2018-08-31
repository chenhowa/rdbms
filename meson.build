project('demo-dbms', 'cpp', default_options : ['cpp_std=c++11'])
#add_global_arguments('-lfruit', language: 'cpp')
incdir = include_directories('src/headers/', 'src/headers/imp', 'src/headers/interfaces')
#imps = ['src/imp/file_sort.cpp']
stream_root = 'src/imp/stream/'
utils_src = [   stream_root + 'file_input_stream.cpp',
                stream_root + 'string_input_stream.cpp',
                stream_root + 'file_output_stream.cpp',
                stream_root + 'string_output_stream.cpp'
            ]

# Build executable for BlockBuffer unit tests
buffer_root = 'src/imp/buffer/'
buffer_imps = [ buffer_root + 'block_buffer.cpp' ]
buffer_mocks = []
buffer_src = buffer_imps + buffer_mocks + ['src/main.buffer.test.cpp'] + utils_src
buffer_test = executable('buffer.test', buffer_src, include_directories : incdir,
                cpp_args : '-lfruit', link_args : '-lfruit')
test('blockbuffer', buffer_test)

# Build executable for testing FileHasher unit tests
# file_hash_imps = ['src/imp/file_hash.cpp', 'src/imp/block_buffer.cpp']
# test_mocks = []
# test_file_hash_src = file_hash_imps + ['src/main.file_hash.test.cpp']
# test_file_hash = executable('file_hash.test', test_file_hash_src,
#                                    include_directories : incdir, 
#                                    cpp_args : '-lfruit', link_args : '-lfruit')
# test('filehash', test_file_hash)

# Build executable for PRODUCT
# imps = buffer_imps + file_hash_imps
# src = imps + ['src/main.cpp']
# ex = executable('dbms.ex', src, include_directories : incdir)

