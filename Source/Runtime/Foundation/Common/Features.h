#pragma once

#include "Foundation/Common/Predefined.h"

// Checks whether the compiler used supports SEH (Structured Exception Handling).
#if defined(KITSUNE_COMPILER_MSVC)
    #define KITSUNE_COMPILER_SUPPORTS_SEH 1
#endif
