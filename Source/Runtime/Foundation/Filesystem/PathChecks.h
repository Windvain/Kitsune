#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Filesystem/Path.h"

namespace Kitsune::Filesystem
{
    [[nodiscard]] KITSUNE_API_ bool PathExists(const Path& str);

    [[nodiscard]] KITSUNE_API_ bool IsFile(const Path& str);
    [[nodiscard]] KITSUNE_API_ bool IsDirectory(const Path& str);
}
