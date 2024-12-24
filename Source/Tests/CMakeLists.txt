kitsune_add_test(
    TARGET FoundationTests
    SOURCES
    "FoundationTests/AddressOfTests.cpp"
    "FoundationTests/FoundationMain.cpp"
    "FoundationTests/MemoryTests.cpp"
    "FoundationTests/ScopedPtrTests.cpp"
    "FoundationTests/SharedPtrTests.cpp"

    DEPENDENCIES
    KitsuneFoundation
)
