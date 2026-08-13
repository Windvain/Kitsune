#pragma once

#include "Foundation/Filesystem/Path.h"

namespace Kitsune
{
    // Retrieves a path to the current executable. Note that this function is NOT
    // thread-safe. Make sure that NO CODE will call OS APIs in the duration that this
    // function is running.
    [[nodiscard]] KITSUNE_API Path GetExecutablePath();

    // Gets the name of the executable. This function is also not thread-safe.
    [[nodiscard]]
    inline Path GetExecutableName(bool removeExt = false)
    {
        Path path = GetExecutablePath();
        return removeExt ? path.GetFileStem() : path.GetFilename();
    }
}
