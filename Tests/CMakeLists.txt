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
    "FoundationTests/IsValidUnicodeTests.cpp"
    "FoundationTests/Main.cpp"
    "FoundationTests/StringTests.cpp"
    "FoundationTests/StringViewTests.cpp"
)

add_test(FoundationTests COMMAND FoundationTests)
set_target_properties(FoundationTests PROPERTIES FOLDER "Tests")

# ========== SystemInformationTests ========================================]
add_executable(SystemInformationTests ${TEST_EXE_TYPE}
    "SystemInformationTests/Main.cpp"
)

add_test(SystemInformationTests COMMAND SystemInformationTests)
set_target_properties(SystemInformationTests PROPERTIES FOLDER "Tests")
