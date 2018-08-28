project('demo-dbms', 'cpp', default_options : ['cpp_std=c++11'])
incdir = include_directories('src/headers')
#imps = ['src/imp/file_hash.cpp', 'src/imp/file_sort.cpp']
imps = ['src/imp/block_buffer.cpp']
src = imps + ['src/main.cpp']
test_src = imps + ['src/main.buffer.test.cpp']
ex = executable('dbms.ex', src, include_directories : incdir)
test_buffer = executable('buffer.test', test_src, include_directories : incdir)
test('BlockBuffer Unit tests', test_buffer)
