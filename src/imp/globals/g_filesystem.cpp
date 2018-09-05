
#ifndef G_FILESYSTEM_CPP
#define G_FILESYSTEM_CPP

#include "filesystem.hpp"

fruit::Injector<IFileSystem> inj(getIFileSystem);
IFileSystem *g_filesystem(inj);


#endif // G_FILESYSTEM_CPP