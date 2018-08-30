project('demo-dbms', 'cpp', default_options : ['cpp_std=c++11'])
#add_global_arguments('-lfruit', language: 'cpp')
incdir = include_directories('src/headers')
#imps = ['src/imp/file_sort.cpp']

# Build executable for BlockBuffer unit tests
buffer_imps = ['src/imp/block_buffer.cpp']
test_mocks = []
test_buffer_src = buffer_imps + test_mocks + ['src/main.buffer.test.cpp']
test_buffer = executable('buffer.test', test_buffer_src, include_directories : incdir,
                cpp_args : '-lfruit', link_args : '-lfruit')
test('blockbuffer', test_buffer)

# Build executable for testing FileHasher unit tests
# file_hash_imps = ['src/imp/file_hash.cpp', 'src/imp/block_buffer.cpp']
# test_file_hash_src = file_hash_imps + ['src/main.file_hash.test.cpp']
# test_file_hash = executable('file_hash.test', test_file_hash_src, include_directories : incdir)
# test('filehash', test_file_hash)

# Build executable for PRODUCT
# imps = buffer_imps + file_hash_imps
# src = imps + ['src/main.cpp']
# ex = executable('dbms.ex', src, include_directories : incdir)

