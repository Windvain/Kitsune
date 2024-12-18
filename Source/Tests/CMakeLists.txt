kitsune_add_test(
    TARGET FoundationTests
    SOURCES
    "FoundationTests/AddressOfTests.cpp"
    "FoundationTests/FoundationMain.cpp"
    "FoundationTests/MemoryTests.cpp"

    DEPENDENCIES
    KitsuneFoundation
)
