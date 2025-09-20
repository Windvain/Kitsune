#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Filesystem/Path.h"

namespace Kitsune::Filesystem
{
    [[nodiscard]] bool PathExists(const Path& str);

    [[nodiscard]] bool IsFile(const Path& str);
    [[nodiscard]] bool IsDirectory(const Path& str);
}
