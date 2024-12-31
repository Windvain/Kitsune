kitsune_add_test(
    TARGET FoundationTests
    SOURCES
    "FoundationTests/AddressOfTests.cpp"
    "FoundationTests/ArrayTests.cpp"
    "FoundationTests/CompareStrings.h"
    "FoundationTests/CopyTests.cpp"
    "FoundationTests/DestroyTests.cpp"
    "FoundationTests/DistanceTests.cpp"
    "FoundationTests/EqualTests.cpp"
    "FoundationTests/FillTests.cpp"
    "FoundationTests/FindTests.cpp"
    "FoundationTests/ForEachTests.cpp"
    "FoundationTests/FoundationMain.cpp"
    "FoundationTests/IteratorWrappers.h"
    "FoundationTests/MemoryTests.cpp"
    "FoundationTests/MoveTests.cpp"
    "FoundationTests/ReverseIteratorTests.cpp"
    "FoundationTests/ReverseTests.cpp"
    "FoundationTests/ScopedPtrTests.cpp"
    "FoundationTests/SharedPtrTests.cpp"
    "FoundationTests/SwapTests.cpp"
    "FoundationTests/TestContainer.h"
    "FoundationTests/UninitializedTests.cpp"

    DEPENDENCIES
    KitsuneFoundation
)
