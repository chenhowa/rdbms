project('demo-dbms', 'cpp')
incdir = include_directories('src/headers')
#imps = ['src/imp/file_hash.cpp', 'src/imp/file_sort.cpp']
imps = ['src/imp/block_buffer.cpp']
src = imps + ['src/main.cpp']
test_src = imps + ['src/main.test.cpp']
ex = executable('dbms', src, include_directories : incdir)
test_ex = executable('test_ex', test_src, include_directories : incdir)
test('BlockBuffer Unit tests', test_ex)
