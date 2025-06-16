#pragma once

#include "Foundation/Common/Predefined.h"
#include "Foundation/Filesystem/WindowsPath.h"

namespace Kitsune::Filesystem
{
#if defined(KITSUNE_OS_WINDOWS)
    using Path = Filesystem::WindowsPath;
#endif
}
