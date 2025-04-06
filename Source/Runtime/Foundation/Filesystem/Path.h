#pragma once

#include "Foundation/Common/Predefined.h"

#if defined(KITSUNE_OS_WINDOWS)
    #include "Foundation/Filesystem/WindowsPath.h"
    namespace Kitsune { using Path = WindowsPath; }
#else
    #error No implementation was found.
#endif
