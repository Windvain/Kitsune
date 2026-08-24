#pragma once

#include "Foundation/Filesystem/Path.h"

namespace Kitsune::Filesystem
{
    [[nodiscard]]
    KITSUNE_API Path ToAbsolute(PathView path);
}
