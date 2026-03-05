#pragma once

#ifdef _WIN32
#include <io.h>
#include <sys/stat.h>

#define mkdir(path, mode) mkdir(path)
#endif

void host_init();
