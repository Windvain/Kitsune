#pragma once

#include "Foundation/Filesystem/Path.h"

namespace Kitsune::Filesystem
{
    // Sets the current directory of the process. This function is thread-safe.
    KITSUNE_API void SetCurrentDirectory(PathView path);

    // Retrieves the current directory of the process. This function is thread-safe.
    KITSUNE_API Path GetCurrentDirectory();
}
