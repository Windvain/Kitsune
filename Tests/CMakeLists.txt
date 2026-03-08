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
    "FoundationTests/AddressOfTests.cpp"
    "FoundationTests/AdvanceTests.cpp"
    "FoundationTests/ArrayTests.cpp"
    "FoundationTests/BackInsertIteratorTests.cpp"
    "FoundationTests/BacktraceFrameTests.cpp"
    "FoundationTests/ContainsTests.cpp"
    "FoundationTests/CopyTests.cpp"
    "FoundationTests/CountTests.cpp"
    "FoundationTests/DecodeTests.cpp"
    "FoundationTests/DestroyTests.cpp"
    "FoundationTests/DistanceTests.cpp"
    "FoundationTests/EncodeTests.cpp"
    "FoundationTests/EqualTests.cpp"
    "FoundationTests/FillTests.cpp"
    "FoundationTests/FindTests.cpp"
    "FoundationTests/ForEachTests.cpp"
    "FoundationTests/Main.cpp"
    "FoundationTests/MoveTests.cpp"
    "FoundationTests/ReplaceTests.cpp"
    "FoundationTests/ReverseTests.cpp"
    "FoundationTests/ScopedPtrTests.cpp"
    "FoundationTests/SharedPtrTests.cpp"
    "FoundationTests/SourceLocationTests.cpp"
    "FoundationTests/StringTests.cpp"
    "FoundationTests/StringViewTests.cpp"
    "FoundationTests/SwapTests.cpp"
    "FoundationTests/TestContainer.h"
    "FoundationTests/TestIterators.h"
    "FoundationTests/TestStrings.h"
    "FoundationTests/ToAddressTests.cpp"
    "FoundationTests/TranscodeTests.cpp"
    "FoundationTests/UninitializedTests.cpp"
    "FoundationTests/Utf8EncodingTests.cpp"
    "FoundationTests/Utf16EncodingTests.cpp"
    "FoundationTests/Utf32EncodingTests.cpp"
    "FoundationTests/ValidTests.cpp"
    "FoundationTests/Vector2Tests.cpp"
)

add_test(FoundationTests COMMAND FoundationTests)
set_target_properties(FoundationTests PROPERTIES FOLDER "Tests")

# ========== SystemInformationTests ========================================]
add_executable(SystemInformationTests ${TEST_EXE_TYPE}
    "SystemInformationTests/Main.cpp"
)

add_test(SystemInformationTests COMMAND SystemInformationTests)
set_target_properties(SystemInformationTests PROPERTIES FOLDER "Tests")
