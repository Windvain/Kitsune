#pragma once

#include "Foundation/Filesystem/Path.h"

namespace Kitsune::Filesystem
{
    [[nodiscard]]
    Path ToAbsolute(PathView path);
}
