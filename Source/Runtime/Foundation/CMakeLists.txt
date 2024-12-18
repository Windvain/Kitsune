kitsune_add_library(
    TARGET KitsuneFoundation
    SOURCES
    "Common/Features.h"
    "Common/Macros.h"
    "Common/Predefined.h"
    "Common/Types.h"

    "Diagnostics/BadAllocException.h"
    "Diagnostics/IException.h"
    "Diagnostics/OutOfRangeException.h"

    "Memory/AddressOf.h"
    "Memory/CMallocApi.h"
    "Memory/IMemoryApi.h"
    "Memory/Memory.cpp"
    "Memory/Memory.h"
)
