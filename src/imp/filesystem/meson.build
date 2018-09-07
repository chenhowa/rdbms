# builds the filesystem


inc = [inc_filesystem, inc_i_filesystem]
src = [
    'filesystem.cpp',
]
libs = [
]

filesystem_lib = library('filesystem', src,
                include_directories : inc,
                link_with : libs)



inc = [
    inc_filesystem,
    inc_i_filesystem
]
src = [
    'g_filesystem.cpp'
]
g_filesystem_lib = library('g_filesystem', src,
                                include_directories : inc,
                                link_with : filesystem_lib)

inc = [
    inc_filesystem,
    inc_i_filesystem
]
src = [
    'file_manager.cpp',
    'mock_file_manager.cpp'
]
libs = [
    g_filesystem_lib,
    filesystem_lib
]
file_manager_lib = library('file_manager', src,
                            include_directories: inc,
                            link_with : libs )
