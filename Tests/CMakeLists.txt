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
# MSVC corrupts UTF-8 strings if /utf-8 is not specified.
# It first encodes the string as UTF-8, then encodes it again in Windows-1252.
#
# Thanks, Microsoft.
# https://stackoverflow.com/questions/59046071/c-u8-literal-unexpected-encoding-on-windows
if (MSVC)
    add_compile_options("/utf-8")
endif()

# ========== FoundationTests ========================================]
add_executable(FoundationTests ${TEST_EXE_TYPE}
    "FoundationTests/Algorithms/AdvanceTests.cpp"
    "FoundationTests/Algorithms/ContainsTests.cpp"
    "FoundationTests/Algorithms/CopyTests.cpp"
    "FoundationTests/Algorithms/CountTests.cpp"
    "FoundationTests/Algorithms/DestroyTests.cpp"
    "FoundationTests/Algorithms/DistanceTests.cpp"
    "FoundationTests/Algorithms/EqualTests.cpp"
    "FoundationTests/Algorithms/FillTests.cpp"
    "FoundationTests/Algorithms/FindTests.cpp"
    "FoundationTests/Algorithms/ForEachTests.cpp"
    "FoundationTests/Algorithms/MoveTests.cpp"
    "FoundationTests/Algorithms/ReplaceTests.cpp"
    "FoundationTests/Algorithms/ReverseTests.cpp"
    "FoundationTests/Algorithms/SwapTests.cpp"
    "FoundationTests/Algorithms/UninitializedTests.cpp"

    "FoundationTests/Containers/ArrayTests.cpp"
    "FoundationTests/Containers/PairTests.cpp"
    "FoundationTests/Containers/TreeSetTests.cpp"

    "FoundationTests/Diagnostics/BacktraceFrameTests.cpp"
    "FoundationTests/Diagnostics/SourceLocationTests.cpp"

    "FoundationTests/Filesystem/FileStreamTests.cpp"

    "FoundationTests/Include/PushBackContainer.h"
    "FoundationTests/Include/StatefulAllocator.h"
    "FoundationTests/Include/StringEqualAssert.h"
    "FoundationTests/Include/TestContainer.h"
    "FoundationTests/Include/TestIterators.h"
    "FoundationTests/Include/TrackingAllocator.h"

    "FoundationTests/Iterators/BackInsertIteratorTests.cpp"
    "FoundationTests/Iterators/IteratorTraitsTests.cpp"
    "FoundationTests/Iterators/ReverseIteratorTests.cpp"
    "FoundationTests/Iterators/ToAddressTests.cpp"

    "FoundationTests/Maths/Vector2Tests.cpp"
    "FoundationTests/Maths/Vector3Tests.cpp"
    "FoundationTests/Maths/Vector4Tests.cpp"

    "FoundationTests/Memory/AddressOfTests.cpp"
    "FoundationTests/Memory/ScopedPtrTests.cpp"
    "FoundationTests/Memory/SharedPtrTests.cpp"

    "FoundationTests/String/EncodingHelperTests.cpp"
    "FoundationTests/String/StringTests.cpp"
    "FoundationTests/String/StringViewTests.cpp"
    "FoundationTests/String/Utf8EncodingTests.cpp"
    "FoundationTests/String/Utf16EncodingTests.cpp"
    "FoundationTests/String/Utf32EncodingTests.cpp"

    "FoundationTests/Threading/AtomicTests.cpp"

    "FoundationTests/Utilities/FunctorTests.cpp"

    "FoundationTests/Main.cpp"
)

add_test(FoundationTests COMMAND FoundationTests)

target_include_directories(FoundationTests PRIVATE "FoundationTests/Include/")
set_target_properties(FoundationTests PROPERTIES FOLDER "Tests")

# ========== SystemInformationTests ========================================]
add_executable(SystemInformationTests ${TEST_EXE_TYPE}
    "SystemInformationTests/Main.cpp"
)

add_test(SystemInformationTests COMMAND SystemInformationTests)
set_target_properties(SystemInformationTests PROPERTIES FOLDER "Tests")
