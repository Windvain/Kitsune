if (WIN32)
    set(TEST_EXE_TYPE WIN32)
endif()

include_directories(
    "${PROJECT_SOURCE_DIR}/External/googletest/googletest/include"
    "${PROJECT_SOURCE_DIR}/External/googletest/googlemock/include"
)

link_libraries(
    GTest::gtest
    GTest::gmock

    KitsuneEngine
)

# ========== FoundationTests ========================================]
add_executable(FoundationTests ${TEST_EXE_TYPE}
    "FoundationTests/AddressOfTests.cpp"
    "FoundationTests/ArrayTests.cpp"
    "FoundationTests/BackInsertIteratorTests.cpp"
    "FoundationTests/BasicStringTests.cpp"
    "FoundationTests/CharTraitsTests.cpp"
    "FoundationTests/CompareStrings.h"
    "FoundationTests/CopyTests.cpp"
    "FoundationTests/CountTests.cpp"
    "FoundationTests/DestroyTests.cpp"
    "FoundationTests/DistanceTests.cpp"
    "FoundationTests/EqualTests.cpp"
    "FoundationTests/FillTests.cpp"
    "FoundationTests/FindTests.cpp"
    "FoundationTests/ForEachTests.cpp"
    "FoundationTests/FormatTests.cpp"
    "FoundationTests/FormatterTests.cpp"
    "FoundationTests/FoundationMain.cpp"
    "FoundationTests/FromCodepointsTests.cpp"
    "FoundationTests/FunctionTests.cpp"
    "FoundationTests/IteratorWrappers.h"
    "FoundationTests/LoggerTests.cpp"
    "FoundationTests/MemoryTests.cpp"
    "FoundationTests/MoveTests.cpp"
    "FoundationTests/ReplaceTests.cpp"
    "FoundationTests/ReverseIteratorTests.cpp"
    "FoundationTests/ReverseTests.cpp"
    "FoundationTests/ScopedPtrTests.cpp"
    "FoundationTests/SharedPtrTests.cpp"
    "FoundationTests/StringViewTests.cpp"
    "FoundationTests/SwapTests.cpp"
    "FoundationTests/TestContainer.h"
    "FoundationTests/ToCodepointsTests.cpp"
    "FoundationTests/UninitializedTests.cpp"
    "FoundationTests/Vector2Tests.cpp"
    "FoundationTests/WindowsPathTests.cpp"
    "FoundationTests/WriteStreamIteratorTests.cpp"
)

add_test(FoundationTests COMMAND FoundationTests)
set_target_properties(FoundationTests PROPERTIES FOLDER "Tests")
