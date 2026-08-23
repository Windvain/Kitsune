#pragma once

#include "Foundation/Filesystem/Path.h"

namespace Kitsune::Filesystem
{
    // Retrieves a path to the current executable.
    [[nodiscard]] KITSUNE_API Path GetExecutablePath();

    // Gets the name of the executable.
    [[nodiscard]]
    inline Path GetExecutableName(bool removeExt = false)
    {
        Path path = GetExecutablePath();
        return removeExt ? path.GetFileStem() : path.GetFilename();
    }
}
